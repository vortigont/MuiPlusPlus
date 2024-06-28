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

Not much documentation so far :disappointed: but I'm working on it. Lib's API could change significantly while lib is under early development.

### Demo

Just a snapshot of my work-in-progress on Soldering Iron menu

Menu navigation demo capture.
|<img src="pics/menu_demo01.png?raw=true" alt="Menu Demo Volts" />
|-

|<img src="pics/menu_demo_pwr.png?raw=true" alt="Menu Demo PWR" />
|-

Reference code could be found in [ESPIron/hid.cpp](https://github.com/vortigont/ESPIron-PTS200/blob/main/ESPIron/hid.cpp)

Stay tuned...

### How it works

#### Design

The thing is - this lib does not build or create any menu's at all. Technically it is just a containter for generic 'MuiItem' classes - objects groupped together, where each object has some callbacks assinged to it. Those objects are iterated and callbacks are called on it. Whatever the function of those objects are - solely defined in a user code. It could draw something on a screen, or change it's state or issue an event, or maybe just do nothing at all.

There is a main container object class `MuiPlusPlus`, it maintains and accounts all `MuiItem` objects (or any objects that are derived from `MuiItem`).
`MuiItem`'s could be collected into `Pages` (or better term here would be `chains`), each `MuiItem` object could belong to multiple pages, or does not belong to anyone. Think about it as a graph where nodes are `MuiItem`s and and "Pages" (or chains) is just a list of interconnected nodes. The "page" is node itself, it just contains a list of all other nodes it connects to.


#### Interaction

`MuiPlusPlus` container interacts with other user code through the exchange of events. Events are defined as enum class `mui_event_t`. It mostly intended to pass cursor navigation events to `MuiPlusPlus` that are coming from keys/encoder, etc...
This library does NOT support any gpio buttons, pads, encoders on it's own. It expects that user code should deal with it. It gives more freedom to use any button library around and also simplifies cases when user code needs to switch button controls from menu to other functions not related to menu navigation, etc...
Basically there are 6 events that could be directly associated with navigations keys:

- `moveUp`,           // move cursor up or previous element or decrement value, etc...
- `moveDown`,         // move cursor down or next element or increment value, etc...
- `moveLeft`,         // move cursor left or previous element or decrement value, etc...
- `moveRight`,        // move cursor right or next element or increment value, etc...
- `enter`,            // commit, ok, enter, or whatever action...

this is enough to operate with menu using one/tree keys or encoder with button. I've started developing this lib for 3 button (encoder) operation. So, one/5 button modes could have some design gaps, not tested so far.

Events should be passed via `MuiPlusPlus::muiEvent` call.

#### **MuiItem** object

`MuiItem` object has the following properties

 - `muiItemId  id` - is is just an `uint32_t` identifier for the objects, it is used to lookup the objects and identify it in the container. No two items with same id could be added to the `MuiPlusPlus` container.
 - `const char* name` - a mnemonic name for the item. Could be null or any char string. Remember that string is NOT copied to the `MuiItem` object on creation. So the pointer to the char* string MUST persist for whole life-time of the object. It could be used to draw item's name on the screen in different scenarios, i.e. it could be a page's name, control name, etc...
 - `render()` - virtual method. It is called to make Item do it's job - draw something, execute something, etc... Usually implemented in derived classes.

#### Navigating Menu and Items

What `MuiPlusPlus` does it maintains a list `pages` with a chain of Items on it. One of the pages is considered `active` at the moment. All items on the active page are "rendered" (i.e. called `render()` method) one by one. Items on other non-acive pages are not executed `render()` call at all.
When page is activated, `MuiPlusPlus` creates a virtual 'cursor' that is navigating though `Items chain` on the page. Cursor is navigated via events supplied to `MuiPlusPlus::muiEvent` call. All items are considered as a chains in a linked-list, it does not matter where or how items are drawing itselfs on hte screen. Order of items in the chain is same order as when items were added/assigned to the page.

#### **MuiItem** states on a page

Each `MuiItem` has a set of propersties that controls which states the item could fall/change into.

 - `focuced` state - it denotes that Item currently has menu's virtual cursor focus. It does not mean that Item is receiving any event or action currently, it just has focus. Think about it as if item should draw itself on screen in inversed color or draw a box around itself
 - `selected` state - it denotes that Item currently has menu's virtual cursor focus and "selected" (or "activated" if you like). It means that from now on, it will receive ALL external cursor events and it must act accordingly. I.e. it can increment/decrement on `moveUp`/`moveDown` events, etc. An important thing to remember that item MUST release selection and signal back to `MuiPlusPlus` container that it is no longer active and cursor command should be used to navigate through the items.
 - `constant` state - an item could be defined as `constant`, Such items won't be able to receive focus or get selected for any events. But such items will be rendered on a page. Think of it as some static text or a dynamic value that should be printed on a page, but you can't interact with that.

Now some more examples for the items, it's properties and states.

Items that is both _focusable_ and _selectable_ could be incrementers/decrementers, i.e. if you need to set a temperature. Fist you use cursor to focus the item, then "enable" it with `enter` event, then use same cursor events (i.e. keys) to increment/decrement value, then use `enter` eveent again to deactivate it and pass cursor control back to item navigation.

Items that are _focusable_ but NOT _selectable_ could be checkboxes. You do not need to activate check box to toggle it's state, right? So you can focus on it, then toggle it's state with `enter` event, then go on navigating to the next item.

