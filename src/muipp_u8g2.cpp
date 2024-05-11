//#include "stdint.h"
#include "muipp_u8g2.h"
#include "Arduino.h"

// **************************
// MUI functions

//extern U8G2_SH1107_64X128_F_HW_I2C u8g2;

void MuiItem_U8g2_PageTitle::render(const MuiItem* parent){
  if (_font)
    _u8g2.setFont(_font);


  _u8g2.setCursor(_x, _y);
  // specified cursor position will be the top reference for the Title text
  _u8g2.setFontPosTop();
  _u8g2.print(parent->getName());
}


void MuiItem_U8g2_BackButton::render(const MuiItem* parent){
  if (_font)
    _u8g2.setFont(_font);

  // draw button
  _u8g2.drawButtonUTF8(_x, _y, focused ? U8G2_BTN_INV : 0, 0, 1, 1, name);

  //_u8g2.setCursor(_x, _y);
  // specified cursor position will be the top reference for the Title text
  //_u8g2.setFontPosTop();
  //_u8g2.print(name);
}


mui_event MuiItem_U8g2_BackButton::muiEvent(mui_event e){
  switch(e.eid){
    // actions 'focus' and 'enter' will trigger 'back' event
    case mui_event_t::select :
    case mui_event_t::enter :
      Serial.println("BackButton");
      return mui_event(mui_event_t::prevPage);
  }
  return {};
}


void MuiItem_U8g2_DynamicScrollList::render(const MuiItem* parent){
  Serial.printf("Print list of %u items\n", _size_cb());

//  if (_font)
  //  _u8g2.setFont(_font);

  _u8g2.setCursor(_x, _y);
  // specified cursor position will be the top reference for the Title text
  _u8g2.setFontPosTop();

  // normalize index position which items should be displayed
  int visible_idx = clamp(_index - _num_of_rows/2, 0, static_cast<int>(_size_cb()-1) );
  for (int i = 0; i != _num_of_rows; ++i){
    // change font for active/inactive row
    if (visible_idx == _index && _font1)
      _u8g2.setFont(_font1);
    else if (_font2)
      _u8g2.setFont(_font2);

    // draw button
    Serial.printf("btn %u:%s\n", visible_idx, _cb(visible_idx));
    _u8g2.drawButtonUTF8(_x, _y + _y_shift*i, visible_idx == _index ? U8G2_BTN_INV : 0, 0, 1, 1, _cb(visible_idx));

    if (++visible_idx >= _size_cb())
      return;
  }
}


mui_event MuiItem_U8g2_DynamicScrollList::muiEvent(mui_event e){
  Serial.printf("DynamicScrollList::muiEvent %u\n", static_cast<uint32_t>(e.eid));
  switch(e.eid){
    // cursor actions - move to previous position in a list
    case mui_event_t::moveUp :
    case mui_event_t::moveLeft : {
      if (_index)
        --_index;
      else
        _index = _size_cb() - 1;
      break;
    }
    // cursor actions - move to next position in a list
    case mui_event_t::moveDown :
    case mui_event_t::moveRight : {
      ++_index;
      if (_index == _size_cb() )
        _index = 0;
      break;
    }

    case mui_event_t::enter : {
      if (listopts.page_selector)
        return mui_event(mui_event_t::goPageByName, 0, static_cast<void*>(const_cast<char*>(_cb(_index))) );
      if (_action)
        _action(_index);
      break;
    }
  }
  return {};
}



