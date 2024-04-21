#pragma once
#include "U8g2lib.h"
#include "muiplusplus.hpp"

/**
 * @brief this item print current page title
 * title tring will be passed here by MuiPlusPlus class renderer
 * 
 */
class MuiItem_U8g2_PageTitle : public MuiItem {
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
  MuiItem_U8g2_PageTitle(U8G2 &u8g2, muiItem_id id, const uint8_t* font = nullptr, u8g2_uint_t x = 0, u8g2_uint_t y = 0)
    : MuiItem(id, nullptr, {false, false}), _u8g2(u8g2), _font(font), _x(x), _y(y) {};

  void render(const MuiItem* parent) override;
};


using dynlist_callback_t = std::function< const char* (size_t index)>;


class MuiItem_U8g2_DynamicList : public MuiItem {
  U8G2 &_u8g2;
  dynlist_callback_t _cb;
  size_t _len;
  int _y_shift, _num_of_rows;
  const uint8_t *_font1, *_font2;
  u8g2_uint_t _x, _y;
  // current list index
  int _index{0};
  //template <typename Iter>
  //void listwriter (Iter it, Iter end) {
  //    for (; it!=end; ++it) { /*...*/ }
  //}

public:
  /**
   * @brief Construct a new MuiItem_U8g2_PageTitle object
   * 
   * @param u8g2 refernce to display object
   * @param id assigned id for the item
   * @param font use font for printing, if null, then do not switch font
   * @param y_shift pexels to shift on Y for every new row
   * @param num_of_rows rows to display
   * @param font1 font to use for highlighted row, do not change if null
   * @param font2 font to use for other rows, same as font1 if null
   * @param x, y Coordinates of the top left corner to start printing
   */
  MuiItem_U8g2_DynamicList(U8G2 &u8g2,
      muiItem_id id,
      dynlist_callback_t callback,
      size_t count,
      int y_shift,
      int num_of_rows = 3,
      const uint8_t* font1 = nullptr,
      const uint8_t* font2 = nullptr,
      u8g2_uint_t x = 0, u8g2_uint_t y = 0)
    : MuiItem(id, nullptr), _u8g2(u8g2), _cb(callback), _len(count), _y_shift(y_shift), _num_of_rows(num_of_rows), _font1(font1), _font2(font2), _x(x), _y(y) {};

  // event handler
  mui_event muiEvent(mui_event e) override;

  void render(const MuiItem* parent) override;
};
