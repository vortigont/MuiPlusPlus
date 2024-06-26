/*
    This file is a part of ESPIron-PTS200 project
    https://github.com/vortigont/ESPIron-PTS200

    Copyright © 2024 Emil Muratov (vortigont)

    ESPIron-PTS200 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/
#pragma once
#include <array>


#define MENU_MAIN_CFG_SIZE          6
#define MENU_TEMPERATURE_CFG_SIZE   5
#define MENU_TIMEOUTS_CFG_SIZE      5
#define MENU_PWR_CONTROL_CFG_SIZE   4


#define MAINSCREEN_FONT             u8g2_font_unifont_t_cyrillic

// 12x16 wrong? small gothic font
#define SMALL_TEXT_FONT             u8g2_font_glasstown_nbp_t_all
#define SMALL_TEXT_FONT_Y_OFFSET    12

// 12x16 wrong? small gothic font
#define MAIN_MENU_FONT1             u8g2_font_glasstown_nbp_t_all

// 10x14
#define MAIN_MENU_FONT2             u8g2_font_bauhaus2015_tr    //  u8g2_font_smart_patrol_nbp_tr

// 10x14 OK! thin, quite readable
#define MAIN_MENU_FONT3             u8g2_font_unifont_t_cyrillic

// 11x15    nice bold font
#define PAGE_TITLE_FONT             u8g2_font_bauhaus2015_tr
#define PAGE_TITLE_FONT_Y_OFFSET    15

// 12x16 wrong? small gothic font
#define PAGE_TITLE_FONT_SMALL       u8g2_font_glasstown_nbp_t_all

// large numeric font for digits sliders
#define NUMERIC_FONT1               u8g2_font_profont29_tn


// 12x17
//#define MAIN_MENU_FONT3     u8g2_font_shylock_nbp_t_all
// 7x7
//#define MAIN_MENU_FONT3     u8g2_font_mercutio_sc_nbp_t_all

#define MAIN_MENU_X_OFFSET  10
#define MAIN_MENU_Y_OFFSET  15
#define MAIN_MENU_Y_SHIFT   16
#define MAIN_MENU_ROWS      3
#define MENU_LIST_Y_OFFSET  25

#define PAGE_BACK_BTN_X_OFFSET  100
#define PAGE_BACK_BTN_Y_OFFSET  55

#define NUMBERSLIDE_X_OFFSET    10

#define PWR_PD_VALUE_OFFSET     90


// Default temperature control value (recommended soldering temperature: 300~380°C)
#define TEMP_DEFAULT      220   // 默认温度
#define TEMP_MIN          150   // 最小温度
#define TEMP_MAX          450   // 最大温度
#define TEMP_STEP         5     // temperature change step / 旋转编码器温度变化步进
#define TEMP_NOTIP        500   // virtual temperature threshold when tip is not installed (op amp max ~650 C)
#define TEMP_STANDBY      120   // 休眠温度
#define TEMP_STANDBY_MIN  100
#define TEMP_STANDBY_MAX  180
#define TEMP_STANDBY_STEP 10
#define TEMP_BOOST        50    // 升温步进
#define TEMP_BOOST_MIN    30
#define TEMP_BOOST_MAX    100
#define TEMP_BOOST_STEP   10


/**
 * Text-Dictionary Enums for language resources
 * the order of enums MUST match with elements in dictionary array
 */
enum dict_index {
    D_Boost = (0),
    D_Error,
    D_Disabled,
    D_Heating,
    D_idle,
    D_min,
    D_none,
    D_NoTip,
    D_OK,
    D_Ramping,
    D_PwrRamp_label,
    D_PwrRamp_hint,
    D_return,
    D_PDVoltage,
    D_QCMode,
    D_QCVoltage,
    D_sec,
    D_SaveLast_box,
    D_SaveLast_hint,
    D_Settings,
    D_set_t,
    D_Standby,
// Notes goes below
    D_Note_QCWarn,
    D_Temp_SaveLastWrkDescr,
    DICT___SIZE
};


// put common international unicode strings here

static constexpr const char* T_CelsiusChar = "°C";



