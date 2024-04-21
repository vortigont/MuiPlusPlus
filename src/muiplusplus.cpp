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


muiItem_id MuiPlusPlus::makePage(const char* name, item_opts options){
  //auto z = std::make_unique<MuiPage>(++_items_index, name, options);
  //pages.emplace_back(std::move(z));
  pages.emplace_back(MuiPage(++_items_index, name, options));
  return _items_index;
}

mui_err_t MuiPlusPlus::addMuippItem(MuiItem_pt item, muiItem_id page_id){
  Serial.println("Adding item");
  auto i = std::find_if(items.cbegin(), items.cend(), MatchID<MuiItem_pt>(item->id));
  if ( i != items.cend() ){
    Serial.printf("item:%u already exist!\n", item->id);
    return mui_err_t::id_exist;
  }

  // link item with the specified page
  if (page_id){
    mui_err_t err = addItemToPage(item->id, page_id);
    if (err != mui_err_t::ok) return err;
  }

  // add item to container
  items.emplace_back(std::move(item));
  return mui_err_t::ok;
}

mui_err_t MuiPlusPlus::addItemToPage(muiItem_id item_id, muiItem_id page_id){
  if (!page_id || !page_id) return mui_err_t::id_err;

  // check if such page exist
  auto p = _page_by_id(page_id);
  if ( p == pages.cend() )
    return mui_err_t::id_err;

  // I do not care if such item does not exist NOW, it won't be rendered/found after, so not a big deal
/*
  auto i = std::find_if(items.cbegin(), items.cend(), MatchID<MuiItem_pt>(item_id));
  if ( i == items.cend() ){
    return mui_err_t::id_err;
  }
*/
  (*p).items.push_back(item_id);
  //(*p).addMuippItem(item_id);
  Serial.printf("Added item:%u to page:%u\n", item_id, page_id);
  return mui_err_t::ok;
}

void MuiPlusPlus::menuStart(muiItem_id page, muiItem_id item){
  // switch to page, if error, then select first page by default
  if( switchTo(page, item) != mui_err_t::ok){
    prevPage = currentPage = pages.begin();
    item_active = false;
  }
}

mui_err_t MuiPlusPlus::switchTo(muiItem_id page, muiItem_id item){
  auto p = _page_by_id(page);
  if ( p == pages.cend() ){
    return mui_err_t::id_err;
  }
  prevPage = currentPage;
  currentPage = p;

  // if I need to switch to specific item on a page, let's check if it is registered there
  if (item){
    auto it = std::find( (*currentPage).items.begin(), (*currentPage).items.end(), item );
    if (it != (*currentPage).items.end()){
      focusedItem = _item_by_id(item);
      if (focusedItem != items.end()){
        // OK, item is indeed found, we are happy
        item_active = true;
        return mui_err_t::ok;
      }
    }
  }
  /*
    else item is not found or == 0.
    Let's just focus on a first item on a page. If this item is passive, then it will
    simply do nothing and next cursor move events will switch to a proper one
  */
  focusedItem = _item_by_id((*currentPage).items.at(0)); // std::find_if(items.begin(), items.end(), MatchID<MuiItem_pt>((*currentPage).items.at(0)));
  item_active = false;
  return mui_err_t::ok;
}


void MuiPlusPlus::render(){
  // won't run with no pages or items
  if (!pages.size() || !items.size())
    return;

  //Serial.printf("Render %u items on page:%u\n", (*currentPage).items.size(), (*currentPage).id);

  // render each item on a page
  for (auto id : (*currentPage).items ){
    // find Item with specified id
    auto itm = std::find_if(items.begin(), items.end(), MatchID<MuiItem_pt>(id));
    if ( itm == items.end() )
      continue;                 // item not found, proceed with next one
    
    //Serial.printf("Render item:%u\n", id);
    // render selected item passing it a reference to current page
    (*itm)->render(&(*currentPage));
  }

}

mui_event MuiPlusPlus::muiEvent(mui_event e){
  Serial.printf("MPP event:%u\n", static_cast<uint32_t>(e.eid));
  if (e.eid == mui_event_t::noop) return e;

  // if current Item is focused and active - pass navigation and value events
  // there and process reply event
  if (item_active && focusedItem != items.end()){
    if (static_cast<size_t>(e.eid) < 100 || static_cast<size_t>(e.eid) >= 200 )
        return _feedback_event((*focusedItem)->muiEvent(e));
  }

  // otherwise pass event to menu navigation function
  if (static_cast<size_t>(e.eid) < 200)
    return _menu_navigation(e);

  return {};
}

mui_event MuiPlusPlus::_menu_navigation(mui_event e){
  return {};
  switch(e.eid){
    // cursor actions
    case mui_event_t::moveUp :
    case mui_event_t::moveLeft :

    case mui_event_t::moveDown :
    case mui_event_t::moveRight :
    case mui_event_t::enter : {
    }
  }

}

  mui_event MuiPlusPlus::_feedback_event(mui_event e){
    return {};
  }







