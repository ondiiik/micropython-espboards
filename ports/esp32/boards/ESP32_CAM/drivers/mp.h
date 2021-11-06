/*
 * mp.h
 *
 *  Created on: Jun 12, 2020
 *      Author: ondiiik
 */
#pragma once


#ifdef __cplusplus
extern "C" {
#endif

#include "py/mpconfig.h"

#include "py/misc.h"
#include "py/mpprint.h"
#include "py/runtime.h"

#include "modcampy.h"
#include "mp_namespace.h"

#ifdef __cplusplus
}
#endif


#define MP_LOG(_type_, _color_, _tag_, ...) \
    do \
    { \
        mp_printf(&mp_plat_print, "[%s] %s:%d", _type_, _tag_, __LINE__); \
        mp_printf(&mp_plat_print, " :: "_color_ __VA_ARGS__); \
        mp_printf(&mp_plat_print, MP_DEFAULT"\n"); \
    } \
    while (false)

#define MP_LOGE(_tag_, ...) MP_LOG(MP_BOLD(RED)   "!!" MP_DEFAULT, MP_BOLD(RED),   _tag_, __VA_ARGS__)
#define MP_LOGW(_tag_, ...) MP_LOG(MP_BOLD(BROWN) "**" MP_DEFAULT, MP_BOLD(BROWN), _tag_, __VA_ARGS__)
#define MP_LOGI(_tag_, ...) MP_LOG(MP_BOLD(GREEN) ".." MP_DEFAULT, MP_BOLD(WHITE), _tag_, __VA_ARGS__)
#define MP_LOGD(_tag_, ...) MP_LOG(               ".."           ,               , _tag_, __VA_ARGS__)
#define MP_LOGV(_tag_, ...) MP_LOG(               "  "           ,               , _tag_, __VA_ARGS__)

#define MP_COLOR_BLACK   "30"
#define MP_COLOR_RED     "31"
#define MP_COLOR_GREEN   "32"
#define MP_COLOR_BROWN   "33"
#define MP_COLOR_BLUE    "34"
#define MP_COLOR_PURPLE  "35"
#define MP_COLOR_CYAN    "36"
#define MP_COLOR_WHITE   "37"
#define MP_NORMAL(COLOR) "\033[0;" MP_COLOR_##COLOR "m"
#define MP_BOLD(COLOR)   "\033[1;" MP_COLOR_##COLOR "m"
#define MP_DEFAULT       "\033[0m"
