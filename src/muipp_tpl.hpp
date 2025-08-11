#pragma once
#include <functional>
#include <string_view>

using muiItemId = uint32_t;

namespace muipp {

// callback function that returns index size
using size_cb_t = std::function< size_t (void)>;
// callback function that accepts index value
using index_cb_t = std::function< void (size_t index)>;
// callback that just returns string
using string_cb_t = std::function< const char* (void)>;
// callback function that accepts index and returns const char* string associated with index
using stringbyindex_cb_t = std::function< const char* (size_t index)>;
// callback function for constrained numeric
template <typename T>
using constrain_val_cb_t = std::function< void (muiItemId id, T value, T min, T max, T step)>;
// stringifying function, it accepts some object value and returns a srting that identifies the value (i.e. convert int to asci, etc...)
template <typename T>
using stringify_cb_t = std::function< std::string (T value)>;

enum class text_align_t {
  baseline = 0,
  center,
  top,
  bottom,
  left,
  right
};


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
