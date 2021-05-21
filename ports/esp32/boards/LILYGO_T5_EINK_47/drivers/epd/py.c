/*
 * This file is part of the MicroPython ESP32 project, https://github.com/lewisxhe/MicroPython_ESP32_psRAM_LoBo
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 OSi (Ondrej Sienczak)
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
#include "py/binary.h"
#include "py/objarray.h"
#include "py/runtime.h"


#include "epd_driver.h"
#include <stdint.h>


typedef struct my_epd_obj
{
    mp_obj_base_t   base;
    mp_obj_array_t* fb;
}
my_epd_obj;


static mp_obj_array_t* new_ba(size_t aSize)
{
    mp_obj_array_t* ba = m_malloc(sizeof(*ba) + aSize);
    ba->base.type      = &mp_type_bytearray;
    ba->typecode       = BYTEARRAY_TYPECODE;
    ba->free           = 0;
    ba->len            = aSize;
    ba->items          = ba + 1;
    return ba;
}


static bool                singleton;
static const mp_obj_type_t py_epd_type;


mp_obj_t my_epd_make_new(const mp_obj_type_t* aType,
                         size_t               aArgsCnt,
                         size_t               aKwCnt,
                         const mp_obj_t*      aArgs)
{
    if (singleton)
    {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Epd object can be created only once (singleton)"));
    }
    
    singleton = true;
    
    mp_arg_check_num(aArgsCnt, aKwCnt, 0, 0, false);
    
    my_epd_obj* self = m_new(my_epd_obj, 1);
    self->base.type  = &py_epd_type;
    
    epd_init();
    self->fb = new_ba(EPD_WIDTH / 2 * EPD_HEIGHT);
    
    return MP_OBJ_FROM_PTR(self);
}


STATIC mp_obj_t py_epd_fb(mp_obj_t aSelf)
{
    my_epd_obj* self = MP_OBJ_TO_PTR(aSelf);
    return self->fb;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_epd_fb_obj, py_epd_fb);


STATIC mp_obj_t py_epd_on(mp_obj_t aSelf)
{
    MP_THREAD_GIL_EXIT();
    epd_poweron();
    MP_THREAD_GIL_ENTER();
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_epd_on_obj, py_epd_on);


STATIC mp_obj_t py_epd_off(mp_obj_t aSelf)
{
    MP_THREAD_GIL_EXIT();
    epd_poweroff();
    MP_THREAD_GIL_ENTER();
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_epd_off_obj, py_epd_off);


STATIC mp_obj_t py_epd_power_off(mp_obj_t aSelf)
{
    MP_THREAD_GIL_EXIT();
    epd_poweroff_all();
    MP_THREAD_GIL_ENTER();
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_epd_power_off_obj, py_epd_power_off);


STATIC mp_obj_t py_epd_clear(mp_obj_t aSelf)
{
    MP_THREAD_GIL_EXIT();
    epd_clear();
    MP_THREAD_GIL_ENTER();
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_epd_clear_obj, py_epd_clear);


STATIC mp_obj_t py_epd_flush(mp_obj_t aSelf)
{
    my_epd_obj* self = MP_OBJ_TO_PTR(aSelf);
    Rect_t      area = { 0, 0, EPD_WIDTH, EPD_HEIGHT };
    
    MP_THREAD_GIL_EXIT();
    epd_draw_image(area, self->fb->items, BLACK_ON_WHITE);
    MP_THREAD_GIL_ENTER();
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_epd_flush_obj, py_epd_flush);


STATIC mp_obj_t py_epd_clear_area(size_t          aArgsCnt,
                                  const mp_obj_t* aArgs)
{
    Rect_t area =
    {
        MP_OBJ_SMALL_INT_VALUE(aArgs[1]),
        MP_OBJ_SMALL_INT_VALUE(aArgs[2]),
        MP_OBJ_SMALL_INT_VALUE(aArgs[3]),
        MP_OBJ_SMALL_INT_VALUE(aArgs[4])
    };
    
    MP_THREAD_GIL_EXIT();
    epd_clear_area(area);
    MP_THREAD_GIL_ENTER();
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(py_epd_clear_area_obj, 5, 5, py_epd_clear_area);



STATIC const mp_rom_map_elem_t py_epd_locals_dict_table[] =
{
    { MP_ROM_QSTR(MP_QSTR_fb),          MP_ROM_PTR(&py_epd_fb_obj)         },
    { MP_ROM_QSTR(MP_QSTR_on),          MP_ROM_PTR(&py_epd_on_obj)         },
    { MP_ROM_QSTR(MP_QSTR_off),         MP_ROM_PTR(&py_epd_off_obj)        },
    { MP_ROM_QSTR(MP_QSTR_power_off),   MP_ROM_PTR(&py_epd_power_off_obj)  },
    { MP_ROM_QSTR(MP_QSTR_clear),       MP_ROM_PTR(&py_epd_clear_obj)      },
    { MP_ROM_QSTR(MP_QSTR_clear_area),  MP_ROM_PTR(&py_epd_clear_area_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush),       MP_ROM_PTR(&py_epd_flush_obj)      },
    { MP_ROM_QSTR(MP_QSTR_WIDTH),       MP_ROM_INT(EPD_WIDTH)              },
    { MP_ROM_QSTR(MP_QSTR_HEIGHT),      MP_ROM_INT(EPD_HEIGHT)             },
};
STATIC MP_DEFINE_CONST_DICT(py_epd_locals_dict, py_epd_locals_dict_table);


static const mp_obj_type_t py_epd_type =
{
    { &mp_type_type },
    .name        = MP_QSTR_Epd,
    .make_new    = my_epd_make_new,
    .locals_dict = MP_ROM_PTR(&py_epd_locals_dict)
};


STATIC const mp_map_elem_t globals_dict_table[] =
{
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_epd) },
    { MP_ROM_QSTR(MP_QSTR_Epd),      MP_ROM_PTR(&py_epd_type) },
};
STATIC MP_DEFINE_CONST_DICT(globals_dict, globals_dict_table);


const mp_obj_module_t mp_module_epd =
{
    .base    = {&mp_type_module},
    .globals = MP_ROM_PTR(&globals_dict),
};
MP_REGISTER_MODULE(MP_QSTR_epd, mp_module_epd, 1);

