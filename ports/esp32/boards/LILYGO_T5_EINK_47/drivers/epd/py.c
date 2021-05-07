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
#include "py/runtime.h"
#include "py/obj.h"

#include "epd_driver.h"


STATIC mp_obj_t py_epd_init(void)
{
    MP_THREAD_GIL_EXIT();
    epd_init();
    MP_THREAD_GIL_ENTER();
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_epd_init_obj, py_epd_init);


STATIC mp_obj_t py_epd_on(void)
{
    MP_THREAD_GIL_EXIT();
    epd_poweron();
    MP_THREAD_GIL_ENTER();
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_epd_on_obj, py_epd_on);


STATIC mp_obj_t py_epd_off(void)
{
    MP_THREAD_GIL_EXIT();
    epd_poweroff();
    MP_THREAD_GIL_ENTER();
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_epd_off_obj, py_epd_off);


STATIC mp_obj_t py_epd_power_off(void)
{
    MP_THREAD_GIL_EXIT();
    epd_poweroff_all();
    MP_THREAD_GIL_ENTER();
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_epd_power_off_obj, py_epd_power_off);


STATIC mp_obj_t py_epd_clear(void)
{
    MP_THREAD_GIL_EXIT();
    epd_clear();
    MP_THREAD_GIL_ENTER();
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_epd_clear_obj, py_epd_clear);


STATIC mp_obj_t py_epd_draw_image(size_t          aArgsCnt,
                                  const mp_obj_t* aArgs)
{
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(aArgs[4], &bufinfo, MP_BUFFER_READ);
    
    Rect_t area =
    {
        MP_OBJ_SMALL_INT_VALUE(aArgs[0]),
        MP_OBJ_SMALL_INT_VALUE(aArgs[1]),
        MP_OBJ_SMALL_INT_VALUE(aArgs[2]),
        MP_OBJ_SMALL_INT_VALUE(aArgs[3])
    };
    
    MP_THREAD_GIL_EXIT();
    epd_draw_image(area, bufinfo.buf, MP_OBJ_SMALL_INT_VALUE(aArgs[5]));
    MP_THREAD_GIL_ENTER();
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(py_epd_draw_image_obj, 6, 6, py_epd_draw_image);


STATIC mp_obj_t py_epd_clear_area(size_t          aArgsCnt,
                                  const mp_obj_t* aArgs)
{
    Rect_t area =
    {
        MP_OBJ_SMALL_INT_VALUE(aArgs[0]),
        MP_OBJ_SMALL_INT_VALUE(aArgs[1]),
        MP_OBJ_SMALL_INT_VALUE(aArgs[2]),
        MP_OBJ_SMALL_INT_VALUE(aArgs[3])
    };
    
    MP_THREAD_GIL_EXIT();
    epd_clear_area(area);
    MP_THREAD_GIL_ENTER();
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(py_epd_clear_area_obj, 4, 4, py_epd_clear_area);


STATIC const mp_map_elem_t globals_dict_table[] =
{
    { MP_ROM_QSTR(MP_QSTR___name__),        MP_ROM_QSTR(MP_QSTR_epd)         },
    { MP_ROM_QSTR(MP_QSTR___init__),        (mp_obj_t)&py_epd_init_obj       },
    { MP_ROM_QSTR(MP_QSTR_on),              (mp_obj_t)&py_epd_on_obj         },
    { MP_ROM_QSTR(MP_QSTR_off),             (mp_obj_t)&py_epd_off_obj        },
    { MP_ROM_QSTR(MP_QSTR_power_off),       (mp_obj_t)&py_epd_power_off_obj  },
    { MP_ROM_QSTR(MP_QSTR_clear),           (mp_obj_t)&py_epd_clear_obj      },
    { MP_ROM_QSTR(MP_QSTR_clear_area),      (mp_obj_t)&py_epd_clear_area_obj },
    { MP_ROM_QSTR(MP_QSTR_draw_image),      (mp_obj_t)&py_epd_draw_image_obj },
    { MP_ROM_QSTR(MP_QSTR_BLACK_ON_WHITE),  MP_ROM_INT(BLACK_ON_WHITE)       },
    { MP_ROM_QSTR(MP_QSTR_WHITE_ON_WHITE),  MP_ROM_INT(WHITE_ON_WHITE)       },
    { MP_ROM_QSTR(MP_QSTR_WHITE_ON_BLACK),  MP_ROM_INT(WHITE_ON_BLACK)       },
    { MP_ROM_QSTR(MP_QSTR_DRAW_BACKGROUND), MP_ROM_INT(DRAW_BACKGROUND)      },
};
STATIC MP_DEFINE_CONST_DICT(globals_dict, globals_dict_table);


const mp_obj_module_t mp_module_epd =
{
    .base    = {&mp_type_module},
    .globals = (mp_obj_t)&globals_dict,
};
MP_REGISTER_MODULE(MP_QSTR_epd, mp_module_epd, 1);

