#pragma once
#include "U8g2lib.h"
#include "muiplusplus.hpp"

// callback function that returns index size
using size_cb_t = std::function< size_t (void)>;
// callback function that accepts index value
using index_cb_t = std::function< void (size_t index)>;
// callback function that accepts index and returns const char* string associated with index
using stringbyindex_cb_t = std::function< const char* (size_t index)>;
// callback function for constrained numeric
template <typename T>
using constrain_val_cb_t = std::function< void (muiItemId id, T value, T min, T max, T step)>;
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

class MuiItem_U8g2_Generic : public MuiItem {
protected:
  U8G2 &_u8g2;
  const uint8_t* _font;
  u8g2_uint_t _x, _y;

public:
  /**
   * @brief Construct a new MuiItem_U8g2_PageTitle object
   * generic object is NOT selectable!
   * @param u8g2 reference to display object
   * @param id assigned id for the item
   * @param font use font for printing, if null, then do not switch font
   * @param x, y Coordinates of the top left corner to start printing
   */
  MuiItem_U8g2_Generic(U8G2 &u8g2, muiItemId id, const char* label, const uint8_t* font = nullptr, u8g2_uint_t x = 0, u8g2_uint_t y = 0)
    : MuiItem(id, label, {false, false}), _u8g2(u8g2), _font(font), _x(x), _y(y) {};
};

/**
 * @brief this item print current page title
 * title string will be passed here by MuiPlusPlus class renderer
 * 
 */
class MuiItem_U8g2_PageTitle : public MuiItem_Uncontrollable {
  U8G2 &_u8g2;
  const uint8_t* _font;
  u8g2_uint_t _x, _y;
public:
  /**
   * @brief Construct a new MuiItem_U8g2_PageTitle object
   * 
   * @param u8g2 refernce to display object
   * @param id assigned id for the item
   * @param font use font for printing, if null, then do not switch font
   * @param x, y Coordinates of the top left corner to start printing
   */
  MuiItem_U8g2_PageTitle(U8G2 &u8g2, muiItemId id, const uint8_t* font = nullptr, u8g2_uint_t x = 0, u8g2_uint_t y = 0)
    : MuiItem_Uncontrollable(id, nullptr, {false, true}), _u8g2(u8g2), _font(font), _x(x), _y(y) { Serial.println("c-tor PTitle"); };

  ~MuiItem_U8g2_PageTitle(){ Serial.println("d-tor PTitle"); }

  void render(const MuiItem* parent) override;
};

class MuiItem_U8g2_StaticText : public MuiItem_Uncontrollable {
  U8G2 &_u8g2;
  const uint8_t* _font;
  const char* _text;
  u8g2_uint_t _x, _y;
public:
  /**
   * @brief Construct a new MuiItem_U8g2_PageTitle object
   * 
   * @param u8g2 refernce to display object
   * @param id assigned id for the item
   * @param font use font for printing, if null, then do not switch font
   * @param x, y Coordinates of the top left corner to start printing
   */
  MuiItem_U8g2_StaticText(U8G2 &u8g2, muiItemId id, const char* txt, const uint8_t* font = nullptr, u8g2_uint_t x = 0, u8g2_uint_t y = 0)
    : MuiItem_Uncontrollable(id, nullptr, {false, true}), _u8g2(u8g2), _text(txt), _font(font), _x(x), _y(y) {};

  // vertical alignment relative to cursor position
  text_align_t v_align{};
  // horizontal alignment relative to cursor position
  text_align_t h_align{};

  void render(const MuiItem* parent) override;
};


class MuiItem_U8g2_BackButton : public MuiItem_U8g2_Generic {
public:
  /**
   * @brief Construct a new MuiItem_U8g2_PageTitle object
   * 
   * @param u8g2 reference to display object
   * @param id assigned id for the item
   * @param font use font for printing, if null, then do not switch font
   * @param x, y Coordinates of the top left corner to start printing
   */
  using MuiItem_U8g2_Generic::MuiItem_U8g2_Generic;

  // render method
  void render(const MuiItem* parent) override;
  // event handler
  mui_event muiEvent(mui_event e) override;
};


struct dynlist_options_t {
  // switch pages on action
  bool page_selector{false};
  // last element of a list acts as 'previous page' event
  bool back_on_last{false};
};

