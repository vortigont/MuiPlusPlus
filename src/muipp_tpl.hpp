#pragma once
#include <string_view>

using muiItemId = uint32_t;

namespace muipp {

// Unary predicate for Mui's label search matching
template <class T>
class MatchLabel {
  std::string_view _lookup;
public:
  explicit MatchLabel(const char* label) : _lookup(label) {}
  constexpr bool operator() (const T& item ){
      // T is MuiPage
      return _lookup.compare(item.getName()) == 0;
  }
};

// Unary predicate for Mui's ID search matching
template <class T>
class MatchID {
  muiItemId _id;
public:
  explicit MatchID(muiItemId id) : _id(id) {}
  constexpr bool operator() (const T& item ){
      // T is MuiItem_pt
      return item->id == _id;
  }
};

// Unary predicate for Mui's ID search matching
template <class T>
class MatchPageID {
  muiItemId _id;
public:
  explicit MatchPageID(muiItemId id) : _id(id) {}
  constexpr bool operator() (const T& item ){
      // T is MuiPage
      return item.id == _id;
  }
};

// a simple constrain function
template<typename T>
T clamp(T value, T min, T max){
  return (value < min)? min : (value > max)? max : value;
}




} // end of namespace muipp
