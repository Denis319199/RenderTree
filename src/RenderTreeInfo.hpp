#ifndef RENDERTREEINFO_HPP
#define RENDERTREEINFO_HPP

#include "EventInfo.hpp"
#include "Usings.hpp"

namespace graphics {

struct MouseInfo {
  PtrDiff cursor_pos_x;
  PtrDiff cursor_pos_y;

  PtrDiff cursor_pos_x_diff;
  PtrDiff cursor_pos_y_diff;

  PtrDiff scroll_offset_x;
  PtrDiff scroll_offset_y;

  MouseButtonEvent last_mouse_button_event_;
};

struct TreeInfo {
  const SizeType max_width;   // max window width
  const SizeType max_height;  // max window height

  MouseInfo mouse_info;
};

}  // namespace graphics

#endif  // RENDERTREEINFO_HPP
