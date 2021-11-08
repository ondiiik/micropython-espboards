// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "time.h"
#include "sys/time.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "soc/soc.h"
#include "soc/gpio_sig_map.h"
#include "soc/i2s_reg.h"
#include "soc/i2s_struct.h"
#include "soc/io_mux_reg.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "driver/periph_ctrl.h"
#include "esp_intr_alloc.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "sensor.h"
#include "sccb.h"
#include "esp_camera.h"
#include "camera_common.h"
#include "xclk.h"
#include "ov2640.h"

typedef enum
{
    CAMERA_NONE = 0,
    CAMERA_UNKNOWN = 1,
    CAMERA_OV7725 = 7725,
    CAMERA_OV2640 = 2640,
    CAMERA_OV3660 = 3660,
    CAMERA_OV5640 = 5640,
} camera_model_t;

#define REG_PID        0x0A
#define REG_VER        0x0B
#define REG_MIDH       0x1C
#define REG_MIDL       0x1D

#define REG16_CHIDH     0x300A
#define REG16_CHIDL     0x300B

#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"
#define TAG ""
#else
#include "esp_log.h"
static const char* TAG = "camera";
#endif

#include "mp.h"
static const char* CAMERA_SENSOR_NVS_KEY = "sensor";
static const char* CAMERA_PIXFORMAT_NVS_KEY = "pixformat";

typedef void (*dma_filter_t)(const dma_elem_t* src, lldesc_t* dma_desc, uint8_t* dst);

typedef struct fb_s
{
    uint8_t* buf;
    size_t len;
    struct fb_s* next;
} fb_item_t;

typedef struct
{
    camera_config_t config;
    sensor_t sensor;
    
    struct campy_FrameBuffer* fb;
    size_t fb_size;
    size_t data_size;
    
    size_t width;
    size_t height;
    size_t in_bytes_per_pixel;
    size_t fb_bytes_per_pixel;
    
    size_t dma_received_count;
    size_t dma_filtered_count;
    size_t dma_per_line;
    size_t dma_buf_width;
    size_t dma_sample_count;
    
    lldesc_t* dma_desc;
    dma_elem_t** dma_buf;
    size_t dma_desc_count;
    size_t dma_desc_cur;
    
    i2s_sampling_mode_t sampling_mode;
    dma_filter_t dma_filter;
    intr_handle_t i2s_intr_handle;
    QueueHandle_t data_ready;
    QueueHandle_t fb_in;
    QueueHandle_t fb_out;
    
    SemaphoreHandle_t frame_ready;
    TaskHandle_t dma_filter_task;
} camera_state_t;

camera_state_t* s_state = NULL;

static void i2s_init();
static int i2s_run();
static void IRAM_ATTR vsync_isr(void* arg);
static void IRAM_ATTR i2s_isr(void* arg);
static esp_err_t dma_desc_init();
static void dma_desc_deinit();
static void dma_filter_task(void* pvParameters);
static void dma_filter_grayscale(const dma_elem_t* src, lldesc_t* dma_desc, uint8_t* dst);
static void dma_filter_grayscale_highspeed(const dma_elem_t* src, lldesc_t* dma_desc, uint8_t* dst);
static void dma_filter_yuyv(const dma_elem_t* src, lldesc_t* dma_desc, uint8_t* dst);
static void dma_filter_yuyv_highspeed(const dma_elem_t* src, lldesc_t* dma_desc, uint8_t* dst);
static void dma_filter_jpeg(const dma_elem_t* src, lldesc_t* dma_desc, uint8_t* dst);
static void i2s_stop(bool* need_yield);

static bool is_hs_mode()
{
    return s_state->config.xclk_freq_hz > 10000000;
}

static size_t i2s_bytes_per_sample(i2s_sampling_mode_t mode)
{
    switch (mode)
    {
        case SM_0A00_0B00:
            return 4;
        case SM_0A0B_0B0C:
            return 4;
        case SM_0A0B_0C0D:
            return 2;
        default:
            assert(0 && "invalid sampling mode");
            return 0;
    }
}

static int IRAM_ATTR _gpio_get_level(gpio_num_t gpio_num)
{
    if (gpio_num < 32)
    {
        return (GPIO.in >> gpio_num) & 0x1;
    }
    else
    {
        return (GPIO.in1.data >> (gpio_num - 32)) & 0x1;
    }
}

static void IRAM_ATTR vsync_intr_disable()
{
    gpio_set_intr_type(s_state->config.pin_vsync, GPIO_INTR_DISABLE);
}

static void vsync_intr_enable()
{
    gpio_set_intr_type(s_state->config.pin_vsync, GPIO_INTR_NEGEDGE);
}

static void skip_frame()
{
    if (s_state == NULL)
    {
        MP_RAISE(Exception, "Camera not initialized");
    }
    
    int64_t st_t = esp_timer_get_time();
    while (_gpio_get_level(s_state->config.pin_vsync) == 0)
    {
        if ((esp_timer_get_time() - st_t) > 1000000LL)
        {
            MP_RAISE(Exception, "Timeout waiting for VSYNC");
        }
    }
    while (_gpio_get_level(s_state->config.pin_vsync) != 0)
    {
        if ((esp_timer_get_time() - st_t) > 1000000LL)
        {
            MP_RAISE(Exception, "Timeout waiting for VSYNC");
        }
    }
    while (_gpio_get_level(s_state->config.pin_vsync) == 0)
    {
        if ((esp_timer_get_time() - st_t) > 1000000LL)
        {
            MP_RAISE(Exception, "Timeout waiting for VSYNC");
        }
    }
}

