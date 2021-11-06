#ifndef RENDERTREE_HPP
#define RENDERTREE_HPP

#include "TreeBlock.hpp"
#include "Usings.hpp"

#include <iostream>

namespace graphics {

struct TreeInfo {
  SizeType max_width;  // max window width
  SizeType max_height; // max window height

  SizeType cursor_pos_x;
  SizeType cursor_pos_y;

  PtrDiff scroll_offset_x;
  PtrDiff scroll_offset_y;
};

class RenderTree {
public:
  RenderTree(Area area, SizeType max_width, SizeType max_height,
             TreeBlockHandlerBase *handler)
      : root_{new TreeBlock{handler}}, tree_info_{max_width, max_height},
        is_render_required_{true}, fbo_{}, texture_{}, vao_{}, vbo_{},
        shader_program_{} {
    root_->SetPosX(area.pos_x);
    root_->SetPosY(area.pos_y);
    root_->SetWidth(area.width);
    root_->SetHeight(area.height);
    InitShaders();
    CreateBuffers();
  }

  ~RenderTree() {
    glDeleteFramebuffers(1, &fbo_);
    glDeleteTextures(1, &texture_);
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
    glDeleteProgram(shader_program_);
    glDeleteShader(vs_object_);
    glDeleteShader(fs_object_);
  }

  bool Render() {
    if (is_render_required_) {
      if (root_->handler_ == nullptr) {
        const auto &area{root_->area_};
        glViewport(area.pos_x, area.pos_y, area.width, area.height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                GL_STENCIL_BUFFER_BIT);

        BlendFBOAndFramebuffer(area);

        const auto &children_list{root_->GetChildrenList()};
        for (auto child : children_list) {
          Render(child);
        }
      } else {
        Render(root_);
      }

      // set back full-screen viewport
      const auto &cur_window_area{root_->area_};
      glViewport(cur_window_area.pos_x, cur_window_area.pos_y,
                 cur_window_area.width, cur_window_area.height);

      // copy fbo into default back framebuffer
      glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
      glBlitFramebuffer(cur_window_area.pos_x, cur_window_area.pos_y,
                        cur_window_area.pos_x + cur_window_area.width,
                        cur_window_area.pos_y + cur_window_area.height,
                        cur_window_area.pos_x, cur_window_area.pos_y,
                        cur_window_area.pos_x + cur_window_area.width,
                        cur_window_area.pos_y + cur_window_area.height,
                        GL_COLOR_BUFFER_BIT, GL_NEAREST);
      glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

      is_render_required_ = false;
      return true;
    }

    return false;
  }

  void InsertAtRoot(TreeBlock *block) {
    root_->children_list_.push_back(block);
    block->parent_ = root_;
    block->render_tree_ = this;
    block->SetPosX(block->area_.pos_x);
    block->SetPosY(block->area_.pos_y);
    is_render_required_ = true;
  }

  void ProcessMouseMovement(SizeType pos_x, SizeType pos_y) {
    tree_info_.cursor_pos_x = pos_x;
    tree_info_.cursor_pos_y = root_->area_.height - pos_y;

    root_->ProcessMouseMovement(tree_info_.cursor_pos_x,
                                tree_info_.cursor_pos_y);
    auto &children_list{root_->GetChildrenList()};
    for (auto &child : children_list) {
      ProcessMouseMovement(child);
    }
  }

  void ProcessMouseButton(MouseButtonEvent button_event) {
    last_mouse_button_event_ = button_event;
    ProcessMouseButton(root_);
  }

  void ProcessMouseScroll(PtrDiff offset_x, PtrDiff offset_y) {
    tree_info_.scroll_offset_x = offset_x;
    tree_info_.scroll_offset_y = offset_y;
    ProcessMouseScroll(root_);
  }

  void ChangeArea(Area area) {
    if (auto &root_area{root_->area_}; root_area != area) {
      root_area = area;

      auto &root_children{root_->GetChildrenList()};
      for (auto &child : root_children) {
        ChangeArea(child);
      }

      is_render_required_ = true;
    }
  }

  void KeyEvent() {}

  void RenderIsRequired() { is_render_required_ = true; }

  const TreeInfo &GetTreeInfo() const noexcept { return tree_info_; }

private:
  void ProcessMouseMovement(TreeBlock *block) {
    const auto &area{block->GetArea()};

    block->ProcessMouseMovement(tree_info_.cursor_pos_x,
                                tree_info_.cursor_pos_y);
    block->CheckHover();

    auto &children_list{block->GetChildrenList()};
    for (auto &child : children_list) {
      ProcessMouseMovement(child);
    }
  }

