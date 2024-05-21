# MuiPlusPlus library

On-Screen Menu navigation and UI controls library for 32 bit MCUs.


`MuiPlusPlus` is "Minimalistic UI on C Plus Plus". It all started with [MUI](https://github.com/olikraus/u8g2/wiki/muimanual#mui) - a really really nice minimalistic UI lib on top of [u8g2](https://github.com/olikraus/u8g2) by [olikraus](https://github.com/olikraus).
I needed a menu lib for my project with monochrome LCD, [MUI](https://github.com/olikraus/u8g2/wiki/muimanual#mui) looked just what I needed fisrt. But then found that `MUI` is in pure C and it is really difficult to integrate it with C++ class-based dynamic menu design. It is missing functional callbacks, dynamic menu content is also quite limited or prety awkward to implement. It was discussed in theads [here](https://github.com/olikraus/u8g2/issues/2258) and [here](https://github.com/olikraus/u8g2/discussions/2252).
To my surprize most of the libs around here for Arduino LCD menu's are designed in similar manner and lacks functional callbacks, even if they are build on C++ classes. The intention is to save resources as much as possible compromising flexibily. (i.e. [GEM](https://github.com/Spirik/GEM), [ArduinoMenu](https://github.com/neu-rah/ArduinoMenu)). 

So I created this pretty simple lib to satisfy my needs working on modern 32 MCUs where I can afford some memory allocation for the sake of flexibily.

**MuiPlusPlus** is heavily using STL, C++17 (needs `std::string_view`, `std::ostringstream`) and classes inheritance to build dynamic menus, lists, or any user-defined derivates from the base classes. Functional callbacks and capturing lambdas are available for any Menu Item element, so it is pretty easy to integrate menu with arbitrary user class instance.
It is more memory hungry than tiny static libs, so is intended to run on 32 bit MCUs with around hundred KiB of memory. But since menu elements and pages could be created/deallocated dynamicaly it still stays quite compact and does not waste mem for long lived structures.

Code style I use is - create menu object on-demand, use it to change some option or setting, then deallocate. Large complex menu structures could be split into independed objects and loaded only those that needed right now.

**MuiPlusPlus** uses abstract events for controls and navigation, so could be integrated with any other button/encoder lib around.
Main navigation and event class is independent of any graphics binding, menu elements are build around [u8g2](https://github.com/olikraus/u8g2) lib, but could be created/adopted/inherited to any other graphics library around.
Currently lib is in early development state and pretty limited in functions and basic elements.
I use it to work on my [ESPIron-PTS200](https://github.com/vortigont/ESPIron-PTS200) project and create a screen navigation menu for soldering iron control.


No documentation or examples yet :disappointed: but I will work on it once the API will stabilize a bit.

### Demo

Just a snashot of my work-in-progress on Soldering Iron menu

Menu navigation demo capture.
|<img src="pics/menu_demo01.png?raw=true" alt="Menu demo" />
|-

Reference code could be found in [ESPIron/hid.cpp](https://github.com/vortigont/ESPIron-PTS200/blob/main/ESPIron/hid.cpp)

