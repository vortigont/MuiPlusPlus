#include "muiplusplus.hpp"
#include <algorithm>

/*
void MuiPage::add(MuiItem_pt&& item){
  // remove exiting items with specified id if present
  auto i = std::find_if(container.cbegin(), container.cend(), MatchLabel<MuiPage_pt>(item->getid));
  if ( i != container.cend() ){
    container.erase(i);
  }
  container.emplace_back(item);
  // invalidate iterators
  currentItem = container.begin();

}

void MuiPlusPlus::add(MuiPage_pt&& page){
  // remove exiting page with specified id if present
  auto i = std::find_if(container.cbegin(), container.cend(), MatchLabel<MuiPage_pt>(page->getid));
  if ( i != container.cend() ){
    container.erase(i);
  }
  container.emplace_back(page);
  // invalidate iterators
  currentItem = prevItem = container.begin();
}
*/

MuiPlusPlus::MuiPlusPlus(){
  // invalidate iterator
  currentPage = pages.end();
}

muiItemId MuiPlusPlus::makePage(const char* name, muiItemId parent, item_opts options){
  //auto z = std::make_unique<MuiPage>(++_items_index, name, options);
  //pages.emplace_back(std::move(z));
  pages.emplace_back(MuiPage(++_pages_index, name, parent, options));
  return _pages_index;
}

mui_err_t MuiPlusPlus::addMuippItem(MuiItem_pt item, muiItemId page_id){
  Serial.printf("Adding item %u, page %u\n", item->id, page_id);
  auto i = std::find_if(items.cbegin(), items.cend(), MatchID<MuiItem_pt>(item->id));
  if ( i != items.cend() ){
    Serial.printf("item:%u already exist!\n", item->id);
    return mui_err_t::id_exist;
  }

  // add item to container
  //items.emplace_back(std::move(item));
  items.emplace_back(item);

  // link item with the specified page
  if (page_id){
    mui_err_t err = addItemToPage(item->id, page_id);
    if (err != mui_err_t::ok) return err;
  }

  return mui_err_t::ok;
}


mui_err_t MuiPlusPlus::addMuippItem(MuiItem *item, muiItemId page_id){
  MuiItem_pt p(item);
  return addMuippItem(std::move(p), page_id);
}


mui_err_t MuiPlusPlus::addItemToPage(muiItemId item_id, muiItemId page_id){
  if (!item_id || !page_id) return mui_err_t::id_err;

  // check if such page exist
  auto p = _page_by_id(page_id);
  if ( p == pages.cend() ){
    Serial.printf("page:%u not found\n", page_id);
    return mui_err_t::id_err;
  }

  // I do not care if such item does not exist NOW, it won't be rendered/found after, so not a big deal
//
  //auto i = std::find_if(items.cbegin(), items.cend(), MatchID<MuiItem_pt>(item_id));
  auto i = _item_by_id(item_id);
  if ( i == items.end() ){
    Serial.printf("item:%u not found\n", item_id);
    return mui_err_t::id_err;
  }
//
  (*p).items.emplace_back((*i));
  Serial.printf("bound item:%u to page:%u\n", item_id, page_id);
  return mui_err_t::ok;
}

void MuiPlusPlus::menuStart(muiItemId page, muiItemId item){
  // switch to page, if error, then select first page by default
  if( goPageId(page, item) != mui_err_t::ok){
    currentPage = pages.begin();
    (*currentPage).itm_selected = false;
  }
}


mui_err_t MuiPlusPlus::goPageId(muiItemId page_id, muiItemId item_id){
  Serial.printf("goPageId:%u,%u\n", page_id, item_id);
  auto p = _page_by_id(page_id);
  // check if no such page or page has no any items at all?
  if ( p == pages.end() || !(*p).items.size() ){
    return mui_err_t::id_err;
  }

  // unfocus and notify current item if it is defined
  if ( (currentPage != pages.end()) && ((*currentPage).currentItem != (*currentPage).items.end()) && (*(*currentPage).currentItem)->focused ){
    (*(*currentPage).currentItem)->focused = false;
    (*(*currentPage).currentItem)->muiEvent(mui_event(mui_event_t::unfocus));
  }

  currentPage = p;

  // try to focus and select specified item on a page
  if ( item_id && (goItmId(item_id) == mui_err_t::ok) ) return mui_err_t::ok;

  /*
    else item is not found or == 0.
    Let's check if autoselect item is not a zero on a page, then focus and select it
  */
  if ((*currentPage).autoSelect && (goItmId((*currentPage).autoSelect) == mui_err_t::ok) ){
    (*currentPage).itm_selected = true;
    return mui_err_t::ok;
  }

  // Otherwise, let's just focus on a first item on a page. If this item is passive, then it will simply do nothing and next cursor move events will switch to a proper one
  (*currentPage).currentItem = (*currentPage).items.begin(); // _item_by_id((*currentPage).items.front());
  (*currentPage).itm_selected = false;
  return mui_err_t::ok;
}


mui_err_t MuiPlusPlus::goPageLbl(const char* label){
  auto p = _page_by_label(label);
  if ( p != pages.cend() ){
    return goPageId((*p).id);
  }
  return mui_err_t::id_err;
}

