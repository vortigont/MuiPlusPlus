/*
LOG macro will enable/disable logs to serial depending on LAMP_DEBUG build-time flag
*/
#pragma once

static constexpr const char* S_V = "V: ";
static constexpr const char* S_D = "D: ";
static constexpr const char* S_I = "I: ";
static constexpr const char* S_W = "W: ";
static constexpr const char* S_E = "E: ";

#ifndef MENU_DEBUG_PORT
#define MENU_DEBUG_PORT Serial
#endif

// undef possible LOG macros
#ifdef LOG
  #undef LOG
#endif
#ifdef LOGV
  #undef LOGV
#endif
#ifdef LOGD
  #undef LOGD
#endif
#ifdef LOGI
  #undef LOGI
#endif
#ifdef LOGW
  #undef LOGW
#endif
#ifdef LOGE
  #undef LOGE
#endif

#if defined(MENU_DEBUG_LEVEL) && MENU_DEBUG_LEVEL == 5
	#define LOGV(tag, func, ...) MENU_DEBUG_PORT.print(S_V); MENU_DEBUG_PORT.print(tag); MENU_DEBUG_PORT.print((char)0x9); MENU_DEBUG_PORT.func(__VA_ARGS__)
#else
	#define LOGV(...)
#endif

#if defined(MENU_DEBUG_LEVEL) && MENU_DEBUG_LEVEL > 3
	#define LOGD(tag, func, ...) MENU_DEBUG_PORT.print(S_D); MENU_DEBUG_PORT.print(tag); MENU_DEBUG_PORT.print((char)0x9); MENU_DEBUG_PORT.func(__VA_ARGS__)
#else
	#define LOGD(...)
#endif

#if defined(MENU_DEBUG_LEVEL) && MENU_DEBUG_LEVEL > 2
	#define LOGI(tag, func, ...) MENU_DEBUG_PORT.print(S_I); MENU_DEBUG_PORT.print(tag); MENU_DEBUG_PORT.print((char)0x9); MENU_DEBUG_PORT.func(__VA_ARGS__)
	// compat macro
	#define LOG(func, ...) MENU_DEBUG_PORT.func(__VA_ARGS__)
#else
	#define LOGI(...)
	// compat macro
	#define LOG(...)
#endif

#if defined(MENU_DEBUG_LEVEL) && MENU_DEBUG_LEVEL > 1
	#define LOGW(tag, func, ...) MENU_DEBUG_PORT.print(S_W); MENU_DEBUG_PORT.print(tag); MENU_DEBUG_PORT.print((char)0x9); MENU_DEBUG_PORT.func(__VA_ARGS__)
#else
	#define LOGW(...)
#endif

#if defined(MENU_DEBUG_LEVEL) && MENU_DEBUG_LEVEL > 0
	#define LOGE(tag, func, ...) MENU_DEBUG_PORT.print(S_E); MENU_DEBUG_PORT.print(tag); MENU_DEBUG_PORT.print((char)0x9); MENU_DEBUG_PORT.func(__VA_ARGS__)
#else
	#define LOGE(...)
#endif

// Per app macros
#if defined(ADC_DEBUG_LEVEL) && ADC_DEBUG_LEVEL > 3
	#define ADC_LOGD(func, ...) MENU_DEBUG_PORT.print(S_D); MENU_DEBUG_PORT.print(T_ADC); MENU_DEBUG_PORT.print((char)0x9); MENU_DEBUG_PORT.func(__VA_ARGS__)
#else
	#define ADC_LOGD(...)
#endif

#if defined(ADC_DEBUG_LEVEL) && ADC_DEBUG_LEVEL == 5
	#define ADC_LOGV(func, ...) MENU_DEBUG_PORT.print(S_V); MENU_DEBUG_PORT.print(T_ADC); MENU_DEBUG_PORT.print((char)0x9); MENU_DEBUG_PORT.func(__VA_ARGS__)
#else
	#define ADC_LOGV(...)
#endif

#if defined(CTRL_DEBUG_LEVEL) && CTRL_DEBUG_LEVEL == 5
	#define CTRL_LOGV(func, ...) MENU_DEBUG_PORT.print(S_V); MENU_DEBUG_PORT.print(T_CTRL); MENU_DEBUG_PORT.print((char)0x9); MENU_DEBUG_PORT.func(__VA_ARGS__)
#else
	#define CTRL_LOGV(...)
#endif

#if defined(PWM_DEBUG_LEVEL) && PWM_DEBUG_LEVEL == 5
	#define PWM_LOGV(func, ...) MENU_DEBUG_PORT.print(S_V); MENU_DEBUG_PORT.print(T_PWM); MENU_DEBUG_PORT.print((char)0x9); MENU_DEBUG_PORT.func(__VA_ARGS__)
#else
	#define PWM_LOGV(...)
#endif
