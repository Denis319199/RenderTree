#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <iostream>
#include <mutex>

#include "RenderTree.hpp"
#include "Usings.hpp"

namespace graphics {

class Window {
 public:
  static void InitTools() {
    if (!glfwInit()) {
      exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
  }

  static void Terminate() { glfwTerminate(); }

  Window(SizeType width, SizeType height, SizeType max_width,
         SizeType max_height, const char *title) noexcept
      : window_ptr_{}, render_tree_{} {
    windows_vec_.push_back(this);

    window_ptr_ = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window_ptr_) {
      exit(1);
    }
    glfwMakeContextCurrent(window_ptr_);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      std::cout << "Failed to initialize OpenGL context" << std::endl;
      exit(1);
    }

    render_tree_ =
        new RenderTree{{0, 0, width, height}, max_width, max_height, nullptr};

    glfwSetWindowCloseCallback(window_ptr_, WindowCloseCallback);
    glfwSetCursorPosCallback(window_ptr_, CursorPosCallback);
    glfwSetMouseButtonCallback(window_ptr_, MouseButtonCallback);
    glfwSetWindowSizeCallback(window_ptr_, WindowSizeCallback);
    glfwSetScrollCallback(window_ptr_, ScrollCallback);
    glfwSetCursorEnterCallback(window_ptr_, CursorEnterCallback);
    glfwSetWindowSizeLimits(window_ptr_, width, height, max_width, max_height);
  }

  ~Window() {
    auto iter{windows_vec_.begin()};
    auto end_iter{windows_vec_.end()};
    while (iter != end_iter) {
      if ((*iter)->window_ptr_ == window_ptr_) {
        windows_vec_.erase(iter);
        break;
      }
    }

    glfwDestroyWindow(window_ptr_);
  }

  void StartLoop() {
    while (!glfwWindowShouldClose(window_ptr_)) {
      if (render_tree_->Render()) {
        glfwSwapBuffers(window_ptr_);
      }

      glfwPollEvents();
    }
  }

  void InsertAtRoot(TreeBlock *tree_block) {
    render_tree_->InsertAtRoot(tree_block);
  }

  // private: FIXME!
  static void WindowCloseCallback(GLFWwindow *window) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  static void CursorPosCallback(GLFWwindow *window, double pos_x,
                                double pos_y) {
    auto render_tree_ptr{GetWindowObject(window)->render_tree_};
    auto right_pos_y{render_tree_ptr->GetRootArea().height - pos_y};
    render_tree_ptr->ProcessMouseMovement(pos_x, right_pos_y);
  }

  static void MouseButtonCallback(GLFWwindow *window, int button, int action,
                                  int mods) {
    auto button_event{GetMouseButtonEventFromGLFWEvent(button, action, mods)};
    if (button_event.button != MouseButton::kUnknown &&
        button_event.mod != KeyModifier::kUnknown &&
        button_event.action != Action::kUnknown) {
      GetWindowObject(window)->render_tree_->ProcessMouseButton(button_event);
    }
  }

  static void WindowSizeCallback(GLFWwindow *window, int width, int height) {
    GetWindowObject(window)->render_tree_->ChangeArea(
        {0, 0, static_cast<SizeType>(width), static_cast<SizeType>(height)});
  }

  static void ScrollCallback(GLFWwindow *window, double offset_x,
                             double offset_y) {
    GetWindowObject(window)->render_tree_->ProcessMouseScroll(offset_x,
                                                              offset_y);
  }

  static void CursorEnterCallback(GLFWwindow *window, int entered) {
    if (!entered) {
      GetWindowObject(window)->render_tree_->ProcessCursorEnter();
    }
  }

  static Window *GetWindowObject(GLFWwindow *window_ptr) {
    for (auto win_obj : windows_vec_) {
      if (win_obj->window_ptr_ == window_ptr) {
        return win_obj;
      }
    }
  }

  inline static Vector<Window *> windows_vec_{};
  GLFWwindow *window_ptr_;
  RenderTree *render_tree_;
};
}  // namespace graphics

#endif  // WINDOW_HPP
