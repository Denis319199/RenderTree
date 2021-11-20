#ifndef RENDERTREE_HPP
#define RENDERTREE_HPP

#include <iostream>

#include "EventInfo.hpp"
#include "RenderTreeInfo.hpp"
#include "ShaderProgram.hpp"
#include "TreeBlockAreas.hpp"
#include "TreeBlockHandler.hpp"
#include "Usings.hpp"

namespace graphics {

class TreeBlock;

// Class RenderTree is responsible for right calls of processing methods in
// TreeBlock classes. An instance of the class should be supplied with
// information about a window and its events.
class RenderTree {
 public:
  RenderTree(Area area, SizeType max_width, SizeType max_height,
             TreeBlockHandlerBase *handler) noexcept;

  ~RenderTree();

  RenderTree(const RenderTree &) = delete;

  RenderTree &operator=(const RenderTree &) = delete;

  [[nodiscard]] bool Render() noexcept;

  void InsertAtRoot(TreeBlock *block) noexcept;

  void ProcessCursorLeaveWindow() noexcept;

  void ProcessMouseMovement(PtrDiff pos_x, PtrDiff pos_y) noexcept;

  void ProcessMouseButton(MouseButtonEvent button_event) noexcept;

  void ProcessMouseScroll(PtrDiff offset_x, PtrDiff offset_y) noexcept;

  void ChangeArea(Area area) noexcept;

  [[nodiscard]] const Area &GetRootArea() const noexcept;

 private:
  void ProcessMouseMovement(TreeBlock *block) noexcept;

  void ProcessMouseButton(TreeBlock *block) noexcept;

  void ChangeArea(TreeBlock *block) noexcept;

  void ProcessMouseScroll(TreeBlock *block) noexcept;

  void CreateBuffers() noexcept;

  void Render(TreeBlock *block) noexcept;

  void BlendFBOAndFramebuffer(const Area &area) noexcept;

  void DisableCheckingHover() noexcept;

  void EnableCheckingHover() noexcept;

  void CheckHover() noexcept;

  TreeBlock *FindHoveredBlock(TreeBlock *block) const noexcept;

  friend TreeBlock;

  TreeBlock *root_;  // points to special window based block
  TreeBlock *hovered_block_;
  TreeInfo tree_info_;

  bool is_render_required_;
  bool check_hover_;

  GLuint fbo_;            // buffer is to store result of previous rendering
  GLuint texture_;        // texture is attached to fbo
  GLuint vao_;            // vao
  GLuint rectangle_vbo_;  // stores verteces for the texture rectangle
  GLuint coords_vbo_;

  ShaderProgram sp_;
};

}  // namespace graphics

#endif  // RENDERTREE_HPP
