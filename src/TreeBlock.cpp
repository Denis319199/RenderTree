#include "TreeBlock.hpp"

namespace graphics {

TreeBlock::TreeBlock(TreeBlockHandlerBase *handler) noexcept
    : area_{},
      children_list_{},
      render_tree_{},
      handler_{handler},
      hover_{},
      is_hover_activated_{} {}

void TreeBlock::SetRelativeNormalizedWidth(float width) noexcept {
  if (parent_) {
    if (width > 1.f) {
      width = 1.f;
    } else if (width < 0.f) {
      width = 0.f;
    }

    SetWidth(parent_->area_.width * width);
  }
}

void TreeBlock::SetRelativeNormalizedHeight(float height) noexcept {
  if (parent_) {
    if (height > 1.f) {
      height = 1.f;
    } else if (height < 0.f) {
      height = 0.f;
    }

    SetHeight(parent_->area_.height * height);
  }
}

void TreeBlock::SetRelativePosX(SizeType pos_x) noexcept {
  if (parent_) {
    SetPosX(parent_->area_.pos_x + pos_x);
  }
}

void TreeBlock::SetRelativePosY(SizeType pos_y) noexcept {
  if (parent_) {
    SetPosX(parent_->area_.pos_y + pos_y);
  }
}

void TreeBlock::SetRelativeNormalizedPosX(float pos_x) noexcept {
  if (parent_) {
    if (pos_x > 1.f) {
      pos_x = 1.f;
    } else if (pos_x < 0.f) {
      pos_x = 0.f;
    }

    SetRelativePosX(parent_->area_.width * pos_x);
  }
}

void TreeBlock::SetRelativeNormalizedPosY(float pos_y) noexcept {
  if (parent_) {
    if (pos_y > 1.f) {
      pos_y = 1.f;
    } else if (pos_y < 0.f) {
      pos_y = 0.f;
    }

    SetRelativePosY(parent_->area_.height * pos_y);
  }
}

void TreeBlock::EnableHoverRerender() noexcept { is_hover_activated_ = true; }

void TreeBlock::DisableHoverRerender() noexcept { is_hover_activated_ = false; }

[[nodiscard]] bool TreeBlock::IsHovered() const noexcept { return hover_; }

[[nodiscard]] const Area &TreeBlock::GetArea() const noexcept { return area_; }

[[nodiscard]] NormalizedArea TreeBlock::GetRelaftiveNormalizedArea()
    const noexcept {
  const auto &parent_area_{parent_->area_};
  auto norm_pos_x{static_cast<float>(parent_area_.width) /
                  static_cast<float>(area_.pos_x)};
  auto norm_pos_y{static_cast<float>(parent_area_.height) /
                  static_cast<float>(area_.pos_y)};
  auto norm_width{static_cast<float>(parent_area_.width) /
                  static_cast<float>(area_.width)};
  auto norm_height{static_cast<float>(parent_area_.height) /
                   static_cast<float>(area_.height)};
  return {norm_pos_x, norm_pos_y, norm_width, norm_height};
}

[[nodiscard]] const List<TreeBlock *> &TreeBlock::GetChildrenList()
    const noexcept {
  return children_list_;
}

[[nodiscard]] bool TreeBlock::Render() const noexcept {
  if (handler_ != nullptr) {
    glViewport(area_.pos_x, area_.pos_y, area_.width, area_.height);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    return handler_->Render(*this);
  }

  return false;
}

[[nodiscard]] bool TreeBlock::ProcessChangedArea() noexcept {
  auto old_area{area_};

  if (handler_) {
    handler_->ProcessChangedArea(*this);
  }

  SetWidth(area_.width);
  SetHeight(area_.height);
  if (area_ != old_area) {
    return true;
  }

  return false;
}

void TreeBlock::ProcessMouseMovement() noexcept {
  if (handler_) {
    handler_->ProcessMouseMovement(*this);
  }
}

void TreeBlock::ProcessMouseButton() noexcept {
  if (handler_) {
    handler_->ProcessMouseButton(*this);
  }
}

void TreeBlock::ProcessMouseScroll() noexcept {
  if (handler_ != nullptr) {
    handler_->ProcessMouseScroll(*this);
  }
}

void TreeBlock::CheckAndSetPosX(SizeType pos_x) noexcept {
  const auto &parent_area{parent_->area_};

  auto max_pos_x = parent_area.pos_x + parent_area.width;
  if (pos_x >= parent_area.pos_x) {
    if (pos_x + area_.width <= max_pos_x) {
      area_.pos_x = pos_x;
    } else {
      area_.pos_x = max_pos_x - area_.width;
    }
  } else {
    area_.pos_x = parent_area.pos_x;
  }
}

void TreeBlock::CheckAndSetPosY(SizeType pos_y) noexcept {
  const auto &parent_area{parent_->area_};

  auto max_pos_y = parent_area.pos_y + parent_area.height;
  if (pos_y >= parent_area.pos_y) {
    if (pos_y + area_.height <= max_pos_y) {
      area_.pos_y = pos_y;
    } else {
      area_.pos_y = max_pos_y - area_.height;
    }
  } else {
    area_.pos_y = parent_area.pos_y;
  }
}

}  // namespace graphics