mui_err_t MuiPlusPlus::goItmId(muiItemId item_id){
  if (!item_id) return mui_err_t::id_err;
  // if I need to switch to specific item on a page, let's check if it is registered there
  auto it = std::find_if( (*currentPage).items.begin(), (*currentPage).items.end(), MatchID<MuiItem_pt>(item_id) );
  if (it == (*currentPage).items.end()) return mui_err_t::id_err;

  // OK, item is indeed found, we are happy

  // unfocus and notify current item if it is defined
  if ( (currentPage != pages.end()) && ((*currentPage).currentItem != (*currentPage).items.end()) && (*(*currentPage).currentItem)->focused ){
    (*(*currentPage).currentItem)->focused = false;
    (*(*currentPage).currentItem)->muiEvent(mui_event(mui_event_t::unfocus));
  }

  (*currentPage).currentItem = it;
  // check if item is selectable, then foucus on it and select it
  if ((*it)->getSelectable()){
    (*currentPage).itm_selected = true;
    // update new item's focus flag
    (*it)->focused = true;
    // notify item that it received focus
    (*it)->muiEvent(mui_event(mui_event_t::focus));
  }
  return mui_err_t::ok;
}


void MuiPlusPlus::render(){
  // won't run with no pages or items
  if (!pages.size() || !items.size())
    return;

  //Serial.printf("Render %u items on page:%u\n", (*currentPage).items.size(), (*currentPage).id);

  // render each item on a page
  for (auto itm : (*currentPage).items ){
    // find Item with specified id
    //auto itm = std::find_if(items.begin(), items.end(), MatchID<MuiItem_pt>(id));
    //if ( itm == items.end() )
    //  continue;                 // item not found, proceed with next one
    
    //Serial.printf("Render item:%u\n", id);
    // render selected item passing it a reference to current page
    (*itm).render(&(*currentPage));
  }

}

mui_event MuiPlusPlus::muiEvent(mui_event e){
  Serial.printf("MPP event:%u\n", static_cast<uint32_t>(e.eid));
  if (e.eid == mui_event_t::noop) return e;

  // if focused Item on current page exist and active - pass navigation and value events there and process reply event
  if ( (*currentPage).currentItem != (*currentPage).items.cend() && (*currentPage).itm_selected ){
    if (static_cast<size_t>(e.eid) < 100 || static_cast<size_t>(e.eid) >= 200 )
      _feedback_event( (* (*currentPage).currentItem )->muiEvent(e) );
    return {};
  }

  // otherwise pass event to menu navigation function
  if (static_cast<size_t>(e.eid) < 200)
    return _menu_navigation(e);

  return {};
}

mui_event MuiPlusPlus::_menu_navigation(mui_event e){
  // do not work on empty pages (for now)
  if ( (*currentPage).items.size() == 0 ) return {};

  switch(e.eid){
    // cursor actions

    // focus on previous item on a page
    case mui_event_t::moveUp :
    case mui_event_t::moveLeft :
      (*(*currentPage).currentItem)->focused = false;
      // notify current item that it has lost focus
      (*(*currentPage).currentItem)->muiEvent(mui_event(mui_event_t::unfocus));
      // focus on previous item
      if ((*currentPage).currentItem != (*currentPage).items.begin() && (*currentPage).currentItem != (*currentPage).items.end())
        --(*currentPage).currentItem;
      else  // if there is no prev item, switch to last one
        (*currentPage).currentItem = std::prev((*currentPage).items.end());
      // update focus flag
      (*(*currentPage).currentItem)->focused = true;
      // notify item that it received focus
      _feedback_event( (*(*currentPage).currentItem)->muiEvent(mui_event(mui_event_t::focus)) );
      break;

    // focus on next item on a page
    case mui_event_t::moveDown :
    case mui_event_t::moveRight :
      (*(*currentPage).currentItem)->focused = false;
      // notify current item that it has lost focus
      _feedback_event( (*(*currentPage).currentItem)->muiEvent(mui_event(mui_event_t::unfocus)) );
      // move focus on next item
      ++(*currentPage).currentItem;
      if ((*currentPage).currentItem == (*currentPage).items.end())
        (*currentPage).currentItem = (*currentPage).items.begin();

      // update focus flag
      (*(*currentPage).currentItem)->focused = true;
      // notify item that it received focus
      _feedback_event( (*(*currentPage).currentItem)->muiEvent(mui_event(mui_event_t::focus)) );
      break;

    // activate focused item, i.e. it will start receiving events from Menu
    case mui_event_t::enter :
      (*currentPage).itm_selected = true;
      (*(*currentPage).currentItem)->selected = true;
      _feedback_event( (*(*currentPage).currentItem)->muiEvent(mui_event(mui_event_t::select)) );
      break;
  }
  return {};
}

void MuiPlusPlus::_feedback_event(mui_event e){
  Serial.printf("_feedback_event:%u\n", static_cast<uint32_t>(e.eid));
  switch(e.eid){
    case mui_event_t::goPageByName :
      goPageLbl(static_cast<const char*>(e.arg) );
      break;
    // 
    case mui_event_t::prevPage :
      _prev_page();
      break;
    break;
  }
}

void MuiPlusPlus::_prev_page(){
  // check if current page has any parent page 
  if ( (*currentPage).parent_page == 0 ){
    return;
  }

  goPageId((*currentPage).parent_page);
}

mui_err_t MuiPlusPlus::pageAutoSelect(muiItemId page_id, muiItemId item_id){
  Serial.printf("pageAutoSelect:%u,%u\n", page_id, item_id);
  auto p = _page_by_id(page_id);
  if ( p == pages.cend() ){
    return mui_err_t::id_err;
  }

  auto it = std::find_if( (*p).items.begin(), (*p).items.end(), MatchID<MuiItem_pt>(item_id) );
  if (it != (*p).items.end()){
    // OK, item is indeed found, we are happy
    (*p).autoSelect = item_id;
    return mui_err_t::ok;
  }

  return mui_err_t::id_err;
}





