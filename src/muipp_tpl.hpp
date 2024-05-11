#pragma once

using muiItemId = uint32_t;


// Unary predicate for Mui's label search matching
template <class T>
class MatchLabel : public std::unary_function<T, bool>{
  std::string_view _lookup;
public:
  explicit MatchLabel(const char* label) : _lookup(label) {}
  bool operator() (const T& item ){
      // T is MuiPage
      return _lookup.compare(item.getName()) == 0;
  }
};

// Unary predicate for Mui's ID search matching
template <class T>
class MatchID : public std::unary_function<T, bool>{
  muiItemId _id;
public:
  explicit MatchID(muiItemId id) : _id(id) {}
  bool operator() (const T& item ){
      // T is MuiItem_pt
      return item->id == _id;
  }
};

// Unary predicate for Mui's ID search matching
template <class T>
class MatchPageID : public std::unary_function<T, bool>{
  muiItemId _id;
public:
  explicit MatchPageID(muiItemId id) : _id(id) {}
  bool operator() (const T& item ){
      // T is MuiPage
      return item.id == _id;
  }
};

// a simple constrain function
template<typename T>
T clamp(T value, T min, T max){
  return (value < min)? min : (value > max)? max : value;
}
