#ifndef TREEBLOCKAREAS_HPP
#define TREEBLOCKAREAS_HPP

#include "Usings.hpp"

namespace graphics {

struct Area {
  SizeType pos_x;
  SizeType pos_y;
  SizeType width;
  SizeType height;

  [[nodiscard]] bool operator==(const Area &other) const noexcept;

  [[nodiscard]] bool operator!=(const Area &other) const noexcept;

  [[nodiscard]] bool DoesPointFallWithinArea(SizeType x,
                                             SizeType y) const noexcept;
};

struct NormalizedArea {
  float pos_x;
  float pos_y;
  float width;
  float height;
};
}  // namespace graphics

#endif  // TREEBLOCKAREAS_HPP
