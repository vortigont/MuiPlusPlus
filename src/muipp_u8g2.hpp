#pragma once
#include <string>
#include "U8g2lib.h"
#include "muiplusplus.hpp"


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

class Item_U8g2_Generic {
protected:
  U8G2 &_u8g2;
  const uint8_t* _font;
  // item's initial cursor position
  u8g2_uint_t _x, _y;

  // horizontal alignment relative to cursor position
  text_align_t h_align;
  // vertical alignment relative to cursor position
  text_align_t v_align;

public:

  /**
   * @brief Construct a new MuiItem_U8g2_PageTitle object
   * generic object is NOT selectable!
   * @param u8g2 reference to display object
   * @param id assigned id for the item
   * @param font use font for printing, if null, then do not switch font
   * @param x, y Coordinates of the top left corner to start printing
   */
  Item_U8g2_Generic(U8G2 &u8g2, const uint8_t* font = nullptr, u8g2_uint_t x = 0, u8g2_uint_t y = 0, text_align_t halign = text_align_t::left, text_align_t valign = text_align_t::baseline) : _u8g2(u8g2), _font(font), _x(x), _y(y), h_align(halign), v_align(valign) {};

  u8g2_uint_t getX() const { return _x; }

  u8g2_uint_t getY() const { return _y; }

  // adjust cursor position
  void setCursor( u8g2_uint_t x, u8g2_uint_t  y){ x = _x; _y = y; }

  // adjust text alignment
  void setTextAlignment(text_align_t hAlign, text_align_t vAlign){ h_align = hAlign; v_align = vAlign; }

  // same as getXoffset(const char* text), but for arbitrary coordinate and alignmnet

  /**
   * @brief calculate adjusted x cursor position
   * to print provided text acording to text alignment parameters (v_alignm h_align)
   * (it will also adjust curent FontPos according to valign argument)
   * 
   * @param text 
   * @return u8g2_uint_t - adjusted x position to start printing aligned text from
   * 
   */
  u8g2_uint_t getXoffset(u8g2_uint_t x, text_align_t halign, text_align_t valign, const char* text);


  /**
   * @brief calculate adjusted x cursor position for current alignment settings (v_alignm h_align)
   * 
   * @param text 
   * @return u8g2_uint_t - adjusted x position to start printing aligned text from
   * (it will also adjust curent FontPos according to v_align member value)
   */
  u8g2_uint_t getXoffset(const char* text){ return getXoffset(_x, h_align, v_align, text); };

};

/**
 * @brief this item will print current page title at top left corner by default
 * title string will be passed here by MuiPlusPlus class renderer
 * 
 */
class MuiItem_U8g2_PageTitle : public Item_U8g2_Generic, public MuiItem_Uncontrollable {
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
    : Item_U8g2_Generic(u8g2, font, x, y),
      MuiItem_Uncontrollable(id, nullptr) { v_align = text_align_t::top; };

  //~MuiItem_U8g2_PageTitle(){ Serial.println("d-tor PTitle"); }

  void render(const MuiItem* parent) override;
};

class MuiItem_U8g2_StaticText : public Item_U8g2_Generic, public MuiItem_Uncontrollable {
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
    : Item_U8g2_Generic(u8g2, font, x, y),
      MuiItem_Uncontrollable(id, txt) {};

  void render(const MuiItem* parent) override;
};


class MuiItem_U8g2_TextCallBack : public Item_U8g2_Generic, public MuiItem_Uncontrollable {
  string_cb_t _cb;
public:
  /**
   * @brief Construct a new MuiItem_U8g2_PageTitle object
   * 
   * @param u8g2 refernce to display object
   * @param id assigned id for the item
   * @param font use font for printing, if null, then do not switch font
   * @param x, y Coordinates of the top left corner to start printing
   */
  MuiItem_U8g2_TextCallBack(U8G2 &u8g2, muiItemId id, string_cb_t callback,
      const uint8_t* font = nullptr, u8g2_uint_t x = 0, u8g2_uint_t y = 0,
      text_align_t halign = text_align_t::left,
      text_align_t valign = text_align_t::baseline)
        : Item_U8g2_Generic(u8g2, font, x, y),
          MuiItem_Uncontrollable(id), _cb(callback) {};

