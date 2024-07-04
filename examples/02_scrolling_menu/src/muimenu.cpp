/*
  MuiPlusPlus example

  Copyright (C) Emil Muratov, 2024
  GitHub: https://github.com/vortigont/MuiPlusPlus

 *  This program or library is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU General Public License version 2
 *  as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 *  Public License version 2 for more details.
 *
 *  You should have received a copy of the GNU General Public License version 2
 *  along with this library; if not, get one at
 *  https://opensource.org/licenses/GPL-2.1
 */

#include "main.h"
#include "muimenu.hpp"
#define MENU_DEBUG_LEVEL 5
#include "log.h"

// declare our external display object from main.cpp, I will need it to call renderer
extern U8G2_SH1107_64X128_F_HW_I2C u8g2;

// shortcut type aliases for AsyncButton
using ESPButton::event_t;

// let's select some fonts for our Menu
#define MAIN_MENU_FONT              u8g2_font_bauhaus2015_tr
#define SMALL_TEXT_FONT             u8g2_font_glasstown_nbp_t_all


// messages for non-menu display operations
const char* incr = "incr button";
const char* decr = "decr button";
const char* ok = "ok button";
const char* anyk = "Press any key";
const char* quitmenu = "menu closed";

const char* stub_text = anyk;

// Constructor, will create buttons object memebers, assigning pings to it
DisplayControls::DisplayControls() : _btn(BUTTON_ACTION, LOW), _encdr(BUTTON_DECR, BUTTON_INCR, LOW) {};

/**
 * @brief this method will setup our DisplayControl class and begins it's operation
 * 
 */
void DisplayControls::begin(){
  // enable only two events for our OK button - click and longPress
  _btn.deactivateAll();
  _btn.enableEvent(event_t::click);
  _btn.enableEvent(event_t::longPress);
  _btn.enable();
  // enable 'encoder' buttons
  _encdr.begin();

  // subscribe to button events - this is required by AsyncButton lib, address it's documentation for more details on this
  esp_event_handler_instance_register(EBTN_EVENTS, ESP_EVENT_ANY_ID, DisplayControls::_event_picker, this, &_evt_btn_handler);

  // subscribe to encoder events
  esp_event_handler_instance_register(EBTN_ENC_EVENTS, ESP_EVENT_ANY_ID, DisplayControls::_event_picker, this, &_evt_enc_handler);

}

/**
 * @brief Destroy the Display Controls object
 * On destruct I have to unregister button events to reclaim memory
 */
DisplayControls::~DisplayControls(){
  // unregister button events
  if (_evt_btn_handler){
    esp_event_handler_instance_unregister(EBTN_EVENTS, ESP_EVENT_ANY_ID, _evt_btn_handler);
    _evt_btn_handler = nullptr;
  }
  if (_evt_enc_handler){
    esp_event_handler_instance_unregister(EBTN_ENC_EVENTS, ESP_EVENT_ANY_ID, _evt_enc_handler);
    _evt_enc_handler = nullptr;
  }
}

/**
 * @brief this method picks button presses and pass events to respective handler methods - for OK button and for pseudoencoder
 * for details pls address to AsyncButton lib
 * 
 */
void DisplayControls::_event_picker(void* arg, esp_event_base_t base, int32_t id, void* data){
  // OK button events
  if (base == EBTN_EVENTS){
    // pick events only for "enter/OK" gpio button
    if (reinterpret_cast<const EventMsg*>(data)->gpio == BUTTON_ACTION){
      LOGV(T_HID, printf, "btn event:%d, gpio:%d\n", id, reinterpret_cast<EventMsg*>(data)->gpio);
      static_cast<DisplayControls*>(arg)->_evt_button(ESPButton::int2event_t(id), reinterpret_cast<const EventMsg*>(data));
    }
    return;
  }

  // encoder events
  if (base == EBTN_ENC_EVENTS){
    LOGV(T_HID, printf, "enc event:%d, cnt:%d\n", id, reinterpret_cast<EventMsg*>(data)->cntr);
    // pick encoder events and pass it to _menu member
    static_cast<DisplayControls*>(arg)->_evt_encoder(ESPButton::int2event_t(id), reinterpret_cast<const EventMsg*>(data));
    return;
  }
}


