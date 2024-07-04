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

// declare external display object, I will need it to call renderer
extern U8G2_SH1107_64X128_F_HW_I2C u8g2;

// shortcut type aliases
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

DisplayControls::DisplayControls() : _btn(BUTTON_ACTION, LOW), _encdr(BUTTON_DECR, BUTTON_INCR, LOW) {};

void DisplayControls::begin(){
  _btn.deactivateAll();
  _btn.enableEvent(event_t::click);
  _btn.enableEvent(event_t::longPress);
  _btn.enable();
  // enable 'encoder' buttons
  _encdr.begin();

  // subscribe to button events
  esp_event_handler_instance_register(EBTN_EVENTS, ESP_EVENT_ANY_ID, DisplayControls::_event_picker, this, &_evt_btn_handler);

  // subscribe to encoder events
  esp_event_handler_instance_register(EBTN_ENC_EVENTS, ESP_EVENT_ANY_ID, DisplayControls::_event_picker, this, &_evt_enc_handler);

}

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
void DisplayControls::_evt_button(ESPButton::event_t e, const EventMsg* m){
    // here I'll check if currenlty in Menu mode, than I'll send events to the Menu object
    // otherwise I'll route button presses to do some silly message printing

  switch(e){
    // Use click event to send 'enter/ok' to MUI menu
    case event_t::click :{
      if (_inMenu && _menu){
        // we are in Menu, let's pass button "OK" event to it
        _menu_ok_action();
      } else {
        // we are not in menu, let's just print text on display
        stub_text = ok;
        Serial.println("OK Click");
      }
      break;
    }

    // use OK button longPress to act like "open menu" in normal mode and as "escape/return back" when in menu 
    case event_t::longPress : {
      if (!_inMenu){
        // currenlty I'm not in menu, so need to create menu object from here
        _menu = std::make_unique<TemperatureSetup>(_btn, _encdr);
        _inMenu = true;
      } else {
        // we are in Menu, send 'escape' event there, and check if I receive 'quitMenu' event back
        auto e = _menu->muiEvent( mui_event(mui_event_t::escape) );
        // signal switch to Main Work Screen if quitMenu event received, long press will always quit Menu to main screen
        if (e.eid == mui_event_t::quitMenu){
          // release our menu object
          _menu.release();
          _inMenu = false;
          stub_text = quitmenu;
          Serial.println("menu object destroyed");
        }
      }
      break;
    }
  }

  // redraw screen
  _rr = true;
}