// EN-US
namespace lang_en_us {

// Iron Modes (On main screen)
static constexpr const char* T_Idle         = "Idle";               // state display
static constexpr const char* T_Heating      = "Heating";            // state display
static constexpr const char* T_Standby      = "Standby";            // state display in 'Standby'
static constexpr const char* T_Boost        = "Boost";              // state display
static constexpr const char* T_Ramping      = "Ramping!";           // state display power ramp
static constexpr const char* T_NoTip        = "No tip!";            // state display when tip is missing

// Others
static constexpr const char* T_Disabled     = "Disabled";           // disabled option
static constexpr const char* T_Error        = "Error";
static constexpr const char* T_min          = "min.";               // short for 'minutes'
static constexpr const char* T_none         = "none";               // none option
static constexpr const char* T_OK           = "OK";                 // OK label/message
static constexpr const char* T_PDVoltage    = "PD Voltage:";        // PowerDelivery trigger voltage
static constexpr const char* T_QCMode       = "QC Mode:";           // QC trigger mode
static constexpr const char* T_QCVoltage    = "QC Voltage:";        // QC trigger voltage
static constexpr const char* T_sec          = "sec.";               // short for 'seconds'
static constexpr const char* T_setT         = "Set:";               // Target temperature on main screen
static constexpr const char* T_return       = "<Back";              // return back in menu's

// Menu Page names
static constexpr const char* T_Settings = "Settings";

// Main configuration menu
static constexpr const char* T_TempSettings = "Temperature";
static constexpr const char* T_TimersSettings = "Timeouts";
static constexpr const char* T_TipSettings = "Tip";
static constexpr const char* T_PowerSupply = "Power Supply";
static constexpr const char* T_Information = "Information";
static constexpr const char* T_Language = "Language";

// Temperature settings menu
static constexpr const char* T_SaveLastT = "Save work temp.";
static constexpr const char* T_TempDefltWrk = "Working temp.";
static constexpr const char* T_TempStandby = "Standby temp.";
static constexpr const char* T_TempBoost = "Boost-up temp.";
// Temperature settings hints
static constexpr const char* T_SaveLast_box = "use last temp.";
static constexpr const char* T_SaveLast_hint = "instead of default one";

// Timeouts settings menu
static constexpr const char* T_TimeStandby = "Standby time";
static constexpr const char* T_TimeIdle = "Idle timeout";
static constexpr const char* T_TimeSuspend = "Suspend time";
static constexpr const char* T_TimeBoost = "Boost timeout";

// Power Supply settings menu
static constexpr const char* T_PwrPD = "PD Trigger";
static constexpr const char* T_PwrQC = "QC Trigger";
static constexpr const char* T_PwrRamp = "Power Ramp";


//  **** Descriptions and Notes ****

// Temp settings descr
static constexpr const char* T_Temp_SaveLastWrkDescr = "Save last work Temperature";
// QC feature warning
static constexpr const char* T_Note_QCWarn = "QC trigger is experimental!!!\nMight work unstable! Do unplug 'n replug the Iron on QC-mode change!";
// PWM Ramping settings hints
static constexpr const char* T_PwrRamp_label = "Use PWM Ramp";
static constexpr const char* T_PwrRamp_hint = "Smooth power-up to prevent PSU protection kick-in";



} // end  of namespace lang_en_us

/**
 * @brief general purpose dictionary
 * order of items MUST match enum dict_index
 * 
 */
static constexpr std::array<const char *, DICT___SIZE> dictionary = {
    lang_en_us::T_Boost,
    lang_en_us::T_Error,
    lang_en_us::T_Disabled,
    lang_en_us::T_Heating,
    lang_en_us::T_Idle,
    lang_en_us::T_min,
    lang_en_us::T_none,
    lang_en_us::T_NoTip,
    lang_en_us::T_OK,
    lang_en_us::T_Ramping,
    lang_en_us::T_PwrRamp_label,
    lang_en_us::T_PwrRamp_hint,
    lang_en_us::T_return,
    lang_en_us::T_PDVoltage,
    lang_en_us::T_QCMode,
    lang_en_us::T_QCVoltage,
    lang_en_us::T_sec,
    lang_en_us::T_SaveLast_box,
    lang_en_us::T_SaveLast_hint,
    lang_en_us::T_Settings,
    lang_en_us::T_setT,
    lang_en_us::T_Standby,
// Notes goes below
    lang_en_us::T_Note_QCWarn,
    lang_en_us::T_Temp_SaveLastWrkDescr
};

// Main Configuration menu items
static constexpr std::array<const char *, MENU_MAIN_CFG_SIZE> menu_MainConfiguration = {
    lang_en_us::T_TempSettings,
    lang_en_us::T_TimersSettings,
    lang_en_us::T_TipSettings,
    lang_en_us::T_PowerSupply,
    lang_en_us::T_Information,
    lang_en_us::T_return
};

// Temperature menu items
static constexpr std::array<const char *, MENU_TEMPERATURE_CFG_SIZE> menu_TemperatureOpts = {
    lang_en_us::T_TempDefltWrk,
    lang_en_us::T_TempStandby,
    lang_en_us::T_TempBoost,
    lang_en_us::T_SaveLastT,
    lang_en_us::T_return
};

// Timeout menu items
static constexpr std::array<const char *, MENU_TIMEOUTS_CFG_SIZE> menu_TimeoutOpts = {
    lang_en_us::T_TimeStandby,
    lang_en_us::T_TimeIdle,
    lang_en_us::T_TimeSuspend,
    lang_en_us::T_TimeBoost,
    lang_en_us::T_return
};

// Power Control menu items
static constexpr std::array<const char *, MENU_PWR_CONTROL_CFG_SIZE> menu_PwrControlOpts = {
    lang_en_us::T_PwrPD,
    lang_en_us::T_PwrQC,
    lang_en_us::T_PwrRamp,
    lang_en_us::T_return
};

// QC Selector
static constexpr std::array<const char *, 3> menu_QCFunctionOpts = {
    lang_en_us::T_none,
    "QC3",
    "QC2"
};