// actions to take for middle button presses
/**
 * @brief this method receives events from _event_picker() when "OK" buton is pressed 
 * then it decides what to do this with 'press', either
 * - print some stub text when not in Menu mode
 * - pass event to MuiPlusPlus when in Menu mode
 *  
 * @param e 
 * @param m 
 */
void DisplayControls::_evt_button(ESPButton::event_t e, const EventMsg* m){

    // here I'll check if currenlty in Menu mode, than I'll send events to the Menu object
    // otherwise I'll route button presses to do stub message printing

  switch(e){
    // Use click event to send 'enter/ok' to MUI menu
    case event_t::click :{
      if (_inMenu && _menu){
        // we are in Menu, let's pass button "OK" event to it, for that we call another function that will do additional checks on returned status
        _menu_ok_action();
      } else {
        // we are not in menu, let's just print stub text on display showing that "OK" button has been pressed
        stub_text = ok;
        Serial.println("OK Click");
      }
      break;
    }

    // use OK button longPress to act like "open menu" in normal mode and as "escape/return back" when in menu 
    case event_t::longPress : {
      if (!_inMenu){
        // currenlty I'm not in menu, so I need to dynamically create menu object from here and
        // let it drive the display with menu functions
        // for this I'm creating an instance of MuiPlusPlus object
        // so the idea is - Menu Object is instantiated in memory ONLY when I'm entering menu and released on quit
        _menu = std::make_unique<MuiPlusPlus>();
        _buildMenu();
        // set the flag, indicating that now I have menu object created for this same _evt_button() function would know to redirect further "OK" keypresses to menu from now on 
        _inMenu = true;
      } else {
        // we are already in Menu,
        // so I'm sending 'escape' mui_event there to _menu object, and save what event I receive in return to this press
        // let's save it as 'e' object (a mui_event structure)
        auto e = _menu->muiEvent( mui_event(mui_event_t::escape) );

        // Now I need to check if I received a reply with 'quitMenu' event back from menu object
        // if that is so then I need to switch to Main Work Screen since menu has exited, long press will always quit Menu to main screen
        if (e.eid == mui_event_t::quitMenu){
          // release our menu object - i.e. destruct it, releasing all memory
          if (_menu)
            _menu.release();
          // set flag to indicate we are no longer in menu
          _inMenu = false;
          // change a message we print on a screen
          stub_text = quitmenu;
          Serial.println("menu object destroyed");
        }
      }
      break;
    }
  }

  // redraw screen after each key event
  _rr = true;
}

// encoder events picker
void DisplayControls::_evt_encoder(ESPButton::event_t e, const EventMsg* m){
  LOGD(T_HID, printf, "_evt_encoder:%u, cnt:%d\n", e2int(e), m->cntr);
  // I do not need counter value here (for now), just figure out if it was increment or decrement button press via getting the gpio which triggered and event

  // if I'm in menu, I'm passing this event to a helper member function _menu_encoder_action() that will transfer it to MuiPlusPlus
  if (_inMenu && _menu)
    _menu_encoder_action(m);
  else {
  // otherwise, when I'm not in menu, just change a message displayed on screen
    if (m->gpio == BUTTON_INCR){
      stub_text = incr;
    } else {
      stub_text = decr;
    }    
  }
  // redraw screen
  _rr = true;
}

/**
 * @brief this method I'll call when my "enter" button is pressed and I need to pass "enter"
 * event to menu and receive reply event from menu to uderstand when mune has exit
 * 
 */
void DisplayControls::_menu_ok_action(){
  // we are in menu, let's send "enter" event to it and check returned event reply
  auto e = _menu->muiEvent( /* sending a structure with 'mui_event_t::enter' */  mui_event(mui_event_t::enter) );

  // e - is a structure of type `mui_event`, let's check what event is inside it to see if menu has ended it's operation and quit
  // if quit, then I'll destroy menu object 
  if (e.eid == mui_event_t::quitMenu){
    // if that is so then I need to switch to Main Work Screen since menu has exited, long press will always quit Menu to main screen
    _menu.release();
    // release our menu object - i.e. destruct it, releasing all memory
    _inMenu = false;
    stub_text = quitmenu;
    Serial.println("menu object destroyed");
  }
}

