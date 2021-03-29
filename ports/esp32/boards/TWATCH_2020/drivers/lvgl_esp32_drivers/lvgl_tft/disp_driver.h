/**
 * @file disp_driver.h
 */

#ifndef DISP_DRIVER_H
#define DISP_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
//#include "lvgl/lvgl.h"
#include "lvgl/src/lv_hal/lv_hal_disp.h"
#include "st7789.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/* Initialize display */
void disp_driver_init(void);

/* Display flush callback */
void disp_driver_flush(struct _disp_drv_t  *drv, const lv_area_t *area, lv_color_t *color_map);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*DISP_DRIVER_H*/
