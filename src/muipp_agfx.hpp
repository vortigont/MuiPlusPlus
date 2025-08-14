#pragma once
#include <string>
#include "Arduino_GFX.h"
#include "canvas/Arduino_Canvas_Mono.h"
#include "muiplusplus.hpp"

/**
 * @brief aggregate carries options to print text 
 * 
 */
struct AGFX_text_t {
  uint16_t color, bgcolor;
  const uint8_t* font{nullptr};
  uint8_t font_size;
  muipp::text_align_t halign{muipp::text_align_t::left}, valign{muipp::text_align_t::baseline};
  bool transp_bg{true};
};

/**
 * @brief ArduinoGFX lib's generic item
 * 
 */
class MuiItem_AGFX_GenericTXT {
protected:
  // item's initial cursor position
  int16_t _x, _y;
  // text printing options
  AGFX_text_t cfg;

public:

  /**
   * @brief Construct a new MuiItem_U8g2_PageTitle object
   * generic object is NOT selectable!
   * @param id assigned id for the item
   * @param font use font for printing, if null, then do not switch font
   * @param x, y Coordinates of the top left corner to start printing
   */
  MuiItem_AGFX_GenericTXT(int16_t x = 0, int16_t y = 0, AGFX_text_t tcfg = {}) : _x(x), _y(y), cfg(tcfg) {};

  int16_t getX() const { return _x; }

  int16_t getY() const { return _y; }

  // adjust cursor position
  void setCursor( int16_t x, int16_t  y){ x = _x; _y = y; }

  // adjust text alignment
  void setTextAlignment(muipp::text_align_t hAlign, muipp::text_align_t vAlign){ cfg.halign = hAlign; cfg.valign = vAlign; }

  /**
   * @brief adjusted current cursor's position to print provided text acording to text alignment parameters (v_align h_align)
   * current cursor postion is considered to be at lower left corner of the area to print text to
   * @note this call must be followed by text 'print' call
   * 
   * @param text 
   * 
   */
  void alignText(Arduino_GFX* g, int16_t w, int16_t h, muipp::text_align_t halign, muipp::text_align_t valign, const char* text);
  void alignText(Arduino_GFX* g, int16_t w, int16_t h, const char* text){ alignText(g, w, h, cfg.halign, cfg.valign, text); };
  void alignText(Arduino_GFX* g, const char* text){ alignText(g, g->width(), g->height(), cfg.halign, cfg.valign, text); };
};

/**
 * @brief displays static text at specific place
 * @note prints a text by pointer! Text must be persistent in memory (intended to print strings from ROM)
 * 
 */
class MuiItem_AGFX_StaticText : public MuiItem_AGFX_GenericTXT, public MuiItem_Uncontrollable {
  // text block
  int16_t  xx{0}, yy{0};
  uint16_t ww{0}, hh{0};

public:
  /**
   * @brief Construct a new MuiItem_U8g2_PageTitle object
   * 
   * @param u8g2 refernce to display object
   * @param id assigned id for the item
   * @param font use font for printing, if null, then do not switch font
   * @param x, y Coordinates of the top left corner to start printing
   */
  MuiItem_AGFX_StaticText(muiItemId id, const char* text, u8g2_uint_t x = 0, u8g2_uint_t y = 0, AGFX_text_t tcfg = {})
    : MuiItem_AGFX_GenericTXT(x, y, tcfg),
      MuiItem_Uncontrollable(id, text) {};

  void render(const MuiItem* parent, void* r = nullptr) override;
};
  

class MuiItem_AGFX_TextCallBack : public MuiItem_AGFX_GenericTXT, public MuiItem_Uncontrollable {
  muipp::string_cb_t _cb;
public:
  /**
   * @brief Construct a new MuiItem_U8g2_PageTitle object
   * 
   * @param id assigned id for the item
   * @param callback - callbak that returns text to draw
   * @param x, y Coordinates of the top left corner to start printing
   * @param font use font for printing, if null, then do not switch current font
   */
  MuiItem_AGFX_TextCallBack(muiItemId id, muipp::string_cb_t callback,
      int16_t x = 0, int16_t y = 0,
      AGFX_text_t tcfg = {})
        : MuiItem_AGFX_GenericTXT(x, y, tcfg),
          MuiItem_Uncontrollable(id), _cb(callback) {};

  void render(const MuiItem* parent, void* r = nullptr) override;
};


class CanvasTextScroller {
public:
  CanvasTextScroller(uint16_t w, uint16_t h) : _c(w, h, nullptr) { _c.begin(); }    // canvas will malloc in c-tor

  void begin(const char* text, float speed, const uint8_t* font);

  /**
   * @brief checks if scroll redraw is peding
   * if true, than a subsecuent call to scroll() is requred to actually render the text
   * 
   */
  bool scroll_pending() const;

  bool scroll();

  const uint8_t* getFramebuffer(){ return _c.getFramebuffer(); }

  int16_t getW() const { return _c.width(); }
  int16_t getH() const { return _c.height(); }

private:
  Arduino_Canvas_Mono _c;
  float _speed;    // pixels per ms
  std::string _text;

  // text block
  int16_t  xx, yy;
  uint16_t ww, hh;

  float _xPos;   // current X position
  unsigned long _lastUpdate;

  // 
  /**
   * @brief optimized drawing only visibile chars
   * @note does not work properly with unicode
   * 
   */
  void _drawVisible();
  void _drawall();
};

class MuiItem_AGFX_TextScroller : public MuiItem_Uncontrollable {
  CanvasTextScroller _scroller;
  int16_t _x, _y;
  AGFX_text_t _tcfg;
public:
  /**
   * @brief Construct a new MuiItem_U8g2_PageTitle object
   * 
   * @param id assigned id for the item
   * @param x, y Coordinates of the top left corner to start printing
   * @param w, h canvas size where srolled text will be printed
   * @param speed - speed of scrolling, pixel per second
   * @param tcfg text decoration config
   */
  MuiItem_AGFX_TextScroller(muiItemId id,
      int16_t x, int16_t y,
      uint16_t w, uint16_t h,
      AGFX_text_t tcfg = {})
        : MuiItem_Uncontrollable(id), _scroller(w, h), _x(x), _y(y), _tcfg(tcfg) {};

  void setText(const char* text, float speed){ _scroller.begin(text, speed, _tcfg.font); }
  void render(const MuiItem* parent, void* r = nullptr) override;
  bool refresh_req() const override { return _scroller.scroll_pending(); };
};
