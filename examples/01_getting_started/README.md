## Getting started

Let's go through a step by step process of creating control menu for soldering Iron as in my [ESPIron](https://github.com/vortigont/ESPIron-PTS200) project.
I will use it as an example for a simple reason - I've build this lib for this specific project so far.

In this example we'll set our environment, button libs, screen, etc... so that I could skip detailed comments on this later on.

All examples are ready-to-build [Platformio](https://platformio.org/) projects. Pls switch to example folder and run `pio run -t upload && pio device monitor` to build for esp32-S2 board and `pio run -e esp32 -t upload && pio device monitor -e esp32` for generic ESP32.
Arduino IDE users should also be fine, but I have not tested it, sorry.

Pls, use 

As for the hardware setup for this I will use the same Iron :) It's the only monochrome screen I have in possession currently.
You will need something like this:

 - Any 32 bit MCU. I used ESP32-S2, ESP32 also works fine, others not tested yet.
 - C++17 compatible toolchain
 - 3 gpios to use as momentary buttons, `+/-` and `enter`, or same thing could be used with rotary encoder and button
 - Any screen that is supported via [U8g2](https://github.com/olikraus/u8g2). I use SH1107.