static esp_err_t dma_desc_init()
{
    assert(s_state->width % 4 == 0);
    size_t line_size = s_state->width * s_state->in_bytes_per_pixel * i2s_bytes_per_sample(s_state->sampling_mode);
    MP_LOGD(TAG, "Line width (for DMA): %d bytes", line_size);
    
    size_t dma_per_line = 1;
    size_t buf_size     = line_size;
    
    while (buf_size >= 4096)
    {
        buf_size     /= 2;
        dma_per_line *= 2;
    }
    
    size_t dma_desc_count   = dma_per_line * 4;
    s_state->dma_buf_width  = line_size;
    s_state->dma_per_line   = dma_per_line;
    s_state->dma_desc_count = dma_desc_count;
    MP_LOGD(TAG, "DMA buffer size: %d, DMA buffers per line: %d", buf_size, dma_per_line);
    MP_LOGD(TAG, "DMA buffer count: %d", dma_desc_count);
    MP_LOGD(TAG, "DMA buffer total: %d bytes", buf_size * dma_desc_count);
    
    s_state->dma_buf = (dma_elem_t**)malloc(sizeof(dma_elem_t*) * dma_desc_count);
    
    if (s_state->dma_buf == NULL)
    {
        return ESP_ERR_NO_MEM;
    }
    
    s_state->dma_desc = (lldesc_t*) malloc(sizeof(lldesc_t) * dma_desc_count);
    
    if (s_state->dma_desc == NULL)
    {
        return ESP_ERR_NO_MEM;
    }
    
    size_t dma_sample_count = 0;
    
    for (int i = 0; i < dma_desc_count; ++i)
    {
        MP_LOGD(TAG, "Allocating DMA buffer #%d, size=%d", i, buf_size);
        
        dma_elem_t* buf = (dma_elem_t*) malloc(buf_size);
        
        if (buf == NULL)
        {
            return ESP_ERR_NO_MEM;
        }
        
        s_state->dma_buf[i] = buf;
        MP_LOGV(TAG, "dma_buf[%d]=%p", i, buf);
        
        lldesc_t* pd = &s_state->dma_desc[i];
        pd->length   = buf_size;
        
        if ((s_state->sampling_mode == SM_0A0B_0B0C) && (i + 1) % dma_per_line == 0)
        {
            pd->length -= 4;
        }
        
        dma_sample_count += pd->length / 4;
        
        pd->size         = pd->length;
        pd->owner        = 1;
        pd->sosf         = 1;
        pd->buf          = (uint8_t*) buf;
        pd->offset       = 0;
        pd->empty        = 0;
        pd->eof          = 1;
        pd->qe.stqe_next = &s_state->dma_desc[(i + 1) % dma_desc_count];
    }
    
    s_state->dma_sample_count = dma_sample_count;
    return ESP_OK;
}

static void dma_desc_deinit()
{
    if (s_state->dma_buf)
    {
        for (int i = 0; i < s_state->dma_desc_count; ++i)
        {
            free(s_state->dma_buf[i]);
        }
    }
    free(s_state->dma_buf);
    free(s_state->dma_desc);
}

static inline void IRAM_ATTR i2s_conf_reset()
{
    const uint32_t lc_conf_reset_flags = I2S_IN_RST_M | I2S_AHBM_RST_M
                                         | I2S_AHBM_FIFO_RST_M;
    I2S0.lc_conf.val |= lc_conf_reset_flags;
    I2S0.lc_conf.val &= ~lc_conf_reset_flags;
    
    const uint32_t conf_reset_flags = I2S_RX_RESET_M | I2S_RX_FIFO_RESET_M
                                      | I2S_TX_RESET_M | I2S_TX_FIFO_RESET_M;
    I2S0.conf.val |= conf_reset_flags;
    I2S0.conf.val &= ~conf_reset_flags;
    while (I2S0.state.rx_fifo_reset_back)
    {
        ;
    }
}

