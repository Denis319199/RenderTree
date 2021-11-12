#ifndef EVENTINFO_HPP
#define EVENTINFO_HPP

#include "PDH_GLFW_OpenGL.hpp"

namespace graphics {

enum class MouseButton : unsigned char { kLeft, kMiddle, kRight, kUnknown };

enum class KeyModifier : unsigned char {
  kNone,
  kShift,
  kControl,
  kAlt,
  kCapsLock,
  kNumLock,
  kUnknown
};

enum class Action : unsigned char { kPress, kRelease, kUnknown };

struct MouseButtonEvent {
  MouseButton button;
  KeyModifier mod;
  Action action;
};

inline MouseButtonEvent GetMouseButtonEventFromGLFWEvent(int button, int action,
                                                         int mods) {
  MouseButton button_res;
  switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
      button_res = MouseButton::kLeft;
      break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
      button_res = MouseButton::kMiddle;
      break;
    case GLFW_MOUSE_BUTTON_RIGHT:
      button_res = MouseButton::kRight;
      break;
    default:
      button_res = MouseButton::kUnknown;
  }

  KeyModifier mod_res;
  switch (mods) {
    case 0:
      mod_res = KeyModifier::kNone;
      break;
    case GLFW_MOD_SHIFT:
      mod_res = KeyModifier::kShift;
      break;
    case GLFW_MOD_CONTROL:
      mod_res = KeyModifier::kControl;
      break;
    case GLFW_MOD_ALT:
      mod_res = KeyModifier::kAlt;
      break;
    case GLFW_MOD_CAPS_LOCK:
      mod_res = KeyModifier::kCapsLock;
      break;
    case GLFW_MOD_NUM_LOCK:
      mod_res = KeyModifier::kNumLock;
      break;
    default:
      mod_res = KeyModifier::kUnknown;
  }

  Action action_res;
  switch (action) {
    case GLFW_PRESS:
      action_res = Action::kPress;
      break;
    case GLFW_RELEASE:
      action_res = Action::kRelease;
      break;
    default:
      action_res = Action::kUnknown;
  }

  return {button_res, mod_res, action_res};
}

}  // namespace graphics

#endif  // EVENTINFO_HPP
