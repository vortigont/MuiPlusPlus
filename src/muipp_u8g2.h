#pragma once
#include "U8g2lib.h"
#include "muiplusplus.hpp"

// callback function that returns index size
using size_cb_t = std::function< size_t (void)>;
// callback function that accepts index value
using index_cb_t = std::function< void (size_t index)>;
// callback function that acppets index and returns const char* string associated with index
using stringbyindex_cb_t = std::function< const char* (size_t index)>;



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
    : MuiItem_Uncontrollable(id, nullptr, {false, false}), _u8g2(u8g2), _font(font), _x(x), _y(y) {};

  void render(const MuiItem* parent) override;
};


class MuiItem_U8g2_BackButton : public MuiItem {
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
  MuiItem_U8g2_BackButton(U8G2 &u8g2, muiItemId id, const char* label, const uint8_t* font = nullptr, u8g2_uint_t x = 0, u8g2_uint_t y = 0)
    : MuiItem(id, label, {true, false}), _u8g2(u8g2), _font(font), _x(x), _y(y) {};

  // render method
  void render(const MuiItem* parent) override;
  // event handler
  mui_event muiEvent(mui_event e) override;
};


struct dynlist_options_t {
  // switch pages on action
  bool page_selector{false};
  // last element of a list acts as 'previous' event
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
  //template <typename Iter>
  //void listwriter (Iter it, Iter end) {
  //    for (; it!=end; ++it) { /*...*/ }
  //}

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

