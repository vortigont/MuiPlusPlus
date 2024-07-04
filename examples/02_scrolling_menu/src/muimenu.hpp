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


/*
    this class controls our buttons and
    switch display information between some regural jobs and Configuration menu.
    It's nearly same funtionality as in previous example, just wrapped into class object

*/
class DisplayControls {
  // "OK" action button object
  GPIOButton<ESPEventPolicy> _btn;

  // Two button pseudo-encoder object is responsible for inr/decr buttons handling
  PseudoRotaryEncoder _encdr;

  // screen refresh required flag
  bool _rr{true};

  // in-Menu state flag
  bool _inMenu{false};

  // a placeholder for our MuiPlusPlus menu object, initially empty
  std::unique_ptr<MuiPlusPlus> _menu;

  // action button event handler, we will initialize it upon DisplayControls's instance creation to attach gpio button with events 
  esp_event_handler_instance_t _evt_btn_handler{nullptr};

  // encoder event handler
  esp_event_handler_instance_t _evt_enc_handler{nullptr};

  /**
   * @brief this method will dispatch events from "OK" button and virtual encoder
   * and forward it to the respective handlers. The thing is that buttons could
   * behave differently depending on which derived menu class is loaded.
   * Won't need it in this example, but maybe used later on in other examples
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
   * event to menu and receive reply event from menu to uderstand when menu has exited
   * 
   */
  void _menu_ok_action();

  /**
   * @brief this method I'll call when my "+/-" buttons are pressed and I need to pass "cursor"
   * event to menu and receive reply event from menu to uderstand when menu has exited
   * 
   */
  void _menu_encoder_action(const EventMsg* m);

  // menu builder function
  void _buildMenu();

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

