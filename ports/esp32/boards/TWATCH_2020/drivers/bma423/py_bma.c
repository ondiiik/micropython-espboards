/*
 * This file is part of the MicroPython ESP32 project, https://github.com/lewisxhe/MicroPython_ESP32_psRAM_LoBo
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 lewisxhe (https://github.com/lewisxhe)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "sdkconfig.h"


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp32/rom/ets_sys.h"
#include "esp_system.h"
#include "esp_task_wdt.h"

#include "driver/gpio.h"
#include "mphalport.h"
#include "extmod/machine_i2c.h"
#include "modmachine.h"
#include "bma.h"
#include "py/runtime.h"
#include "py/obj.h"
#include "mpconfigport.h"

#include "esp_log.h"

typedef struct _machine_hw_i2c_obj_t
{
    mp_obj_base_t base;
    i2c_port_t    port : 8;
    gpio_num_t    scl  : 8;
    gpio_num_t    sda  : 8;
}
machine_hw_i2c_obj_t;

static i2c_bus_t*      i2c_handle = NULL;
static bma423_handle_t bma_handle = NULL;




STATIC mp_obj_t bma_sensor_init(size_t          n_args,
                                const mp_obj_t* pos_args,
                                mp_map_t*       kw_args)
{
    enum
    {
        ARG_i2c,
        ARG_address,
        ARG_irq,
    };
    
    static const mp_arg_t allowed_args[] =
    {
        { MP_QSTR_i2c,     MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_address, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_irq,     MP_ARG_OBJ, {.u_obj = mp_const_none} }
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);


    machine_hw_i2c_obj_t* config = (machine_hw_i2c_obj_t *)args[ARG_i2c].u_obj;

    if (config == mp_const_none)
    {
        return mp_const_none;
    }

    uint8_t address;
    
    if (args[ARG_address].u_obj == mp_const_none)
    {
        address = BMA4_I2C_ADDR_SECONDARY;
    }
    else
    {
        mp_obj_t addr = args[ARG_address].u_obj;
        address       = mp_obj_get_int(addr);
    }


    if (i2c_handle)
    {
        return mp_const_none;
    }

    i2c_handle           = (i2c_bus_t *)calloc(1, sizeof(i2c_bus_t));
    i2c_handle->i2c_port = config->port;
    bma_handle           = bma423_create((i2c_bus_handle_t *)i2c_handle, address);

    if (!bma423_reinit(bma_handle))
    {
        bma423_deinit(bma_handle);
        free(i2c_handle);
        i2c_handle = NULL;
        mp_raise_OSError(1);
        return mp_const_none;
    }
    
    bma423_set_remap_axes( bma_handle);
    bma423_enableAccel(    bma_handle);
    bma423_attachInterrupt(bma_handle);

    if (args[ARG_irq].u_obj == mp_const_none)
    {
        bma423_disalbeIrq(bma_handle);
    }
    else
    {
        mp_obj_t irq = args[ARG_irq].u_obj;
        
        if (false == mp_obj_is_true(irq))
        {
            bma423_disalbeIrq(bma_handle);
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bma_sensor_init_obj, 0, bma_sensor_init);




STATIC mp_obj_t bma_sensor_deinit(void)
{
    if (bma_handle)
    {
        bma423_deinit(bma_handle);
        bma_handle = NULL;
    }
    
    if (i2c_handle)
    {
        free(i2c_handle);
        i2c_handle = NULL;
    }
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(bma_sensor_deinit_obj, bma_sensor_deinit);




STATIC mp_obj_t accel_read(void)
{
    Accel    accel;
    mp_obj_t value[3] ;
    
    if (!bma_getAccel(bma_handle, &accel))
    {
        return mp_const_none;
    }
    
    value[0]   = MP_OBJ_NEW_SMALL_INT(accel.x);
    value[1]   = MP_OBJ_NEW_SMALL_INT(accel.y);
    value[2]   = MP_OBJ_NEW_SMALL_INT(accel.z);
    
    mp_obj_t t = mp_obj_new_tuple(3, value);
    return   t;

}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(accel_read_obj, accel_read);




STATIC mp_obj_t direction_read(void)
{
    return MP_OBJ_NEW_SMALL_INT(bma423_direction(bma_handle));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(direction_read_obj, direction_read);




STATIC mp_obj_t temperature_read(void)
{
    return mp_obj_new_float(bma423_temperature(bma_handle));
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(temperature_read_obj, temperature_read);




STATIC mp_obj_t stepcount_read(void)
{
    return mp_obj_new_int_from_uint(bma423_getCounter(bma_handle));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(stepcount_read_obj, stepcount_read);




STATIC mp_obj_t interrupt_read(void)
{
    if (bma423_readInterrupt(bma_handle))
    {
        if (bma423_isStepCounter(bma_handle))
        {
            return  MP_OBJ_NEW_SMALL_INT(BMA_STEP_COUNTER);
        }
        else if (bma423_isDoubleClick(bma_handle))
        {
            return  MP_OBJ_NEW_SMALL_INT(BMA_DOUBLE_WAKEUP);
        }
    }
    
    return  MP_OBJ_NEW_SMALL_INT(BMA_INVALID_STATE);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(interrupt_read_obj, interrupt_read);




STATIC mp_obj_t interrupt_en(mp_obj_t val)
{
    if (mp_obj_is_true(val))
    {
        return mp_obj_new_bool(bma423_enableIrq(bma_handle));
    }
    else
    {
        return mp_obj_new_bool(bma423_disalbeIrq(bma_handle));
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(interrupt_en_obj, interrupt_en);




STATIC mp_obj_t stepcounter_clear(void)
{
    return mp_obj_new_bool(bma423_clearCounter(bma_handle));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(stepcounter_clear_obj, stepcounter_clear);




STATIC const mp_map_elem_t globals_dict_table[] =
{
    { MP_ROM_QSTR(MP_QSTR___name__),              MP_ROM_QSTR(MP_QSTR_bma423) },
    { MP_ROM_QSTR(MP_QSTR_init),                  (mp_obj_t)&bma_sensor_init_obj},
    { MP_ROM_QSTR(MP_QSTR___del__),               (mp_obj_t)&bma_sensor_deinit_obj},
    { MP_ROM_QSTR(MP_QSTR_irq_read),              (mp_obj_t)&interrupt_read_obj},
    { MP_ROM_QSTR(MP_QSTR_irq_enable),            (mp_obj_t)&interrupt_en_obj},
    { MP_ROM_QSTR(MP_QSTR_step_count),            (mp_obj_t)&stepcount_read_obj},
    { MP_ROM_QSTR(MP_QSTR_step_reset),            (mp_obj_t)&stepcounter_clear_obj},
    { MP_ROM_QSTR(MP_QSTR_accel),                 (mp_obj_t)&accel_read_obj},
    { MP_ROM_QSTR(MP_QSTR_direction),             (mp_obj_t)&direction_read_obj},
    { MP_ROM_QSTR(MP_QSTR_temp),                  (mp_obj_t)&temperature_read_obj},
    { MP_ROM_QSTR(MP_QSTR_IRQ_INVALID),           MP_ROM_INT(BMA_INVALID_STATE) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_STEP_COUNTER),      MP_ROM_INT(BMA_STEP_COUNTER) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_DOUBLE_WAKEUP),     MP_ROM_INT(BMA_DOUBLE_WAKEUP) },
    { MP_ROM_QSTR(MP_QSTR_DIRECTION_TOP_EDGE),    MP_ROM_INT(DIRECTION_TOP_EDGE) },
    { MP_ROM_QSTR(MP_QSTR_DIRECTION_BOTTOM_EDGE), MP_ROM_INT(DIRECTION_BOTTOM_EDGE) },
    { MP_ROM_QSTR(MP_QSTR_DIRECTION_LEFT_EDGE),   MP_ROM_INT(DIRECTION_LEFT_EDGE) },
    { MP_ROM_QSTR(MP_QSTR_DIRECTION_RIGHT_EDGE),  MP_ROM_INT(DIRECTION_RIGHT_EDGE) },
    { MP_ROM_QSTR(MP_QSTR_DIRECTION_DISP_UP),     MP_ROM_INT(DIRECTION_DISP_UP) },
    { MP_ROM_QSTR(MP_QSTR_DIRECTION_DISP_DOWN),   MP_ROM_INT(DIRECTION_DISP_DOWN) },
};
STATIC MP_DEFINE_CONST_DICT(globals_dict, globals_dict_table);


const mp_obj_module_t mp_module_bma423 = {
    .base    = {&mp_type_module},
    .globals = (mp_obj_t) &globals_dict,
};
MP_REGISTER_MODULE(MP_QSTR_bma423, mp_module_bma423, 1);

