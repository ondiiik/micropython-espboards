/**
 * @file touch_driver.h
 */

#ifndef TOUCH_DRIVER_H
#define TOUCH_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdbool.h>
//#include "lvgl/lvgl.h"
#include "lvgl/src/lv_hal/lv_hal_indev.h"
#include "ft6x36.h"

/*********************
*      DEFINES
*********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void touch_driver_init(void);
bool touch_driver_read(lv_indev_drv_t *drv, lv_indev_data_t *data);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TOUCH_DRIVER_H */

