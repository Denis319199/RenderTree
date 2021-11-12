#ifndef TREEBLOCK_HPP
#define TREEBLOCK_HPP

#include "EventInfo.hpp"
#include "RenderTreeInfo.hpp"
#include "TreeBlockAreas.hpp"
#include "TreeBlockHandler.hpp"
#include "Usings.hpp"

namespace graphics {

class RenderTree;

class TreeBlock {
 public:
  TreeBlock(TreeBlockHandlerBase *handler) noexcept : handler_{handler} {}

  void SetWidth(SizeType width) noexcept;

  void SetHeight(SizeType height) noexcept;

  void SetRelativeNormalizedWidth(float width) noexcept;

  void SetRelativeNormalizedHeight(float height) noexcept;

  void SetPosX(SizeType pos_x) noexcept;

  void SetPosY(SizeType pos_y) noexcept;

  void SetRelativePosX(SizeType pos_x) noexcept;

  void SetRelativePosY(SizeType pos_y) noexcept;

  void SetRelativeNormalizedPosX(float pos_x) noexcept;

  void SetRelativeNormalizedPosY(float pos_y) noexcept;

  void ActivateHoverRerender() noexcept;

  void DisactivateHoverRerender() noexcept;

  void RenderIsRequired() noexcept;

  [[nodiscard]] bool IsHovered() const noexcept;

  [[nodiscard]] bool IsCursorOutOfWindow() const noexcept;

  [[nodiscard]] const Area &GetArea() const noexcept;

  [[nodiscard]] const MouseInfo &GetMouseInfo() const noexcept;

  [[nodiscard]] NormalizedArea GetRelaftiveNormalizedArea() const noexcept;

  [[nodiscard]] const List<TreeBlock *> &GetChildrenList() const noexcept;

 private:
  [[nodiscard]] bool Render() const noexcept;

  [[nodiscard]] bool ProcessChangedArea() noexcept;

  void ProcessHover() noexcept;

  void ProcessMouseMovement() noexcept;

  void ProcessMouseButton() noexcept;

  void ProcessMouseScroll() noexcept;

  void CheckAndSetPosX(SizeType pos_x) noexcept;

  void CheckAndSetPosY(SizeType pos_y) noexcept;

  void CheckHover() noexcept;

  friend class RenderTree;

  Area area_;  // contains block's position and size
  TreeBlock *parent_;
  List<TreeBlock *> children_list_;
  RenderTree *render_tree_;
  TreeBlockHandlerBase *handler_;

  bool hover_;  // if equals false, then a cursor is out of this block,
                // otherwise a cursor is within
  bool is_hover_activated_;  // defines if is_render_required_ flag should be
                             // set when hover_ changes its own state
};

}  // namespace graphics

#endif  // TREEBLOCK_HPP
