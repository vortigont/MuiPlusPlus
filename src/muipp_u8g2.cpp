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

void MuiItem_U8g2_StaticText::render(const MuiItem* parent){
  if (_font)
    _u8g2.setFont(_font);

  _u8g2.setCursor(_x, _y);
  // specified cursor position will be the top reference for the Title text
  _u8g2.setFontPosTop();
  _u8g2.drawUTF8(_x, _y, _text);
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


//template <typename Iter>
//void listwriter (Iter it, Iter end) {
//    for (; it!=end; ++it) { /*...*/ }
//}

void MuiItem_U8g2_DynamicScrollList::render(const MuiItem* parent){
  Serial.printf("Print list of %u items\n", _size_cb());

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
      // if dynamic list works as page seletor, we will stich pages here
      if (listopts.page_selector){
        // last item in a list acts as "go to previous page"
        if (listopts.back_on_last && (_index == _size_cb() - 1))
          return mui_event(mui_event_t::prevPage);
        else  // otherwise - switch to page by it's label
          return mui_event(mui_event_t::goPageByName, 0, static_cast<void*>(const_cast<char*>(_cb(_index))) );
      }
      // otherwise we call a callback function to decide next event action
      if (_action)
        _action(_index);
      break;
    }

    // all other events pass back to menuNavigator
    default:
      return e;  
  }

  return {};
}

//  ***
//  MuiItem_U8g2_CheckBox

void MuiItem_U8g2_CheckBox::render(const MuiItem* parent){
  _u8g2.setCursor(_x, _y);
  if (_font)
    _u8g2.setFont(_font);
  // specified cursor position will be the top reference for the Title text
  _u8g2.setFontPosBottom();

  int8_t a = _u8g2.getAscent();
  _u8g2.drawFrame(_x, _y-a, a, a);
  // draw mark in a checkbox if value is true
  if (_v){
    a -= 4;
    _u8g2.drawBox(_x + 2, _y-a-2, a, a);
    a += 4;   // not sure how this alignment works :(
  }

  u8g2_int_t w{0};
  if (getName()){
    w = _u8g2.getUTF8Width(getName());
    a += 4;       /* add gap between the checkbox and the text area */
    _u8g2.drawUTF8(_x + a, _y, getName());
  }

  u8g2_DrawButtonFrame(_u8g2.getU8g2(), _x, _y, focused ? U8G2_BTN_INV : 0, w+a, 1, 1);
}

mui_event MuiItem_U8g2_CheckBox::muiEvent(mui_event e){
  switch(e.eid){
    case mui_event_t::enter : {
      _v = !_v;
      // call a callback function to submit a new box value
      if (_action)
        _action(_v);
      break;
    }

    // all other events pass back to menuNavigator
    default:
      return e;
  }
  return {};
}

