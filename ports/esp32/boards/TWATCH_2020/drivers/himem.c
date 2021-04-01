/*
 * This file is part of the MicroPython ESP32 project
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Ondrej Sienczak (OSi)
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
#include <string.h>
#include "py/objstr.h"
#include "py/runtime.h"
#include "modmachine.h"
#include "mphalport.h"
#include "esp32/himem.h"




#define ESPY__DO_OR_DIE(_fn_, _exc_) \
    do \
    { \
        esp_err_t _rc_ = _fn_; \
        \
        if (_rc_ != 0) \
        { \
            mp_raise_msg_varg(&mp_type_##_exc_, MP_ERROR_TEXT(#_fn_)" reported 0x%X", _rc_); \
        } \
    } \
    while (false)




STATIC esp_himem_handle_t      _himem;
STATIC esp_himem_rangehandle_t _range;
STATIC size_t                  _size;




STATIC uintptr_t _align(uintptr_t aMem)
{
    return (aMem / ESP_HIMEM_BLKSZ) * ESP_HIMEM_BLKSZ;
}



enum _Direction
{
    HIMEM_2_RAM,
    RAM_2_HIMEM
};


STATIC void _dir_copy(void*           aRam,
                      void*           aHimem,
                      size_t          aSize,
                      enum _Direction aDir)
{
    if (HIMEM_2_RAM == aDir)
    {
        memcpy(aRam, aHimem, aSize);
    }
    else
    {
        memcpy(aHimem, aRam, aSize);
    }
}


STATIC void _himem_copy(void*           aRam,
                        uintptr_t       aHimem,
                        size_t          aSize,
                        enum _Direction aDir)
{
    /*
     * Copy unaligned head
     */
    uintptr_t       aligned = _align(aHimem);
    uint8_t*        ram     = aRam;
    uint8_t const * end     = ram + aSize;
    
    if (aligned != aHimem)
    {
        size_t ofs = aHimem          - aligned;
        size_t cnt = ESP_HIMEM_BLKSZ - ofs;
        
        if (cnt > aSize)
        {
            cnt = aSize;
        }
        
        uint8_t* himem;
        
        ESPY__DO_OR_DIE(esp_himem_map(_himem, _range, aligned, 0, ESP_HIMEM_BLKSZ, 0, (void**)&himem), MemoryError);
        _dir_copy(ram, himem + ofs, cnt, aDir);
        ESPY__DO_OR_DIE(esp_himem_unmap(_range, himem, ESP_HIMEM_BLKSZ), MemoryError);
        
        ram    += cnt;
        aHimem += cnt;
    }
    
    if (ram == end)
    {
        return;
    }
    
    
    /*
     * Now copy aligned part
     */
    uintptr_t bytes = _align(end - ram);
    
    while (bytes != 0)
    {
        uint8_t* himem;
        
        ESPY__DO_OR_DIE(esp_himem_map(_himem, _range, aHimem, 0, ESP_HIMEM_BLKSZ, 0, (void**)&himem), MemoryError);
        _dir_copy(ram, himem, ESP_HIMEM_BLKSZ, aDir);
        ESPY__DO_OR_DIE(esp_himem_unmap(_range, himem, ESP_HIMEM_BLKSZ), MemoryError);
        
        ram    += ESP_HIMEM_BLKSZ;
        aHimem += ESP_HIMEM_BLKSZ;
        bytes  -= ESP_HIMEM_BLKSZ;
    }
    
    
    /*
     * And also copy size unaligned tail
     */
    if (ram != end)
    {
        uint8_t* himem;
        
        ESPY__DO_OR_DIE(esp_himem_map(_himem, _range, aHimem, 0, ESP_HIMEM_BLKSZ, 0, (void**)&himem), MemoryError);
        _dir_copy(ram, himem, end - ram, aDir);
        ESPY__DO_OR_DIE(esp_himem_unmap(_range, himem, ESP_HIMEM_BLKSZ), MemoryError);
    }
}


STATIC void _obj2buff(mp_buffer_info_t* aBuff,
                      mp_obj_t          aObj,
                      unsigned          aRW)
{
    if (mp_obj_is_str_or_bytes(aObj))
    {
        GET_STR_DATA_LEN(aObj, buf, len);
        aBuff->buf = buf;
        aBuff->len = len;
    }
    else
    {
        mp_get_buffer_raise(aObj, aBuff, aRW);
    }
}




STATIC mp_obj_t himem_init(void)
{
    _size = esp_himem_get_free_size();
    
    ESPY__DO_OR_DIE(esp_himem_alloc(_size, &_himem),                     MemoryError);
    ESPY__DO_OR_DIE(esp_himem_alloc_map_range(ESP_HIMEM_BLKSZ, &_range), MemoryError);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(himem_init_obj, himem_init);




STATIC mp_obj_t himem_size(void)
{
    return MP_OBJ_NEW_SMALL_INT(_size);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(himem_size_obj, himem_size);




STATIC mp_obj_t himem_page(void)
{
    return MP_OBJ_NEW_SMALL_INT(ESP_HIMEM_BLKSZ);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(himem_page_obj, himem_page);




STATIC mp_obj_t himem_read(mp_obj_t aAddress,
                           mp_obj_t aSize)
{
    uintptr_t      addr = mp_obj_get_int(aAddress);
    size_t         size = mp_obj_get_int(aSize);
    vstr_t         bytes;
    
    vstr_init_len(&bytes, size);
    
    _himem_copy(bytes.buf, addr, size, HIMEM_2_RAM);
    
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &bytes);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(himem_read_obj, himem_read);




STATIC mp_obj_t himem_write(mp_obj_t aAddress,
                            mp_obj_t aBuffer)
{
    uintptr_t        addr  = mp_obj_get_int(aAddress);
    mp_buffer_info_t buff;
    
    _obj2buff(&buff, aBuffer, MP_BUFFER_READ);
    _himem_copy(buff.buf, addr, buff.len, RAM_2_HIMEM);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(himem_write_obj, himem_write);




STATIC const mp_map_elem_t globals_dict_table[] =
{
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_himem)   },
    { MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&himem_init_obj)  },
    { MP_ROM_QSTR(MP_QSTR_size),     MP_ROM_PTR(&himem_size_obj)  },
    { MP_ROM_QSTR(MP_QSTR_page),     MP_ROM_PTR(&himem_page_obj)  },
    { MP_ROM_QSTR(MP_QSTR_read),     MP_ROM_PTR(&himem_read_obj)  },
    { MP_ROM_QSTR(MP_QSTR_write),    MP_ROM_PTR(&himem_write_obj) }
};
STATIC MP_DEFINE_CONST_DICT(globals_dict, globals_dict_table);


const mp_obj_module_t mp_module_himem =
{
    .base    = {&mp_type_module},
    .globals = (mp_obj_t)&globals_dict,
};
MP_REGISTER_MODULE(MP_QSTR_himem, mp_module_himem, 1);

