#ifndef TREEBLOCKHANDELER_HPP
#define TREEBLOCKHANDELER_HPP

#include "Usings.hpp"

namespace graphics {

class TreeBlock;

class TreeBlockHandlerBase {
protected:
  TreeBlockHandlerBase() {}

public:
  virtual bool Render(const TreeBlock &block) { return false; }

  virtual void ProcessChangedArea(TreeBlock &block) {}

  virtual void ProcessHover(TreeBlock &block) {}

  virtual void ProcessMouseMovement(TreeBlock &block, SizeType pos_x,
                                    SizeType pos_y) {}

  virtual void ProcessMouseButton(TreeBlock &block,
                                  MouseButtonEvent button_event) {}

  virtual void ProcessMouseScroll(TreeBlock &block, PtrDiff offset_x,
                                  PtrDiff offset_y) {}

  virtual void ProcessKey(TreeBlock &block) {}
};

} // namespace graphics

#endif // TREEBLOCKHANDELER_HPP