  void ProcessMouseButton(TreeBlock *block) {
    block->ProcessMouseButton(last_mouse_button_event_);

    auto &children_list{block->GetChildrenList()};
    for (auto &child : children_list) {
      ProcessMouseButton(child);
    }
  }

  void ChangeArea(TreeBlock *block) {
    auto res{block->ProcessChangedArea()};

    if (res) {
      auto &block_children{block->GetChildrenList()};
      for (auto &child : block_children) {
        ChangeArea(child);
      }
    }
  }

  void ProcessMouseScroll(TreeBlock *block) {
    block->ProcessMouseScroll(tree_info_.scroll_offset_x,
                              tree_info_.scroll_offset_y);

    auto &block_children{block->GetChildrenList()};
    for (auto &child : block_children) {
      ProcessMouseScroll(child);
    }
  }

  void InitShaders() {
    // preparing vertex shader
    vs_object_ = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs_object_, 1, &vs_code, nullptr);
    glCompileShader(vs_object_);
    GLint compile_status;
    glGetShaderiv(vs_object_, GL_COMPILE_STATUS, &compile_status);
    if (compile_status == GL_FALSE) {
      GLchar info_log[512];
      GLsizei length;
      glGetShaderInfoLog(vs_object_, 512, &length, info_log);
      std::cout << info_log;
      exit(1);
    }

    // preparing fragment shader
    fs_object_ = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs_object_, 1, &fs_code, nullptr);
    glCompileShader(fs_object_);
    glGetShaderiv(fs_object_, GL_COMPILE_STATUS, &compile_status);
    if (compile_status == GL_FALSE) {
      GLchar info_log[512];
      GLsizei length;
      glGetShaderInfoLog(fs_object_, 512, &length, info_log);
      std::cout << info_log;
      exit(1);
    }

    // linking the shaders
    shader_program_ = glCreateProgram();
    glAttachShader(shader_program_, vs_object_);
    glAttachShader(shader_program_, fs_object_);
    glLinkProgram(shader_program_);
    GLint link_status;
    glGetProgramiv(shader_program_, GL_LINK_STATUS, &link_status);
    if (link_status == GL_FALSE) {
      GLchar info_log[512];
      GLsizei length;
      glGetProgramInfoLog(shader_program_, 512, &length, info_log);
      std::cout << info_log;
      exit(1);
    }
  }

  void CreateBuffers() {
    // creates fbo and texture for it
    glUseProgram(shader_program_);

    glCreateFramebuffers(1, &fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tree_info_.max_width,
                 tree_info_.max_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           texture_, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // checks that created fbo is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      exit(1);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    const GLfloat kSquareCoods[]{-1, 1, 1, 1, -1, -1, 1, -1};
    glCreateVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glCreateBuffers(1, &vbo_);
    glNamedBufferStorage(vbo_, sizeof(kSquareCoods), kSquareCoods, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUseProgram(0);
  }

  void Render(TreeBlock *block) {
    if (block->Render()) {
      BlendFBOAndFramebuffer(block->area_);
    }

    const auto &children_list{block->GetChildrenList()};
    for (auto child : children_list) {
      Render(child);
    }
  }

  void BlendFBOAndFramebuffer(const Area &area) {
    // blending the old frame and the current block
    glUseProgram(shader_program_);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glBindVertexArray(vao_);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glBlendFunc(GL_ONE, GL_ZERO);
    glDisable(GL_BLEND);
    glUseProgram(0);

    // copy the result of blending into fbo
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);
    glBlitFramebuffer(area.pos_x, area.pos_y, area.pos_x + area.width,
                      area.pos_y + area.height, area.pos_x, area.pos_y,
                      area.pos_x + area.width, area.pos_y + area.height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  }

  TreeBlock *root_; // points to special window based block
  TreeInfo tree_info_;

  bool is_render_required_;
  MouseButtonEvent last_mouse_button_event_;

  GLuint fbo_;     // buffer is to store result of previous rendering
  GLuint texture_; // texture is attached to fbo
  GLuint vao_;     // vao for the texture rectangle
  GLuint vbo_;     // stores verteces for the texture rectangle
  GLuint vs_object_;
  GLuint fs_object_;
  GLuint shader_program_;

  static constexpr GLfloat kDefaultColor[4]{0.f, 0.f, 0.f, 1.f};

  const char *vs_code = "#version 450 core\n"
                        "layout(location = 0) in vec2 vertex;\n"
                        "void main() { gl_Position = vec4(vertex, 0, 1); }";

  const char *fs_code =
      "#version 450 core\n"
      "uniform sampler2D tex;\n"
      "out vec4 fragment_color;\n"
      "void main() { fragment_color = texture(tex, gl_FragCoord.xy); }";
};

} // namespace graphics

#endif // RENDERTREE_HPP