/**
 * @brief Dynamic List element creates a vertical list
 * of text labels that could scrolled up/down, one of list labels could be selected
 * and execute assigned callback with an index number of that item
 * selected item is displayed inverted
 * -------
 * label 1
 * label 2
 * *label3*
 * label4
 * -------
 * 
 * @note if 'opts.page_selector' flag is set, then list will act as a page switcher,
 * i.e. on 'enter' event it will try to switch to a page with name matching to selected list label
 * @note if 'opts.back_on_last' flag is set, then last element of a list will act 'back' event and switch to a previous page
 * 
 */
class MuiItem_U8g2_DynamicScrollList : public MuiItem {
  U8G2 &_u8g2;
  stringbyindex_cb_t _cb;
  size_cb_t _size_cb;
  index_cb_t _action;
  int _y_shift, _num_of_rows;
  u8g2_uint_t _x, _y;
  const uint8_t *_font1, *_font2;
  // current list index
  int _index{0};

public:
  /**
   * @brief Construct a MuiItem_U8g2_DynamicScrollList object
   * 
   * @param u8g2 reference to display object
   * @param id assigned id for the item
   * @param label_cb function that returns const char* for the specified label index
   * @param action_cb function called with index of an active element in a list when "enter" event is received
   * @param count total number of lables in a list
   * @param y_shift pexels to shift on Y axis for every new row
   * @param num_of_rows max rows to display at once
   * @param font1 font to use for highlighted row, do not change if null
   * @param font2 font to use for other rows, same as font1 if null
   * @param x, y Coordinates of the top left corner to start printing
   */
  MuiItem_U8g2_DynamicScrollList(U8G2 &u8g2,
      muiItemId id,
      stringbyindex_cb_t label_cb,
      size_cb_t count,
      index_cb_t action_cb,
      int y_shift,
      int num_of_rows = 3,
      u8g2_uint_t x = 0, u8g2_uint_t y = 0,
      const uint8_t* font1 = nullptr,
      const uint8_t* font2 = nullptr
  )
    : MuiItem(id, nullptr), _u8g2(u8g2), _cb(label_cb), _size_cb(count), _action(action_cb), _y_shift(y_shift), _num_of_rows(num_of_rows), _font1(font1), _font2(font2), _x(x), _y(y) {};

  // list options
  dynlist_options_t listopts;

  // event handler
  mui_event muiEvent(mui_event e) override;

  void render(const MuiItem* parent) override;
};


class MuiItem_U8g2_CheckBox : public MuiItem_U8g2_Generic {
  // checkbox value
  bool _v;
  index_cb_t _action;
public:
  /**
   * @brief Construct a new MuiItem_U8g2_PageTitle object
   * 
   * @param u8g2 reference to display object
   * @param id assigned id for the item
   * @param font use font for printing, if null, then do not switch font
   * @param x, y Coordinates of the top left corner to start printing
   */
  MuiItem_U8g2_CheckBox(U8G2 &u8g2, muiItemId id, const char* label, bool value, index_cb_t action_cb = nullptr, const uint8_t* font = nullptr, u8g2_uint_t x = 0, u8g2_uint_t y = 0)
    : MuiItem_U8g2_Generic(u8g2, id, label, font, x, y), _v(value), _action(action_cb){}  // { setSelectable(false); }

  // render method
  void render(const MuiItem* parent) override;
  // event handler
  mui_event muiEvent(mui_event e) override;
};


template <typename T>
class MuiItem_U8g2_NumberHSlide : public MuiItem_U8g2_Generic {
  // integer value
  //const char* _format;
  T& _v;
  T _minv, _maxv, _step;
  stringify_cb_t<T>     _mkstring;
  constrain_val_cb_t<T> _onSelect;
  constrain_val_cb_t<T> _onDeSelect;
  constrain_val_cb_t<T> _onChange;
  const uint8_t* _font2;   // font for minor values
  u8g2_uint_t _offset;

