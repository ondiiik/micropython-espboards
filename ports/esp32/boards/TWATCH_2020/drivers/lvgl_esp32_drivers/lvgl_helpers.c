/**
 * @file lvgl_helpers.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "cfg.h"
#include "lvgl_helpers.h"
#include "esp_log.h"

#include "lvgl_tft/disp_spi.h"

#include "lvgl_spi_conf.h"

#include "lvgl/src/lv_core/lv_refr.h"

/*********************
 *      DEFINES
 *********************/

 #define TAG "lvgl_helpers"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/* Interface and driver initialization */
void lvgl_driver_init(void)
{
    ESP_LOGI(TAG, "Display hor size: %d, ver size: %d", LV_HOR_RES_MAX, LV_VER_RES_MAX);
    ESP_LOGI(TAG, "Display buffer size: %d", DISP_BUF_SIZE);
    ESP_LOGI(TAG, "Initializing SPI master for display");
    lvgl_spi_driver_init(TFT_SPI_HOST,
        DISP_SPI_MISO, DISP_SPI_MOSI, DISP_SPI_CLK,
        SPI_BUS_MAX_TRANSFER_SZ, 1,
        -1, -1);
    disp_spi_add_device(TFT_SPI_HOST);
    disp_driver_init();
}


/* Initialize spi bus master */
bool lvgl_spi_driver_init(int host,
    int miso_pin, int mosi_pin, int sclk_pin,
    int max_transfer_sz,
    int dma_channel,
    int quadwp_pin, int quadhd_pin)
{
    assert((SPI_HOST <= host) && (VSPI_HOST >= host));

    const char *spi_names[] = {
        "SPI_HOST", "HSPI_HOST", "VSPI_HOST"
    };

    ESP_LOGI(TAG, "Configuring SPI host %s (%d)", spi_names[host], host);
    ESP_LOGI(TAG, "MISO pin: %d, MOSI pin: %d, SCLK pin: %d",
        miso_pin, mosi_pin, sclk_pin);
    ESP_LOGI(TAG, "Max transfer size: %d (bytes)", max_transfer_sz);

    spi_bus_config_t buscfg = {
        .miso_io_num = miso_pin,
	.mosi_io_num = mosi_pin,
	.sclk_io_num = sclk_pin,
	.quadwp_io_num = quadwp_pin,
	.quadhd_io_num = quadhd_pin,
        .max_transfer_sz = max_transfer_sz
    };

    ESP_LOGI(TAG, "Initializing SPI bus...");
    esp_err_t ret = spi_bus_initialize(host, &buscfg, dma_channel);
    assert(ret == ESP_OK);

    return ESP_OK != ret;
}

