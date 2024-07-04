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



#include <Arduino.h>
#include "main.h"
#include "muimenu.hpp"


// create screen driver
U8G2_SH1107_64X128_F_HW_I2C u8g2(U8G2_R1, SH1107_RST_PIN);

// Our object that controls screen information
DisplayControls display;


// Arduino's Setup
void setup() {
#ifdef ARDUINO_USB_MODE
  Serial.setTxTimeoutMs(0);
#endif
  Serial.begin(115200);
#ifdef ARDUINO_USB_MODE
  // let serial to settle a bit
  delay(2000);
#endif

  Serial.println("Start MuiPP example");

  // create event loop for ESPAsyncButton
  esp_event_loop_create_default();

  // init U8G2 display
  u8g2.initDisplay();
  u8g2.begin();
  u8g2.sendF("ca", 0xa8, 0x3f);

  // start display operations
  display.begin();

  Serial.println("Press any keys once to see it's actions");
  Serial.println("Long Press OK key to open menu");
}


// MAIN loop
void loop() {
  // we will only check if screen needs to be refreshed here each 100ms, all other events are processed asynchronously
  display.drawScreen();
  delay(100);
}


