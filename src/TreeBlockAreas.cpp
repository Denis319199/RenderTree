#include "TreeBlockAreas.hpp"

namespace graphics {

[[nodiscard]] bool Area::operator==(const Area &other) const noexcept {
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

[[nodiscard]] bool Area::operator!=(const Area &other) const noexcept {
  return !(*this == other);
}

[[nodiscard]] bool Area::DoesPointFallWithinArea(SizeType x,
                                                 SizeType y) const noexcept {
  auto max_pos_x{pos_x + width};
  auto max_pos_y{pos_y + height};

  if (x >= pos_x && x <= max_pos_x) {
    if (y >= pos_y && y <= max_pos_y) {
      return true;
    }
  }

  return false;
}
}  // namespace graphics