/**
 * @brief this method I'll call when my "+/-" buttons are pressed and I need to pass "cursor movement"
 * event to menu and receive reply event from menu to uderstand when mune has exit
 * 
 */
void DisplayControls::_menu_encoder_action(const EventMsg* m){
  // we are in menu, let's send "encoder" events to menu
  // I do not care about returned events from menu for encoder buttons presses for now because I know that
  // menu could quit only on "OK" button press or longPress
  if (m->gpio == BUTTON_INCR){
    _menu->muiEvent( mui_event(mui_event_t::moveDown) );
  } else {
    _menu->muiEvent( mui_event(mui_event_t::moveUp) );
  }
}


/**
 * @brief this method will render screen
 * when in menu it calls MuiPlusPlus object to do it's job
 * when not in menu it just prints stub text messages
 * 
 */
void DisplayControls::drawScreen(){
  if (!_rr) return;

  u8g2.clearBuffer();
  if (_inMenu && _menu){
    // when in menu - call MuiPlusPlus renderer
    Serial.printf("Render menu:%lu ms\n", millis());
    // call Mui renderer
    _menu->render();
  } else {
    // if not in menu, we simply print static text with specified state
    Serial.println("Render welcome screen");
    u8g2.setFont(SMALL_TEXT_FONT);
    u8g2.setCursor(0, u8g2.getDisplayHeight()/2);
    u8g2.print(stub_text);
  }
  u8g2.sendBuffer();

  _rr = false;
}


/**
 * @brief this method will build our menu
 * in this example it would be also very simple - a just a scroll list of menu items to display
 * let's assume we have following strtucture
 * 
 * 'root menu title'
 *  |- Temperature
 *  |     |- Some stub page
 *  |- Timeouts
 *  |     |- Some stub page
 *  |- Tip
 *  |     |- Some stub page
 *  |- Power Supply
 *  |     |- Some stub page
 *  |- Information
 *  |     |- Some stub page
 *  |- <Back
 * 
 * 
 * This menu consist of scroll list of 6 items - 5 pages to switch to and a special "<Back" list item that should act as menu exit
 * My Oled screen fits about 3 lines of readable text, so what I need is a scrool list that could display 3 line at a time and have
 * a cursor tracking position in that list
 * 
 * "Some stub page" will be a page with the following items:
 * 
 * - Page title
 * - a button in the middle of the screen with "OK" label acting as "return" on press
 * 
 */
