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

#pragma once
#include "espasyncbutton.hpp"
#include "muipp_u8g2.hpp"
#include "literals.h"



// forward declaration for MuiMenu class
class MuiMenu;

/*
    this class controls our buttons and
    switch display information between some regural jobs and Configuration menu.
    It's nearly same funtionality as in previous example, just wrapped under class object

*/
class DisplayControls {
  // action button object
  GPIOButton<ESPEventPolicy> _btn;

  // Two button pseudo-encoder object
  PseudoRotaryEncoder _encdr;

  // screen refresh required flag
  bool _rr{true};

  // in Menu flag
  bool _inMenu{false};

  // a placeholder for our menu object, initially empty
  std::unique_ptr<MuiMenu> _menu;

  // action button event handler
  esp_event_handler_instance_t _evt_btn_handler{nullptr};

  // encoder event handler
  esp_event_handler_instance_t _evt_enc_handler{nullptr};

  /**
   * @brief this method will dispatch events from OK button and virtual encoder
   * and forward it to the respective handlers. The thing is that buttons could
   * behave differently depending on which derived menu class is loaded.
   * Won't need it in this example, but maybe used later on
   * 
   * @param arg ESP Evenloop argument
   * @param base ESP Event loop base
   * @param id event id
   * @param event_data event data pointer
   */
  static void _event_picker(void* arg, esp_event_base_t base, int32_t id, void* event_data);

  /**
   * @brief button events handler
   * 
   * @param e 
   * @param m 
   */
  void _evt_button(ESPButton::event_t e, const EventMsg* m);
  // encoder events handler
  void _evt_encoder(ESPButton::event_t e, const EventMsg* m);

  /**
   * @brief this method I'll call when my "enter" button is pressed and I need to pass "enter"
   * event to menu and receive reply event from menu to uderstand when mune has exit
   * 
   */
  void _menu_ok_action();

  /**
   * @brief this method I'll call when my "+/-" buttons are pressed and I need to pass "cursor"
   * event to menu and receive reply event from menu to uderstand when mune has exit
   * 
   */
  void _menu_encoder_action(const EventMsg* m);

public:
  // constructor
  DisplayControls();

  // destructor
  ~DisplayControls();

  // start our display control
  void begin();

  // draw something on screen, either some sily stub text, or render menu
  void drawScreen();

};



/*
Let's create here an abstract class for our future menu sections
I to create a modular menu configuration to segregate it into different sections
based on functionality and load/unload required pieces on demand
So this will be our building brick to make such sections, later on we'll combine it together
*/


/**
 * @brief generic parent class for menu objects
 * it will handle button and encoder events, manage screen refresh, etc...
 * pls tailor it to your needs if using some other button/encoder lib to 
 * convert button actions to MuiPluPlus events as required
 * 
 * menu building/handling functions must be implemented in derived classes
 * 
 */
class MuiMenu : public MuiPlusPlus {


protected:
  /**
   * @brief reference to button object
   * menu object can adjust it's properties to a fine grained control
   * 
   */
  GPIOButton<ESPEventPolicy> &btn;

  /**
   * @brief reference to pseudo-encoder object
   * menu object can adjust it's properties to a fine grained control
   * 
   */
  PseudoRotaryEncoder &encdr;

public:
  // c-tor
  MuiMenu(GPIOButton<ESPEventPolicy> &button, PseudoRotaryEncoder &encoder) : btn(button), encdr(encoder) {};

  virtual ~MuiMenu(){};
};



// working temperature values for the Iron that we want to configure via menu
struct Temperatures {
    int32_t working{TEMP_DEFAULT}, standby{TEMP_STANDBY}, boost{TEMP_BOOST}, deflt{TEMP_DEFAULT};
    bool savewrk{false};
};



/**
 * @brief temperature control menu
 * this class implements scrolled options menu to set/save/restore various
 * temperature settings for the Iron

 * the idea is to maintain an array with integer temperature values that this control menu block should set/display
 * Iron firmware saves/restores those values in NVS, but I'll ommit it here for simplicity 
 * 
 */
class TemperatureSetup : public MuiMenu {
  // configured temperatures

  /*
    An array with integer temperature values that this control menu block should set/display
    number of values are defined by the size of array with temperature labels 'menu_TemperatureOpts'
    (-2 is for 'SaveLast' checkbox flag and 'back' button)
  */
  std::array<int32_t, menu_TemperatureOpts.size()-2> _temp;

  // save flag - a checkbox in menu that controls saving/restoring last used temperature
  bool save_work;

  // menu builder function
  void _buildMenu();

public:
  // c-tor
  TemperatureSetup(GPIOButton<ESPEventPolicy> &button, PseudoRotaryEncoder &encoder);
  // d-tor
  ~TemperatureSetup();

};
