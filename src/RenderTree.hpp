#ifndef RENDERTREE_HPP
#define RENDERTREE_HPP

#include <iostream>

#include "EventInfo.hpp"
#include "RenderTreeInfo.hpp"
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

  void ProcessCursorEnter() noexcept;

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

  void InitShaders() noexcept;

  void CreateBuffers() noexcept;

  void Render(TreeBlock *block) noexcept;

  void BlendFBOAndFramebuffer(const Area &area) noexcept;

  friend TreeBlock;

  TreeBlock *root_;  // points to special window based block
  TreeInfo tree_info_;

  bool is_render_required_;

  GLuint fbo_;      // buffer is to store result of previous rendering
  GLuint texture_;  // texture is attached to fbo
  GLuint vao_;      // vao for the texture rectangle
  GLuint vbo_;      // stores verteces for the texture rectangle
  GLuint vs_object_;
  GLuint fs_object_;
  GLuint shader_program_;

  const char *vs_code =
      "#version 450 core\n"
      "layout(location = 0) in vec2 vertex;\n"
      "void main() { gl_Position = vec4(vertex, 0, 1); }";

  const char *fs_code =
      "#version 450 core\n"
      "uniform sampler2D tex;\n"
      "out vec4 fragment_color;\n"
      "void main() { fragment_color = texture(tex, gl_FragCoord.xy); }";
};

}  // namespace graphics

#endif  // RENDERTREE_HPP
