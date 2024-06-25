
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
#include <type_traits>

// define gpio's for our buttons
#define BUTTON_ACTION     GPIO_NUM_0     // middle push-button
#define BUTTON_INCR       GPIO_NUM_2     // incrementer “+” push-button
#define BUTTON_DECR       GPIO_NUM_4     // decrementer “-” push-button

// define RST pin for display
#define SH1107_RST_PIN    7     // display reset pin


static constexpr const char* T_HID = "HID";


// cast enum to int
template <class E>
constexpr std::common_type_t<int, std::underlying_type_t<E>>
e2int(E e) {
    return static_cast<std::common_type_t<int, std::underlying_type_t<E>>>(e);
}