  void render(const MuiItem* parent) override;
};

/**
 * @brief Action button
 * this element could be focused on a page and on "action" event will generate supplied mui_event
 * with argument provided
 * @note item is focusable
 * @note item is NOT selectable
 */
class MuiItem_U8g2_ActionButton : public Item_U8g2_Generic, public MuiItem {
protected:
  mui_event _action;
public:
  MuiItem_U8g2_ActionButton(
    U8G2 &u8g2, muiItemId id,
    mui_event onAction,                                                           // button action
    const char* lbl,                                                              // button label
    const uint8_t* font = nullptr, u8g2_uint_t x = 0, u8g2_uint_t y = 0,          // look and position
    text_align_t halign = text_align_t::left, text_align_t valign = text_align_t::baseline
    )
    : Item_U8g2_Generic(u8g2, font, x, y, halign, valign),
      MuiItem(id, lbl, {false, false}), _action(onAction) {};

  // render method
  void render(const MuiItem* parent) override;

  // event handler
  mui_event muiEvent(mui_event e) override;
};

/**
 * @brief Back button
 * this element could be focused on a page and on "action" event will generate 'back' event,
 * it will be process by MuiPlusPlus as either switch to previous page or other action if overriden
 * by default it will be placed at down-right corner
 */
class MuiItem_U8g2_BackButton : public MuiItem_U8g2_ActionButton {
public:
  MuiItem_U8g2_BackButton(U8G2 &u8g2, muiItemId id, const char* lbl, const uint8_t* font = nullptr)
    : MuiItem_U8g2_ActionButton(u8g2, id, {mui_event_t::prevPage}, lbl, font, u8g2.getDisplayWidth(), u8g2.getDisplayHeight(), text_align_t::right, text_align_t::bottom) {};
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
class MuiItem_U8g2_DynamicScrollList : public Item_U8g2_Generic, public MuiItem {
  stringbyindex_cb_t _cb;
  size_cb_t _size_cb;
  index_cb_t _action;
  int _y_shift, _num_of_rows;
  const uint8_t *_font2;
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
    : Item_U8g2_Generic(u8g2, font1, x, y),
      MuiItem(id, nullptr), _cb(label_cb), _size_cb(count), _action(action_cb), _y_shift(y_shift), _num_of_rows(num_of_rows), _font2(font2) {};

  // list options
  dynlist_options_t listopts;

  // event handler
  mui_event muiEvent(mui_event e) override;

  void render(const MuiItem* parent) override;
};

/**
 * @brief item draws a boolean checkbox
 * and will call a callback function on action event
 * 
 */
class MuiItem_U8g2_CheckBox : public Item_U8g2_Generic, public MuiItem {
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
    : Item_U8g2_Generic(u8g2, font, x, y),
      MuiItem(id, label, {false, false}),_v(value), _action(action_cb){}

  // render method
  void render(const MuiItem* parent) override;
  // event handler
  mui_event muiEvent(mui_event e) override;
};


template <typename T>
class MuiItem_U8g2_NumberHSlide : public Item_U8g2_Generic, public MuiItem {
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
    T& value,
    T min, T max, T step,
    stringify_cb_t<T> makeString = nullptr,
    constrain_val_cb_t<T> onSelect = nullptr,
    constrain_val_cb_t<T> onDeSelect = nullptr,
    constrain_val_cb_t<T> onChange = nullptr,
    const uint8_t* font1 = nullptr,
    const uint8_t* font2 = nullptr,
    u8g2_uint_t x = 0, u8g2_uint_t y = 0, u8g2_uint_t offset = 20)
    : Item_U8g2_Generic(u8g2, font1, x, y),
      MuiItem(id, label),
      _v(value), _minv(min), _maxv(max), _step(step),
      _mkstring(makeString), _onSelect(onSelect), _onDeSelect(onDeSelect), _onChange(onChange),
      _font2(font2), _offset(offset) { }