void DisplayControls::_buildMenu(){

  // create root page with label: "Settings"
  muiItemId root_page = _menu->makePage(lang_en_us::T_Settings);

  // create "Page title" item and assign it to root page - it will render page's label as a Title at top left corner
  _menu->addMuippItem(new MuiItem_U8g2_PageTitle (u8g2, _menu->nextIndex(), PAGE_TITLE_FONT_SMALL ),  root_page );

  // Now I create and add to main page a scroll-list with settings selection options

  // start with making an id for our future list
  muiItemId scroll_list_id = _menu->nextIndex();

  /*
    MuiItem_U8g2_DynamicScrollList item renders a scrolling list of text labels on a page,
    there might be multiple items in such lists but you can set how many lines could be displayed at once
    and it will adjust to the position while going through the list
  */
  auto list = new MuiItem_U8g2_DynamicScrollList(
    u8g2,                                           // U8g2 object to draw to
    scroll_list_id,                                 // ID for the item
    /*
      this callback lambda will be called by DynamicScrollList to get a text label in the list by it's position index.
      I.e. you have a list of 10 items, this callback should return n-th item label as a `const char*`
      Our menu list is stored in a static text array `menu_TemperatureOpts`, but this could be any code that dynamically generates those labels
    */
    [](size_t index){ return menu_MainConfiguration.at(index); },
    // next callback is called by DynamicScrollList to find out the size (total number) of elements in a list, here I simply return arra's size
    [](){ return menu_MainConfiguration.size(); },
    nullptr,                                        // action callback (not needed in this example)
    MAIN_MENU_Y_SHIFT, MAIN_MENU_ROWS,              // offset on Y axis in pixels to shift down for each line of text and total number of lines in menu to display at once
    MAIN_MENU_X_OFFSET, MAIN_MENU_Y_OFFSET,         // x,y cursor position where to start printing our list
    MAIN_MENU_FONT3, MAIN_MENU_FONT3                // font to use printing highlighted / non-highlighted item, I use same font here, but you could use different one, for ex. to make highlighted item bold
  );

  // Now the list object is created,
  // let's set specific options for this meuItem
  
  /*
    dynamic list will act as page selector,
    it means that on ''enter'' event the highlighted scroll item's text label will be used as a key
    to search and switch to the page with same 'label'.
    I.e. if you create any ohter page and assign it a label matching label in your list, it will switch to that page
    It is called "page selector". If you won't set this flag, then DynamicScrollList will try to call a callback function
    with an index of currently selected item in the list (not needed in this example)
  */
  list->listopts.page_selector = true;

  // this flag means that last item of a list will act as 'back' event, not a page selector.
  // On "enter" event it will generate 'escape' event and try to return to the previous page/item if possible
  list->listopts.back_on_last = true;

  // there is no other items on this page where we can move our cursor to, only thing we have is that scroll list
  // so let's set that an attempt to unselect this item on a page will genreate "menuQuit" event and MuiPP will try quit the menu or return to previous page (not applicable here)
  list->on_escape = mui_event_t::quitMenu;

  // add our created list menu object to the root page
  _menu->addMuippItem(list, root_page);

  // this is the only active Item on a page, so  let's autoselect it
  // it means that whenever this page is loaded, the item on this page with specified id will be focused and selected
  // so it will receive all cursor events by default
  // literally it means that cursor will operate moving "inside" the scroll list
  _menu->pageAutoSelect(root_page, scroll_list_id);

  // now we have our page selector, but need to create all other pages where that selector leads to
  // let it be some simple stubs, looking pretty same
  // for that I can create a coubple of items and reuse it multiple times, placing it in differetnt pages

  // create another "Page title" item, I'll add it to other pages later on, this item uses different font
  // first I need an id for it to remember
  muiItemId title2_id = _menu->nextIndex();
  // now I can create and add this item to the container without binding it to any page
  // I can do the binding later on
  _menu->addMuippItem( new MuiItem_U8g2_PageTitle(u8g2, title2_id, PAGE_TITLE_FONT) );


  // I need a "Quit" button that I'll place on those stub pages

  // generate new ID for the Item
  muiItemId quit_idx = _menu->nextIndex();

  // create a new Action Button object to act like a quit button
  auto quitbtn = new MuiItem_U8g2_ActionButton(
    u8g2,                       // u8g2 object
    quit_idx,                   // unique ID for the Item
    mui_event_t::escape,        // which event to generate on button action, `escape` will attempt to switch to previous page
    "Return back",              // button label
    SMALL_TEXT_FONT,            // text font
    u8g2.getDisplayWidth()/2,   // horizontal placement button to the center of screen
    u8g2.getDisplayHeight()/2,  // vertical placement button to the center of screen
    text_align_t::center,       // text horizontal align to center
    text_align_t::bottom);      // text vertical align to bottom

  // again I add that new item to container without binding to any page
  _menu->addMuippItem(quitbtn, quit_idx);

  // now let's do some automation - I have an array with text labels for our menu - `menu_MainConfiguration`
  // I need to create pages for each of those list items so that dynamic list could switch to that page
  // I know that all elements but the last one should be an independedn page, last one acts as 'back' item
  // so let's use a loop to create those stub pages for each list item

  for (auto i = 0; i != (menu_MainConfiguration.size() - 1); ++i){    // remember that last item I do not need
    // make page, bound to a parent root page with a title from our scroll list items
    muiItemId page = _menu->makePage(menu_MainConfiguration.at(i), root_page);
    // add page title element to the page, that same one object we created recently
    _menu->addItemToPage(title2_id, page);
    // add quit button element to the page, same one to all pages would do
    _menu->addItemToPage(quit_idx, page);

    // make quit button item autoselected on this page, i.e. it will receive cursor focus on page load
    _menu->pageAutoSelect(page, quit_idx);
  }


  // start our menu from a root page
  _menu->menuStart(root_page);
}


