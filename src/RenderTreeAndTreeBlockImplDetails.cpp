#include "RenderTree.hpp"
#include "TreeBlock.hpp"

namespace graphics {

////////////IMPLEMENTATION OF THE DEPENDENT PART OF CLASS TreeBlock////////////

RenderTree::RenderTree(Area area, SizeType max_width, SizeType max_height,
                       TreeBlockHandlerBase *handler) noexcept
    : root_{new TreeBlock{handler}},
      hovered_block_{},
      tree_info_{max_width, max_height},
      is_render_required_{true},
      check_hover_{true},
      fbo_{},
      texture_{},
      vao_{},
      rectangle_vbo_{},
      coords_vbo_{},
      sp_{"./shaders/rtree/rtree.vs", "./shaders/rtree/rtree.fs"} {
  root_->SetPosX(area.pos_x);
  root_->SetPosY(area.pos_y);
  root_->SetWidth(area.width);
  root_->SetHeight(area.height);

  constexpr auto max_val{std::numeric_limits<PtrDiff>::max()};
  auto &mouse_info{tree_info_.mouse_info};
  mouse_info.cursor_pos_x = max_val;
  mouse_info.cursor_pos_y = max_val;

  CreateBuffers();
}

[[nodiscard]] bool RenderTree::Render() noexcept {
  if (is_render_required_) {
    Render(root_);

    // set back full-screen viewport
    const auto &cur_window_area{root_->area_};
    glViewport(cur_window_area.pos_x, cur_window_area.pos_y,
               cur_window_area.width, cur_window_area.height);

    // copy fbo into default back framebuffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_);
    glBlitFramebuffer(cur_window_area.pos_x, cur_window_area.pos_y,
                      cur_window_area.pos_x + cur_window_area.width,
                      cur_window_area.pos_y + cur_window_area.height,
                      cur_window_area.pos_x, cur_window_area.pos_y,
                      cur_window_area.pos_x + cur_window_area.width,
                      cur_window_area.pos_y + cur_window_area.height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    is_render_required_ = false;
    return true;
  }

  return false;
}

void RenderTree::InsertAtRoot(TreeBlock *block) noexcept {
  block->parent_ = root_;
  block->render_tree_ = this;
  block->SetPosX(block->area_.pos_x);
  block->SetPosY(block->area_.pos_y);

  const auto &mouse_info{tree_info_.mouse_info};
  if (block->area_.DoesPointFallWithinArea(mouse_info.cursor_pos_x,
                                           mouse_info.cursor_pos_y)) {
    hovered_block_->ProcessHover();
    hovered_block_ = block;
    block->ProcessHover();
  }

  root_->children_list_.push_back(block);
  is_render_required_ = true;
}

void RenderTree::ProcessCursorLeaveWindow() noexcept {
  auto &mouse_info{tree_info_.mouse_info};
  mouse_info.cursor_pos_x_diff = 0.f;
  mouse_info.cursor_pos_y_diff = 0.f;

  constexpr auto max_val{std::numeric_limits<PtrDiff>::max()};
  mouse_info.cursor_pos_x = max_val;
  mouse_info.cursor_pos_y = max_val;

  if (check_hover_ && hovered_block_) {
    hovered_block_->ProcessHover();
    hovered_block_ = nullptr;
  }
  root_->ProcessHover();
}

void RenderTree::ProcessMouseMovement(PtrDiff pos_x, PtrDiff pos_y) noexcept {
  auto &mouse_info{tree_info_.mouse_info};

  constexpr auto max_val{std::numeric_limits<PtrDiff>::max()};
  auto old_cursor_pos_x{mouse_info.cursor_pos_x};

  mouse_info.cursor_pos_x_diff = pos_x - mouse_info.cursor_pos_x;
  mouse_info.cursor_pos_y_diff = pos_y - mouse_info.cursor_pos_y;
  mouse_info.cursor_pos_x = pos_x;
  mouse_info.cursor_pos_y = pos_y;

  if (old_cursor_pos_x == max_val) {
    root_->ProcessHover();
  }
  CheckHover();

  root_->ProcessMouseMovement();
  auto &children_list{root_->GetChildrenList()};
  for (auto &child : children_list) {
    ProcessMouseMovement(child);
  }
}

void RenderTree::ChangeArea(Area area) noexcept {
  if (auto &root_area{root_->area_}; root_area != area) {
    root_area = area;

    auto &root_children{root_->GetChildrenList()};
    for (auto &child : root_children) {
      ChangeArea(child);
    }

    is_render_required_ = true;
  }
}

[[nodiscard]] const Area &RenderTree::GetRootArea() const noexcept {
  return root_->area_;
}

void RenderTree::ProcessMouseMovement(TreeBlock *block) noexcept {
  block->ProcessMouseMovement();

  auto &children_list{block->GetChildrenList()};
  for (auto &child : children_list) {
    ProcessMouseMovement(child);
  }
}

void RenderTree::ProcessMouseButton(TreeBlock *block) noexcept {
  block->ProcessMouseButton();

  auto &children_list{block->GetChildrenList()};
  for (auto &child : children_list) {
    ProcessMouseButton(child);
  }
}

void RenderTree::ChangeArea(TreeBlock *block) noexcept {
  auto res{block->ProcessChangedArea()};

  if (res) {
    auto &block_children{block->GetChildrenList()};
    for (auto &child : block_children) {
      ChangeArea(child);
    }
  }
}

void RenderTree::ProcessMouseScroll(TreeBlock *block) noexcept {
  block->ProcessMouseScroll();

  auto &block_children{block->children_list_};
  for (auto &child : block_children) {
    ProcessMouseScroll(child);
  }
}

void RenderTree::Render(TreeBlock *block) noexcept {
  if (block->Render()) {
    BlendFBOAndFramebuffer(block->area_);
  }

  const auto &children_list{block->GetChildrenList()};
  for (auto child : children_list) {
    Render(child);
  }
}

TreeBlock *RenderTree::FindHoveredBlock(TreeBlock *block) const noexcept {
  if (block->children_list_.empty()) {
    return nullptr;
  }

  auto iter{block->children_list_.rbegin()};
  auto end_iter{block->children_list_.rend()};
  const auto &mouse_info{tree_info_.mouse_info};

  while (iter != end_iter) {
    if ((*iter)->area_.DoesPointFallWithinArea(mouse_info.cursor_pos_x,
                                               mouse_info.cursor_pos_y)) {
      if (auto result{FindHoveredBlock(*iter)}; result) {
        return result;
      } else {
        return *iter;
      }
    }
    ++iter;
  }
}

void RenderTree::CheckHover() noexcept {
  if (!check_hover_) {
    return;
  }

  auto &mouse_info{tree_info_.mouse_info};

  if (!hovered_block_) {  // case when the curosor entered the window
    hovered_block_ = FindHoveredBlock(root_);
    if (hovered_block_) {
      hovered_block_->ProcessHover();
    }
  } else if (hovered_block_->area_.DoesPointFallWithinArea(
                 mouse_info.cursor_pos_x, mouse_info.cursor_pos_y)) {
    auto possible_hovered_block{FindHoveredBlock(root_)};
    if (possible_hovered_block != hovered_block_) {
      hovered_block_->ProcessHover();
      hovered_block_ = possible_hovered_block;
      hovered_block_->ProcessHover();
    }
  } else {
    hovered_block_->ProcessHover();
    hovered_block_ = FindHoveredBlock(root_);
    if (hovered_block_) {
      hovered_block_->ProcessHover();
    }
  }
}

////////////IMPLEMENTATION OF THE DEPENDENT PART OF CLASS TreeBlock////////////

void TreeBlock::SetWidth(SizeType width) noexcept {
  if (parent_) {
    auto old_width{area_.width};
    const auto &parent_area{parent_->area_};

    if (width <= parent_area.width) {
      area_.width = width;
    } else {
      area_.width = parent_area.width;
    }

    if (old_width != area_.width) {
      CheckAndSetPosX(area_.pos_x);
      if (render_tree_) {
        render_tree_->CheckHover();
        render_tree_->is_render_required_ = true;
      }
    }
  } else {
    area_.width = width;
  }
}

void TreeBlock::SetHeight(SizeType height) noexcept {
  if (parent_) {
    auto old_height{area_.height};
    const auto &parent_area{parent_->area_};

    if (height <= parent_area.height) {
      area_.height = height;
    } else {
      area_.height = parent_area.height;
    }

    if (old_height != area_.height) {
      CheckAndSetPosY(area_.pos_y);
      if (render_tree_) {
        render_tree_->CheckHover();
        render_tree_->is_render_required_ = true;
      }
    }
  } else {
    area_.height = height;
  }
}

void TreeBlock::SetPosX(SizeType pos_x) noexcept {
  if (parent_) {
    auto old_pos_x = area_.pos_x;

    CheckAndSetPosX(pos_x);

    if (render_tree_ && old_pos_x != area_.pos_x) {
      render_tree_->CheckHover();
      render_tree_->is_render_required_ = true;
    }
  } else {
    area_.pos_x = pos_x;
  }
}

void TreeBlock::SetPosY(SizeType pos_y) noexcept {
  if (parent_) {
    auto old_pos_y = area_.pos_y;

    CheckAndSetPosY(pos_y);

    if (render_tree_ && old_pos_y != area_.pos_y) {
      render_tree_->CheckHover();
      render_tree_->is_render_required_ = true;
    }
  } else {
    area_.pos_y = pos_y;
  }
}

void TreeBlock::RenderIsRequired() noexcept {
  assert(render_tree_ &&
         "Error in an invocation of the RenderIsRequired method. There is no "
         "RenderTree object binded to a block");
  render_tree_->is_render_required_ = true;
}

[[nodiscard]] const MouseInfo &TreeBlock::GetMouseInfo() const noexcept {
  assert(render_tree_ &&
         "Error in an invocation of the GetMouseInfo method.There is no "
         "RenderTree object binded to a block");
  return render_tree_->tree_info_.mouse_info;
}

void TreeBlock::ProcessHover() noexcept {
  hover_ ^= 1;

  if (handler_) {
    handler_->ProcessHover(*this);

    if (is_hover_activated_) {
      render_tree_->is_render_required_ = true;
    }
  }
}

[[nodiscard]] bool TreeBlock::IsCursorOutOfWindow() const noexcept {
  assert(render_tree_ &&
         "Error in an invocation of the IsCursorOutOfWindow method.There is no "
         "RenderTree object binded to a block");
  constexpr auto max_val{std::numeric_limits<PtrDiff>::max()};
  return render_tree_->tree_info_.mouse_info.cursor_pos_x == max_val;
}

void TreeBlock::DisableCheckingHover() noexcept {
  assert(
      render_tree_ &&
      "Error in an invocation of the DisableCheckingHover method.There is no "
      "RenderTree object binded to a block");
  render_tree_->DisableCheckingHover();
}

void TreeBlock::EnableCheckingHover() noexcept {
  assert(render_tree_ &&
         "Error in an invocation of the EnableCheckingHover method.There is no "
         "RenderTree object binded to a block");
  render_tree_->EnableCheckingHover();
}

}  // namespace graphics
