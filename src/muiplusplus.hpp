#pragma once

#include <list>
#include <memory>
#include <vector>
#include "muipp_tpl.hpp"
//#include <string_view>
//#include "clib/mui.h"
//#include "clib/mui_u8g2.h"
#include "Arduino.h"

enum class mui_err_t {
  ok = 0,
  error,            // generic error
  outofmem,         // memory allocation error
  id_exist,         // specified id already exist and can't be used
  id_err            // specified id can't be used
};

enum class mui_event_t : size_t {
  // === following events are common for MuiPlusPlus and for MuiItem objects
  noop = 0,         // no operation
  // navigation and control
  moveUp,           // move cursor up or previous element or decrement value, etc...
  moveDown,         // move cursor down or next element or increment value, etc...
  moveLeft,         // move cursor left or previous element or decrement value, etc...
  moveRight,        // move cursor right or next element or increment value, etc...
  enter,            // commit, ok, enter, or whatever action...
  // === following events are handled by MuiPlusPlus only and won't be passed to MuiItem's
  // menu items navigation
  focus = 100,      // item received cursor focus
  unfocus,          // item lost focus
  select,           // select item (by default focused item)
  unselect,         // unselect current item
  nextItem,         // switch focus to next item on a page and select it
  prevItem,         // switch focus to previous item on a page and select it
  nextPage,         // switch to next page and focus on first item
  prevPage,         // switch to prev page and focus on first item
  goPageByID,       // open page with provided id
  goPageByName,     // open page with provided Name, (void*) arg should point to const cahr*
  goItemByID,       // focus and select another item by ID
  goItemByName,     // focus and select another item by Name, (void*) arg should point to const cahr*
  escape,           // escape, return back, unselect etc...
  hideItem,
  unhideItem,
  hideToggle,       // toggle hide status for an item
  quitMenu,

  // == all events below are specific to MuiItems only and won't be processed by MuiPlusPlus::event()
  // value change
  value =  200,     // any absolute value (i.e. encoder conter, etc...)
  step,             // relative value, +- offset from current
  blob,             // supplied blob object via (void*), mui_event::param could define blob size or coded type...
  string,           // supplied const char* via (void*), mui_event::param could define string len etc...
  custom1,          // placeholders
  custom2,
  custom3,
  custom4,
  custom5,
  _noop             // no operation
};

struct mui_event {
  // event id
  mui_event_t eid;
  int32_t param;
  void* arg;
  mui_event(mui_event_t eid = mui_event_t::noop, int32_t param = 0, void* arg = nullptr) : eid(eid), param(param), arg(arg) {}
};

struct item_opts {
  // defines if item can get focus on a page and receive control events
  bool selectable{true};
  /**
   * hidden elements wont receive render() calls and can't be selected for control inputs
   * but still could receive events from other items directly
  */
  bool hidden{false};
};


// aliases

// MuiItem event callback type
using mui_event_cb = std::function<mui_event (mui_event e)>;



/**
 * @brief 
 * 
 */
class MuiItem {
protected:
  // mnemonic name for the item
  const char* name;
  // Item properties struct
  item_opts opt;
  // event callback function
  mui_event_cb cb = nullptr;

public:
  // numeric identificator of item
  const muiItem_id  id;

  //MuiItem(){};
  MuiItem(muiItem_id id, const char* name = nullptr, item_opts options = item_opts()) : id(id), name(name), opt(options) {};
  virtual ~MuiItem(){};

  virtual const char* getName() const { return name; };

  virtual const char* setName(const char* newname) { name = newname; return name; };

  /**
   * @brief returns true if Item can be selected (focused) on a page
   * and will receive control events
   * @return true 
   * @return false 
   */
  virtual bool getSelectable() const { return opt.selectable; }

  virtual void setSelectable(bool value) { opt.selectable = value; }

  virtual bool getHidden() const { return opt.hidden; }