static void i2s_init()
{
    camera_config_t* config = &s_state->config;
    
    // Configure input GPIOs
    gpio_num_t pins[] =
    {
        config->pin_d7,
        config->pin_d6,
        config->pin_d5,
        config->pin_d4,
        config->pin_d3,
        config->pin_d2,
        config->pin_d1,
        config->pin_d0,
        config->pin_vsync,
        config->pin_href,
        config->pin_pclk
    };
    gpio_config_t conf =
    {
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    for (int i = 0; i < sizeof(pins) / sizeof(gpio_num_t); ++i)
    {
        if (rtc_gpio_is_valid_gpio(pins[i]))
        {
            rtc_gpio_deinit(pins[i]);
        }
        conf.pin_bit_mask = 1LL << pins[i];
        gpio_config(&conf);
    }
    
    // Route input GPIOs to I2S peripheral using GPIO matrix
    gpio_matrix_in(config->pin_d0, I2S0I_DATA_IN0_IDX, false);
    gpio_matrix_in(config->pin_d1, I2S0I_DATA_IN1_IDX, false);
    gpio_matrix_in(config->pin_d2, I2S0I_DATA_IN2_IDX, false);
    gpio_matrix_in(config->pin_d3, I2S0I_DATA_IN3_IDX, false);
    gpio_matrix_in(config->pin_d4, I2S0I_DATA_IN4_IDX, false);
    gpio_matrix_in(config->pin_d5, I2S0I_DATA_IN5_IDX, false);
    gpio_matrix_in(config->pin_d6, I2S0I_DATA_IN6_IDX, false);
    gpio_matrix_in(config->pin_d7, I2S0I_DATA_IN7_IDX, false);
    gpio_matrix_in(config->pin_vsync, I2S0I_V_SYNC_IDX, false);
    gpio_matrix_in(0x38, I2S0I_H_SYNC_IDX, false);
    gpio_matrix_in(config->pin_href, I2S0I_H_ENABLE_IDX, false);
    gpio_matrix_in(config->pin_pclk, I2S0I_WS_IN_IDX, false);
    
    // Enable and configure I2S peripheral
    periph_module_enable(PERIPH_I2S0_MODULE);
    // Toggle some reset bits in LC_CONF register
    // Toggle some reset bits in CONF register
    i2s_conf_reset();
    // Enable slave mode (sampling clock is external)
    I2S0.conf.rx_slave_mod = 1;
    // Enable parallel mode
    I2S0.conf2.lcd_en = 1;
    // Use HSYNC/VSYNC/HREF to control sampling
    I2S0.conf2.camera_en = 1;
    // Configure clock divider
    I2S0.clkm_conf.clkm_div_a = 1;
    I2S0.clkm_conf.clkm_div_b = 0;
    I2S0.clkm_conf.clkm_div_num = 2;
    // FIFO will sink data to DMA
    I2S0.fifo_conf.dscr_en = 1;
    // FIFO configuration
    I2S0.fifo_conf.rx_fifo_mod = s_state->sampling_mode;
    I2S0.fifo_conf.rx_fifo_mod_force_en = 1;
    I2S0.conf_chan.rx_chan_mod = 1;
    // Clear flags which are used in I2S serial mode
    I2S0.sample_rate_conf.rx_bits_mod = 0;
    I2S0.conf.rx_right_first = 0;
    I2S0.conf.rx_msb_right = 0;
    I2S0.conf.rx_msb_shift = 0;
    I2S0.conf.rx_mono = 0;
    I2S0.conf.rx_short_sync = 0;
    I2S0.timing.val = 0;
    I2S0.timing.rx_dsync_sw = 1;
    
    // Allocate I2S interrupt, keep it disabled
    ESP_ERROR_CHECK(esp_intr_alloc(ETS_I2S0_INTR_SOURCE,
                                   ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_LOWMED | ESP_INTR_FLAG_IRAM,
                                   &i2s_isr, NULL, &s_state->i2s_intr_handle));
}

static void IRAM_ATTR i2s_start_bus()
{
    s_state->dma_desc_cur = 0;
    s_state->dma_received_count = 0;
    //s_state->dma_filtered_count = 0;
    esp_intr_disable(s_state->i2s_intr_handle);
    i2s_conf_reset();
    
    I2S0.rx_eof_num = s_state->dma_sample_count;
    I2S0.in_link.addr = (uint32_t) &s_state->dma_desc[0];
    I2S0.in_link.start = 1;
    I2S0.int_clr.val = I2S0.int_raw.val;
    I2S0.int_ena.val = 0;
    I2S0.int_ena.in_done = 1;
    
    esp_intr_enable(s_state->i2s_intr_handle);
    I2S0.conf.rx_start = 1;
    if (s_state->config.pixel_format == PIXFORMAT_JPEG)
    {
        vsync_intr_enable();
    }
}

static int i2s_run()
{
    for (int i = 0; i < s_state->dma_desc_count; ++i)
    {
        lldesc_t* d = &s_state->dma_desc[i];
        MP_LOGV(TAG, "DMA desc %2d: %u %u %u %u %u %u %p %p",
                 i, d->length, d->size, d->offset, d->eof, d->sosf, d->owner, d->buf, d->qe.stqe_next);
        memset(s_state->dma_buf[i], 0, d->length);
    }
    
    //todo: wait for vsync
    MP_LOGV(TAG, "Waiting for negative edge on VSYNC");
    
    int64_t st_t = esp_timer_get_time();
    while (_gpio_get_level(s_state->config.pin_vsync) != 0)
    {
        if ((esp_timer_get_time() - st_t) > 1000000LL)
        {
            MP_LOGE(TAG, "Timeout waiting for VSYNC");
            return -1;
        }
    }
    MP_LOGV(TAG, "Got VSYNC");
    i2s_start_bus();
    return 0;
}

static void IRAM_ATTR i2s_stop_bus()
{
    esp_intr_disable(s_state->i2s_intr_handle);
    vsync_intr_disable();
    i2s_conf_reset();
    I2S0.conf.rx_start = 0;
}

static void IRAM_ATTR i2s_stop(bool* need_yield)
{
    if (!s_state->fb->bad)
    {
        i2s_stop_bus();
    }
    else
    {
        s_state->dma_received_count = 0;
    }
    
    size_t val = SIZE_MAX;
    BaseType_t higher_priority_task_woken;
    BaseType_t ret = xQueueSendFromISR(s_state->data_ready, &val, &higher_priority_task_woken);
    if (need_yield && !*need_yield)
    {
        *need_yield = (ret == pdTRUE && higher_priority_task_woken == pdTRUE);
    }
}

static void IRAM_ATTR signal_dma_buf_received(bool* need_yield)
{
    size_t dma_desc_filled = s_state->dma_desc_cur;
    s_state->dma_desc_cur = (dma_desc_filled + 1) % s_state->dma_desc_count;
    s_state->dma_received_count++;
    if (!s_state->fb->ref && s_state->fb->bad)
    {
        *need_yield = false;
        return;
    }
    BaseType_t higher_priority_task_woken;
    BaseType_t ret = xQueueSendFromISR(s_state->data_ready, &dma_desc_filled, &higher_priority_task_woken);
    if (ret != pdTRUE)
    {
        if (!s_state->fb->ref)
        {
            s_state->fb->bad = 1;
        }
        //ESP_EARLY_LOGW(TAG, "qsf:%d", s_state->dma_received_count);
        //ets_printf("qsf:%d\n", s_state->dma_received_count);
        //ets_printf("qovf\n");
    }
    *need_yield = (ret == pdTRUE && higher_priority_task_woken == pdTRUE);
}

static void IRAM_ATTR i2s_isr(void* arg)
{
    I2S0.int_clr.val = I2S0.int_raw.val;
    bool need_yield = false;
    signal_dma_buf_received(&need_yield);
    if (s_state->config.pixel_format != PIXFORMAT_JPEG
        && s_state->dma_received_count == s_state->height * s_state->dma_per_line)
    {
        i2s_stop(&need_yield);
    }
    if (need_yield)
    {
        portYIELD_FROM_ISR();
    }
}

static void IRAM_ATTR vsync_isr(void* arg)
{
    GPIO.status1_w1tc.val = GPIO.status1.val;
    GPIO.status_w1tc = GPIO.status;
    bool need_yield = false;
    //if vsync is low and we have received some data, frame is done
    if (_gpio_get_level(s_state->config.pin_vsync) == 0)
    {
        if (s_state->dma_received_count > 0)
        {
            signal_dma_buf_received(&need_yield);
            //ets_printf("end_vsync\n");
            if (s_state->dma_filtered_count > 1 || s_state->fb->bad)
            {
                i2s_stop(&need_yield);
            }
            //ets_printf("vs\n");
        }
        if (s_state->dma_filtered_count < 2)
        {
            I2S0.conf.rx_start = 0;
            I2S0.in_link.start = 0;
            I2S0.int_clr.val = I2S0.int_raw.val;
            i2s_conf_reset();
            s_state->dma_desc_cur = (s_state->dma_desc_cur + 1) % s_state->dma_desc_count;
            //I2S0.rx_eof_num = s_state->dma_sample_count;
            I2S0.in_link.addr = (uint32_t) &s_state->dma_desc[s_state->dma_desc_cur];
            I2S0.in_link.start = 1;
            I2S0.conf.rx_start = 1;
            s_state->dma_received_count = 0;
        }
    }
    if (need_yield)
    {
        portYIELD_FROM_ISR();
    }
}

static void IRAM_ATTR camera_fb_done()
{
    xSemaphoreGive(s_state->frame_ready);
}

static void IRAM_ATTR dma_finish_frame()
{
    size_t buf_len = s_state->width * s_state->fb_bytes_per_pixel / s_state->dma_per_line;
    
    if (!s_state->fb->ref)
    {
        // is the frame bad?
        if (s_state->fb->bad)
        {
            s_state->fb->bad = 0;
            s_state->fb->len = 0;
            *((uint32_t*)s_state->fb->buf) = 0;
            i2s_start_bus();
        }
        else
        {
            s_state->fb->len = s_state->dma_filtered_count * buf_len;
            
            if (0 != s_state->fb->len)
            {
                //find the end marker for JPEG. Data after that can be discarded
                if (s_state->fb->format == PIXFORMAT_JPEG)
                {
                    uint8_t* dptr = &s_state->fb->buf[s_state->fb->len - 1];
                    while (dptr > s_state->fb->buf)
                    {
                        if (dptr[0] == 0xFF && dptr[1] == 0xD9 && dptr[2] == 0x00 && dptr[3] == 0x00)
                        {
                            dptr += 2;
                            s_state->fb->len = dptr - s_state->fb->buf;
                            if ((s_state->fb->len & 0x1FF) == 0)
                            {
                                s_state->fb->len += 1;
                            }
                            if ((s_state->fb->len % 100) == 0)
                            {
                                s_state->fb->len += 1;
                            }
                            break;
                        }
                        dptr--;
                    }
                }
                //send out the frame
                camera_fb_done();
            }
            else
            {
                //frame was empty?
                i2s_start_bus();
            }
        }
    }
    else if (s_state->fb->len)
    {
        camera_fb_done();
    }
    s_state->dma_filtered_count = 0;
}

static void IRAM_ATTR dma_filter_buffer(size_t buf_idx)
{
    //no need to process the data if frame is in use or is bad
    if (s_state->fb->ref || s_state->fb->bad)
    {
        return;
    }
    
    //check if there is enough space in the frame buffer for the new data
    size_t buf_len = s_state->width * s_state->fb_bytes_per_pixel / s_state->dma_per_line;
    size_t fb_pos = s_state->dma_filtered_count * buf_len;
    if (fb_pos > s_state->fb_size - buf_len)
    {
        //size_t processed = s_state->dma_received_count * buf_len;
        //ets_printf("[%s:%u] ovf pos: %u, processed: %u\n", __FUNCTION__, __LINE__, fb_pos, processed);
        return;
    }
    
    //convert I2S DMA buffer to pixel data
    (*s_state->dma_filter)(s_state->dma_buf[buf_idx], &s_state->dma_desc[buf_idx], s_state->fb->buf + fb_pos);
    
    //first frame buffer
    if (!s_state->dma_filtered_count)
    {
        //check for correct JPEG header
        if (s_state->sensor.pixformat == PIXFORMAT_JPEG)
        {
            uint32_t sig = *((uint32_t*)s_state->fb->buf) & 0xFFFFFF;
            if (sig != 0xffd8ff)
            {
                ets_printf("bh 0x%08x\n", sig);
                s_state->fb->bad = 1;
                return;
            }
        }
        //set the frame properties
        s_state->fb->width = resolution[s_state->sensor.status.framesize].width;
        s_state->fb->height = resolution[s_state->sensor.status.framesize].height;
        s_state->fb->format = s_state->sensor.pixformat;
        
        uint64_t us = (uint64_t)esp_timer_get_time();
        s_state->fb->timestamp.tv_sec = us / 1000000UL;
        s_state->fb->timestamp.tv_usec = us % 1000000UL;
    }
    s_state->dma_filtered_count++;
}

static void IRAM_ATTR dma_filter_task(void* pvParameters)
{
    s_state->dma_filtered_count = 0;
    while (true)
    {
        size_t buf_idx;
        if (xQueueReceive(s_state->data_ready, &buf_idx, portMAX_DELAY) == pdTRUE)
        {
            if (buf_idx == SIZE_MAX)
            {
                //this is the end of the frame
                dma_finish_frame();
            }
            else
            {
                dma_filter_buffer(buf_idx);
            }
        }
    }
}

static void IRAM_ATTR dma_filter_jpeg(const dma_elem_t* src, lldesc_t* dma_desc, uint8_t* dst)
{
    size_t end = dma_desc->length / sizeof(dma_elem_t) / 4;
    // manually unrolling 4 iterations of the loop here
    for (size_t i = 0; i < end; ++i)
    {
        dst[0] = src[0].sample1;
        dst[1] = src[1].sample1;
        dst[2] = src[2].sample1;
        dst[3] = src[3].sample1;
        src += 4;
        dst += 4;
    }
}

static void IRAM_ATTR dma_filter_grayscale(const dma_elem_t* src, lldesc_t* dma_desc, uint8_t* dst)
{
    size_t end = dma_desc->length / sizeof(dma_elem_t) / 4;
    for (size_t i = 0; i < end; ++i)
    {
        // manually unrolling 4 iterations of the loop here
        dst[0] = src[0].sample1;
        dst[1] = src[1].sample1;
        dst[2] = src[2].sample1;
        dst[3] = src[3].sample1;
        src += 4;
        dst += 4;
    }
}

static void IRAM_ATTR dma_filter_grayscale_highspeed(const dma_elem_t* src, lldesc_t* dma_desc, uint8_t* dst)
{
    size_t end = dma_desc->length / sizeof(dma_elem_t) / 8;
    for (size_t i = 0; i < end; ++i)
    {
        // manually unrolling 4 iterations of the loop here
        dst[0] = src[0].sample1;
        dst[1] = src[2].sample1;
        dst[2] = src[4].sample1;
        dst[3] = src[6].sample1;
        src += 8;
        dst += 4;
    }
    // the final sample of a line in SM_0A0B_0B0C sampling mode needs special handling
    if ((dma_desc->length & 0x7) != 0)
    {
        dst[0] = src[0].sample1;
        dst[1] = src[2].sample1;
    }
}

static void IRAM_ATTR dma_filter_yuyv(const dma_elem_t* src, lldesc_t* dma_desc, uint8_t* dst)
{
    size_t end = dma_desc->length / sizeof(dma_elem_t) / 4;
    for (size_t i = 0; i < end; ++i)
    {
        dst[0] = src[0].sample1;//y0
        dst[1] = src[0].sample2;//u
        dst[2] = src[1].sample1;//y1
        dst[3] = src[1].sample2;//v
        
        dst[4] = src[2].sample1;//y0
        dst[5] = src[2].sample2;//u
        dst[6] = src[3].sample1;//y1
        dst[7] = src[3].sample2;//v
        src += 4;
        dst += 8;
    }
}

static void IRAM_ATTR dma_filter_yuyv_highspeed(const dma_elem_t* src, lldesc_t* dma_desc, uint8_t* dst)
{
    size_t end = dma_desc->length / sizeof(dma_elem_t) / 8;
    for (size_t i = 0; i < end; ++i)
    {
        dst[0] = src[0].sample1;//y0
        dst[1] = src[1].sample1;//u
        dst[2] = src[2].sample1;//y1
        dst[3] = src[3].sample1;//v
        
        dst[4] = src[4].sample1;//y0
        dst[5] = src[5].sample1;//u
        dst[6] = src[6].sample1;//y1
        dst[7] = src[7].sample1;//v
        src += 8;
        dst += 8;
    }
    if ((dma_desc->length & 0x7) != 0)
    {
        dst[0] = src[0].sample1;//y0
        dst[1] = src[1].sample1;//u
        dst[2] = src[2].sample1;//y1
        dst[3] = src[2].sample2;//v
    }
}

static void IRAM_ATTR dma_filter_rgb888(const dma_elem_t* src, lldesc_t* dma_desc, uint8_t* dst)
{
    size_t end = dma_desc->length / sizeof(dma_elem_t) / 4;
    uint8_t lb, hb;
    for (size_t i = 0; i < end; ++i)
    {
        hb = src[0].sample1;
        lb = src[0].sample2;
        dst[0] = (lb & 0x1F) << 3;
        dst[1] = (hb & 0x07) << 5 | (lb & 0xE0) >> 3;
        dst[2] = hb & 0xF8;
        
        hb = src[1].sample1;
        lb = src[1].sample2;
        dst[3] = (lb & 0x1F) << 3;
        dst[4] = (hb & 0x07) << 5 | (lb & 0xE0) >> 3;
        dst[5] = hb & 0xF8;
        
        hb = src[2].sample1;
        lb = src[2].sample2;
        dst[6] = (lb & 0x1F) << 3;
        dst[7] = (hb & 0x07) << 5 | (lb & 0xE0) >> 3;
        dst[8] = hb & 0xF8;
        
        hb = src[3].sample1;
        lb = src[3].sample2;
        dst[9] = (lb & 0x1F) << 3;
        dst[10] = (hb & 0x07) << 5 | (lb & 0xE0) >> 3;
        dst[11] = hb & 0xF8;
        src += 4;
        dst += 12;
    }
}

static void IRAM_ATTR dma_filter_rgb888_highspeed(const dma_elem_t* src, lldesc_t* dma_desc, uint8_t* dst)
{
    size_t end = dma_desc->length / sizeof(dma_elem_t) / 8;
    uint8_t lb, hb;
    for (size_t i = 0; i < end; ++i)
    {
        hb = src[0].sample1;
        lb = src[1].sample1;
        dst[0] = (lb & 0x1F) << 3;
        dst[1] = (hb & 0x07) << 5 | (lb & 0xE0) >> 3;
        dst[2] = hb & 0xF8;
        
        hb = src[2].sample1;
        lb = src[3].sample1;
        dst[3] = (lb & 0x1F) << 3;
        dst[4] = (hb & 0x07) << 5 | (lb & 0xE0) >> 3;
        dst[5] = hb & 0xF8;
        
        hb = src[4].sample1;
        lb = src[5].sample1;
        dst[6] = (lb & 0x1F) << 3;
        dst[7] = (hb & 0x07) << 5 | (lb & 0xE0) >> 3;
        dst[8] = hb & 0xF8;
        
        hb = src[6].sample1;
        lb = src[7].sample1;
        dst[9] = (lb & 0x1F) << 3;
        dst[10] = (hb & 0x07) << 5 | (lb & 0xE0) >> 3;
        dst[11] = hb & 0xF8;
        
        src += 8;
        dst += 12;
    }
    if ((dma_desc->length & 0x7) != 0)
    {
        hb = src[0].sample1;
        lb = src[1].sample1;
        dst[0] = (lb & 0x1F) << 3;
        dst[1] = (hb & 0x07) << 5 | (lb & 0xE0) >> 3;
        dst[2] = hb & 0xF8;
        
        hb = src[2].sample1;
        lb = src[2].sample2;
        dst[3] = (lb & 0x1F) << 3;
        dst[4] = (hb & 0x07) << 5 | (lb & 0xE0) >> 3;
        dst[5] = hb & 0xF8;
    }
}

/*
 * Public Methods
 * */

esp_err_t camera_probe(const camera_config_t* config, camera_model_t* out_camera_model)
{
    if (s_state != NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }
    
    s_state = (camera_state_t*) m_malloc0(sizeof(*s_state));
    if (!s_state)
    {
        return ESP_ERR_NO_MEM;
    }
    
    MP_LOGD(TAG, "Enabling XCLK output");
    camera_enable_out_clock(config);
    
    MP_LOGD(TAG, "Initializing SSCB");
    SCCB_Init(config->pin_sscb_sda, config->pin_sscb_scl);
    
    if (config->pin_pwdn >= 0)
    {
        MP_LOGD(TAG, "Resetting camera by power down line");
        gpio_config_t conf = { 0 };
        conf.pin_bit_mask = 1LL << config->pin_pwdn;
        conf.mode = GPIO_MODE_OUTPUT;
        gpio_config(&conf);
        
        // carefull, logic is inverted compared to reset pin
        gpio_set_level(config->pin_pwdn, 1);
        vTaskDelay(10 / portTICK_PERIOD_MS);
        gpio_set_level(config->pin_pwdn, 0);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    
    if (config->pin_reset >= 0)
    {
        MP_LOGD(TAG, "Resetting camera");
        gpio_config_t conf = { 0 };
        conf.pin_bit_mask = 1LL << config->pin_reset;
        conf.mode = GPIO_MODE_OUTPUT;
        gpio_config(&conf);
        
        gpio_set_level(config->pin_reset, 0);
        vTaskDelay(10 / portTICK_PERIOD_MS);
        gpio_set_level(config->pin_reset, 1);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    
    MP_LOGD(TAG, "Searching for camera address");
    vTaskDelay(10 / portTICK_PERIOD_MS);
    uint8_t slv_addr = SCCB_Probe();
    if (slv_addr == 0)
    {
        *out_camera_model = CAMERA_NONE;
        camera_disable_out_clock();
        return ESP_ERR_CAMERA_NOT_DETECTED;
    }
    
    //slv_addr = 0x30;
    MP_LOGD(TAG, "Detected camera at address=0x%02x", slv_addr);
    sensor_id_t* id = &s_state->sensor.id;
    
    if (slv_addr == 0x30)
    {
        MP_LOGD(TAG, "Resetting OV2640");
        //camera might be OV2640. try to reset it
        SCCB_Write(0x30, 0xFF, 0x01);//bank sensor
        SCCB_Write(0x30, 0x12, 0x80);//reset
        vTaskDelay(10 / portTICK_PERIOD_MS);
        slv_addr = SCCB_Probe();
    }
    
    s_state->sensor.slv_addr = slv_addr;
    s_state->sensor.xclk_freq_hz = config->xclk_freq_hz;
    
    id->PID = SCCB_Read(s_state->sensor.slv_addr, REG_PID);
    id->VER = SCCB_Read(s_state->sensor.slv_addr, REG_VER);
    id->MIDL = SCCB_Read(s_state->sensor.slv_addr, REG_MIDL);
    id->MIDH = SCCB_Read(s_state->sensor.slv_addr, REG_MIDH);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    MP_LOGD(TAG, "Camera PID=0x%02x VER=0x%02x MIDL=0x%02x MIDH=0x%02x",
             id->PID, id->VER, id->MIDH, id->MIDL);
                 
    
    
    switch (id->PID)
    {
        case OV2640_PID:
            *out_camera_model = CAMERA_OV2640;
            ov2640_init(&s_state->sensor);
            break;
        default:
            id->PID = 0;
            *out_camera_model = CAMERA_UNKNOWN;
            camera_disable_out_clock();
            MP_LOGE(TAG, "Detected camera not supported.");
            return ESP_ERR_CAMERA_NOT_SUPPORTED;
    }
    
    MP_LOGD(TAG, "Doing SW reset of sensor");
    s_state->sensor.reset(&s_state->sensor);
    
    return ESP_OK;
}



void esp_camera_init(const camera_config_t* config)
{
    if (NULL == s_state)
    {
        MP_RAISE(Exception, "Camera not initialized");
    }
    
    if (s_state->sensor.id.PID == 0)
    {
        MP_RAISE(Exception, "Camera not supported");
    }
    
    memcpy(&s_state->config, config, sizeof(*config));
    esp_err_t   err        = ESP_OK;
    framesize_t frame_size = (framesize_t)config->frame_size;
    pixformat_t pix_format = (pixformat_t)config->pixel_format;
    
    switch (s_state->sensor.id.PID)
    {
        case OV2640_PID:
            if (frame_size > FRAMESIZE_UXGA)
            {
                frame_size = FRAMESIZE_UXGA;
            }
            break;
        default:
            MP_RAISE(Exception, "Camera not supported");
    }
    
    s_state->width  = resolution[frame_size].width;
    s_state->height = resolution[frame_size].height;
    
    if (pix_format == PIXFORMAT_GRAYSCALE)
    {
        s_state->fb_size = s_state->width * s_state->height;
        
        if (is_hs_mode() && (s_state->sensor.id.PID != OV7725_PID))
        {
            s_state->sampling_mode = SM_0A00_0B00;
            s_state->dma_filter    = &dma_filter_grayscale_highspeed;
        }
        else
        {
            s_state->sampling_mode = SM_0A0B_0C0D;
            s_state->dma_filter    = &dma_filter_grayscale;
        }
        
        s_state->in_bytes_per_pixel = 2;       // camera sends YU/YV
        s_state->fb_bytes_per_pixel = 1;       // frame buffer stores Y8
    }
    else if ((pix_format == PIXFORMAT_YUV422) ||
             (pix_format == PIXFORMAT_RGB565))
    {
        s_state->fb_size = s_state->width * s_state->height * 2;
        if (is_hs_mode() && (s_state->sensor.id.PID != OV7725_PID))
        {
            s_state->sampling_mode = SM_0A00_0B00;
            s_state->dma_filter    = &dma_filter_yuyv_highspeed;
        }
        else
        {
            s_state->sampling_mode = SM_0A0B_0C0D;
            s_state->dma_filter    = &dma_filter_yuyv;
        }
        
        s_state->in_bytes_per_pixel = 2;       // camera sends YU/YV
        s_state->fb_bytes_per_pixel = 2;       // frame buffer stores YU/YV/RGB565
    }
    else if (pix_format == PIXFORMAT_RGB888)
    {
        s_state->fb_size = s_state->width * s_state->height * 3;
        if (is_hs_mode())
        {
            s_state->sampling_mode = SM_0A00_0B00;
            s_state->dma_filter    = &dma_filter_rgb888_highspeed;
        }
        else
        {
            s_state->sampling_mode = SM_0A0B_0C0D;
            s_state->dma_filter    = &dma_filter_rgb888;
        }
        s_state->in_bytes_per_pixel = 2;       // camera sends RGB565
        s_state->fb_bytes_per_pixel = 3;       // frame buffer stores RGB888
    }
    else if (pix_format == PIXFORMAT_JPEG)
    {
        if ((s_state->sensor.id.PID != OV2640_PID) && 
            (s_state->sensor.id.PID != OV3660_PID) &&
            (s_state->sensor.id.PID != OV5640_PID))
        {
            esp_camera_deinit();
            MP_RAISE(Exception, "JPEG format is only supported for ov2640, ov3660 and ov5640");
        }
        
        (*s_state->sensor.set_quality)(&s_state->sensor, config->jpeg_quality);
        s_state->in_bytes_per_pixel = 2;
        s_state->fb_bytes_per_pixel = 2;
        
        _esp_camera_recalculate_compression(config->jpeg_quality);
        
        s_state->dma_filter         = &dma_filter_jpeg;
        s_state->sampling_mode      = SM_0A00_0B00;
    }
    else
    {
        esp_camera_deinit();
        MP_RAISE(Exception, "Requested format is not supported");
    }
    
    MP_LOGD(TAG, "in_bpp: %d, fb_bpp: %d, fb_size: %d, mode: %d, width: %d height: %d",
             s_state->in_bytes_per_pixel, s_state->fb_bytes_per_pixel,
             s_state->fb_size, s_state->sampling_mode,
             s_state->width, s_state->height);
             
    i2s_init();
    
    if (ESP_OK != dma_desc_init())
    {
        esp_camera_deinit();
        MP_RAISE(Exception, "Failed to initialize I2S and DMA");
    }
    
    s_state->data_ready = xQueueCreate(16, sizeof(size_t));
    
    if (s_state->data_ready == NULL)
    {
        esp_camera_deinit();
        MP_RAISE(Exception, "Failed to dma queue");
    }
    
    s_state->frame_ready = xSemaphoreCreateBinary();
    
    if (s_state->frame_ready == NULL)
    {
        esp_camera_deinit();
        MP_RAISE(Exception, "Failed to create semaphore");
    }
    
    //ToDo: core affinity?
#if CONFIG_CAMERA_CORE0
    if (!xTaskCreatePinnedToCore(&dma_filter_task, "dma_filter", 4096, NULL, 10, &s_state->dma_filter_task, 0))
#elif CONFIG_CAMERA_CORE1
    if (!xTaskCreatePinnedToCore(&dma_filter_task, "dma_filter", 4096, NULL, 10, &s_state->dma_filter_task, 1))
#else
    if (!xTaskCreate(&dma_filter_task, "dma_filter", 4096, NULL, 10, &s_state->dma_filter_task))
#endif
    {
        esp_camera_deinit();
        MP_RAISE(Exception, "Failed to create DMA filter task");
    }
    
    vsync_intr_disable();
    
    gpio_uninstall_isr_service(); // Uninstall service owned by micropython
    err = gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1 | ESP_INTR_FLAG_IRAM);
    if (err != ESP_OK)
    {
        esp_camera_deinit();
        MP_LOGE(TAG, "gpio_install_isr_service failed (%x)", err);
        MP_RAISE(Exception, "Register GPIO service failed");
    }
    
    err = gpio_isr_handler_add(s_state->config.pin_vsync, &vsync_isr, NULL);
    if (err != ESP_OK)
    {
        esp_camera_deinit();
        MP_LOGE(TAG, "vsync_isr_handler_add failed (%x)", err);
        MP_RAISE(Exception, "Register ISR handler failed");
    }
    
    s_state->sensor.status.framesize = frame_size;
    s_state->sensor.pixformat        = pix_format;
    
    MP_LOGD(TAG, "Setting frame size to %dx%d", s_state->width, s_state->height);
    
    if (s_state->sensor.set_framesize(&s_state->sensor, frame_size) != 0)
    {
        esp_camera_deinit();
        MP_RAISE(Exception, "Failed to set frame size");
    }
    
    s_state->sensor.set_pixformat(&s_state->sensor, pix_format);
    
    if (s_state->sensor.id.PID == OV2640_PID)
    {
        s_state->sensor.set_gainceiling(&s_state->sensor, GAINCEILING_2X);
        s_state->sensor.set_bpc(        &s_state->sensor, false);
        s_state->sensor.set_wpc(        &s_state->sensor, true);
        s_state->sensor.set_lenc(       &s_state->sensor, true);
    }
    
    skip_frame();
    
    s_state->sensor.init_status(&s_state->sensor);
    
    MP_LOGD(TAG, "Camera initialized as %p", s_state);
}


void _esp_camera_recalculate_compression(int aJpegQuality)
{
    int compression_ratio_bound = 4;
    
    if (aJpegQuality > 10)
    {
        compression_ratio_bound = 16;
    }
    else if (aJpegQuality > 5)
    {
        compression_ratio_bound = 10;
    }
    
    s_state->fb_size = (s_state->width * s_state->height * s_state->fb_bytes_per_pixel) / compression_ratio_bound;
}


static void _init_skip(void)
{
    m_free(s_state);
    s_state = NULL;
    camera_disable_out_clock();
}


void campy_Camera_init(struct campy_Camera* camera)
{
    camera_model_t camera_model = CAMERA_NONE;
    esp_err_t      err          = camera_probe(&(camera->config), &camera_model);
    
    if (err != ESP_OK)
    {
        _init_skip();
        MP_LOGE(TAG, "Camera probe failed with error 0x%x", err);
        MP_RAISE(Exception, "Unable to detect camera");
    }
    
    
    switch (camera_model) {
        case CAMERA_OV2640:
            MP_LOGI(TAG, "Detected OV2640 camera");
            memcpy(camera->model, "OV2640", sizeof(camera->model));
            break;
            
        default:
            _init_skip();
            MP_RAISE(Exception, "Camera not supported");
    }
    
    
    esp_camera_init(&(camera->config));
}


void esp_camera_deinit()
{
    MP_LOGD(TAG, "Deinitialization of %p", s_state);
    
    if (s_state == NULL)
    {
        MP_RAISE(Exception, "Camera not initialized");
    }
    
    if (s_state->dma_filter_task)
    {
        vTaskDelete(s_state->dma_filter_task);
    }
    
    if (s_state->data_ready)
    {
        vQueueDelete(s_state->data_ready);
    }
    
    if (s_state->fb_in)
    {
        vQueueDelete(s_state->fb_in);
    }
    
    if (s_state->fb_out)
    {
        vQueueDelete(s_state->fb_out);
    }
    
    if (s_state->frame_ready)
    {
        vSemaphoreDelete(s_state->frame_ready);
    }
    
    gpio_isr_handler_remove(s_state->config.pin_vsync);
    
    if (s_state->i2s_intr_handle)
    {
        esp_intr_disable(s_state->i2s_intr_handle);
        esp_intr_free(s_state->i2s_intr_handle);
    }
    
    dma_desc_deinit();
    s_state = NULL;
    
    camera_disable_out_clock();
    periph_module_disable(PERIPH_I2S0_MODULE);
}

#define FB_GET_TIMEOUT (4000 / portTICK_PERIOD_MS)

struct campy_FrameBuffer* esp_camera_fb_get()
{
    /*
     * Checks if camera is initialized
     */
    if (s_state == NULL)
    {
        MP_RAISE(Exception, "Camera not initialized");
    }
    
    /*
     * Is transfer already running?
     */
    if (!I2S0.conf.rx_start)
    {
        /*
         * Attach new buffer
         */
        s_state->fb = campy_FrameBuffer_new();
        
        /*
         * Launch transfer
         */
        if (i2s_run())
        {
            MP_RAISE(Exception, "Transfer error");
        }
    }
    
    /*
     * Wait for transfer completation - variant single buffer
     */
    bool need_yield = false;
    
    if (xSemaphoreTake(s_state->frame_ready, FB_GET_TIMEOUT) != pdTRUE)
    {
        i2s_stop(&need_yield);
        MP_RAISE(Exception, "Failed to get the frame on time");
    }
    
    return s_state->fb;
}

sensor_t* esp_camera_sensor_get()
{
    if (s_state == NULL)
    {
        MP_RAISE(Exception, "Camera not initialized");
    }
    
    return &s_state->sensor;
}


/*
 * See description with declaration
 */
struct campy_Camera* campy_Camera_new(const camera_config_t* aConfig)
{
    struct campy_Camera* self = m_new_obj(struct campy_Camera);
    
    self->base.type = &MP_NAMESPACE2(campy, Camera);
    memcpy(&(self->config), aConfig, sizeof(self->config));
    
    return self;
}


/*
 * See description with declaration
 */
struct campy_FrameBuffer* campy_FrameBuffer_new(void)
{
    if (NULL == s_state)
    {
        MP_RAISE(Exception, "Camera not initialized");
    }
    
    struct campy_FrameBuffer* self = m_new0(struct campy_FrameBuffer, 1);
    
    self->base.type = &MP_NAMESPACE2(campy, FrameBuffer);
    self->buf       = (uint8_t*)m_malloc(s_state->fb_size);
    
    return self;
}
