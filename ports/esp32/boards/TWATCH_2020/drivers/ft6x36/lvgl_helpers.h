/**
 * @file lvgl_helpers.h
 */

#ifndef LVGL_HELPERS_H
#define LVGL_HELPERS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>
#include "cfg.h"
#include "lvgl_touch/touch_driver.h"

/*********************
 *      DEFINES
 *********************/
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/* Initialize detected SPI and I2C bus and devices */
void lvgl_touch_init(void);

/* Initialize I2C master  */
bool lvgl_i2c_driver_init(int port, int sda_pin, int scl_pin, int speed);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LVGL_HELPERS_H */