  virtual void setHidden(bool value) { opt.hidden = value; }

  void setEventCallBack(mui_event_cb c){ cb = c; };

  /**
   * @brief item event receiver function
   * by default it will call assigned callback, if nothing was assigned then it will return noop
   * @param e 
   * @return mui_event_t 
   */
  virtual mui_event muiEvent(mui_event e){ return cb ? cb(e) : mui_event(); }

  /**
   * @brief render item
   * 
   * @param page 
   */
  virtual void render(const MuiItem* parent){};
};

using MuiItem_pt = std::unique_ptr<MuiItem>;

class MuiItem_Uncontrollable : public MuiItem {
public:
  bool getSelectable() const override final { return false; }
  void setSelectable(bool value) override final {}
};


class MuiPage : public MuiItem {
  friend class MuiPlusPlus;
  std::vector<muiItem_id> items;
  //std::vector<muiItem_id>::iterator currentItem;
  // previous page id (points back to self by default)

public:
  using MuiItem::MuiItem;
  //void addMuippItem(muiItem_id item_id){ items.push_back(item_id); currentItem = items.begin(); };
};

//using MuiPage_pt = std::unique_ptr<MuiPage>;


class MuiPlusPlus {

  // sequence number for the items
  muiItem_id _items_index{0};

//protected:
  std::list<MuiItem_pt> items;
  std::vector<MuiPage> pages;
  std::vector<MuiPage>::iterator currentPage;
  std::vector<MuiPage>::iterator prevPage;
  std::list<MuiItem_pt>::iterator focusedItem;
  //std::list<MuiItem_pt>::iterator previousItem;
  // if item_active is true, than focused item will receive events from a cursor
  bool item_active{false};

  /**
   * @brief find page by it's id
   * 
   * @param id 
   * @return std::vector<MuiPage>::iterator 
   */
  std::vector<MuiPage>::iterator _page_by_id(muiItem_id id){ return std::find_if(pages.begin(), pages.end(), MatchPageID<MuiPage>(id)); }

  /**
   * @brief find item byt it's id
   * 
   * @param id 
   * @return std::list<MuiItem_pt>::iterator 
   */
  std::list<MuiItem_pt>::iterator _item_by_id(muiItem_id id){ return std::find_if(items.begin(), items.end(), MatchID<MuiItem_pt>(id)); }

  /**
   * @brief event handler that item may return in reply to input event
   * 
   * @param e 
   */
  mui_event _feedback_event(mui_event e);

  /**
   * @brief hanles event on navigating the menu
   * i.e. moving cursor on items, selecting/deselecting, etc...
   * 
   * @param e 
   */
  mui_event _menu_navigation(mui_event e);

  void _move_to_prev_item();

public:

  /**
   * @brief start menu from specified page and (optionally) selecting an item
   * 
   */
  void menuStart(muiItem_id page, muiItem_id item = 0);

  uint32_t nextIndex(){ return ++_items_index; }

  muiItem_id makePage(const char* name = nullptr, item_opts options = item_opts());

  mui_err_t addMuippItem(MuiItem_pt item, muiItem_id page_id = 0);

  //mui_err_t addMuippItem(MuiItem&& item, muiItem_id page_id = 0);//{ addMuippItem( std::make_unique<MuiItem_pt>(std::move(item)), page_id); };

  mui_err_t addItemToPage(muiItem_id item_id, muiItem_id page_id);

  /**
   * @brief event sink
   * will receive events to menu current page / selected item, etc...
   * 
   * @param e input event struct
   * @return mui_event responce event struct
   */
  mui_event muiEvent(mui_event e);

  /**
   * @brief switch to specified page with specified id
   * 
   * @param page 
   */
  mui_err_t switchTo(muiItem_id page, muiItem_id item = 0);


  // before calling render on each items
  //void setPreExec();

  // render menu on screen
  void render();

  // after calling render items
  //void setPostExec();
};


