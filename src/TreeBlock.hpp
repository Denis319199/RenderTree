#ifndef TREEBLOCK_HPP
#define TREEBLOCK_HPP

#include "EventInfo.hpp"
#include "TreeBlockHandler.hpp"
#include "Usings.hpp"

namespace graphics {

struct Area {
  SizeType pos_x;
  SizeType pos_y;
  SizeType width;
  SizeType height;

  bool operator==(const Area &other) {
    if (other.pos_x != pos_x) {
      return false;
    }
    if (other.pos_y != pos_y) {
      return false;
    }
    if (other.width != width) {
      return false;
    }
    if (other.height != height) {
      return false;
    }

    return true;
  }

  bool operator!=(const Area &other) { return !(*this == other); }

  bool DoesPointFallWithinArea(SizeType x, SizeType y) const noexcept {
    auto max_pos_x{pos_x + width};
    auto max_pos_y{pos_y + height};

    if (x >= pos_x && x <= max_pos_x) {
      if (y >= pos_y && y <= max_pos_y) {
        return true;
      }
    }

    return false;
  }
};

class RenderTree;

class TreeBlock {
public:
  TreeBlock(TreeBlockHandlerBase *handler) : handler_{handler} {}

  void SetWidth(SizeType width);

  void SetHeight(SizeType height);

  void SetPosX(SizeType pos_x);

  void SetPosY(SizeType pos_y);

  void SetRelativePosX(SizeType pos_x);

  void SetRelativePosY(SizeType pos_y);

  void ActivateHoverRerender() { is_hover_activated_ = true; }

  void DisactivateHoverRerender() { is_hover_activated_ = false; }

  void RenderIsRequired();

  bool IsHovered() const noexcept { return hover_; }

  const Area &GetArea() const noexcept { return area_; }

  const List<TreeBlock *> &GetChildrenList() const noexcept {
    return children_list_;
  }

private:
  bool Render() {
    if (handler_ != nullptr) { 
      glViewport(area_.pos_x, area_.pos_y, area_.width, area_.height);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
              GL_STENCIL_BUFFER_BIT);
      
      return handler_->Render(*this);
    }

    return false;
  }

  bool ProcessChangedArea();

  void ProcessHover();

  void ProcessMouseMovement(SizeType pos_x, SizeType pos_y);

  void ProcessMouseButton(MouseButtonEvent button_event);

  void ProcessMouseScroll(PtrDiff offset_x, PtrDiff offset_y) { 
    if (handler_ != nullptr) {
      handler_->ProcessMouseScroll(*this, offset_x, offset_y);
    }
  }

  List<TreeBlock *> &GetChildrenList() { return children_list_; }

  void CheckAndSetPosX(SizeType pos_x);

  void CheckAndSetPosY(SizeType pos_y);

  void CheckHover();

  friend class RenderTree;

  Area area_; // contains block's position and size
  TreeBlock *parent_;
  List<TreeBlock *> children_list_;
  RenderTree *render_tree_;
  TreeBlockHandlerBase *handler_;

  bool hover_; // if equals false, then a cursor is out of this block, otherwise
               // a cursor is within
  bool is_hover_activated_; // defines if is_render_required_ flag should be set
                            // when hover_ changes its state
};

} // namespace graphics

#endif // TREEBLOCK_HPP
