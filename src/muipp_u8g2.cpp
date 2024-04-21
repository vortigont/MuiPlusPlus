//#include "stdint.h"
#include "muipp_u8g2.h"
#include "Arduino.h"

// **************************
// MUI functions

//extern U8G2_SH1107_64X128_F_HW_I2C u8g2;

void MuiItem_U8g2_PageTitle::render(const MuiItem* parent){
  Serial.printf("Print header %s\n", parent->getName());

  if (_font)
    _u8g2.setFont(_font);

  _u8g2.setCursor(_x, _y);
  // specified cursor position will be the top reference for the Title text
  _u8g2.setFontPosTop();
  _u8g2.print(parent->getName());
}

void MuiItem_U8g2_DynamicList::render(const MuiItem* parent){
  Serial.println("Print list");

//  if (_font)
  //  _u8g2.setFont(_font);

  _u8g2.setCursor(_x, _y);
  // specified cursor position will be the top reference for the Title text
  _u8g2.setFontPosTop();

  // normalize index position which items should be displayed
  int visible_idx = clamp(_index - _num_of_rows/2, 0, static_cast<int>(_len-1) );
  for (int i = 0; i != _num_of_rows; ++i){
    // change font for active/inactive row
    if (visible_idx == _index && _font1)
      _u8g2.setFont(_font1);
    else if (_font2)
      _u8g2.setFont(_font2);

    // draw button
    _u8g2.drawButtonUTF8(_x, _y + _y_shift*i, visible_idx == _index ? U8G2_BTN_INV : 0, 0, 1, 1, _cb(visible_idx));

    if (++visible_idx >= _len)
      return;
  }

/*
  // print previous item
  if (_font2)
    _u8g2.setFont(_font2);
  _u8g2.print(_cb(_index ? _index-1 : _index));
  //_u8g2.setCursor(_x, _y + _y_shift);

  if (_font1)
    _u8g2.setFont(_font1);

  _u8g2.drawButtonUTF8(_x, _y + _y_shift, U8G2_BTN_INV, 0, 1, 1, _cb(_index));
  //  drawButtonUTF8(u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t flags, u8g2_uint_t width, u8g2_uint_t padding_h, u8g2_uint_t padding_v, const char *text)
  //_u8g2.print(_cb(_index) );

  if (_index+1 != _len){
    _u8g2.setCursor(_x, _y + 2*_y_shift);
    if (_font2)
      _u8g2.setFont(_font2);
    _u8g2.print(_cb(_index + 1));
  }
*/
}

mui_event MuiItem_U8g2_DynamicList::muiEvent(mui_event e) {
  Serial.printf("DynamicList event:%u\n", static_cast<uint32_t>(e.eid));
  switch(e.eid){
    // cursor actions - move to previous position in a list
    case mui_event_t::moveUp :
    case mui_event_t::moveLeft : {
      if (_index)
        --_index;
      else
        _index = _len - 1;
      break;
    }
    // cursor actions - move to next position in a list
    case mui_event_t::moveDown :
    case mui_event_t::moveRight : {
      ++_index;
      if (_index == _len )
        _index = 0;
      break;
    }

/*
    case mui_event_t::enter : {
      if (item_active && focusedItem != items.end()){
        return _feedback_event((*focusedItem)->muiEvent(e));
      }
      // otherwise pass to navigation controls
      return _menu_navigation(e);
    }
*/
  }

  return {};
}



