#include "RenderTree.hpp"

namespace graphics {

RenderTree::~RenderTree() {
  glDeleteFramebuffers(1, &fbo_);
  glDeleteTextures(1, &texture_);
  glDeleteVertexArrays(1, &vao_);
  glDeleteBuffers(1, &rectangle_vbo_);
  glDeleteBuffers(1, &coords_vbo_);
  /*glDeleteProgram(shader_program_);
  glDeleteShader(vs_object_);
  glDeleteShader(fs_object_);*/
}

void RenderTree::ProcessMouseButton(MouseButtonEvent button_event) noexcept {
  tree_info_.mouse_info.last_mouse_button_event_ = button_event;
  ProcessMouseButton(root_);
}

void RenderTree::ProcessMouseScroll(PtrDiff offset_x,
                                    PtrDiff offset_y) noexcept {
  auto &mouse_info{tree_info_.mouse_info};
  mouse_info.scroll_offset_x = offset_x;
  mouse_info.scroll_offset_y = offset_y;
  ProcessMouseScroll(root_);
}

// void RenderTree::InitShaders() noexcept {
//   // preparing vertex shader
//   vs_object_ = glCreateShader(GL_VERTEX_SHADER);
//   glShaderSource(vs_object_, 1, &vs_code, nullptr);
//   glCompileShader(vs_object_);
//   GLint compile_status;
//   glGetShaderiv(vs_object_, GL_COMPILE_STATUS, &compile_status);
//   if (compile_status == GL_FALSE) {
//     GLchar info_log[512];
//     GLsizei length;
//     glGetShaderInfoLog(vs_object_, 512, &length, info_log);
//     std::cout << info_log;
//     exit(1);
//   }
//
//   // preparing fragment shader
//   fs_object_ = glCreateShader(GL_FRAGMENT_SHADER);
//   glShaderSource(fs_object_, 1, &fs_code, nullptr);
//   glCompileShader(fs_object_);
//   glGetShaderiv(fs_object_, GL_COMPILE_STATUS, &compile_status);
//   if (compile_status == GL_FALSE) {
//     GLchar info_log[512];
//     GLsizei length;
//     glGetShaderInfoLog(fs_object_, 512, &length, info_log);
//     std::cout << info_log;
//     exit(1);
//   }
//
//   // linking the shaders
//   shader_program_ = glCreateProgram();
//   glAttachShader(shader_program_, vs_object_);
//   glAttachShader(shader_program_, fs_object_);
//   glLinkProgram(shader_program_);
//   GLint link_status;
//   glGetProgramiv(shader_program_, GL_LINK_STATUS, &link_status);
//   if (link_status == GL_FALSE) {
//     GLchar info_log[512];
//     GLsizei length;
//     glGetProgramInfoLog(shader_program_, 512, &length, info_log);
//     std::cout << info_log;
//     exit(1);
//   }
// }

void RenderTree::CreateBuffers() noexcept {
  // creates fbo and texture for it
  glCreateFramebuffers(1, &fbo_);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
  glCreateTextures(GL_TEXTURE_2D, 1, &texture_);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tree_info_.max_width,
               tree_info_.max_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         texture_, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  // checks that created fbo is complete
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    exit(1);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  GLfloat kSquareCoods[]{-1.f, -1.f, -1.f, 1.f, 1.f, -1.f, 1.f, 1.f};

  glCreateVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glCreateBuffers(1, &rectangle_vbo_);
  glNamedBufferStorage(rectangle_vbo_, sizeof(kSquareCoods), kSquareCoods,
                       NULL);
  glBindBuffer(GL_ARRAY_BUFFER, rectangle_vbo_);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

  glCreateBuffers(1, &coords_vbo_);
  glNamedBufferStorage(coords_vbo_, sizeof(kSquareCoods), nullptr,
                       GL_DYNAMIC_STORAGE_BIT);
  glBindBuffer(GL_ARRAY_BUFFER, coords_vbo_);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void RenderTree::BlendFBOAndFramebuffer(const Area &area) noexcept {
  auto right_pos_x{area.pos_x + area.width};
  auto upper_pos_y{area.pos_y + area.height};

  GLfloat coords_arr[8];
  coords_arr[0] = static_cast<GLfloat>(area.pos_x) /
                  static_cast<GLfloat>(tree_info_.max_width);
  coords_arr[1] = static_cast<GLfloat>(area.pos_y) /
                  static_cast<GLfloat>(tree_info_.max_height);
  coords_arr[2] = coords_arr[0];
  coords_arr[3] = static_cast<GLfloat>(upper_pos_y) /
                  static_cast<GLfloat>(tree_info_.max_height);
  coords_arr[4] = static_cast<GLfloat>(right_pos_x) /
                  static_cast<GLfloat>(tree_info_.max_width);
  coords_arr[5] = coords_arr[1];
  coords_arr[6] = coords_arr[4];
  coords_arr[7] = coords_arr[3];

  glNamedBufferSubData(coords_vbo_, 0, sizeof(coords_arr), coords_arr);

  glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0,
                         0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // blending rendered blocks and the current block
  glUseProgram(sp_.shader_program);
  glEnable(GL_BLEND);
  glBlendFuncSeparate(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA, GL_ZERO, GL_ZERO);
  glBlendEquation(GL_FUNC_ADD);
  glBindTexture(GL_TEXTURE_2D, texture_);
  glBindVertexArray(vao_);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glEnableVertexAttribArray(0);
  glDisableVertexAttribArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindVertexArray(0);
  glBlendFunc(GL_ONE, GL_ZERO);
  glDisable(GL_BLEND);
  glUseProgram(0);

  glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         texture_, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // copy the result of blending into fbo
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);
  glBlitFramebuffer(area.pos_x, area.pos_y, right_pos_x, upper_pos_y,
                    area.pos_x, area.pos_y, right_pos_x, upper_pos_y,
                    GL_COLOR_BUFFER_BIT, GL_NEAREST);

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void RenderTree::EnableCheckingHover() noexcept {
  check_hover_ = true;
  CheckHover();
}

void RenderTree::DisableCheckingHover() noexcept { check_hover_ = false; }

}  // namespace graphics
