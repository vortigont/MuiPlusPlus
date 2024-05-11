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
  const muiItemId  id;

  bool focused{false};
  bool selected{false};

  //MuiItem(){};
  MuiItem(muiItemId id, const char* name = nullptr, item_opts options = item_opts()) : id(id), name(name), opt(options) {};
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

  //virtual void setSelectable(bool value) { opt.selectable = value; }

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

// Item pointer type declaration
//using MuiItem_pt = std::unique_ptr<MuiItem>;
using MuiItem_pt = std::shared_ptr<MuiItem>;

class MuiItem_Uncontrollable : public MuiItem {
public:
  using MuiItem::MuiItem;
  bool getSelectable() const override final { return false; }
  //void setSelectable(bool value) override final {}
};


/**
 * @brief MuiPage is just a containter MuiItem with refs to other MuiItem objects
 * 
 */
class MuiPage : public MuiItem {
  friend class MuiPlusPlus;
  std::list<MuiItem_pt> items;
  std::list<MuiItem_pt>::iterator currentItem;
  // if itm_selected is true, than focused item will receive events from a cursor
  bool itm_selected{false};
  // parent page id (zero -> no parent page)
  muiItemId parent_page;
  
public:
  MuiPage(muiItemId id, const char* name = nullptr, muiItemId parent = 0, item_opts options = item_opts())
    : MuiItem(id, name, options), parent_page(parent) { currentItem = items.end(); };

  /**
   * @brief specifies if there is an item on a page that will
   * get focus and autoselected when switching to this page
   * could be usefull for menu-lists, etc...
   * 
   */
  muiItemId autoSelect{0};

  //using MuiItem::MuiItem;
  //void addMuippItem(muiItemId item_id){ items.push_back(item_id); currentItem = items.begin(); };
};



class MuiPlusPlus {

  // sequence number for the items
  muiItemId _items_index{0};
  muiItemId _pages_index{0};

//protected:
  std::list<MuiItem_pt> items;
  std::list<MuiPage> pages;
  std::list<MuiPage>::iterator currentPage;
  //std::vector<MuiPage>::iterator prevPage;
  //std::list<MuiItem_pt>::iterator focusedItem;
  //std::list<MuiItem_pt>::iterator previousItem;
  // if item_active is true, than focused item will receive events from a cursor

  /**
   * @brief find page by it's id
   * 
   * @param id 
   * @return std::list<MuiPage>::iterator 
   */
  std::list<MuiPage>::iterator _page_by_id(muiItemId id){ return std::find_if(pages.begin(), pages.end(), MatchPageID<MuiPage>(id)); }

  /**
   * @brief find item byt it's id
   * 
   * @param id 
   * @return std::list<MuiItem_pt>::iterator 
   */
  std::list<MuiPage>::iterator _page_by_label(const char* label){ return std::find_if(pages.begin(), pages.end(), MatchLabel<MuiPage>(label)); }

  /**
   * @brief find item by it's id
   * 
   * @param id 
   * @return std::list<MuiItem_pt>::iterator 
   */
  std::list<MuiItem_pt>::iterator _item_by_id(muiItemId id){ return std::find_if(items.begin(), items.end(), MatchID<MuiItem_pt>(id)); }


  /**
   * @brief handler for an event that item may return in reply to input event
   * 
   */
  void _feedback_event(mui_event e);

  /**
   * @brief hanles event on navigating the menu
   * i.e. moving cursor on items, selecting/deselecting, etc...
   * 
   * @param e 
   */
  mui_event _menu_navigation(mui_event e);

  void _prev_page();

public:

  MuiPlusPlus();

  /**
   * @brief start menu from specified page and (optionally) selecting an item
   * 
   */
  void menuStart(muiItemId page, muiItemId item = 0);

  /**
   * @brief generate next available id for the item
   * todo: validate that id is free
   * 
   * @return uint32_t 
   */
  uint32_t nextIndex(){ return ++_items_index; }

  /**
   * @brief create new page
   * 
   * @param name - page label. Pointer MUST persist while page exists!
   * @param parent - parent page id, 0 - if it is a root page
   * @param options - options struct
   * @return muiItemId 
   */
  muiItemId makePage(const char* name = nullptr, muiItemId parent = 0, item_opts options = item_opts());

  /**
   * @brief assign item on a page as "autoselecting"
   * i.t. this item will get focus and autoselected when switching to this page
   * could be usefull for menu-lists, sinlge active items etc...
   * @param page_idx page index
   * @param item_id item index
   * @return mui_err_t return ok or err if either page or item does not exist or not bound
   */
  mui_err_t pageAutoSelect(muiItemId page_id, muiItemId item_id);

  mui_err_t addMuippItem(MuiItem_pt item, muiItemId page_id = 0);

  mui_err_t addMuippItem(MuiItem *item, muiItemId page_id = 0);

  //mui_err_t addMuippItem(MuiItem&& item, muiItemId page_id = 0);//{ addMuippItem( std::make_unique<MuiItem_pt>(std::move(item)), page_id); };

  mui_err_t addItemToPage(muiItemId item_id, muiItemId page_id);

  /**
   * @brief event sink
   * will receive events to menu current page / selected item, etc...
   * 
   * @param e input event struct
   * @return mui_event responce event struct
   */
  mui_event muiEvent(mui_event e);

  /**
   * @brief switch to specified page with id 'page' and (optionally) item id
   * 
   * @param page id to switch to
   * @param item id to switch to
   */
  mui_err_t goPageId(muiItemId page_id, muiItemId item_id = 0);

  /**
   * @brief switch to specified page by label
   * @note page search is 
   * @param page 
   */
  mui_err_t goPageLbl(const char* label);

  /**
   * @brief focus on specified item on a page
   * if item on a page is not found, focus on next available item
   * @param item_idx 
   * @return mui_err_t 
   */
  mui_err_t goItmId(muiItemId item_id);

  // before calling render on each items
  //void setPreExec();

  // render menu on screen
  void render();

  // after calling render items
  //void setPostExec();

// other private methods
private:

};

