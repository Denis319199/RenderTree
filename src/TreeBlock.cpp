#include "TreeBlock.hpp"
#include "RenderTree.hpp"

namespace graphics {

void TreeBlock::SetWidth(SizeType width) {
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
      CheckHover();
      render_tree_->RenderIsRequired();
    }
  } else {
    area_.width = width;
  }
}

void TreeBlock::SetHeight(SizeType height) {
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
      CheckHover();
      render_tree_->RenderIsRequired();
    }
  } else {
    area_.height = height;
  }
}

void TreeBlock::SetPosX(SizeType pos_x) {
  if (parent_) {
    auto old_pos_x = area_.pos_x;

    CheckAndSetPosX(pos_x);

    if (old_pos_x != area_.pos_x) {
      CheckHover();
      render_tree_->RenderIsRequired();
    }
  } else {
    area_.pos_x = pos_x;
  }
}

void TreeBlock::SetPosY(SizeType pos_y) {
  if (parent_) {
    auto old_pos_y = area_.pos_y;

    CheckAndSetPosY(pos_y);

    if (old_pos_y != area_.pos_y) {
      CheckHover();
      render_tree_->RenderIsRequired();
    }
  } else {
    area_.pos_y = pos_y;
  }
}

void TreeBlock::SetRelativePosX(SizeType pos_x) {
  SetPosX(parent_->area_.pos_x + pos_x);
}

void TreeBlock::SetRelativePosY(SizeType pos_y) {
  SetPosX(parent_->area_.pos_y + pos_y);
}

void TreeBlock::RenderIsRequired() { render_tree_->RenderIsRequired(); }

bool TreeBlock::ProcessChangedArea() {
  auto old_area{area_};

  if (handler_ != nullptr) {
    handler_->ProcessChangedArea(*this);
  }

  SetWidth(area_.width);
  SetHeight(area_.height);
  if (area_ != old_area) {
    return true;
  } 

  return false;
}

void TreeBlock::ProcessHover() {
  hover_ ^= 1;

  if (handler_ != nullptr) {
    auto old_area{area_};
    handler_->ProcessHover(*this);

    if (is_hover_activated_) {
      render_tree_->RenderIsRequired();
    }
  }
}

void TreeBlock::ProcessMouseMovement(SizeType pos_x, SizeType pos_y) {
  if (handler_ != nullptr) {
    auto old_area{area_};
    handler_->ProcessMouseMovement(*this, pos_x, pos_y);
  }
}

void TreeBlock::ProcessMouseButton(MouseButtonEvent button_event) {
  if (handler_ != nullptr) {
    auto old_area{area_};
    handler_->ProcessMouseButton(*this, button_event);
  }
}

void TreeBlock::CheckHover() {
  const auto &tree_info_{render_tree_->GetTreeInfo()};

  if (area_.pos_x <= tree_info_.cursor_pos_x &&
      area_.pos_x + area_.width >= tree_info_.cursor_pos_x &&
      area_.pos_y <= tree_info_.cursor_pos_y &&
      area_.pos_y + area_.height >= tree_info_.cursor_pos_y) {
    if (!hover_) {
      ProcessHover();
    }
  } else if (hover_) {
    ProcessHover();
  }
}

void TreeBlock::CheckAndSetPosX(SizeType pos_x) {
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

void TreeBlock::CheckAndSetPosY(SizeType pos_y) {
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

} // namespace graphics