  //~MuiItem_U8g2_NumberHSlide(){ Serial.println("d-tor HSlide"); }

  // render method
  void render(const MuiItem* parent) override;
  // event handler
  mui_event muiEvent(mui_event e) override;
};


class MuiItem_U8g2_ValuesList : public Item_U8g2_Generic, public MuiItem {
  // callbacks
  string_cb_t _getCurrent;
  std::function< void (void)> _onNext;
  std::function< void (void)> _onPrev;
  // cursor x position for value 
  u8g2_uint_t _xval;
  // value horizontal alignment
  text_align_t _val_halign;

public:

  MuiItem_U8g2_ValuesList(
    U8G2 &u8g2,
    muiItemId id,
    const char* label,
    string_cb_t getCurrent,
    std::function< void (void)> onNext,
    std::function< void (void)> onPrevious,
    u8g2_uint_t xlbl, u8g2_uint_t xval,
    u8g2_uint_t y,
    const uint8_t* font = nullptr,
    text_align_t lbl = text_align_t::left,
    text_align_t val_halign = text_align_t::left,
    text_align_t valign = text_align_t::baseline
  ) : Item_U8g2_Generic(u8g2, font, xlbl, y, lbl, valign),
      MuiItem(id, label), _getCurrent(getCurrent), _onNext(onNext), _onPrev(onPrevious), _xval(xval), _val_halign(val_halign) {}

  // render method
  void render(const MuiItem* parent) override;
  // event handler
  mui_event muiEvent(mui_event e) override;
};

// ***********************************

//    templates implementations

// ***********************************
template <typename T>
void MuiItem_U8g2_NumberHSlide<T>::render(const MuiItem* parent){
  _u8g2.setFontPosCenter();
  std::string val_str(_mkstring ? _mkstring(_v) : std::to_string(_v) );
  if (_font)
    _u8g2.setFont(_font);
  auto vlen = _u8g2.getStrWidth(val_str.data());

  // check if prev value step is valid, then print it
  if ( (_minv != _maxv) && ((_v - _step) >= _minv) ){
    if (_font2)
      _u8g2.setFont(_font2);

    std::string pval_str(_mkstring ? _mkstring(_v - _step) : std::to_string(_v - _step));
    auto prevx = _x - vlen/2 - _offset - _u8g2.getStrWidth(pval_str.data());
    _u8g2.setCursor(prevx, _y);
    _u8g2.print(pval_str.data());
  }

  // print value string
  if (_font)
    _u8g2.setFont(_font);

  _u8g2.drawButtonUTF8(_x - vlen/2, _y, U8G2_BTN_INV, 0, 1, 1, val_str.data());


  // check if next value step is valid, then print it
  if ( (_minv != _maxv) && (_v + _step <= _maxv)){
    if (_font2)
      _u8g2.setFont(_font2);
    _u8g2.setCursor(_x + vlen/2 + _offset, _y);
    std::string nval_str( _mkstring ? _mkstring(_v + _step) : std::to_string(_v + _step));
    _u8g2.print(nval_str.data());
  }
}

template <typename T>
mui_event MuiItem_U8g2_NumberHSlide<T>::muiEvent(mui_event e){
  switch(e.eid){
    // cursor actions - incr/decr value
    case mui_event_t::moveUp :
    case mui_event_t::moveLeft : {
      _v = (_minv != _maxv) ? muipp::clamp(_v - _step, _minv, _maxv) : _v - _step;
      break;
    }
    // cursor actions - incr/decr value
    case mui_event_t::moveDown :
    case mui_event_t::moveRight : {
      _v = (_minv != _maxv) ? muipp::clamp(_v + _step, _minv, _maxv) : _v + _step;
      break;
    }

    // accept raw integer value (todo: make cast for float)
    case mui_event_t::value : {
      _v = (_minv != _maxv) ? muipp::clamp(static_cast<T>(e.param), _minv, _maxv) : e.param;
      break;
    }

    // enter acts as escape to release selection
    case mui_event_t::enter : {
      return mui_event(on_escape);
      break;
    }
  }

  // no-op
  return {};
}
