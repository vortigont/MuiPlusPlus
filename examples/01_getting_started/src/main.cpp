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



/*
  As already mentioned in /README/md, MuiPluPlus is agnostinc to button/encoder libraries since it uses it's own
  event structures and user must prepare the code that is responsible to translating button presses to desired events


  Let's set out dependency libraries first
  Here I will use my own library for buttons handling https://github.com/vortigont/ESPAsyncButton
  It's a full asynchronous button lib for ESP32 and fits nicely to applications where you rely on asynchronous events
  rather then polling something in Arduino's loop()

  For screen handling use U8g2 lib since I wrote MuiItems onnly for that lib currently
*/

#include <Arduino.h>
// Button library
#include "espasyncbutton.hpp"
// MuiPlusPlus library with some ready-to use Items with U8G2 libs
#include "muipp_u8g2.hpp"

// define gpio's for our buttons
#define BUTTON_ACTION     GPIO_NUM_0     // middle push-button
#define BUTTON_INCR       GPIO_NUM_2     // incrementer “+” push-button
#define BUTTON_DECR       GPIO_NUM_4     // decrementer “-” push-button

// define RST pin for display
#define SH1107_RST_PIN    7     // display reset pin


// let's select some fonts for our Menu
#define MAIN_MENU_FONT              u8g2_font_bauhaus2015_tr
#define SMALL_TEXT_FONT             u8g2_font_glasstown_nbp_t_all



// I will use full screen buffer for display, for 32 mcu's memory footprint is not that big to to tradeoff with partial updates
// those are defaults for I2C gpios in U8g2 lib
//#define U8X8_PIN_I2C_CLOCK 12	/* 1 = Input/high impedance, 0 = drive low */
//#define U8X8_PIN_I2C_DATA 13	/* 1 = Input/high impedance, 0 = drive low */

// create screen
U8G2_SH1107_64X128_F_HW_I2C u8g2(U8G2_R1, SH1107_RST_PIN);

/*
  Set our buttons - one will act as "OK/Enter",
  other two as virtual encoder that generates counting events.
  For details on how this works, pls address to https://github.com/vortigont/ESPAsyncButton
  You can use any other button handling lib, I'll explain how to convert button presses into MuiPlusPlus events later on
*/ 

// action buttons
AsyncEventButton b_ok(BUTTON_ACTION, LOW);
AsyncEventButton b_decr(BUTTON_DECR, LOW);
AsyncEventButton b_incr(BUTTON_INCR, LOW);

// Our menu container
MuiPlusPlus muiplus;

// flag indicating that we entered menu screen
bool inMenu{false};

// flag that indicates a screen refresh is required
bool refreshScreen{true};

// messages for non-menu display operations
const char* incr = "incr button";
const char* decr = "decr button";
const char* ok = "ok button";
const char* anyk = "Press any key";
const char* quitmenu = "menu closed";

const char* stub_text = anyk;


// Forward declarations for our functions


/**
 * @brief this function will be called periodically to render something on the screen
 * by default it will draw some stub text messages, but when Entering menu, it will call renderer for MuiPP menu
 * 
 */
void screen_render();

// setup our buttons
void setup_buttons();

// here we will set our menu configuration
void setup_menu();

// Arduino's Setup
void setup() {
#ifdef ARDUINO_USB_MODE
  Serial.setTxTimeoutMs(0);
#endif
  Serial.begin(115200);
  // let serial to settle a bit
  delay(1000);

  Serial.println("Start MuiPP example");

  // init U8G2 display
  u8g2.initDisplay();
  u8g2.begin();
  u8g2.sendF("ca", 0xa8, 0x3f);

  setup_menu();

  setup_buttons();

  Serial.println("Press any keys once to see it's actions");
  Serial.println("Long Press OK key to open menu");
}


// MAIN loop
void loop() {
  // we will only check if screen needs to be refreshed here each 100ms, all other events are processed asynchronously
  screen_render();
  delay(100);
}




