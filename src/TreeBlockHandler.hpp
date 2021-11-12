#ifndef TREEBLOCKHANDELER_HPP
#define TREEBLOCKHANDELER_HPP

#include "EventInfo.hpp"
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

  virtual void ProcessMouseMovement(TreeBlock &block) {}

  virtual void ProcessMouseButton(TreeBlock &block) {}

  virtual void ProcessMouseScroll(TreeBlock &block) {}

  virtual void ProcessKey(TreeBlock &block) {}
};

}  // namespace graphics

#endif  // TREEBLOCKHANDELER_HPP
