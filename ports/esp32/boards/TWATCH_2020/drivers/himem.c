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
#include "extmod/vfs.h"
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
STATIC bool                    _locked;
STATIC size_t                  _raw_size;
STATIC size_t                  _fs_size;
STATIC size_t                  _block_size = ESP_HIMEM_BLKSZ;
STATIC size_t                  _block_cnt;




STATIC uintptr_t _align(uintptr_t aMem)
{
    return (aMem / ESP_HIMEM_BLKSZ) * ESP_HIMEM_BLKSZ;
}



enum _Direction
{
    HIMEM_2_RAM,
    RAM_2_HIMEM,
    HIMEM_SET
};


STATIC void _dir_copy(void*    aRam,
                      void*    aHimem,
                      size_t   aSize,
                      unsigned aDir)
{
    enum _Direction dir = (enum _Direction)(aDir & 255);
    
    if (HIMEM_2_RAM == dir)
    {
        memcpy(aRam, aHimem, aSize);
    }
    else if (RAM_2_HIMEM == dir)
    {
        memcpy(aHimem, aRam, aSize);
    }
    else
    {
        memset(aHimem, aDir >> 8, aSize);
    }
}


STATIC void _himem_op(void*           aRam,
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
    size_t size = _raw_size + _fs_size;
    
    if (0 == size)
    {
        _fs_size   = esp_himem_get_free_size();
        _block_cnt = _fs_size / _block_size;
        
        ESPY__DO_OR_DIE(esp_himem_alloc(_fs_size, &_himem),                  MemoryError);
        ESPY__DO_OR_DIE(esp_himem_alloc_map_range(ESP_HIMEM_BLKSZ, &_range), MemoryError);
    }
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(himem_init_obj, himem_init);




STATIC mp_obj_t himem_fs_size(size_t          aArgsCnt,
                              const mp_obj_t* aArgs)
{
    if (aArgsCnt > 0)
    {
        if (_locked)
        {
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Size of area locked"));
        }
        
        if (!mp_obj_is_small_int(aArgs[0]))
        {
            mp_raise_msg(&mp_type_TypeError, MP_ERROR_TEXT("Size of area shall be an integer"));
        }
        
        size_t size     = _raw_size + _fs_size;
        size_t new_size = _align(MP_OBJ_SMALL_INT_VALUE(aArgs[0]) + ESP_HIMEM_BLKSZ - 1);
        
        if (new_size > size)
        {
            mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Requested size of area to large"));
        }
        
        _fs_size   = new_size;
        _block_cnt = _fs_size / _block_size;
        _raw_size  = size - new_size;
    }
    
    return MP_OBJ_NEW_SMALL_INT(_fs_size);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(himem_fs_size_obj, 0, himem_fs_size);




STATIC mp_obj_t himem_block_size(size_t          aArgsCnt,
                                 const mp_obj_t* aArgs)
{
    if (aArgsCnt > 0)
    {
        if (_locked)
        {
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Size of area locked"));
        }
        
        if (!mp_obj_is_small_int(aArgs[0]))
        {
            mp_raise_msg(&mp_type_TypeError, MP_ERROR_TEXT("Size of area shall be an integer"));
        }
        
        size_t new_size = MP_OBJ_SMALL_INT_VALUE(aArgs[0]);
        
        if (new_size > _fs_size)
        {
            mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Requested size of area to large"));
        }
        
        _block_size = new_size;
        _block_cnt  = _fs_size / _block_size;
    }
    
    return MP_OBJ_NEW_SMALL_INT(_block_size);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(himem_block_size_obj, 0, himem_block_size);




STATIC mp_obj_t himem_raw_size(size_t          aArgsCnt,
                               const mp_obj_t* aArgs)
{
    if (aArgsCnt > 0)
    {
        if (_locked)
        {
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Size of area locked"));
        }
        
        if (!mp_obj_is_small_int(aArgs[0]))
        {
            mp_raise_msg(&mp_type_TypeError, MP_ERROR_TEXT("Size of area shall be an integer"));
        }
        
        size_t size     = _raw_size + _fs_size;
        size_t new_size = _align(MP_OBJ_SMALL_INT_VALUE(aArgs[0]) + ESP_HIMEM_BLKSZ - 1);
        
        if (new_size > size)
        {
            mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Requested size of area to large"));
        }
        
        _fs_size   = size - new_size;
        _block_cnt = _fs_size / _block_size;
        _raw_size  = new_size;
    }
    
    return MP_OBJ_NEW_SMALL_INT(_raw_size);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(himem_raw_size_obj, 0, himem_raw_size);




STATIC mp_obj_t himem_page(void)
{
    return MP_OBJ_NEW_SMALL_INT(ESP_HIMEM_BLKSZ);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(himem_page_obj, himem_page);




STATIC mp_obj_t himem_read(mp_obj_t aAddress,
                           mp_obj_t aSize)
{
    _locked = true;
    
    uintptr_t      addr = mp_obj_get_int(aAddress);
    size_t         size = mp_obj_get_int(aSize);
    vstr_t         bytes;
    
    if (addr + size > _raw_size)
    {
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("Area out of range"));
    }
    
    vstr_init_len(&bytes, size);
    
    _himem_op(bytes.buf, addr, size, HIMEM_2_RAM);
    
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &bytes);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(himem_read_obj, himem_read);




STATIC mp_obj_t himem_write(mp_obj_t aAddress,
                            mp_obj_t aBuffer)
{
    _locked = true;
    
    uintptr_t        addr  = mp_obj_get_int(aAddress);
    mp_buffer_info_t buff;
    
    _obj2buff(&buff, aBuffer, MP_BUFFER_READ);
    
    if (addr + buff.len > _raw_size)
    {
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("Area out of range"));
    }
    
    _himem_op(buff.buf, addr, buff.len, RAM_2_HIMEM);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(himem_write_obj, himem_write);




STATIC mp_obj_t himem_set(mp_obj_t aAddress,
                          mp_obj_t aValue,
                          mp_obj_t aSize)
{
    _locked = true;
    
    uintptr_t addr  = mp_obj_get_int(aAddress);
    unsigned  value = mp_obj_get_int(aValue);
    size_t    size  = mp_obj_get_int(aSize);
    
    if (addr + size > _raw_size)
    {
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("Area out of range"));
    }
    
    _himem_op(NULL, addr, size, HIMEM_SET + (value << 8));
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(himem_set_obj, himem_set);




STATIC mp_obj_t himem_readblocks(size_t          aArgsCnt,
                                 const mp_obj_t* aArgs)
{
    _locked = true;
    
    size_t offset = mp_obj_get_int(aArgs[0]) * _block_size;
    
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(aArgs[1], &bufinfo, MP_BUFFER_WRITE);
    
    if (aArgsCnt == 3)
    {
        offset += mp_obj_get_int(aArgs[2]);
    }
    
    _himem_op(bufinfo.buf, _raw_size + offset, bufinfo.len, HIMEM_2_RAM);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(himem_readblocks_obj, 2, 3, himem_readblocks);




STATIC mp_obj_t himem_writeblocks(size_t          aArgsCnt,
                                  const mp_obj_t* aArgs)
{
    _locked = true;
    
    size_t offset = mp_obj_get_int(aArgs[0]) * _block_size;
    
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(aArgs[1], &bufinfo, MP_BUFFER_WRITE);
    
    if (aArgsCnt == 3)
    {
        offset += mp_obj_get_int(aArgs[2]);
    }
    
    _himem_op(bufinfo.buf, _raw_size + offset, bufinfo.len, RAM_2_HIMEM);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(himem_writeblocks_obj, 2, 3, himem_writeblocks);




STATIC mp_obj_t himem_ioctl(mp_obj_t aCmd,
                            mp_obj_t aArg)
{
    _locked = true;
    
    mp_int_t cmd = mp_obj_get_int(aCmd);
    
    switch (cmd)
    {
        case MP_BLOCKDEV_IOCTL_INIT:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_DEINIT:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_SYNC:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            return MP_OBJ_NEW_SMALL_INT(_block_cnt);
        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            return MP_OBJ_NEW_SMALL_INT(_block_size);
        case MP_BLOCKDEV_IOCTL_BLOCK_ERASE:
        {
            _himem_op(NULL,
                      _raw_size + (mp_obj_get_int(aArg) * _block_size),
                      _block_size,
                      HIMEM_SET + (0xFF << 8));
            
            return MP_OBJ_NEW_SMALL_INT(0);
        }
        default:
            return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(himem_ioctl_obj, himem_ioctl);




STATIC const mp_map_elem_t globals_dict_table[] =
{
    { MP_ROM_QSTR(MP_QSTR___name__),    MP_ROM_QSTR(MP_QSTR_himem)         },
    { MP_ROM_QSTR(MP_QSTR___init__),    MP_ROM_PTR(&himem_init_obj)        },
    { MP_ROM_QSTR(MP_QSTR_read),        MP_ROM_PTR(&himem_read_obj)        },
    { MP_ROM_QSTR(MP_QSTR_write),       MP_ROM_PTR(&himem_write_obj)       },
    { MP_ROM_QSTR(MP_QSTR_set),         MP_ROM_PTR(&himem_set_obj)         },
    { MP_ROM_QSTR(MP_QSTR_readblocks),  MP_ROM_PTR(&himem_readblocks_obj)  },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&himem_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl),       MP_ROM_PTR(&himem_ioctl_obj)       },
    { MP_ROM_QSTR(MP_QSTR_fs_size),     MP_ROM_PTR(&himem_fs_size_obj)     },
    { MP_ROM_QSTR(MP_QSTR_raw_size),    MP_ROM_PTR(&himem_raw_size_obj)    },
    { MP_ROM_QSTR(MP_QSTR_block_size),  MP_ROM_PTR(&himem_block_size_obj)  },
    { MP_ROM_QSTR(MP_QSTR_page),        MP_ROM_PTR(&himem_page_obj)        }
};
STATIC MP_DEFINE_CONST_DICT(globals_dict, globals_dict_table);


const mp_obj_module_t mp_module_himem =
{
    .base    = {&mp_type_module},
    .globals = (mp_obj_t)&globals_dict,
};
MP_REGISTER_MODULE(MP_QSTR_himem, mp_module_himem, 1);

