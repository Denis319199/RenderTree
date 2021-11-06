#ifndef USINGS_HPP

#include <list>
#include <vector>
#include <string>

namespace graphics {
using SizeType = std::size_t;
using PtrDiff = std::ptrdiff_t;

template <class T>
using List = std::list<T>;

template <class T>
using Vector = std::vector<T>;

using String = std::string;
}  // namespace graphics

#endif  // USINGS_HPP
