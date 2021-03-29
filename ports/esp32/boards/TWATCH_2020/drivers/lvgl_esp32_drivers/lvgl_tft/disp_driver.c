/**
 * @file disp_driver.c
 */

#include "disp_driver.h"
#include "disp_spi.h"

void disp_driver_init(void)
{
    st7789_init();
}

void disp_driver_flush(struct _disp_drv_t  *drv, const lv_area_t *area, lv_color_t *color_map)
{
    st7789_flush(drv, area, color_map);
}

