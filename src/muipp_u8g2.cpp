#include "muipp_u8g2.hpp"
#include "Arduino.h"

/*
  display a string on multiple text lines, keeping words intact where possible, and accepting \n to force a new line
  _Originally posted by @LenShustek in https://github.com/olikraus/u8g2/discussions/1479#discussioncomment-6242771_
*/
void printwords(U8G2 &u8g2, const char *msg, int xloc, int yloc /*bottom*/ ) {
   int dspwidth = u8g2.getDisplayWidth(); // display width in pixels
   int strwidth = 0;  // string width in pixels
   char glyph[2]; glyph[1] = 0;
   for (const char *ptr = msg, *lastblank = NULL; *ptr; ++ptr) {
      while (xloc == 0 && (*msg == ' ' || *msg == '\n'))
         if (ptr == msg++) ++ptr; // skip blanks and newlines at the left edge
      glyph[0] = *ptr;
      strwidth += u8g2.getStrWidth(glyph); // accumulate the pixel width
      if (*ptr == ' ')  lastblank = ptr; // remember where the last blank was
      else ++strwidth; // non-blanks will be separated by one additional pixel
      if (*ptr == '\n' ||   // if we found a newline character,
            xloc + strwidth > dspwidth) { // or if we ran past the right edge of the display
         int starting_xloc = xloc;
         // print to just before the last blank, or to just before where we got to
         while (msg < (lastblank ? lastblank : ptr)) {
            glyph[0] = *msg++;
            xloc += u8g2.drawStr(xloc, yloc, glyph); }
         strwidth -= xloc - starting_xloc; // account for what we printed
         yloc += u8g2.getMaxCharHeight(); // advance to the next line
         xloc = 0; lastblank = NULL; } }
   while (*msg) { // print any characters left over
      glyph[0] = *msg++;
      xloc += u8g2.drawStr(xloc, yloc, glyph); } 
}



u8g2_uint_t Item_U8g2_Generic::getXoffset(u8g2_uint_t x, text_align_t halign, text_align_t valign, const char* text){
  // set vertical position for cursor
  switch (valign){
    case text_align_t::top :
      _u8g2.setFontPosTop();
      break;
    case text_align_t::center :
      _u8g2.setFontPosCenter();
      break;
    case text_align_t::bottom :
      _u8g2.setFontPosBottom();
      break;
    default:
      _u8g2.setFontPosBaseline();
  }

  u8g2_uint_t xadj;
  // find horizontal position for cursor
  switch (halign){
    // try to align text centered to defined _x, _y point
    case text_align_t::center :
      xadj = x - _u8g2.getUTF8Width(text)/2;
      break;
    // calculate left offset from the specified x position to fit the string
    case text_align_t::right :
      xadj = x - _u8g2.getUTF8Width(text);
      break;
    default:
      xadj = x;
  }

  return xadj;
}


void MuiItem_U8g2_PageTitle::render(const MuiItem* parent){
  if (_font)
    _u8g2.setFont(_font);

  auto a = getXoffset(parent->getName());
  _u8g2.drawUTF8(a, _y, parent->getName());
}

void MuiItem_U8g2_StaticText::render(const MuiItem* parent){
  if (_font)
    _u8g2.setFont(_font);
  // print text with word-wrap
  printwords(_u8g2, name, _x, _y);
}

void MuiItem_U8g2_TextCallBack::render(const MuiItem* parent){
  if (_font)
    _u8g2.setFont(_font);

  auto a = getXoffset(name);
  _u8g2.drawUTF8(a, _y, _cb());
  //Serial.printf("TextCallBack: %s\n", _cb());
}


void MuiItem_U8g2_ActionButton::render(const MuiItem* parent){
  if (_font)
    _u8g2.setFont(_font);

  // draw button
  auto a = getXoffset(name);
  _u8g2.drawButtonUTF8(a, _y, focused ? U8G2_BTN_INV : 0, 0, 1, 1, name);
}

mui_event MuiItem_U8g2_ActionButton::muiEvent(mui_event e){
  switch(e.eid){
    // actions 'select' and 'enter' will trigger defined event
    case mui_event_t::select :
    case mui_event_t::enter :
      return mui_event(_action);
  }
  return {};
}


//template <typename Iter>
//void listwriter (Iter it, Iter end) {
//    for (; it!=end; ++it) { /*...*/ }
//}

void MuiItem_U8g2_DynamicScrollList::render(const MuiItem* parent){
  Serial.printf("DynScrl print lst of %u items\n", _size_cb());

  _u8g2.setCursor(_x, _y);
  // specified cursor position will be the top reference for the Title text
  _u8g2.setFontPosTop();

  // normalize index position which items should be displayed
  int visible_idx = muipp::clamp(_index - _num_of_rows/2, 0, static_cast<int>(_size_cb()-1) );
  for (int i = 0; i != _num_of_rows; ++i){
    // change font for active/inactive row
    if (visible_idx == _index && _font)
      _u8g2.setFont(_font);
    else if (_font2)
      _u8g2.setFont(_font2);

    // draw button
    Serial.printf("draw Dynlist_btn %u:%s\n", visible_idx, _cb(visible_idx));
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

    // enter acts as escape to release selection
    case mui_event_t::escape :
      return mui_event(on_escape);
  }

  return {};
}

//  ***
//  MuiItem_U8g2_CheckBox

void MuiItem_U8g2_CheckBox::render(const MuiItem* parent){
  _u8g2.setCursor(_x, _y);
  if (_font)
    _u8g2.setFont(_font);
  // specified cursor position will be the bottom reference for the Title text
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
  }
  // by default, return noop
  return {};
}


//  ***
//  MuiItem_U8g2_ValuesList

void MuiItem_U8g2_ValuesList::render(const MuiItem* parent){
  if (_font)
    _u8g2.setFont(_font);

  // draw label
  auto a = getXoffset(name);
  _u8g2.drawUTF8(a, _y, name);

  // value must be printed right after end of label
  if (_x == _xval)
    _xval = _u8g2.getCursorX();

  // draw button - https://github.com/olikraus/u8g2/wiki/u8g2reference#drawbuttonutf8
  u8g2_uint_t flags = 0;
  if (selected)
    flags |= U8G2_BTN_INV;
  else if (focused)
    flags |= U8G2_BTN_BW1;

  //if (_val_align == text_align_t::center)
  //  flags |= U8G2_BTN_HCENTER;

  // calc cursor's offset for value
  const char* s = _getCurrent();
  auto vxoff = getXoffset(_xval, _val_halign, v_align, s);
  // draw button with frame or inversion
  _u8g2.drawButtonUTF8(vxoff, _y, flags, 0, 2, 2, s);
}

mui_event MuiItem_U8g2_ValuesList::muiEvent(mui_event e){
  switch(e.eid){
    // cursor actions - decr value
    case mui_event_t::moveUp :
    case mui_event_t::moveLeft : 
      _onPrev();
      break;
    // cursor actions - incr value
    case mui_event_t::moveDown :
    case mui_event_t::moveRight :
      _onNext();
      break;

    // enter acts as escape to release selection
    case mui_event_t::enter :
      return mui_event(on_escape);
  }

  // by default, return noop
  return {};
}