  void _rndr_unformatted(const MuiItem* parent);
  void _rndr_formatted(const MuiItem* parent);

public:
  /**
   * @brief Construct a new MuiItem_U8g2_NumberHSlide object
   * 
   * @param u8g2 display object
   * @param id item id
   * @param label text label
   * @param value reference to adjustment variable
   * @param min constrain min (set min=max to disable constrain)
   * @param max constrain min (set min=max to disable constrain)
   * @param step change step
   * @param onSelect - callback function to call when Item is selected
   * @param onDeSelect - callback function to call when Item is deselected
   * @param onChange - callback function to call when Item is changed
   * @param font 
   * @param x 
   * @param y 
   */
  MuiItem_U8g2_NumberHSlide(
    U8G2 &u8g2,
    muiItemId id,
    const char* label,
    //const char* format,
    T& value,
    T min, T max, T step,
    stringify_cb_t<T> makeString = nullptr,
    constrain_val_cb_t<T> onSelect = nullptr,
    constrain_val_cb_t<T> onDeSelect = nullptr,
    constrain_val_cb_t<T> onChange = nullptr,
    const uint8_t* font1 = nullptr,
    const uint8_t* font2 = nullptr,
    u8g2_uint_t x = 0, u8g2_uint_t y = 0, u8g2_uint_t offset = 20)
    : MuiItem_U8g2_Generic(u8g2, id, label, font1, x, y), _v(value), _minv(min), _maxv(max), _step(step),
      _mkstring(makeString), _onSelect(onSelect), _onDeSelect(onDeSelect), _onChange(onChange),
      _font2(font2), _offset(offset) { setSelectable(true); Serial.println("c-tor HSlide"); }

  ~MuiItem_U8g2_NumberHSlide(){ Serial.println("d-tor HSlide"); }

  // render method
  void render(const MuiItem* parent) override { if (_mkstring)  _rndr_formatted(parent); else  _rndr_unformatted(parent);  };
  // event handler
  mui_event muiEvent(mui_event e) override;
};



// ***********************************

//    templates implementations

// ***********************************
template <typename T>
void MuiItem_U8g2_NumberHSlide<T>::_rndr_unformatted(const MuiItem* parent){
  _u8g2.setCursor(_x - _offset, _y);
  if (_font2)
    _u8g2.setFont(_font2);

  _u8g2.print(_v - _step);

  _u8g2.setCursor(_x, _y);
  if (_font)
    _u8g2.setFont(_font);

  _u8g2.print(_v);

  _u8g2.setCursor(_x + _offset, _y);
  if (_font2)
    _u8g2.setFont(_font2);

  _u8g2.print(_v + _step);
}

template <typename T>
void MuiItem_U8g2_NumberHSlide<T>::_rndr_formatted(const MuiItem* parent){
  _u8g2.setFontPosCenter();
  std::string val_str(_mkstring(_v));
  if (_font)
    _u8g2.setFont(_font);
  auto vlen = _u8g2.getStrWidth(val_str.data());

  // check if prev value step is valid, then print it
  if ( (_minv != _maxv) && ((_v - _step) >= _minv) ){
    if (_font2)
      _u8g2.setFont(_font2);

    std::string pval_str(_mkstring(_v - _step));
    auto prevx = _x - vlen/2 - _offset - _u8g2.getStrWidth(pval_str.data());
    _u8g2.setCursor(prevx, _y);
    _u8g2.print(pval_str.data());
  }

  // print value string
  if (_font)
    _u8g2.setFont(_font);
  //_u8g2.setCursor(_x - vlen/2, _y);
  _u8g2.drawButtonUTF8(_x - vlen/2, _y, U8G2_BTN_INV, 0, 1, 1, val_str.data());
  //_u8g2.print(val_str.data());

  // check if next value step is valid, then print it
  if ( (_minv != _maxv) && (_v + _step <= _maxv)){
    if (_font2)
      _u8g2.setFont(_font2);
    _u8g2.setCursor(_x + vlen/2 + _offset, _y);
    std::string nval_str(_mkstring(_v + _step));
    _u8g2.print(nval_str.data());
  }
}

template <typename T>
mui_event MuiItem_U8g2_NumberHSlide<T>::muiEvent(mui_event e){
  Serial.printf("NumberHSlide::muiEvent %u\n", static_cast<uint32_t>(e.eid));
  switch(e.eid){
    // cursor actions - incr/decr value
    case mui_event_t::moveUp :
    case mui_event_t::moveLeft : {
      _v = (_minv != _maxv) ? clamp(_v - _step, _minv, _maxv) : _v - _step;
      break;
    }
    // cursor actions - incr/decr value
    case mui_event_t::moveDown :
    case mui_event_t::moveRight : {
      _v = (_minv != _maxv) ? clamp(_v + _step, _minv, _maxv) : _v + _step;
      break;
    }

    // accept raw integer value (todo: make cast for float)
    case mui_event_t::value : {
      _v = (_minv != _maxv) ? clamp(static_cast<T>(e.param), _minv, _maxv) : e.param;
      break;
    }

    // enter acts as escape to release selection
    case mui_event_t::enter : {
      return mui_event(on_escape);
      break;
    }

    // all other events pass back to menuNavigator
    default:
      return e;  
  }

  return {};
}