// setup our buttons
void setup_buttons(){
  // create event loop for ESPAsyncButton
  esp_event_loop_create_default();
  // enable middle button
  b_ok.begin();
  b_decr.begin();
  b_incr.begin();

  /*
    if you are using some other button lib then here is how you should handle your button operations,
    define how and when your buttons should control the Menu and call MuiPP with respective event

  */
  
  // we'll set a lambda callback for our 'action' button click
  b_ok.onClick(
    [](){
      if (inMenu){
        // when we are in Menu, we will sent "enter" event to MuiPP, so it will handle it as "ok","enter","confirm" action depending on selected item
        // another thing is to monitor event that is returned back from MuiPluePlus, i.e. it can return 'quitMenu' event, indicating that menu
        // has exi exited, and buttons should return to their normal operation
        if ( muiplus.muiEvent( mui_event(mui_event_t::enter) ).eid == mui_event_t::quitMenu ){
          stub_text = quitmenu;
          inMenu = false;
        }
      } else {
        // otherwise, when not in menu, we simply set to refresh screen with OK button
        stub_text = ok;
      }
      // whatever the action was, need to refresh the screen
      refreshScreen = true;
      Serial.println("OK Click");
    }
  );

  // we'll set a lambda callback for our 'action' button long press
  // on LongPress we will enter menu start handling cursor events
  b_ok.onLongPress(
    [](){
        inMenu = true;
        refreshScreen = true;
        Serial.println("btn Long press");
    }
  );

  // we'll set a lambda callback for our 'increment' button long press
  b_incr.onClick(
    [](){
      if (inMenu)
        // when we are in Menu, we will sent "moveDown" event to MuiPP,
        // so it will handle it as cursor control
        muiplus.muiEvent( mui_event(mui_event_t::moveDown) );
      else {
        // otherwise, when not in menu, we simply set to refresh screen with OK button
        stub_text = incr;
      }
      refreshScreen = true;
      Serial.println("btn increment");
    }
  );

  // we'll set a lambda callback for our 'increment' button long press
  b_decr.onClick(
    [](){
      if (inMenu)
        // when we are in Menu, we will sent "moveUp" event to MuiPP,
        // so it will handle it as cursor control
        muiplus.muiEvent( mui_event(mui_event_t::moveUp) );
      else {
        // otherwise, when not in menu, we simply set to refresh screen with OK button
        stub_text = decr;
        refreshScreen = true;
      }
      refreshScreen = true;
      Serial.println("btn decrement");
    }
  );

  // enable buttons
  b_ok.enable();
  b_decr.enable();
  b_incr.enable();
}



void screen_render(){
  /*
   this is pretty simple, we won't redraw screen each time, but will check
   if change flag is set, only then redraw.
   It is up to you how handle this, it's beyond the scope of this lib
  */
 if (!refreshScreen)
  return;

  u8g2.clearBuffer();

  if (inMenu){
    // when in menu we call MuiPP renderer, it will go through the list of items on the page and draw it
    Serial.println("Render menu");
    muiplus.render();
  } else {
    // if not in menu, we simply print static text with specified state
    Serial.println("Render welcome screen");
    u8g2.setFont(SMALL_TEXT_FONT);
    u8g2.setCursor(0, u8g2.getDisplayHeight()/2);
    u8g2.print(stub_text);
  }

  // render buffer to screen
  u8g2.sendBuffer();
  refreshScreen = false;
};


void setup_menu(){
/*
 since this is just an introduction example, we'll set only very basic things here,
 Actually our menu will contain only one page with name header and two 'Quit' buttons
 both that will act as "Quit menu" :)
*/


  // create our page with a name "Simple page", let's save page's ID to the variable root_page, we'll need it later
  muiItemId root_page = muiplus.makePage("Simple page");

  // now we'll create a special Item that do nothing but displays current page's title in top left corner of the screen
  // MuiPlusPlus objects attaches Muipp Items in as shared pointers, so it is possible to reuse same object multiple times
  // without dublicating instances

  // we can create a new Item object, add it to MuiPP container and attach to the specific page in one call
  // thing to note here is muiplus.nextIndex() argument
  // when you do not need to keep any references to your objects, just let MuiPP autoassign it next available ID
  muiplus.addMuippItem(new MuiItem_U8g2_PageTitle(u8g2, muiplus.nextIndex(), MAIN_MENU_FONT ), root_page);



  // now we'll make a button with label "Quit", which will just generate "menuQuit" event on action
  // let's see it's options in details

  // generate new ID for the Item
  muiItemId quit_idx = muiplus.nextIndex();

  // create a new Action Button object
  auto quitbtn = new MuiItem_U8g2_ActionButton(
    u8g2,                       // u8g2 object
    quit_idx,                     // unique ID for the Item
    mui_event_t::quitMenu,      // which event to generate on button action
    "Quit menu",                // button label
    SMALL_TEXT_FONT,            // text font
    u8g2.getDisplayWidth()/2,   // horizontal placement button to the center of screen
    u8g2.getDisplayHeight()/2,  // vertical placement button to the center of screen
    text_align_t::center,       // text vertical align to center
    text_align_t::bottom);      // text horizontal aling to bottom

  // attach item to the page
  muiplus.addMuippItem(quitbtn, root_page);

  // Also we'll create another "back" button,
  // it's a special object that will generate mui_event_t::prevPage event on action
  // but since we have only one page here, it will do nothig,
  // it is here so that you can move your cursolr between Quit button and back button and see how it works

  muiplus.addMuippItem(
    new MuiItem_U8g2_BackButton(u8g2, muiplus.nextIndex(), "<Back", SMALL_TEXT_FONT),
    root_page
  );

  // remeber that button's are NOT selectable, you can focus on it and "press" it with action 'enter', but it won't steal
  // your cursor controls


  // now we need to start our menu from the page that we just created
  muiplus.menuStart(root_page);

  // remember to ALWAYS specify start page for menu, otherwise it will point to nowhere, it won't pick
  // any random page for you to start with

}