// encoder events picker
void DisplayControls::_evt_encoder(ESPButton::event_t e, const EventMsg* m){
  LOGD(T_HID, printf, "_evt_encoder:%u, cnt:%d\n", e2int(e), m->cntr);
  // I do not need counter value here (for now), just figure out if it was increment or decrement via gpio which triggered and event
  if (_inMenu && _menu)
    _menu_encoder_action(m);
  else {
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
  // we are in menu, let's send "enter" menu to it and check return event
  auto e = _menu->muiEvent( mui_event(mui_event_t::enter) );
  // check if menu has quit in responce to button event,
  // if quit, then I'll destroy menu object 
  if (e.eid == mui_event_t::quitMenu){
    _menu.release();
    _inMenu = false;
    stub_text = quitmenu;
    Serial.println("menu object destroyed");
  }
}

/**
 * @brief this method I'll call when my "+/-" buttons are pressed and I need to pass "cursor"
 * event to menu and receive reply event from menu to uderstand when mune has exit
 * 
 */
void DisplayControls::_menu_encoder_action(const EventMsg* m){
  // we are in menu, let's send "encoder" events to menu
  // I do not care about returned events from menu for encoder buttons
  if (m->gpio == BUTTON_INCR){
    _menu->muiEvent( mui_event(mui_event_t::moveDown) );
  } else {
    _menu->muiEvent( mui_event(mui_event_t::moveUp) );
  }
}



void DisplayControls::drawScreen(){
  if (!_rr) return;

  u8g2.clearBuffer();
  if (_inMenu){
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

  // take a screenshot
  //u8g2.writeBufferXBM(Serial);
  _rr = false;
}



//  **************************************
//  ***   Temperature Control Menu     ***

TemperatureSetup::TemperatureSetup(GPIOButton<ESPEventPolicy> &button, PseudoRotaryEncoder &encoder) : MuiMenu(button, encoder){
  LOGD(T_HID, println, "Build temp menu");

  // pretend that we restored temperature values from NVS/EEPROM, we just use defaults here
  Temperatures t;

  // copy values to our private array
  _temp.at(0) = t.deflt;
  _temp.at(1) = t.standby;
  _temp.at(2) = t.boost;
  save_work = t.savewrk;
  _buildMenu();
}

TemperatureSetup::~TemperatureSetup(){
  LOGD(T_HID, println, "d-tor TemperatureSetup");
  // pretend we save new temp settings to NVS
  Temperatures t;

  t.deflt   =  _temp.at(0);
  t.standby =  _temp.at(1);
  t.boost   =  _temp.at(2);
  t.savewrk =  save_work;

  // pretend we save new temp settings to NVS
  //nvs_blob_write(T_IRON, T_temperatures, &t, sizeof(decltype(t)));

  //But let's just print it to serial
  Serial.printf("New default temp:%d\n", t.deflt);
  Serial.printf("New standby temp:%d\n", t.standby);
  Serial.printf("New boost temp:%d\n", t.boost);
  Serial.printf("New save last temp box:%u\n", t.savewrk);
}

void TemperatureSetup::_buildMenu(){

  /*
    this tempreture controls setup menu would be made of ribbon-like numeric scrollers
    that allowS to pick a value with a predefined steps using only +/- buttins and "OK"

    To get the idea how i looks like pls check this animation https://github.com/vortigont/MuiPlusPlus/raw/main/pics/menu_demo01.png?raw=true

    Each scroller needs the following inpits:
     - minimal value
     - maximum value
     - increment step value

     - text label
  */


  /*
    To start with I'll create 3 arrays with min/max/step values for my scrollers
    I use arrays because later on I will create menuItems with a loop
  
  */
  constexpr std::array<int32_t, menu_TemperatureOpts.size()-2> def_temp_min = {
    TEMP_MIN,
    TEMP_STANDBY_MIN,
    TEMP_BOOST_MIN
  };

  constexpr std::array<int32_t, menu_TemperatureOpts.size()-2> def_temp_max = {
    TEMP_MAX,
    TEMP_STANDBY_MAX,
    TEMP_BOOST_MAX
  };

  constexpr std::array<int32_t, menu_TemperatureOpts.size()-2> def_temp_step = {
    TEMP_STEP,
    TEMP_STANDBY_STEP,
    TEMP_BOOST_STEP
  };

  // create root page "Temperature"
  muiItemId root_page = makePage(menu_MainConfiguration.at(0));   // provide a label for the page

  // create "Page title" item and assign it to root page
  addMuippItem(new MuiItem_U8g2_PageTitle (u8g2, nextIndex(), PAGE_TITLE_FONT_SMALL ),  root_page );


  // create and add to main page a list with settings selection options
  muiItemId scroll_list_id = nextIndex();

  // Temperature menu options dynamic scroll list
  /*
    MuiItem_U8g2_DynamicScrollList item makes a scrolling list of text labels
    where you can set how many lines could be displayed at once and go through it
  */
  auto list = new MuiItem_U8g2_DynamicScrollList(
    u8g2,                                           // U8g2 object to draw to
    scroll_list_id,                                 // ID for the item
    // this callback lambda will be called by DynamicScrollList to get a text label of the list by it's index,
    // I'll get this label from a static text array, but this could be any dynamically generated list also
    [](size_t index){ return menu_TemperatureOpts.at(index); },
    // next callback is called by DynamicScrollList to find out the size (total number) of elements in a list
    [](){ return menu_TemperatureOpts.size(); },
    nullptr,                                        // action callback (not needed here)
    MAIN_MENU_Y_SHIFT, MAIN_MENU_ROWS,              // offset for each line of text and total number of lines in menu to dispaly
    MAIN_MENU_X_OFFSET, MAIN_MENU_Y_OFFSET,         // x,y cursor
    MAIN_MENU_FONT3, MAIN_MENU_FONT3                // font to use printin highlighted / non-highlighted item
  );

  // let's set specific options for this meuItem

  
  /*
    dynamic list will act as page selector,
    it means that on ''enter'' event the highlighted scroll item's text label will be used as a key
    to search and switch to the page with same 'name'
  */
  list->listopts.page_selector = true;

  // this flag means that last item of a list will act as 'back' event, and will try to retuen to the previous page/item
  list->listopts.back_on_last = true;
  // scroller here is the only active element on a page,
  // thre is no other items where we can move our cursor to
  // so let's set that an attempt to unselect this item will genreate "menuQuit" event
  list->on_escape = mui_event_t::quitMenu;

  // add list menu object to our root page
  addMuippItem(list, root_page);

  // this is the only active Item on a page, so  let's autselect it
  // it means that whenever this page is loaded, the item on this page with specified id will be focused and selected
  // so it will receive all cursor events by default
  pageAutoSelect(root_page, scroll_list_id);


  // create another "Page title" item, I'll add it to other pages later on, this item uses different font
  // and will be used to display page title for all our numeric scrollers
  muiItemId title2_id = nextIndex();
  addMuippItem( new MuiItem_U8g2_PageTitle(u8g2, title2_id, PAGE_TITLE_FONT) );

  /*
    Now I need to create 3 Items NumberHSlide
    those are numeric scrollers I'll use to set tempearture values
    I will place that scrollers to individual pages, each page will set it's own temperature parameter

    remember that I should set each page's title to the SAME value I've used to create a DynamicScrollList
    to same on typing, let's use a loop sine all items are use similar params
  */

  // autogenerate items for each temperature setting value, number of items are equal to number of array elements I control
  for (auto i = 0; i != _temp.size(); ++i){
    // make page, bound to root page with a title from a scroll list menu
    muiItemId page = makePage(menu_TemperatureOpts.at(i), root_page);
    // add page title element, that same one object we created recently
    addItemToPage(title2_id, page);
    // create num slider Mui Item
    muiItemId idx = nextIndex();
    auto hslide = new MuiItem_U8g2_NumberHSlide<int32_t> (
      u8g2, idx,
      nullptr,        // label is not needed, we already created page title element
      _temp.at(i),    // current temp value reference, it will be updated on by the SLider on "action" event
      def_temp_min[i], def_temp_max[i], def_temp_step[i],   // constrains
      nullptr,        // print unformatted numeric value callbak
      nullptr, nullptr, nullptr,    // no callbacks required here, for details pls check MuiItem_U8g2_NumberHSlide declaration
      NUMERIC_FONT1, MAIN_MENU_FONT2,   // set two fonts for selected and neighbouring values on a Slider
      u8g2.getDisplayWidth()/2, u8g2.getDisplayHeight()/2, NUMBERSLIDE_X_OFFSET   // location on screen where to print the numbers
    );
    // since this item is the only active on the page, return to prev page on unselect
    hslide->on_escape = mui_event_t::prevPage;
    // add num slider to page
    addMuippItem(hslide, page);
    // make this item autoselected on this page
    pageAutoSelect(page, idx);
  }

  // ***
  // now I need a page with checkbox "Save last Work temp"
  muiItemId page = makePage(menu_TemperatureOpts.at(menu_TemperatureOpts.size()-2), root_page);
  // add page Title element (still same one)
  addItemToPage(title2_id, page);
  // create checkbox item
  addMuippItem(
    new MuiItem_U8g2_CheckBox(u8g2, nextIndex(), dictionary[D_SaveLast_box], save_work, [this](size_t v){ save_work = v; }, MAINSCREEN_FONT, 0, 35),
    page);

  // create text hint
  addMuippItem(new MuiItem_U8g2_StaticText(u8g2, nextIndex(), dictionary[D_SaveLast_hint], MAIN_MENU_FONT1, 0, 45),
    page);

  // create "Back Button" item to return to scroll menu
  addMuippItem(new MuiItem_U8g2_BackButton(u8g2, nextIndex(), dictionary[D_return], MAIN_MENU_FONT1),
    page);

  // start our menu from root page
  menuStart(root_page);
  
}


