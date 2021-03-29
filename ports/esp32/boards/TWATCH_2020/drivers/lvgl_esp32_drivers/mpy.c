
/*
 * Auto-Generated file, DO NOT EDIT!
 *
 *
 * Generating Objects: 
 */

/*
 * Mpy includes
 */

#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/binary.h"
#include "py/objarray.h"

/*
 * st7789_lvgl includes
 */

#include "lvgl_helpers.h"


/*
 * Helper functions
 */

// Custom function mp object

typedef struct _mp_lv_obj_fun_builtin_var_t {
    mp_obj_base_t base;
    mp_uint_t n_args;
    mp_fun_var_t mp_fun;
    void *lv_fun;
} mp_lv_obj_fun_builtin_var_t;

STATIC mp_obj_t lv_fun_builtin_var_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args);
STATIC mp_int_t mp_func_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags);

STATIC const mp_obj_type_t mp_lv_type_fun_builtin_var = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_BINDS_SELF | MP_TYPE_FLAG_BUILTIN_FUN,
    .name = MP_QSTR_function,
    .call = lv_fun_builtin_var_call,
    .unary_op = mp_generic_unary_op,
    .buffer_p = { .get_buffer = mp_func_get_buffer }
};

STATIC mp_obj_t lv_fun_builtin_var_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_lv_type_fun_builtin_var));
    mp_lv_obj_fun_builtin_var_t *self = MP_OBJ_TO_PTR(self_in);
    mp_arg_check_num(n_args, n_kw, self->n_args, self->n_args, false);
    return self->mp_fun(n_args, args);
}

STATIC mp_int_t mp_func_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    (void)flags;
    assert(MP_OBJ_IS_TYPE(self_in, &mp_lv_type_fun_builtin_var));
    mp_lv_obj_fun_builtin_var_t *self = MP_OBJ_TO_PTR(self_in);

    bufinfo->buf = &self->lv_fun;
    bufinfo->len = sizeof(self->lv_fun);
    bufinfo->typecode = BYTEARRAY_TYPECODE;
    return 0;
}

#define MP_DEFINE_CONST_LV_FUN_OBJ_VAR(obj_name, n_args, mp_fun, lv_fun) \
    const mp_lv_obj_fun_builtin_var_t obj_name = \
        {{&mp_lv_type_fun_builtin_var}, n_args, mp_fun, lv_fun}

// Casting

typedef struct mp_lv_struct_t
{
    mp_obj_base_t base;
    void *data;
} mp_lv_struct_t;

STATIC const mp_lv_struct_t mp_lv_null_obj;

#ifdef LV_OBJ_T
STATIC mp_int_t mp_lv_obj_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags);
#else
STATIC mp_int_t mp_lv_obj_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags){ return 0; }
#endif

STATIC mp_obj_t get_native_obj(mp_obj_t *mp_obj)
{
    if (!MP_OBJ_IS_OBJ(mp_obj)) return mp_obj;
    const mp_obj_type_t *native_type = ((mp_obj_base_t*)mp_obj)->type;
    if (native_type->parent == NULL || 
        (native_type->buffer_p.get_buffer == mp_lv_obj_get_buffer)) return mp_obj;
    while (native_type->parent) native_type = native_type->parent;
    return mp_obj_cast_to_native_base(mp_obj, MP_OBJ_FROM_PTR(native_type));
}

STATIC mp_obj_t dict_to_struct(mp_obj_t dict, const mp_obj_type_t *type);

STATIC mp_obj_t make_new_lv_struct(
    const mp_obj_type_t *type,
    size_t n_args,
    size_t n_kw,
    const mp_obj_t *args);

STATIC mp_obj_t *cast(mp_obj_t *mp_obj, const mp_obj_type_t *mp_type)
{
    mp_obj_t *res = NULL;
    if (mp_obj == mp_const_none && mp_type->make_new == &make_new_lv_struct) {
        res = MP_OBJ_FROM_PTR(&mp_lv_null_obj);
    } else if (MP_OBJ_IS_OBJ(mp_obj)) {
        res = get_native_obj(mp_obj);
        if (res){
            const mp_obj_type_t *res_type = ((mp_obj_base_t*)res)->type;
            if (res_type != mp_type){
                if (res_type == &mp_type_dict &&
                    mp_type->make_new == &make_new_lv_struct)
                        res = dict_to_struct(res, mp_type);
                else res = NULL;
            }
        }
    }
    if (res == NULL) nlr_raise(
        mp_obj_new_exception_msg_varg(
            &mp_type_SyntaxError, MP_ERROR_TEXT("Can't convert %s to %s!"), mp_obj_get_type_str(mp_obj), qstr_str(mp_type->name)));
    return res;
}

// object handling
// This section is enabled only when objects are supported 

#ifdef LV_OBJ_T

typedef LV_OBJ_T* (*lv_create)(LV_OBJ_T * par, const LV_OBJ_T * copy);

typedef struct mp_lv_obj_t {
    mp_obj_base_t base;
    LV_OBJ_T *lv_obj;
    LV_OBJ_T *callbacks;
} mp_lv_obj_t;

STATIC inline LV_OBJ_T *mp_to_lv(mp_obj_t *mp_obj)
{
    if (mp_obj == NULL || mp_obj == mp_const_none) return NULL;
    mp_lv_obj_t *mp_lv_obj = MP_OBJ_TO_PTR(get_native_obj(mp_obj));
    return mp_lv_obj->lv_obj;
}

STATIC inline LV_OBJ_T *mp_get_callbacks(mp_obj_t mp_obj)
{
    if (mp_obj == NULL || mp_obj == mp_const_none) return NULL;
    mp_lv_obj_t *mp_lv_obj = MP_OBJ_TO_PTR(get_native_obj(mp_obj));
    if (!mp_lv_obj->callbacks) mp_lv_obj->callbacks = mp_obj_new_dict(0);
    return mp_lv_obj->callbacks;
}

STATIC inline const mp_obj_type_t *get_BaseObj_type();

STATIC inline mp_obj_t *lv_to_mp(LV_OBJ_T *lv_obj)
{
    if (lv_obj == NULL) return mp_const_none;
    mp_lv_obj_t *self = (mp_lv_obj_t*)lv_obj->user_data;
    if (!self) 
    {
        self = m_new_obj(mp_lv_obj_t);
        *self = (mp_lv_obj_t){
            .base = {get_BaseObj_type()},
            .lv_obj = lv_obj,
            .callbacks = NULL,
        };
        lv_obj->user_data = self;
    }
    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t make_new(
    lv_create create,
    const mp_obj_type_t *type,
    size_t n_args,
    size_t n_kw,
    const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 0, 2, false);
    mp_lv_obj_t *self = m_new_obj(mp_lv_obj_t);
    LV_OBJ_T *parent = n_args > 0? mp_to_lv(args[0]): NULL;
    LV_OBJ_T *copy = n_args > 1? mp_to_lv(args[1]): NULL;
    *self = (mp_lv_obj_t){
        .base = {type}, 
        .lv_obj = create(parent, copy),
        .callbacks = NULL,
    };
    if (!self->lv_obj) return mp_const_none;
    self->lv_obj->user_data = self;
    return MP_OBJ_FROM_PTR(self);
}

STATIC void* mp_to_ptr(mp_obj_t self_in);

STATIC mp_obj_t cast_obj(mp_obj_t type_obj, mp_obj_t obj)
{
    mp_lv_obj_t *self = m_new_obj(mp_lv_obj_t);
    *self = (mp_lv_obj_t){
        .base = {(const mp_obj_type_t*)type_obj},
        .lv_obj = mp_to_ptr(obj),
        .callbacks = NULL,
    };
    if (!self->lv_obj) return mp_const_none;
    return MP_OBJ_FROM_PTR(self);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(cast_obj_obj, cast_obj);
STATIC MP_DEFINE_CONST_CLASSMETHOD_OBJ(cast_obj_class_method, MP_ROM_PTR(&cast_obj_obj));

STATIC mp_int_t mp_lv_obj_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    (void)flags;
    mp_lv_obj_t *self = MP_OBJ_TO_PTR(self_in);

    bufinfo->buf = &self->lv_obj;
    bufinfo->len = sizeof(self->lv_obj);
    bufinfo->typecode = BYTEARRAY_TYPECODE;
    return 0;
}

#endif

STATIC inline mp_obj_t convert_to_bool(bool b)
{
    return b? mp_const_true: mp_const_false;
}

STATIC inline mp_obj_t convert_to_str(const char *str)
{
    return str? mp_obj_new_str(str, strlen(str)): mp_const_none;
}

// struct handling

STATIC inline mp_lv_struct_t *mp_to_lv_struct(mp_obj_t mp_obj)
{
    if (mp_obj == NULL || mp_obj == mp_const_none) return NULL;
    if (!MP_OBJ_IS_OBJ(mp_obj)) nlr_raise(
            mp_obj_new_exception_msg(
                &mp_type_SyntaxError, MP_ERROR_TEXT("Struct argument is not an object!")));
    mp_lv_struct_t *mp_lv_struct = MP_OBJ_TO_PTR(get_native_obj(mp_obj));
    return mp_lv_struct;
}

STATIC inline size_t get_lv_struct_size(const mp_obj_type_t *type)
{
    mp_obj_t size_obj = mp_obj_dict_get(type->locals_dict, MP_OBJ_NEW_QSTR(MP_QSTR_SIZE));
    return (size_t)mp_obj_get_int(size_obj);
}

STATIC mp_obj_t make_new_lv_struct(
    const mp_obj_type_t *type,
    size_t n_args,
    size_t n_kw,
    const mp_obj_t *args)
{
    if ((!MP_OBJ_IS_TYPE(type, &mp_type_type)) || type->make_new != &make_new_lv_struct)
        nlr_raise(
            mp_obj_new_exception_msg(
                &mp_type_SyntaxError, MP_ERROR_TEXT("Argument is not a struct type!")));
    size_t size = get_lv_struct_size(type);
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    mp_lv_struct_t *self = m_new_obj(mp_lv_struct_t);
    *self = (mp_lv_struct_t){
        .base = {type}, 
        .data = m_malloc(size)
    };
    mp_lv_struct_t *other = n_args > 0? mp_to_lv_struct(cast(args[0], type)): NULL;
    if (other) {
        memcpy(self->data, other->data, size);
    } else {
        memset(self->data, 0, size);
    }
    return MP_OBJ_FROM_PTR(self);
}

STATIC void *copy_buffer(const void *buffer, size_t size)
{
    void *new_buffer = m_malloc(size);
    memcpy(new_buffer, buffer, size);
    return new_buffer;
}

// Reference an existing lv struct (or part of it)

STATIC mp_obj_t lv_to_mp_struct(const mp_obj_type_t *type, void *lv_struct)
{
    if (lv_struct == NULL) return mp_const_none;
    mp_lv_struct_t *self = m_new_obj(mp_lv_struct_t);
    *self = (mp_lv_struct_t){
        .base = {type},
        .data = lv_struct
    };
    return MP_OBJ_FROM_PTR(self);
}

STATIC void call_parent_methods(mp_obj_t obj, qstr attr, mp_obj_t *dest)
{
    const mp_obj_type_t *type = mp_obj_get_type(obj);
    while (type->locals_dict != NULL) {
        // generic method lookup
        // this is a lookup in the object (ie not class or type)
        assert(type->locals_dict->base.type == &mp_type_dict); // MicroPython restriction, for now
        mp_map_t *locals_map = &type->locals_dict->map;
        mp_map_elem_t *elem = mp_map_lookup(locals_map, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP);
        if (elem != NULL) {
            mp_convert_member_lookup(obj, type, elem->value, dest);
            break;
        }
        if (type->parent == NULL) {
            break;
        }
        // search parents
        type = type->parent;
    }
}

// Convert dict to struct

STATIC mp_obj_t dict_to_struct(mp_obj_t dict, const mp_obj_type_t *type)
{
    mp_obj_t mp_struct = make_new_lv_struct(type, 0, 0, NULL);
    mp_obj_t *native_dict = cast(dict, &mp_type_dict);
    mp_map_t *map = mp_obj_dict_get_map(native_dict);
    if (map == NULL) return mp_const_none;
    for (uint i = 0; i < map->alloc; i++) {
        mp_obj_t key = map->table[i].key;
        mp_obj_t value = map->table[i].value;
        if (key != MP_OBJ_NULL) {
            mp_obj_t dest[] = {MP_OBJ_SENTINEL, value};
            type->attr(mp_struct, mp_obj_str_get_qstr(key), dest);
            if (dest[0]) nlr_raise(
                mp_obj_new_exception_msg_varg(
                    &mp_type_SyntaxError, MP_ERROR_TEXT("Cannot set field %s on struct %s!"), qstr_str(mp_obj_str_get_qstr(key)), qstr_str(type->name)));
        }
    }
    return mp_struct;
}

// Convert mp object to ptr

STATIC void* mp_to_ptr(mp_obj_t self_in)
{
    mp_buffer_info_t buffer_info;
    if (self_in == mp_const_none)
        return NULL;

//    if (MP_OBJ_IS_INT(self_in))
//        return (void*)mp_obj_get_int(self_in);

    if (!mp_get_buffer(self_in, &buffer_info, MP_BUFFER_READ)) {
        // No buffer protocol - this is not a Struct or a Blob, it's some other mp object.
        // We only allow setting dict directly, since it's useful to setting user_data for passing data to C.
        // On other cases throw an exception, to avoid a crash later
        if (MP_OBJ_IS_TYPE(self_in, &mp_type_dict))
            return MP_OBJ_TO_PTR(self_in);
        else nlr_raise(
                mp_obj_new_exception_msg_varg(
                    &mp_type_SyntaxError, MP_ERROR_TEXT("Cannot convert '%s' to pointer!"), mp_obj_get_type_str(self_in)));
    }

    if (MP_OBJ_IS_STR_OR_BYTES(self_in) || 
        MP_OBJ_IS_TYPE(self_in, &mp_type_bytearray) ||
        MP_OBJ_IS_TYPE(self_in, &mp_type_memoryview))
            return buffer_info.buf;
    else
    {
        void *result;
        if (buffer_info.len != sizeof(result) || buffer_info.typecode != BYTEARRAY_TYPECODE){
            nlr_raise(
                mp_obj_new_exception_msg_varg(
                    &mp_type_SyntaxError, MP_ERROR_TEXT("Cannot convert %s to pointer! (buffer does not represent a pointer)"), mp_obj_get_type_str(self_in)));
        }
        memcpy(&result, buffer_info.buf, sizeof(result));
        return result;
    }
}

// Blob is a wrapper for void* 

STATIC void mp_blob_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "Blob");
}

STATIC mp_int_t mp_blob_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    (void)flags;
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);

    bufinfo->buf = &self->data;
    bufinfo->len = sizeof(self->data);
    bufinfo->typecode = BYTEARRAY_TYPECODE;
    return 0;
}

STATIC const mp_obj_fun_builtin_var_t mp_lv_dereference_obj;

// Sometimes (but not always!) Blob represents a Micropython object.
// In such cases it's safe to cast the Blob back to the Micropython object
// cast argument is the underlying object type, and it's optional.

STATIC mp_obj_t mp_blob_cast(size_t argc, const mp_obj_t *argv)
{
    mp_obj_t self = argv[0];
    void *ptr = mp_to_ptr(self);
    if (argc == 1) return MP_OBJ_FROM_PTR(ptr);
    mp_obj_t type = argv[1];
    if (!MP_OBJ_IS_TYPE(type, &mp_type_type))
        nlr_raise(
            mp_obj_new_exception_msg(
                &mp_type_SyntaxError, MP_ERROR_TEXT("Cast argument must be a type!")));
    return cast(MP_OBJ_FROM_PTR(ptr), type);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_blob_cast_obj, 1, 2, mp_blob_cast);

STATIC const mp_rom_map_elem_t mp_blob_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___dereference__), MP_ROM_PTR(&mp_lv_dereference_obj) },
    { MP_ROM_QSTR(MP_QSTR_cast), MP_ROM_PTR(&mp_blob_cast_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_blob_locals_dict, mp_blob_locals_dict_table);

STATIC const mp_obj_type_t mp_blob_type = {
    { &mp_type_type },
    .name = MP_QSTR_Blob,
    .print = mp_blob_print,
    //.make_new = make_new_blob,
    .locals_dict = (mp_obj_dict_t*)&mp_blob_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer }
};

STATIC const mp_lv_struct_t mp_lv_null_obj = { {&mp_blob_type}, NULL };

STATIC inline mp_obj_t ptr_to_mp(void *data)
{
    return lv_to_mp_struct(&mp_blob_type, data);
}

// Cast pointer to struct

STATIC mp_obj_t mp_lv_cast(mp_obj_t type_obj, mp_obj_t ptr_obj)
{
    mp_lv_struct_t *self = m_new_obj(mp_lv_struct_t);
    *self = (mp_lv_struct_t){
        .base = {(const mp_obj_type_t*)type_obj}, 
        .data = mp_to_ptr(ptr_obj)
    };
    return MP_OBJ_FROM_PTR(self);
}

// Cast instance. Can be used in ISR when memory allocation is prohibited

STATIC inline mp_obj_t mp_lv_cast_instance(mp_obj_t self_in, mp_obj_t ptr_obj)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    self->data = mp_to_ptr(ptr_obj);
    return self_in;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_lv_cast_obj, mp_lv_cast);
STATIC MP_DEFINE_CONST_CLASSMETHOD_OBJ(mp_lv_cast_class_method, MP_ROM_PTR(&mp_lv_cast_obj));

STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_lv_cast_instance_obj, mp_lv_cast_instance);

// Dereference a struct/blob. This allows access to the raw data the struct holds

STATIC mp_obj_t mp_lv_dereference(size_t argc, const mp_obj_t *argv)
{
    mp_obj_t self_in = argv[0];
    mp_obj_t size_in = argc > 1? argv[1]: mp_const_none;
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    size_t size = 0;
    if (size_in == mp_const_none){
        const mp_obj_type_t *type = self->base.type;
        size = get_lv_struct_size(type);
    } else {
        size = (size_t)mp_obj_get_int(size_in);
    }
    mp_obj_array_t *view = MP_OBJ_TO_PTR(mp_obj_new_memoryview(BYTEARRAY_TYPECODE,
        size, self->data));
    view->typecode |= 0x80; // used to indicate writable buffer
    return MP_OBJ_FROM_PTR(view);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_lv_dereference_obj, 1, 2, mp_lv_dereference);

// Callback function handling
// Callback is either a callable object or a pointer. If it's a callable object, set user_data to the callback.
// Multiple callbacks are kept per object/struct using a dict that associate callback name with callback object
// In case of an lv_obj_t, user_data is mp_lv_obj_t which contains a member "callbacks" for that dict.
// In case of a struct, user_data is a pointer to that dict directly

STATIC mp_obj_t get_callback_dict_from_user_data(void *user_data)
{
    if (user_data){
        mp_obj_t obj = MP_OBJ_FROM_PTR(user_data);
#ifdef LV_OBJ_T
        return 
            MP_OBJ_IS_TYPE(obj, &mp_type_dict)? obj: // Handle the case of dict for a struct
            mp_get_callbacks(obj); // Handle the case of mp_lv_obj_t for an lv_obj_t
#else
        return obj;
#endif
    }
    return NULL;
}

STATIC void *mp_lv_callback(mp_obj_t mp_callback, void *lv_callback, qstr callback_name, void **user_data_ptr)
{
    if (lv_callback && mp_obj_is_callable(mp_callback)){
        if (user_data_ptr){
            // user_data is either a dict of callbacks in case of struct, or a pointer to mp_lv_obj_t in case of lv_obj_t
            if (! (*user_data_ptr) ) *user_data_ptr = MP_OBJ_TO_PTR(mp_obj_new_dict(0)); // if it's NULL - it's a dict for a struct
            mp_obj_t callbacks = get_callback_dict_from_user_data(*user_data_ptr);
            mp_obj_dict_store(callbacks, MP_OBJ_NEW_QSTR(callback_name), mp_callback);
        }
        return lv_callback;
    } else {
        return mp_to_ptr(mp_callback);
    }
}

// Dict to hold user data for global callbacks (callbacks without context)



/*
 * LVGL string constants
 */


/*
 * Helper Structs
 */
        

typedef union {
    void*           ptr_val;
    const char*     str_val;
    int             int_val;
    unsigned int    uint_val;
    short           short_val[sizeof(void*) / sizeof(short)];
    unsigned short  ushort_val[sizeof(void*) / sizeof(unsigned short)];
    char            char_val[sizeof(void*) / sizeof(char)];
    unsigned char   uchar_val[sizeof(void*) / sizeof(unsigned char)];
} C_Pointer;


/*
 * Array convertors for short [(sizeof(void *)) / (sizeof(short))]
 */

STATIC short *mp_arr_to_short_____sizeof__void_ptr______div____sizeof__short______(mp_obj_t mp_arr)
{
    mp_obj_t mp_len = mp_obj_len_maybe(mp_arr);
    if (mp_len == MP_OBJ_NULL) return mp_to_ptr(mp_arr);
    mp_int_t len = mp_obj_get_int(mp_len);
    //TODO check dim!
    short *lv_arr = (short*)m_malloc(len * sizeof(short));
    mp_obj_t iter = mp_getiter(mp_arr, NULL);
    mp_obj_t item;
    size_t i = 0;
    while ((item = mp_iternext(iter)) != MP_OBJ_STOP_ITERATION) {
        lv_arr[i++] = (short)mp_obj_get_int(item);
    }
    return (short *)lv_arr;
}
    
STATIC mp_obj_t mp_arr_from_short_____sizeof__void_ptr______div____sizeof__short______(short *arr)
{
    mp_obj_t obj_arr[(sizeof(void *)) / (sizeof(short))];
    for (size_t i=0; i<(sizeof(void *)) / (sizeof(short)); i++){
        obj_arr[i] = mp_obj_new_int(arr[i]);
    }
    return mp_obj_new_list((sizeof(void *)) / (sizeof(short)), obj_arr); // TODO: return custom iterable object!
}
    

/*
 * Array convertors for unsigned short [(sizeof(void *)) / (sizeof(unsigned short))]
 */

STATIC unsigned short *mp_arr_to_unsigned_short_____sizeof__void_ptr______div____sizeof__unsigned_short______(mp_obj_t mp_arr)
{
    mp_obj_t mp_len = mp_obj_len_maybe(mp_arr);
    if (mp_len == MP_OBJ_NULL) return mp_to_ptr(mp_arr);
    mp_int_t len = mp_obj_get_int(mp_len);
    //TODO check dim!
    unsigned short *lv_arr = (unsigned short*)m_malloc(len * sizeof(unsigned short));
    mp_obj_t iter = mp_getiter(mp_arr, NULL);
    mp_obj_t item;
    size_t i = 0;
    while ((item = mp_iternext(iter)) != MP_OBJ_STOP_ITERATION) {
        lv_arr[i++] = (unsigned short)mp_obj_get_int(item);
    }
    return (unsigned short *)lv_arr;
}
    
STATIC mp_obj_t mp_arr_from_unsigned_short_____sizeof__void_ptr______div____sizeof__unsigned_short______(unsigned short *arr)
{
    mp_obj_t obj_arr[(sizeof(void *)) / (sizeof(unsigned short))];
    for (size_t i=0; i<(sizeof(void *)) / (sizeof(unsigned short)); i++){
        obj_arr[i] = mp_obj_new_int_from_uint(arr[i]);
    }
    return mp_obj_new_list((sizeof(void *)) / (sizeof(unsigned short)), obj_arr); // TODO: return custom iterable object!
}
    

/*
 * Array convertors for char [(sizeof(void *)) / (sizeof(char))]
 */

STATIC char *mp_arr_to_char_____sizeof__void_ptr______div____sizeof__char______(mp_obj_t mp_arr)
{
    mp_obj_t mp_len = mp_obj_len_maybe(mp_arr);
    if (mp_len == MP_OBJ_NULL) return mp_to_ptr(mp_arr);
    mp_int_t len = mp_obj_get_int(mp_len);
    //TODO check dim!
    char *lv_arr = (char*)m_malloc(len * sizeof(char));
    mp_obj_t iter = mp_getiter(mp_arr, NULL);
    mp_obj_t item;
    size_t i = 0;
    while ((item = mp_iternext(iter)) != MP_OBJ_STOP_ITERATION) {
        lv_arr[i++] = (char)mp_obj_get_int(item);
    }
    return (char *)lv_arr;
}
    
STATIC mp_obj_t mp_arr_from_char_____sizeof__void_ptr______div____sizeof__char______(char *arr)
{
    mp_obj_t obj_arr[(sizeof(void *)) / (sizeof(char))];
    for (size_t i=0; i<(sizeof(void *)) / (sizeof(char)); i++){
        obj_arr[i] = mp_obj_new_int(arr[i]);
    }
    return mp_obj_new_list((sizeof(void *)) / (sizeof(char)), obj_arr); // TODO: return custom iterable object!
}
    

/*
 * Array convertors for unsigned char [(sizeof(void *)) / (sizeof(unsigned char))]
 */

STATIC unsigned char *mp_arr_to_unsigned_char_____sizeof__void_ptr______div____sizeof__unsigned_char______(mp_obj_t mp_arr)
{
    mp_obj_t mp_len = mp_obj_len_maybe(mp_arr);
    if (mp_len == MP_OBJ_NULL) return mp_to_ptr(mp_arr);
    mp_int_t len = mp_obj_get_int(mp_len);
    //TODO check dim!
    unsigned char *lv_arr = (unsigned char*)m_malloc(len * sizeof(unsigned char));
    mp_obj_t iter = mp_getiter(mp_arr, NULL);
    mp_obj_t item;
    size_t i = 0;
    while ((item = mp_iternext(iter)) != MP_OBJ_STOP_ITERATION) {
        lv_arr[i++] = (unsigned char)mp_obj_get_int(item);
    }
    return (unsigned char *)lv_arr;
}
    
STATIC mp_obj_t mp_arr_from_unsigned_char_____sizeof__void_ptr______div____sizeof__unsigned_char______(unsigned char *arr)
{
    mp_obj_t obj_arr[(sizeof(void *)) / (sizeof(unsigned char))];
    for (size_t i=0; i<(sizeof(void *)) / (sizeof(unsigned char)); i++){
        obj_arr[i] = mp_obj_new_int_from_uint(arr[i]);
    }
    return mp_obj_new_list((sizeof(void *)) / (sizeof(unsigned char)), obj_arr); // TODO: return custom iterable object!
}
    

/*
 * Struct C_Pointer
 */

STATIC inline const mp_obj_type_t *get_mp_C_Pointer_type();

STATIC inline C_Pointer* mp_write_ptr_C_Pointer(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_C_Pointer_type()));
    return (C_Pointer*)self->data;
}

#define mp_write_C_Pointer(struct_obj) *mp_write_ptr_C_Pointer(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_C_Pointer(C_Pointer *field)
{
    return lv_to_mp_struct(get_mp_C_Pointer_type(), (void*)field);
}

#define mp_read_C_Pointer(field) mp_read_ptr_C_Pointer(copy_buffer(&field, sizeof(C_Pointer)))
#define mp_read_byref_C_Pointer(field) mp_read_ptr_C_Pointer(&field)

STATIC void mp_C_Pointer_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    C_Pointer *data = (C_Pointer*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_ptr_val: dest[0] = ptr_to_mp((void*)data->ptr_val); break; // converting from void *;
            case MP_QSTR_str_val: dest[0] = convert_to_str((void*)data->str_val); break; // converting from char *;
            case MP_QSTR_int_val: dest[0] = mp_obj_new_int(data->int_val); break; // converting from int;
            case MP_QSTR_uint_val: dest[0] = mp_obj_new_int_from_uint(data->uint_val); break; // converting from unsigned int;
            case MP_QSTR_short_val: dest[0] = mp_arr_from_short_____sizeof__void_ptr______div____sizeof__short______(data->short_val); break; // converting from short [(sizeof(void *)) / (sizeof(short))];
            case MP_QSTR_ushort_val: dest[0] = mp_arr_from_unsigned_short_____sizeof__void_ptr______div____sizeof__unsigned_short______(data->ushort_val); break; // converting from unsigned short [(sizeof(void *)) / (sizeof(unsigned short))];
            case MP_QSTR_char_val: dest[0] = mp_arr_from_char_____sizeof__void_ptr______div____sizeof__char______(data->char_val); break; // converting from char [(sizeof(void *)) / (sizeof(char))];
            case MP_QSTR_uchar_val: dest[0] = mp_arr_from_unsigned_char_____sizeof__void_ptr______div____sizeof__unsigned_char______(data->uchar_val); break; // converting from unsigned char [(sizeof(void *)) / (sizeof(unsigned char))];
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_ptr_val: data->ptr_val = (void*)mp_to_ptr(dest[1]); break; // converting to void *;
                case MP_QSTR_str_val: data->str_val = (void*)(char*)mp_obj_str_get_str(dest[1]); break; // converting to char *;
                case MP_QSTR_int_val: data->int_val = (int)mp_obj_get_int(dest[1]); break; // converting to int;
                case MP_QSTR_uint_val: data->uint_val = (unsigned int)mp_obj_get_int(dest[1]); break; // converting to unsigned int;
                case MP_QSTR_short_val: memcpy(&data->short_val, mp_arr_to_short_____sizeof__void_ptr______div____sizeof__short______(dest[1]), sizeof(short)*(sizeof(void *)) / (sizeof(short))); break; // converting to short [(sizeof(void *)) / (sizeof(short))];
                case MP_QSTR_ushort_val: memcpy(&data->ushort_val, mp_arr_to_unsigned_short_____sizeof__void_ptr______div____sizeof__unsigned_short______(dest[1]), sizeof(unsigned short)*(sizeof(void *)) / (sizeof(unsigned short))); break; // converting to unsigned short [(sizeof(void *)) / (sizeof(unsigned short))];
                case MP_QSTR_char_val: memcpy(&data->char_val, mp_arr_to_char_____sizeof__void_ptr______div____sizeof__char______(dest[1]), sizeof(char)*(sizeof(void *)) / (sizeof(char))); break; // converting to char [(sizeof(void *)) / (sizeof(char))];
                case MP_QSTR_uchar_val: memcpy(&data->uchar_val, mp_arr_to_unsigned_char_____sizeof__void_ptr______div____sizeof__unsigned_char______(dest[1]), sizeof(unsigned char)*(sizeof(void *)) / (sizeof(unsigned char))); break; // converting to unsigned char [(sizeof(void *)) / (sizeof(unsigned char))];
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_C_Pointer_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct C_Pointer");
}

STATIC const mp_obj_dict_t mp_C_Pointer_locals_dict;

STATIC const mp_obj_type_t mp_C_Pointer_type = {
    { &mp_type_type },
    .name = MP_QSTR_C_Pointer,
    .print = mp_C_Pointer_print,
    .make_new = make_new_lv_struct,
    .attr = mp_C_Pointer_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_C_Pointer_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer }
};

STATIC inline const mp_obj_type_t *get_mp_C_Pointer_type()
{
    return &mp_C_Pointer_type;
}
    

/*
 * st7789_lvgl ENUM_LV_LOG_LEVEL object definitions
 */
    

STATIC const mp_rom_map_elem_t ENUM_LV_LOG_LEVEL_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_TRACE), MP_ROM_PTR(MP_ROM_INT(ENUM_LV_LOG_LEVEL_TRACE)) },
    { MP_ROM_QSTR(MP_QSTR_INFO), MP_ROM_PTR(MP_ROM_INT(ENUM_LV_LOG_LEVEL_INFO)) },
    { MP_ROM_QSTR(MP_QSTR_WARN), MP_ROM_PTR(MP_ROM_INT(ENUM_LV_LOG_LEVEL_WARN)) },
    { MP_ROM_QSTR(MP_QSTR_ERROR), MP_ROM_PTR(MP_ROM_INT(ENUM_LV_LOG_LEVEL_ERROR)) },
    { MP_ROM_QSTR(MP_QSTR_USER), MP_ROM_PTR(MP_ROM_INT(ENUM_LV_LOG_LEVEL_USER)) },
    { MP_ROM_QSTR(MP_QSTR_NONE), MP_ROM_PTR(MP_ROM_INT(ENUM_LV_LOG_LEVEL_NONE)) }
};

STATIC MP_DEFINE_CONST_DICT(ENUM_LV_LOG_LEVEL_locals_dict, ENUM_LV_LOG_LEVEL_locals_dict_table);

STATIC void ENUM_LV_LOG_LEVEL_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "st7789_lvgl ENUM_LV_LOG_LEVEL");
}



STATIC const mp_obj_type_t mp_ENUM_LV_LOG_LEVEL_type = {
    { &mp_type_type },
    .name = MP_QSTR_ENUM_LV_LOG_LEVEL,
    .print = ENUM_LV_LOG_LEVEL_print,
    
    .attr = call_parent_methods,
    .locals_dict = (mp_obj_dict_t*)&ENUM_LV_LOG_LEVEL_locals_dict,
    
    .parent = NULL,
};
    

/*
 * st7789_lvgl LV_RES object definitions
 */
    

STATIC const mp_rom_map_elem_t LV_RES_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_INV), MP_ROM_PTR(MP_ROM_INT(LV_RES_INV)) },
    { MP_ROM_QSTR(MP_QSTR_OK), MP_ROM_PTR(MP_ROM_INT(LV_RES_OK)) }
};

STATIC MP_DEFINE_CONST_DICT(LV_RES_locals_dict, LV_RES_locals_dict_table);

STATIC void LV_RES_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "st7789_lvgl LV_RES");
}



STATIC const mp_obj_type_t mp_LV_RES_type = {
    { &mp_type_type },
    .name = MP_QSTR_LV_RES,
    .print = LV_RES_print,
    
    .attr = call_parent_methods,
    .locals_dict = (mp_obj_dict_t*)&LV_RES_locals_dict,
    
    .parent = NULL,
};
    

/*
 * st7789_lvgl ENUM_LV_COORD object definitions
 */
    

STATIC const mp_rom_map_elem_t ENUM_LV_COORD_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_MAX), MP_ROM_PTR(MP_ROM_INT(ENUM_LV_COORD_MAX)) },
    { MP_ROM_QSTR(MP_QSTR_MIN), MP_ROM_PTR(MP_ROM_INT(ENUM_LV_COORD_MIN)) }
};

STATIC MP_DEFINE_CONST_DICT(ENUM_LV_COORD_locals_dict, ENUM_LV_COORD_locals_dict_table);

STATIC void ENUM_LV_COORD_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "st7789_lvgl ENUM_LV_COORD");
}



STATIC const mp_obj_type_t mp_ENUM_LV_COORD_type = {
    { &mp_type_type },
    .name = MP_QSTR_ENUM_LV_COORD,
    .print = ENUM_LV_COORD_print,
    
    .attr = call_parent_methods,
    .locals_dict = (mp_obj_dict_t*)&ENUM_LV_COORD_locals_dict,
    
    .parent = NULL,
};
    

/*
 * st7789_lvgl LV_ALIGN object definitions
 */
    

STATIC const mp_rom_map_elem_t LV_ALIGN_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_CENTER), MP_ROM_PTR(MP_ROM_INT(LV_ALIGN_CENTER)) },
    { MP_ROM_QSTR(MP_QSTR_IN_TOP_LEFT), MP_ROM_PTR(MP_ROM_INT(LV_ALIGN_IN_TOP_LEFT)) },
    { MP_ROM_QSTR(MP_QSTR_IN_TOP_MID), MP_ROM_PTR(MP_ROM_INT(LV_ALIGN_IN_TOP_MID)) },
    { MP_ROM_QSTR(MP_QSTR_IN_TOP_RIGHT), MP_ROM_PTR(MP_ROM_INT(LV_ALIGN_IN_TOP_RIGHT)) },
    { MP_ROM_QSTR(MP_QSTR_IN_BOTTOM_LEFT), MP_ROM_PTR(MP_ROM_INT(LV_ALIGN_IN_BOTTOM_LEFT)) },
    { MP_ROM_QSTR(MP_QSTR_IN_BOTTOM_MID), MP_ROM_PTR(MP_ROM_INT(LV_ALIGN_IN_BOTTOM_MID)) },
    { MP_ROM_QSTR(MP_QSTR_IN_BOTTOM_RIGHT), MP_ROM_PTR(MP_ROM_INT(LV_ALIGN_IN_BOTTOM_RIGHT)) },
    { MP_ROM_QSTR(MP_QSTR_IN_LEFT_MID), MP_ROM_PTR(MP_ROM_INT(LV_ALIGN_IN_LEFT_MID)) },
    { MP_ROM_QSTR(MP_QSTR_IN_RIGHT_MID), MP_ROM_PTR(MP_ROM_INT(LV_ALIGN_IN_RIGHT_MID)) },
    { MP_ROM_QSTR(MP_QSTR_OUT_TOP_LEFT), MP_ROM_PTR(MP_ROM_INT(LV_ALIGN_OUT_TOP_LEFT)) },
    { MP_ROM_QSTR(MP_QSTR_OUT_TOP_MID), MP_ROM_PTR(MP_ROM_INT(LV_ALIGN_OUT_TOP_MID)) },
    { MP_ROM_QSTR(MP_QSTR_OUT_TOP_RIGHT), MP_ROM_PTR(MP_ROM_INT(LV_ALIGN_OUT_TOP_RIGHT)) },
    { MP_ROM_QSTR(MP_QSTR_OUT_BOTTOM_LEFT), MP_ROM_PTR(MP_ROM_INT(LV_ALIGN_OUT_BOTTOM_LEFT)) },
    { MP_ROM_QSTR(MP_QSTR_OUT_BOTTOM_MID), MP_ROM_PTR(MP_ROM_INT(LV_ALIGN_OUT_BOTTOM_MID)) },
    { MP_ROM_QSTR(MP_QSTR_OUT_BOTTOM_RIGHT), MP_ROM_PTR(MP_ROM_INT(LV_ALIGN_OUT_BOTTOM_RIGHT)) },
    { MP_ROM_QSTR(MP_QSTR_OUT_LEFT_TOP), MP_ROM_PTR(MP_ROM_INT(LV_ALIGN_OUT_LEFT_TOP)) },
    { MP_ROM_QSTR(MP_QSTR_OUT_LEFT_MID), MP_ROM_PTR(MP_ROM_INT(LV_ALIGN_OUT_LEFT_MID)) },
    { MP_ROM_QSTR(MP_QSTR_OUT_LEFT_BOTTOM), MP_ROM_PTR(MP_ROM_INT(LV_ALIGN_OUT_LEFT_BOTTOM)) },
    { MP_ROM_QSTR(MP_QSTR_OUT_RIGHT_TOP), MP_ROM_PTR(MP_ROM_INT(LV_ALIGN_OUT_RIGHT_TOP)) },
    { MP_ROM_QSTR(MP_QSTR_OUT_RIGHT_MID), MP_ROM_PTR(MP_ROM_INT(LV_ALIGN_OUT_RIGHT_MID)) },
    { MP_ROM_QSTR(MP_QSTR_OUT_RIGHT_BOTTOM), MP_ROM_PTR(MP_ROM_INT(LV_ALIGN_OUT_RIGHT_BOTTOM)) }
};

STATIC MP_DEFINE_CONST_DICT(LV_ALIGN_locals_dict, LV_ALIGN_locals_dict_table);

STATIC void LV_ALIGN_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "st7789_lvgl LV_ALIGN");
}



STATIC const mp_obj_type_t mp_LV_ALIGN_type = {
    { &mp_type_type },
    .name = MP_QSTR_LV_ALIGN,
    .print = LV_ALIGN_print,
    
    .attr = call_parent_methods,
    .locals_dict = (mp_obj_dict_t*)&LV_ALIGN_locals_dict,
    
    .parent = NULL,
};
    

/*
 * st7789_lvgl LV_TASK_PRIO object definitions
 */
    

STATIC const mp_rom_map_elem_t LV_TASK_PRIO_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_OFF), MP_ROM_PTR(MP_ROM_INT(LV_TASK_PRIO_OFF)) },
    { MP_ROM_QSTR(MP_QSTR_LOWEST), MP_ROM_PTR(MP_ROM_INT(LV_TASK_PRIO_LOWEST)) },
    { MP_ROM_QSTR(MP_QSTR_LOW), MP_ROM_PTR(MP_ROM_INT(LV_TASK_PRIO_LOW)) },
    { MP_ROM_QSTR(MP_QSTR_MID), MP_ROM_PTR(MP_ROM_INT(LV_TASK_PRIO_MID)) },
    { MP_ROM_QSTR(MP_QSTR_HIGH), MP_ROM_PTR(MP_ROM_INT(LV_TASK_PRIO_HIGH)) },
    { MP_ROM_QSTR(MP_QSTR_HIGHEST), MP_ROM_PTR(MP_ROM_INT(LV_TASK_PRIO_HIGHEST)) }
};

STATIC MP_DEFINE_CONST_DICT(LV_TASK_PRIO_locals_dict, LV_TASK_PRIO_locals_dict_table);

STATIC void LV_TASK_PRIO_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "st7789_lvgl LV_TASK_PRIO");
}



STATIC const mp_obj_type_t mp_LV_TASK_PRIO_type = {
    { &mp_type_type },
    .name = MP_QSTR_LV_TASK_PRIO,
    .print = LV_TASK_PRIO_print,
    
    .attr = call_parent_methods,
    .locals_dict = (mp_obj_dict_t*)&LV_TASK_PRIO_locals_dict,
    
    .parent = NULL,
};
    

/*
 * st7789_lvgl LV_INDEV_TYPE object definitions
 */
    

STATIC const mp_rom_map_elem_t LV_INDEV_TYPE_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_NONE), MP_ROM_PTR(MP_ROM_INT(LV_INDEV_TYPE_NONE)) },
    { MP_ROM_QSTR(MP_QSTR_POINTER), MP_ROM_PTR(MP_ROM_INT(LV_INDEV_TYPE_POINTER)) },
    { MP_ROM_QSTR(MP_QSTR_KEYPAD), MP_ROM_PTR(MP_ROM_INT(LV_INDEV_TYPE_KEYPAD)) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON), MP_ROM_PTR(MP_ROM_INT(LV_INDEV_TYPE_BUTTON)) },
    { MP_ROM_QSTR(MP_QSTR_ENCODER), MP_ROM_PTR(MP_ROM_INT(LV_INDEV_TYPE_ENCODER)) }
};

STATIC MP_DEFINE_CONST_DICT(LV_INDEV_TYPE_locals_dict, LV_INDEV_TYPE_locals_dict_table);

STATIC void LV_INDEV_TYPE_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "st7789_lvgl LV_INDEV_TYPE");
}



STATIC const mp_obj_type_t mp_LV_INDEV_TYPE_type = {
    { &mp_type_type },
    .name = MP_QSTR_LV_INDEV_TYPE,
    .print = LV_INDEV_TYPE_print,
    
    .attr = call_parent_methods,
    .locals_dict = (mp_obj_dict_t*)&LV_INDEV_TYPE_locals_dict,
    
    .parent = NULL,
};
    

/*
 * st7789_lvgl LV_INDEV_STATE object definitions
 */
    

STATIC const mp_rom_map_elem_t LV_INDEV_STATE_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_REL), MP_ROM_PTR(MP_ROM_INT(LV_INDEV_STATE_REL)) },
    { MP_ROM_QSTR(MP_QSTR_PR), MP_ROM_PTR(MP_ROM_INT(LV_INDEV_STATE_PR)) }
};

STATIC MP_DEFINE_CONST_DICT(LV_INDEV_STATE_locals_dict, LV_INDEV_STATE_locals_dict_table);

STATIC void LV_INDEV_STATE_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "st7789_lvgl LV_INDEV_STATE");
}



STATIC const mp_obj_type_t mp_LV_INDEV_STATE_type = {
    { &mp_type_type },
    .name = MP_QSTR_LV_INDEV_STATE,
    .print = LV_INDEV_STATE_print,
    
    .attr = call_parent_methods,
    .locals_dict = (mp_obj_dict_t*)&LV_INDEV_STATE_locals_dict,
    
    .parent = NULL,
};
    

/*
 * st7789_lvgl LV_DRAG_DIR object definitions
 */
    

STATIC const mp_rom_map_elem_t LV_DRAG_DIR_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_HOR), MP_ROM_PTR(MP_ROM_INT(LV_DRAG_DIR_HOR)) },
    { MP_ROM_QSTR(MP_QSTR_VER), MP_ROM_PTR(MP_ROM_INT(LV_DRAG_DIR_VER)) },
    { MP_ROM_QSTR(MP_QSTR_BOTH), MP_ROM_PTR(MP_ROM_INT(LV_DRAG_DIR_BOTH)) },
    { MP_ROM_QSTR(MP_QSTR_ONE), MP_ROM_PTR(MP_ROM_INT(LV_DRAG_DIR_ONE)) }
};

STATIC MP_DEFINE_CONST_DICT(LV_DRAG_DIR_locals_dict, LV_DRAG_DIR_locals_dict_table);

STATIC void LV_DRAG_DIR_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "st7789_lvgl LV_DRAG_DIR");
}



STATIC const mp_obj_type_t mp_LV_DRAG_DIR_type = {
    { &mp_type_type },
    .name = MP_QSTR_LV_DRAG_DIR,
    .print = LV_DRAG_DIR_print,
    
    .attr = call_parent_methods,
    .locals_dict = (mp_obj_dict_t*)&LV_DRAG_DIR_locals_dict,
    
    .parent = NULL,
};
    

/*
 * st7789_lvgl LV_GESTURE_DIR object definitions
 */
    

STATIC const mp_rom_map_elem_t LV_GESTURE_DIR_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_TOP), MP_ROM_PTR(MP_ROM_INT(LV_GESTURE_DIR_TOP)) },
    { MP_ROM_QSTR(MP_QSTR_BOTTOM), MP_ROM_PTR(MP_ROM_INT(LV_GESTURE_DIR_BOTTOM)) },
    { MP_ROM_QSTR(MP_QSTR_LEFT), MP_ROM_PTR(MP_ROM_INT(LV_GESTURE_DIR_LEFT)) },
    { MP_ROM_QSTR(MP_QSTR_RIGHT), MP_ROM_PTR(MP_ROM_INT(LV_GESTURE_DIR_RIGHT)) }
};

STATIC MP_DEFINE_CONST_DICT(LV_GESTURE_DIR_locals_dict, LV_GESTURE_DIR_locals_dict_table);

STATIC void LV_GESTURE_DIR_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "st7789_lvgl LV_GESTURE_DIR");
}



STATIC const mp_obj_type_t mp_LV_GESTURE_DIR_type = {
    { &mp_type_type },
    .name = MP_QSTR_LV_GESTURE_DIR,
    .print = LV_GESTURE_DIR_print,
    
    .attr = call_parent_methods,
    .locals_dict = (mp_obj_dict_t*)&LV_GESTURE_DIR_locals_dict,
    
    .parent = NULL,
};
    

/*
 * st7789_lvgl LV_OPA object definitions
 */
    

STATIC const mp_rom_map_elem_t LV_OPA_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_TRANSP), MP_ROM_PTR(MP_ROM_INT(LV_OPA_TRANSP)) },
    { MP_ROM_QSTR(MP_QSTR__0), MP_ROM_PTR(MP_ROM_INT(LV_OPA_0)) },
    { MP_ROM_QSTR(MP_QSTR__10), MP_ROM_PTR(MP_ROM_INT(LV_OPA_10)) },
    { MP_ROM_QSTR(MP_QSTR__20), MP_ROM_PTR(MP_ROM_INT(LV_OPA_20)) },
    { MP_ROM_QSTR(MP_QSTR__30), MP_ROM_PTR(MP_ROM_INT(LV_OPA_30)) },
    { MP_ROM_QSTR(MP_QSTR__40), MP_ROM_PTR(MP_ROM_INT(LV_OPA_40)) },
    { MP_ROM_QSTR(MP_QSTR__50), MP_ROM_PTR(MP_ROM_INT(LV_OPA_50)) },
    { MP_ROM_QSTR(MP_QSTR__60), MP_ROM_PTR(MP_ROM_INT(LV_OPA_60)) },
    { MP_ROM_QSTR(MP_QSTR__70), MP_ROM_PTR(MP_ROM_INT(LV_OPA_70)) },
    { MP_ROM_QSTR(MP_QSTR__80), MP_ROM_PTR(MP_ROM_INT(LV_OPA_80)) },
    { MP_ROM_QSTR(MP_QSTR__90), MP_ROM_PTR(MP_ROM_INT(LV_OPA_90)) },
    { MP_ROM_QSTR(MP_QSTR__100), MP_ROM_PTR(MP_ROM_INT(LV_OPA_100)) },
    { MP_ROM_QSTR(MP_QSTR_COVER), MP_ROM_PTR(MP_ROM_INT(LV_OPA_COVER)) }
};

STATIC MP_DEFINE_CONST_DICT(LV_OPA_locals_dict, LV_OPA_locals_dict_table);

STATIC void LV_OPA_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "st7789_lvgl LV_OPA");
}



STATIC const mp_obj_type_t mp_LV_OPA_type = {
    { &mp_type_type },
    .name = MP_QSTR_LV_OPA,
    .print = LV_OPA_print,
    
    .attr = call_parent_methods,
    .locals_dict = (mp_obj_dict_t*)&LV_OPA_locals_dict,
    
    .parent = NULL,
};
    

/*
 * st7789_lvgl LV_DISP_SIZE object definitions
 */
    

STATIC const mp_rom_map_elem_t LV_DISP_SIZE_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SMALL), MP_ROM_PTR(MP_ROM_INT(LV_DISP_SIZE_SMALL)) },
    { MP_ROM_QSTR(MP_QSTR_MEDIUM), MP_ROM_PTR(MP_ROM_INT(LV_DISP_SIZE_MEDIUM)) },
    { MP_ROM_QSTR(MP_QSTR_LARGE), MP_ROM_PTR(MP_ROM_INT(LV_DISP_SIZE_LARGE)) },
    { MP_ROM_QSTR(MP_QSTR_EXTRA_LARGE), MP_ROM_PTR(MP_ROM_INT(LV_DISP_SIZE_EXTRA_LARGE)) }
};

STATIC MP_DEFINE_CONST_DICT(LV_DISP_SIZE_locals_dict, LV_DISP_SIZE_locals_dict_table);

STATIC void LV_DISP_SIZE_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "st7789_lvgl LV_DISP_SIZE");
}



STATIC const mp_obj_type_t mp_LV_DISP_SIZE_type = {
    { &mp_type_type },
    .name = MP_QSTR_LV_DISP_SIZE,
    .print = LV_DISP_SIZE_print,
    
    .attr = call_parent_methods,
    .locals_dict = (mp_obj_dict_t*)&LV_DISP_SIZE_locals_dict,
    
    .parent = NULL,
};
    

/*
 * Struct lv_area_t
 */

STATIC inline const mp_obj_type_t *get_mp_lv_area_t_type();

STATIC inline lv_area_t* mp_write_ptr_lv_area_t(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_lv_area_t_type()));
    return (lv_area_t*)self->data;
}

#define mp_write_lv_area_t(struct_obj) *mp_write_ptr_lv_area_t(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_lv_area_t(lv_area_t *field)
{
    return lv_to_mp_struct(get_mp_lv_area_t_type(), (void*)field);
}

#define mp_read_lv_area_t(field) mp_read_ptr_lv_area_t(copy_buffer(&field, sizeof(lv_area_t)))
#define mp_read_byref_lv_area_t(field) mp_read_ptr_lv_area_t(&field)

STATIC void mp_lv_area_t_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    lv_area_t *data = (lv_area_t*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_x1: dest[0] = mp_obj_new_int(data->x1); break; // converting from lv_coord_t;
            case MP_QSTR_y1: dest[0] = mp_obj_new_int(data->y1); break; // converting from lv_coord_t;
            case MP_QSTR_x2: dest[0] = mp_obj_new_int(data->x2); break; // converting from lv_coord_t;
            case MP_QSTR_y2: dest[0] = mp_obj_new_int(data->y2); break; // converting from lv_coord_t;
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_x1: data->x1 = (int16_t)mp_obj_get_int(dest[1]); break; // converting to lv_coord_t;
                case MP_QSTR_y1: data->y1 = (int16_t)mp_obj_get_int(dest[1]); break; // converting to lv_coord_t;
                case MP_QSTR_x2: data->x2 = (int16_t)mp_obj_get_int(dest[1]); break; // converting to lv_coord_t;
                case MP_QSTR_y2: data->y2 = (int16_t)mp_obj_get_int(dest[1]); break; // converting to lv_coord_t;
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_lv_area_t_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct lv_area_t");
}

STATIC const mp_obj_dict_t mp_lv_area_t_locals_dict;

STATIC const mp_obj_type_t mp_lv_area_t_type = {
    { &mp_type_type },
    .name = MP_QSTR_lv_area_t,
    .print = mp_lv_area_t_print,
    .make_new = make_new_lv_struct,
    .attr = mp_lv_area_t_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_lv_area_t_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer }
};

STATIC inline const mp_obj_type_t *get_mp_lv_area_t_type()
{
    return &mp_lv_area_t_type;
}
    
typedef __typeof__( ((lv_color16_t*)(0))->ch ) lv_color16_ch_t;

/*
 * Struct lv_color16_ch_t
 */

STATIC inline const mp_obj_type_t *get_mp_lv_color16_ch_t_type();

STATIC inline lv_color16_ch_t* mp_write_ptr_lv_color16_ch_t(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_lv_color16_ch_t_type()));
    return (lv_color16_ch_t*)self->data;
}

#define mp_write_lv_color16_ch_t(struct_obj) *mp_write_ptr_lv_color16_ch_t(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_lv_color16_ch_t(lv_color16_ch_t *field)
{
    return lv_to_mp_struct(get_mp_lv_color16_ch_t_type(), (void*)field);
}

#define mp_read_lv_color16_ch_t(field) mp_read_ptr_lv_color16_ch_t(copy_buffer(&field, sizeof(lv_color16_ch_t)))
#define mp_read_byref_lv_color16_ch_t(field) mp_read_ptr_lv_color16_ch_t(&field)

STATIC void mp_lv_color16_ch_t_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    lv_color16_ch_t *data = (lv_color16_ch_t*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_green_h: dest[0] = mp_obj_new_int_from_uint(data->green_h); break; // converting from uint16_t;
            case MP_QSTR_red: dest[0] = mp_obj_new_int_from_uint(data->red); break; // converting from uint16_t;
            case MP_QSTR_blue: dest[0] = mp_obj_new_int_from_uint(data->blue); break; // converting from uint16_t;
            case MP_QSTR_green_l: dest[0] = mp_obj_new_int_from_uint(data->green_l); break; // converting from uint16_t;
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_green_h: data->green_h = (uint16_t)mp_obj_get_int(dest[1]); break; // converting to uint16_t;
                case MP_QSTR_red: data->red = (uint16_t)mp_obj_get_int(dest[1]); break; // converting to uint16_t;
                case MP_QSTR_blue: data->blue = (uint16_t)mp_obj_get_int(dest[1]); break; // converting to uint16_t;
                case MP_QSTR_green_l: data->green_l = (uint16_t)mp_obj_get_int(dest[1]); break; // converting to uint16_t;
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_lv_color16_ch_t_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct lv_color16_ch_t");
}

STATIC const mp_obj_dict_t mp_lv_color16_ch_t_locals_dict;

STATIC const mp_obj_type_t mp_lv_color16_ch_t_type = {
    { &mp_type_type },
    .name = MP_QSTR_lv_color16_ch_t,
    .print = mp_lv_color16_ch_t_print,
    .make_new = make_new_lv_struct,
    .attr = mp_lv_color16_ch_t_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_lv_color16_ch_t_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer }
};

STATIC inline const mp_obj_type_t *get_mp_lv_color16_ch_t_type()
{
    return &mp_lv_color16_ch_t_type;
}
    

/*
 * Struct lv_color16_t
 */

STATIC inline const mp_obj_type_t *get_mp_lv_color16_t_type();

STATIC inline lv_color16_t* mp_write_ptr_lv_color16_t(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_lv_color16_t_type()));
    return (lv_color16_t*)self->data;
}

#define mp_write_lv_color16_t(struct_obj) *mp_write_ptr_lv_color16_t(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_lv_color16_t(lv_color16_t *field)
{
    return lv_to_mp_struct(get_mp_lv_color16_t_type(), (void*)field);
}

#define mp_read_lv_color16_t(field) mp_read_ptr_lv_color16_t(copy_buffer(&field, sizeof(lv_color16_t)))
#define mp_read_byref_lv_color16_t(field) mp_read_ptr_lv_color16_t(&field)

STATIC void mp_lv_color16_t_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    lv_color16_t *data = (lv_color16_t*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_ch: dest[0] = mp_read_byref_lv_color16_ch_t(data->ch); break; // converting from lv_color16_ch_t;
            case MP_QSTR_full: dest[0] = mp_obj_new_int_from_uint(data->full); break; // converting from uint16_t;
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_ch: data->ch = mp_write_lv_color16_ch_t(dest[1]); break; // converting to lv_color16_ch_t;
                case MP_QSTR_full: data->full = (uint16_t)mp_obj_get_int(dest[1]); break; // converting to uint16_t;
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_lv_color16_t_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct lv_color16_t");
}

STATIC const mp_obj_dict_t mp_lv_color16_t_locals_dict;

STATIC const mp_obj_type_t mp_lv_color16_t_type = {
    { &mp_type_type },
    .name = MP_QSTR_lv_color16_t,
    .print = mp_lv_color16_t_print,
    .make_new = make_new_lv_struct,
    .attr = mp_lv_color16_t_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_lv_color16_t_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer }
};

STATIC inline const mp_obj_type_t *get_mp_lv_color16_t_type()
{
    return &mp_lv_color16_t_type;
}
    

/*
 * Struct lv_mem_monitor_t
 */

STATIC inline const mp_obj_type_t *get_mp_lv_mem_monitor_t_type();

STATIC inline lv_mem_monitor_t* mp_write_ptr_lv_mem_monitor_t(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_lv_mem_monitor_t_type()));
    return (lv_mem_monitor_t*)self->data;
}

#define mp_write_lv_mem_monitor_t(struct_obj) *mp_write_ptr_lv_mem_monitor_t(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_lv_mem_monitor_t(lv_mem_monitor_t *field)
{
    return lv_to_mp_struct(get_mp_lv_mem_monitor_t_type(), (void*)field);
}

#define mp_read_lv_mem_monitor_t(field) mp_read_ptr_lv_mem_monitor_t(copy_buffer(&field, sizeof(lv_mem_monitor_t)))
#define mp_read_byref_lv_mem_monitor_t(field) mp_read_ptr_lv_mem_monitor_t(&field)

STATIC void mp_lv_mem_monitor_t_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    lv_mem_monitor_t *data = (lv_mem_monitor_t*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_total_size: dest[0] = mp_obj_new_int_from_uint(data->total_size); break; // converting from uint32_t;
            case MP_QSTR_free_cnt: dest[0] = mp_obj_new_int_from_uint(data->free_cnt); break; // converting from uint32_t;
            case MP_QSTR_free_size: dest[0] = mp_obj_new_int_from_uint(data->free_size); break; // converting from uint32_t;
            case MP_QSTR_free_biggest_size: dest[0] = mp_obj_new_int_from_uint(data->free_biggest_size); break; // converting from uint32_t;
            case MP_QSTR_used_cnt: dest[0] = mp_obj_new_int_from_uint(data->used_cnt); break; // converting from uint32_t;
            case MP_QSTR_max_used: dest[0] = mp_obj_new_int_from_uint(data->max_used); break; // converting from uint32_t;
            case MP_QSTR_used_pct: dest[0] = mp_obj_new_int_from_uint(data->used_pct); break; // converting from uint8_t;
            case MP_QSTR_frag_pct: dest[0] = mp_obj_new_int_from_uint(data->frag_pct); break; // converting from uint8_t;
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_total_size: data->total_size = (uint32_t)mp_obj_get_int(dest[1]); break; // converting to uint32_t;
                case MP_QSTR_free_cnt: data->free_cnt = (uint32_t)mp_obj_get_int(dest[1]); break; // converting to uint32_t;
                case MP_QSTR_free_size: data->free_size = (uint32_t)mp_obj_get_int(dest[1]); break; // converting to uint32_t;
                case MP_QSTR_free_biggest_size: data->free_biggest_size = (uint32_t)mp_obj_get_int(dest[1]); break; // converting to uint32_t;
                case MP_QSTR_used_cnt: data->used_cnt = (uint32_t)mp_obj_get_int(dest[1]); break; // converting to uint32_t;
                case MP_QSTR_max_used: data->max_used = (uint32_t)mp_obj_get_int(dest[1]); break; // converting to uint32_t;
                case MP_QSTR_used_pct: data->used_pct = (uint8_t)mp_obj_get_int(dest[1]); break; // converting to uint8_t;
                case MP_QSTR_frag_pct: data->frag_pct = (uint8_t)mp_obj_get_int(dest[1]); break; // converting to uint8_t;
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_lv_mem_monitor_t_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct lv_mem_monitor_t");
}

STATIC const mp_obj_dict_t mp_lv_mem_monitor_t_locals_dict;

STATIC const mp_obj_type_t mp_lv_mem_monitor_t_type = {
    { &mp_type_type },
    .name = MP_QSTR_lv_mem_monitor_t,
    .print = mp_lv_mem_monitor_t_print,
    .make_new = make_new_lv_struct,
    .attr = mp_lv_mem_monitor_t_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_lv_mem_monitor_t_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer }
};

STATIC inline const mp_obj_type_t *get_mp_lv_mem_monitor_t_type()
{
    return &mp_lv_mem_monitor_t_type;
}
    
STATIC void lv_task_t_task_cb_callback(struct _lv_task_t *);

/*
 * Struct lv_task_t
 */

STATIC inline const mp_obj_type_t *get_mp_lv_task_t_type();

STATIC inline lv_task_t* mp_write_ptr_lv_task_t(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_lv_task_t_type()));
    return (lv_task_t*)self->data;
}

#define mp_write_lv_task_t(struct_obj) *mp_write_ptr_lv_task_t(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_lv_task_t(lv_task_t *field)
{
    return lv_to_mp_struct(get_mp_lv_task_t_type(), (void*)field);
}

#define mp_read_lv_task_t(field) mp_read_ptr_lv_task_t(copy_buffer(&field, sizeof(lv_task_t)))
#define mp_read_byref_lv_task_t(field) mp_read_ptr_lv_task_t(&field)

STATIC void mp_lv_task_t_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    lv_task_t *data = (lv_task_t*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_period: dest[0] = mp_obj_new_int_from_uint(data->period); break; // converting from uint32_t;
            case MP_QSTR_last_run: dest[0] = mp_obj_new_int_from_uint(data->last_run); break; // converting from uint32_t;
            case MP_QSTR_task_cb: dest[0] = ptr_to_mp(data->task_cb); break; // converting from callback lv_task_cb_t;
            case MP_QSTR_user_data: dest[0] = ptr_to_mp((void*)data->user_data); break; // converting from void *;
            case MP_QSTR_repeat_count: dest[0] = mp_obj_new_int(data->repeat_count); break; // converting from int32_t;
            case MP_QSTR_prio: dest[0] = mp_obj_new_int_from_uint(data->prio); break; // converting from uint8_t;
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_period: data->period = (uint32_t)mp_obj_get_int(dest[1]); break; // converting to uint32_t;
                case MP_QSTR_last_run: data->last_run = (uint32_t)mp_obj_get_int(dest[1]); break; // converting to uint32_t;
                case MP_QSTR_task_cb: data->task_cb = mp_lv_callback(dest[1], lv_task_t_task_cb_callback ,MP_QSTR_lv_task_t_task_cb, &data->user_data); break; // converting to callback lv_task_cb_t;
                case MP_QSTR_user_data: data->user_data = (void*)mp_to_ptr(dest[1]); break; // converting to void *;
                case MP_QSTR_repeat_count: data->repeat_count = (int32_t)mp_obj_get_int(dest[1]); break; // converting to int32_t;
                case MP_QSTR_prio: data->prio = (uint8_t)mp_obj_get_int(dest[1]); break; // converting to uint8_t;
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_lv_task_t_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct lv_task_t");
}

STATIC const mp_obj_dict_t mp_lv_task_t_locals_dict;

STATIC const mp_obj_type_t mp_lv_task_t_type = {
    { &mp_type_type },
    .name = MP_QSTR_lv_task_t,
    .print = mp_lv_task_t_print,
    .make_new = make_new_lv_struct,
    .attr = mp_lv_task_t_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_lv_task_t_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer }
};

STATIC inline const mp_obj_type_t *get_mp_lv_task_t_type()
{
    return &mp_lv_task_t_type;
}
    
STATIC bool lv_indev_drv_t_read_cb_callback(struct _lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
STATIC void lv_indev_drv_t_feedback_cb_callback(struct _lv_indev_drv_t *, uint8_t);

/*
 * Struct lv_disp_buf_t
 */

STATIC inline const mp_obj_type_t *get_mp_lv_disp_buf_t_type();

STATIC inline lv_disp_buf_t* mp_write_ptr_lv_disp_buf_t(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_lv_disp_buf_t_type()));
    return (lv_disp_buf_t*)self->data;
}

#define mp_write_lv_disp_buf_t(struct_obj) *mp_write_ptr_lv_disp_buf_t(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_lv_disp_buf_t(lv_disp_buf_t *field)
{
    return lv_to_mp_struct(get_mp_lv_disp_buf_t_type(), (void*)field);
}

#define mp_read_lv_disp_buf_t(field) mp_read_ptr_lv_disp_buf_t(copy_buffer(&field, sizeof(lv_disp_buf_t)))
#define mp_read_byref_lv_disp_buf_t(field) mp_read_ptr_lv_disp_buf_t(&field)

STATIC void mp_lv_disp_buf_t_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    lv_disp_buf_t *data = (lv_disp_buf_t*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_buf1: dest[0] = ptr_to_mp((void*)data->buf1); break; // converting from void *;
            case MP_QSTR_buf2: dest[0] = ptr_to_mp((void*)data->buf2); break; // converting from void *;
            case MP_QSTR_buf_act: dest[0] = ptr_to_mp((void*)data->buf_act); break; // converting from void *;
            case MP_QSTR_size: dest[0] = mp_obj_new_int_from_uint(data->size); break; // converting from uint32_t;
            case MP_QSTR_area: dest[0] = mp_read_byref_lv_area_t(data->area); break; // converting from lv_area_t;
            case MP_QSTR_flushing: dest[0] = mp_obj_new_int(data->flushing); break; // converting from int;
            case MP_QSTR_flushing_last: dest[0] = mp_obj_new_int(data->flushing_last); break; // converting from int;
            case MP_QSTR_last_area: dest[0] = mp_obj_new_int_from_uint(data->last_area); break; // converting from uint32_t;
            case MP_QSTR_last_part: dest[0] = mp_obj_new_int_from_uint(data->last_part); break; // converting from uint32_t;
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_buf1: data->buf1 = (void*)mp_to_ptr(dest[1]); break; // converting to void *;
                case MP_QSTR_buf2: data->buf2 = (void*)mp_to_ptr(dest[1]); break; // converting to void *;
                case MP_QSTR_buf_act: data->buf_act = (void*)mp_to_ptr(dest[1]); break; // converting to void *;
                case MP_QSTR_size: data->size = (uint32_t)mp_obj_get_int(dest[1]); break; // converting to uint32_t;
                case MP_QSTR_area: data->area = mp_write_lv_area_t(dest[1]); break; // converting to lv_area_t;
                case MP_QSTR_flushing: data->flushing = (int)mp_obj_get_int(dest[1]); break; // converting to int;
                case MP_QSTR_flushing_last: data->flushing_last = (int)mp_obj_get_int(dest[1]); break; // converting to int;
                case MP_QSTR_last_area: data->last_area = (uint32_t)mp_obj_get_int(dest[1]); break; // converting to uint32_t;
                case MP_QSTR_last_part: data->last_part = (uint32_t)mp_obj_get_int(dest[1]); break; // converting to uint32_t;
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_lv_disp_buf_t_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct lv_disp_buf_t");
}

STATIC const mp_obj_dict_t mp_lv_disp_buf_t_locals_dict;

STATIC const mp_obj_type_t mp_lv_disp_buf_t_type = {
    { &mp_type_type },
    .name = MP_QSTR_lv_disp_buf_t,
    .print = mp_lv_disp_buf_t_print,
    .make_new = make_new_lv_struct,
    .attr = mp_lv_disp_buf_t_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_lv_disp_buf_t_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer }
};

STATIC inline const mp_obj_type_t *get_mp_lv_disp_buf_t_type()
{
    return &mp_lv_disp_buf_t_type;
}
    
STATIC void lv_disp_drv_t_flush_cb_callback(struct _disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
STATIC void lv_disp_drv_t_rounder_cb_callback(struct _disp_drv_t *disp_drv, lv_area_t *area);
STATIC void lv_disp_drv_t_set_px_cb_callback(struct _disp_drv_t *disp_drv, uint8_t *buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y, lv_color_t color, lv_opa_t opa);
STATIC void lv_disp_drv_t_monitor_cb_callback(struct _disp_drv_t *disp_drv, uint32_t time, uint32_t px);
STATIC void lv_disp_drv_t_wait_cb_callback(struct _disp_drv_t *disp_drv);
STATIC void lv_disp_drv_t_gpu_blend_cb_callback(struct _disp_drv_t *disp_drv, lv_color_t *dest, const lv_color_t *src, uint32_t length, lv_opa_t opa);
STATIC void lv_disp_drv_t_gpu_fill_cb_callback(struct _disp_drv_t *disp_drv, lv_color_t *dest_buf, lv_coord_t dest_width, const lv_area_t *fill_area, lv_color_t color);

/*
 * Struct lv_disp_drv_t
 */

STATIC inline const mp_obj_type_t *get_mp_lv_disp_drv_t_type();

STATIC inline lv_disp_drv_t* mp_write_ptr_lv_disp_drv_t(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_lv_disp_drv_t_type()));
    return (lv_disp_drv_t*)self->data;
}

#define mp_write_lv_disp_drv_t(struct_obj) *mp_write_ptr_lv_disp_drv_t(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_lv_disp_drv_t(lv_disp_drv_t *field)
{
    return lv_to_mp_struct(get_mp_lv_disp_drv_t_type(), (void*)field);
}

#define mp_read_lv_disp_drv_t(field) mp_read_ptr_lv_disp_drv_t(copy_buffer(&field, sizeof(lv_disp_drv_t)))
#define mp_read_byref_lv_disp_drv_t(field) mp_read_ptr_lv_disp_drv_t(&field)

STATIC void mp_lv_disp_drv_t_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    lv_disp_drv_t *data = (lv_disp_drv_t*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_hor_res: dest[0] = mp_obj_new_int(data->hor_res); break; // converting from lv_coord_t;
            case MP_QSTR_ver_res: dest[0] = mp_obj_new_int(data->ver_res); break; // converting from lv_coord_t;
            case MP_QSTR_buffer: dest[0] = mp_read_ptr_lv_disp_buf_t((void*)data->buffer); break; // converting from lv_disp_buf_t *;
            case MP_QSTR_antialiasing: dest[0] = mp_obj_new_int_from_uint(data->antialiasing); break; // converting from uint32_t;
            case MP_QSTR_rotated: dest[0] = mp_obj_new_int_from_uint(data->rotated); break; // converting from uint32_t;
            case MP_QSTR_dpi: dest[0] = mp_obj_new_int_from_uint(data->dpi); break; // converting from uint32_t;
            case MP_QSTR_flush_cb: dest[0] = ptr_to_mp((void*)data->flush_cb); break; // converting from callback void (*)(lv_disp_drv_t *disp_drv, lv_area_t *area, lv_color_t *color_p);
            case MP_QSTR_rounder_cb: dest[0] = ptr_to_mp((void*)data->rounder_cb); break; // converting from callback void (*)(lv_disp_drv_t *disp_drv, lv_area_t *area);
            case MP_QSTR_set_px_cb: dest[0] = ptr_to_mp((void*)data->set_px_cb); break; // converting from callback void (*)(lv_disp_drv_t *disp_drv, uint8_t *buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y, lv_color_t color, lv_opa_t opa);
            case MP_QSTR_monitor_cb: dest[0] = ptr_to_mp((void*)data->monitor_cb); break; // converting from callback void (*)(lv_disp_drv_t *disp_drv, uint32_t time, uint32_t px);
            case MP_QSTR_wait_cb: dest[0] = ptr_to_mp((void*)data->wait_cb); break; // converting from callback void (*)(lv_disp_drv_t *disp_drv);
            case MP_QSTR_gpu_blend_cb: dest[0] = ptr_to_mp((void*)data->gpu_blend_cb); break; // converting from callback void (*)(lv_disp_drv_t *disp_drv, lv_color_t *dest, lv_color_t *src, uint32_t length, lv_opa_t opa);
            case MP_QSTR_gpu_fill_cb: dest[0] = ptr_to_mp((void*)data->gpu_fill_cb); break; // converting from callback void (*)(lv_disp_drv_t *disp_drv, lv_color_t *dest_buf, lv_coord_t dest_width, lv_area_t *fill_area, lv_color_t color);
            case MP_QSTR_color_chroma_key: dest[0] = mp_read_byref_lv_color16_t(data->color_chroma_key); break; // converting from lv_color_t;
            case MP_QSTR_user_data: dest[0] = ptr_to_mp(data->user_data); break; // converting from lv_disp_drv_user_data_t;
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_hor_res: data->hor_res = (int16_t)mp_obj_get_int(dest[1]); break; // converting to lv_coord_t;
                case MP_QSTR_ver_res: data->ver_res = (int16_t)mp_obj_get_int(dest[1]); break; // converting to lv_coord_t;
                case MP_QSTR_buffer: data->buffer = (void*)mp_write_ptr_lv_disp_buf_t(dest[1]); break; // converting to lv_disp_buf_t *;
                case MP_QSTR_antialiasing: data->antialiasing = (uint32_t)mp_obj_get_int(dest[1]); break; // converting to uint32_t;
                case MP_QSTR_rotated: data->rotated = (uint32_t)mp_obj_get_int(dest[1]); break; // converting to uint32_t;
                case MP_QSTR_dpi: data->dpi = (uint32_t)mp_obj_get_int(dest[1]); break; // converting to uint32_t;
                case MP_QSTR_flush_cb: data->flush_cb = (void*)mp_lv_callback(dest[1], lv_disp_drv_t_flush_cb_callback ,MP_QSTR_lv_disp_drv_t_flush_cb, &data->user_data); break; // converting to callback void (*)(lv_disp_drv_t *disp_drv, lv_area_t *area, lv_color_t *color_p);
                case MP_QSTR_rounder_cb: data->rounder_cb = (void*)mp_lv_callback(dest[1], lv_disp_drv_t_rounder_cb_callback ,MP_QSTR_lv_disp_drv_t_rounder_cb, &data->user_data); break; // converting to callback void (*)(lv_disp_drv_t *disp_drv, lv_area_t *area);
                case MP_QSTR_set_px_cb: data->set_px_cb = (void*)mp_lv_callback(dest[1], lv_disp_drv_t_set_px_cb_callback ,MP_QSTR_lv_disp_drv_t_set_px_cb, &data->user_data); break; // converting to callback void (*)(lv_disp_drv_t *disp_drv, uint8_t *buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y, lv_color_t color, lv_opa_t opa);
                case MP_QSTR_monitor_cb: data->monitor_cb = (void*)mp_lv_callback(dest[1], lv_disp_drv_t_monitor_cb_callback ,MP_QSTR_lv_disp_drv_t_monitor_cb, &data->user_data); break; // converting to callback void (*)(lv_disp_drv_t *disp_drv, uint32_t time, uint32_t px);
                case MP_QSTR_wait_cb: data->wait_cb = (void*)mp_lv_callback(dest[1], lv_disp_drv_t_wait_cb_callback ,MP_QSTR_lv_disp_drv_t_wait_cb, &data->user_data); break; // converting to callback void (*)(lv_disp_drv_t *disp_drv);
                case MP_QSTR_gpu_blend_cb: data->gpu_blend_cb = (void*)mp_lv_callback(dest[1], lv_disp_drv_t_gpu_blend_cb_callback ,MP_QSTR_lv_disp_drv_t_gpu_blend_cb, &data->user_data); break; // converting to callback void (*)(lv_disp_drv_t *disp_drv, lv_color_t *dest, lv_color_t *src, uint32_t length, lv_opa_t opa);
                case MP_QSTR_gpu_fill_cb: data->gpu_fill_cb = (void*)mp_lv_callback(dest[1], lv_disp_drv_t_gpu_fill_cb_callback ,MP_QSTR_lv_disp_drv_t_gpu_fill_cb, &data->user_data); break; // converting to callback void (*)(lv_disp_drv_t *disp_drv, lv_color_t *dest_buf, lv_coord_t dest_width, lv_area_t *fill_area, lv_color_t color);
                case MP_QSTR_color_chroma_key: data->color_chroma_key = mp_write_lv_color16_t(dest[1]); break; // converting to lv_color_t;
                case MP_QSTR_user_data: data->user_data = mp_to_ptr(dest[1]); break; // converting to lv_disp_drv_user_data_t;
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_lv_disp_drv_t_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct lv_disp_drv_t");
}

STATIC const mp_obj_dict_t mp_lv_disp_drv_t_locals_dict;

STATIC const mp_obj_type_t mp_lv_disp_drv_t_type = {
    { &mp_type_type },
    .name = MP_QSTR_lv_disp_drv_t,
    .print = mp_lv_disp_drv_t_print,
    .make_new = make_new_lv_struct,
    .attr = mp_lv_disp_drv_t_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_lv_disp_drv_t_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer }
};

STATIC inline const mp_obj_type_t *get_mp_lv_disp_drv_t_type()
{
    return &mp_lv_disp_drv_t_type;
}
    

/*
 * Struct lv_ll_t
 */

STATIC inline const mp_obj_type_t *get_mp_lv_ll_t_type();

STATIC inline lv_ll_t* mp_write_ptr_lv_ll_t(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_lv_ll_t_type()));
    return (lv_ll_t*)self->data;
}

#define mp_write_lv_ll_t(struct_obj) *mp_write_ptr_lv_ll_t(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_lv_ll_t(lv_ll_t *field)
{
    return lv_to_mp_struct(get_mp_lv_ll_t_type(), (void*)field);
}

#define mp_read_lv_ll_t(field) mp_read_ptr_lv_ll_t(copy_buffer(&field, sizeof(lv_ll_t)))
#define mp_read_byref_lv_ll_t(field) mp_read_ptr_lv_ll_t(&field)

STATIC void mp_lv_ll_t_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    lv_ll_t *data = (lv_ll_t*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_n_size: dest[0] = mp_obj_new_int_from_uint(data->n_size); break; // converting from uint32_t;
            case MP_QSTR_head: dest[0] = ptr_to_mp((void*)data->head); break; // converting from lv_ll_node_t *;
            case MP_QSTR_tail: dest[0] = ptr_to_mp((void*)data->tail); break; // converting from lv_ll_node_t *;
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_n_size: data->n_size = (uint32_t)mp_obj_get_int(dest[1]); break; // converting to uint32_t;
                case MP_QSTR_head: data->head = (void*)mp_to_ptr(dest[1]); break; // converting to lv_ll_node_t *;
                case MP_QSTR_tail: data->tail = (void*)mp_to_ptr(dest[1]); break; // converting to lv_ll_node_t *;
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_lv_ll_t_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct lv_ll_t");
}

STATIC const mp_obj_dict_t mp_lv_ll_t_locals_dict;

STATIC const mp_obj_type_t mp_lv_ll_t_type = {
    { &mp_type_type },
    .name = MP_QSTR_lv_ll_t,
    .print = mp_lv_ll_t_print,
    .make_new = make_new_lv_struct,
    .attr = mp_lv_ll_t_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_lv_ll_t_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer }
};

STATIC inline const mp_obj_type_t *get_mp_lv_ll_t_type()
{
    return &mp_lv_ll_t_type;
}
    

/*
 * Array convertors for lv_area_t [32]
 */

STATIC lv_area_t *mp_arr_to_lv_area_t___32__(mp_obj_t mp_arr)
{
    mp_obj_t mp_len = mp_obj_len_maybe(mp_arr);
    if (mp_len == MP_OBJ_NULL) return mp_to_ptr(mp_arr);
    mp_int_t len = mp_obj_get_int(mp_len);
    //TODO check dim!
    lv_area_t *lv_arr = (lv_area_t*)m_malloc(len * sizeof(lv_area_t));
    mp_obj_t iter = mp_getiter(mp_arr, NULL);
    mp_obj_t item;
    size_t i = 0;
    while ((item = mp_iternext(iter)) != MP_OBJ_STOP_ITERATION) {
        lv_arr[i++] = mp_write_lv_area_t(item);
    }
    return (lv_area_t *)lv_arr;
}
    
STATIC mp_obj_t mp_arr_from_lv_area_t___32__(lv_area_t *arr)
{
    mp_obj_t obj_arr[32];
    for (size_t i=0; i<32; i++){
        obj_arr[i] = mp_read_lv_area_t(arr[i]);
    }
    return mp_obj_new_list(32, obj_arr); // TODO: return custom iterable object!
}
    

/*
 * Array convertors for uint8_t [32]
 */

STATIC uint8_t *mp_arr_to_uint8_t___32__(mp_obj_t mp_arr)
{
    mp_obj_t mp_len = mp_obj_len_maybe(mp_arr);
    if (mp_len == MP_OBJ_NULL) return mp_to_ptr(mp_arr);
    mp_int_t len = mp_obj_get_int(mp_len);
    //TODO check dim!
    uint8_t *lv_arr = (uint8_t*)m_malloc(len * sizeof(uint8_t));
    mp_obj_t iter = mp_getiter(mp_arr, NULL);
    mp_obj_t item;
    size_t i = 0;
    while ((item = mp_iternext(iter)) != MP_OBJ_STOP_ITERATION) {
        lv_arr[i++] = (uint8_t)mp_obj_get_int(item);
    }
    return (uint8_t *)lv_arr;
}
    
STATIC mp_obj_t mp_arr_from_uint8_t___32__(uint8_t *arr)
{
    mp_obj_t obj_arr[32];
    for (size_t i=0; i<32; i++){
        obj_arr[i] = mp_obj_new_int_from_uint(arr[i]);
    }
    return mp_obj_new_list(32, obj_arr); // TODO: return custom iterable object!
}
    

/*
 * Struct lv_disp_t
 */

STATIC inline const mp_obj_type_t *get_mp_lv_disp_t_type();

STATIC inline lv_disp_t* mp_write_ptr_lv_disp_t(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_lv_disp_t_type()));
    return (lv_disp_t*)self->data;
}

#define mp_write_lv_disp_t(struct_obj) *mp_write_ptr_lv_disp_t(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_lv_disp_t(lv_disp_t *field)
{
    return lv_to_mp_struct(get_mp_lv_disp_t_type(), (void*)field);
}

#define mp_read_lv_disp_t(field) mp_read_ptr_lv_disp_t(copy_buffer(&field, sizeof(lv_disp_t)))
#define mp_read_byref_lv_disp_t(field) mp_read_ptr_lv_disp_t(&field)

STATIC void mp_lv_disp_t_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    lv_disp_t *data = (lv_disp_t*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_driver: dest[0] = mp_read_byref_lv_disp_drv_t(data->driver); break; // converting from lv_disp_drv_t;
            case MP_QSTR_refr_task: dest[0] = mp_read_ptr_lv_task_t((void*)data->refr_task); break; // converting from lv_task_t *;
            case MP_QSTR_scr_ll: dest[0] = mp_read_byref_lv_ll_t(data->scr_ll); break; // converting from lv_ll_t;
            case MP_QSTR_act_scr: dest[0] = ptr_to_mp((void*)data->act_scr); break; // converting from struct _lv_obj_t *;
            case MP_QSTR_prev_scr: dest[0] = ptr_to_mp((void*)data->prev_scr); break; // converting from struct _lv_obj_t *;
            case MP_QSTR_top_layer: dest[0] = ptr_to_mp((void*)data->top_layer); break; // converting from struct _lv_obj_t *;
            case MP_QSTR_sys_layer: dest[0] = ptr_to_mp((void*)data->sys_layer); break; // converting from struct _lv_obj_t *;
            case MP_QSTR_del_prev: dest[0] = mp_obj_new_int_from_uint(data->del_prev); break; // converting from uint8_t;
            case MP_QSTR_bg_color: dest[0] = mp_read_byref_lv_color16_t(data->bg_color); break; // converting from lv_color_t;
            case MP_QSTR_bg_img: dest[0] = ptr_to_mp((void*)data->bg_img); break; // converting from void *;
            case MP_QSTR_bg_opa: dest[0] = mp_obj_new_int_from_uint(data->bg_opa); break; // converting from lv_opa_t;
            case MP_QSTR_inv_areas: dest[0] = mp_arr_from_lv_area_t___32__(data->inv_areas); break; // converting from lv_area_t [32];
            case MP_QSTR_inv_area_joined: dest[0] = mp_arr_from_uint8_t___32__(data->inv_area_joined); break; // converting from uint8_t [32];
            case MP_QSTR_inv_p: dest[0] = mp_obj_new_int_from_uint(data->inv_p); break; // converting from uint32_t;
            case MP_QSTR_last_activity_time: dest[0] = mp_obj_new_int_from_uint(data->last_activity_time); break; // converting from uint32_t;
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_driver: data->driver = mp_write_lv_disp_drv_t(dest[1]); break; // converting to lv_disp_drv_t;
                case MP_QSTR_refr_task: data->refr_task = (void*)mp_write_ptr_lv_task_t(dest[1]); break; // converting to lv_task_t *;
                case MP_QSTR_scr_ll: data->scr_ll = mp_write_lv_ll_t(dest[1]); break; // converting to lv_ll_t;
                case MP_QSTR_act_scr: data->act_scr = (void*)mp_to_ptr(dest[1]); break; // converting to struct _lv_obj_t *;
                case MP_QSTR_prev_scr: data->prev_scr = (void*)mp_to_ptr(dest[1]); break; // converting to struct _lv_obj_t *;
                case MP_QSTR_top_layer: data->top_layer = (void*)mp_to_ptr(dest[1]); break; // converting to struct _lv_obj_t *;
                case MP_QSTR_sys_layer: data->sys_layer = (void*)mp_to_ptr(dest[1]); break; // converting to struct _lv_obj_t *;
                case MP_QSTR_del_prev: data->del_prev = (uint8_t)mp_obj_get_int(dest[1]); break; // converting to uint8_t;
                case MP_QSTR_bg_color: data->bg_color = mp_write_lv_color16_t(dest[1]); break; // converting to lv_color_t;
                case MP_QSTR_bg_img: data->bg_img = (void*)mp_to_ptr(dest[1]); break; // converting to void *;
                case MP_QSTR_bg_opa: data->bg_opa = (uint8_t)mp_obj_get_int(dest[1]); break; // converting to lv_opa_t;
                case MP_QSTR_inv_areas: memcpy(&data->inv_areas, mp_arr_to_lv_area_t___32__(dest[1]), sizeof(lv_area_t)*32); break; // converting to lv_area_t [32];
                case MP_QSTR_inv_area_joined: memcpy(&data->inv_area_joined, mp_arr_to_uint8_t___32__(dest[1]), sizeof(uint8_t)*32); break; // converting to uint8_t [32];
                case MP_QSTR_inv_p: data->inv_p = (uint32_t)mp_obj_get_int(dest[1]); break; // converting to uint32_t;
                case MP_QSTR_last_activity_time: data->last_activity_time = (uint32_t)mp_obj_get_int(dest[1]); break; // converting to uint32_t;
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_lv_disp_t_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct lv_disp_t");
}

STATIC const mp_obj_dict_t mp_lv_disp_t_locals_dict;

STATIC const mp_obj_type_t mp_lv_disp_t_type = {
    { &mp_type_type },
    .name = MP_QSTR_lv_disp_t,
    .print = mp_lv_disp_t_print,
    .make_new = make_new_lv_struct,
    .attr = mp_lv_disp_t_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_lv_disp_t_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer }
};

STATIC inline const mp_obj_type_t *get_mp_lv_disp_t_type()
{
    return &mp_lv_disp_t_type;
}
    

/*
 * Struct lv_indev_drv_t
 */

STATIC inline const mp_obj_type_t *get_mp_lv_indev_drv_t_type();

STATIC inline lv_indev_drv_t* mp_write_ptr_lv_indev_drv_t(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_lv_indev_drv_t_type()));
    return (lv_indev_drv_t*)self->data;
}

#define mp_write_lv_indev_drv_t(struct_obj) *mp_write_ptr_lv_indev_drv_t(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_lv_indev_drv_t(lv_indev_drv_t *field)
{
    return lv_to_mp_struct(get_mp_lv_indev_drv_t_type(), (void*)field);
}

#define mp_read_lv_indev_drv_t(field) mp_read_ptr_lv_indev_drv_t(copy_buffer(&field, sizeof(lv_indev_drv_t)))
#define mp_read_byref_lv_indev_drv_t(field) mp_read_ptr_lv_indev_drv_t(&field)

STATIC void mp_lv_indev_drv_t_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    lv_indev_drv_t *data = (lv_indev_drv_t*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_type: dest[0] = mp_obj_new_int_from_uint(data->type); break; // converting from lv_indev_type_t;
            case MP_QSTR_read_cb: dest[0] = ptr_to_mp((void*)data->read_cb); break; // converting from callback bool (*)(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
            case MP_QSTR_feedback_cb: dest[0] = ptr_to_mp((void*)data->feedback_cb); break; // converting from callback void (*)(lv_indev_drv_t *, uint8_t);
            case MP_QSTR_user_data: dest[0] = ptr_to_mp(data->user_data); break; // converting from lv_indev_drv_user_data_t;
            case MP_QSTR_disp: dest[0] = mp_read_ptr_lv_disp_t((void*)data->disp); break; // converting from lv_disp_t *;
            case MP_QSTR_read_task: dest[0] = mp_read_ptr_lv_task_t((void*)data->read_task); break; // converting from lv_task_t *;
            case MP_QSTR_drag_limit: dest[0] = mp_obj_new_int_from_uint(data->drag_limit); break; // converting from uint8_t;
            case MP_QSTR_drag_throw: dest[0] = mp_obj_new_int_from_uint(data->drag_throw); break; // converting from uint8_t;
            case MP_QSTR_gesture_min_velocity: dest[0] = mp_obj_new_int_from_uint(data->gesture_min_velocity); break; // converting from uint8_t;
            case MP_QSTR_gesture_limit: dest[0] = mp_obj_new_int_from_uint(data->gesture_limit); break; // converting from uint8_t;
            case MP_QSTR_long_press_time: dest[0] = mp_obj_new_int_from_uint(data->long_press_time); break; // converting from uint16_t;
            case MP_QSTR_long_press_rep_time: dest[0] = mp_obj_new_int_from_uint(data->long_press_rep_time); break; // converting from uint16_t;
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_type: data->type = (uint8_t)mp_obj_get_int(dest[1]); break; // converting to lv_indev_type_t;
                case MP_QSTR_read_cb: data->read_cb = (void*)mp_lv_callback(dest[1], lv_indev_drv_t_read_cb_callback ,MP_QSTR_lv_indev_drv_t_read_cb, &data->user_data); break; // converting to callback bool (*)(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
                case MP_QSTR_feedback_cb: data->feedback_cb = (void*)mp_lv_callback(dest[1], lv_indev_drv_t_feedback_cb_callback ,MP_QSTR_lv_indev_drv_t_feedback_cb, &data->user_data); break; // converting to callback void (*)(lv_indev_drv_t *, uint8_t);
                case MP_QSTR_user_data: data->user_data = mp_to_ptr(dest[1]); break; // converting to lv_indev_drv_user_data_t;
                case MP_QSTR_disp: data->disp = (void*)mp_write_ptr_lv_disp_t(dest[1]); break; // converting to lv_disp_t *;
                case MP_QSTR_read_task: data->read_task = (void*)mp_write_ptr_lv_task_t(dest[1]); break; // converting to lv_task_t *;
                case MP_QSTR_drag_limit: data->drag_limit = (uint8_t)mp_obj_get_int(dest[1]); break; // converting to uint8_t;
                case MP_QSTR_drag_throw: data->drag_throw = (uint8_t)mp_obj_get_int(dest[1]); break; // converting to uint8_t;
                case MP_QSTR_gesture_min_velocity: data->gesture_min_velocity = (uint8_t)mp_obj_get_int(dest[1]); break; // converting to uint8_t;
                case MP_QSTR_gesture_limit: data->gesture_limit = (uint8_t)mp_obj_get_int(dest[1]); break; // converting to uint8_t;
                case MP_QSTR_long_press_time: data->long_press_time = (uint16_t)mp_obj_get_int(dest[1]); break; // converting to uint16_t;
                case MP_QSTR_long_press_rep_time: data->long_press_rep_time = (uint16_t)mp_obj_get_int(dest[1]); break; // converting to uint16_t;
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_lv_indev_drv_t_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct lv_indev_drv_t");
}

STATIC const mp_obj_dict_t mp_lv_indev_drv_t_locals_dict;

STATIC const mp_obj_type_t mp_lv_indev_drv_t_type = {
    { &mp_type_type },
    .name = MP_QSTR_lv_indev_drv_t,
    .print = mp_lv_indev_drv_t_print,
    .make_new = make_new_lv_struct,
    .attr = mp_lv_indev_drv_t_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_lv_indev_drv_t_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer }
};

STATIC inline const mp_obj_type_t *get_mp_lv_indev_drv_t_type()
{
    return &mp_lv_indev_drv_t_type;
}
    
typedef __typeof__( ((lv_indev_proc_t*)(0))->types ) lv_indev_proc_types_t;
typedef __typeof__( ((lv_indev_proc_types_t*)(0))->pointer ) lv_indev_proc_types_pointer_t;

/*
 * Struct lv_point_t
 */

STATIC inline const mp_obj_type_t *get_mp_lv_point_t_type();

STATIC inline lv_point_t* mp_write_ptr_lv_point_t(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_lv_point_t_type()));
    return (lv_point_t*)self->data;
}

#define mp_write_lv_point_t(struct_obj) *mp_write_ptr_lv_point_t(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_lv_point_t(lv_point_t *field)
{
    return lv_to_mp_struct(get_mp_lv_point_t_type(), (void*)field);
}

#define mp_read_lv_point_t(field) mp_read_ptr_lv_point_t(copy_buffer(&field, sizeof(lv_point_t)))
#define mp_read_byref_lv_point_t(field) mp_read_ptr_lv_point_t(&field)

STATIC void mp_lv_point_t_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    lv_point_t *data = (lv_point_t*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_x: dest[0] = mp_obj_new_int(data->x); break; // converting from lv_coord_t;
            case MP_QSTR_y: dest[0] = mp_obj_new_int(data->y); break; // converting from lv_coord_t;
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_x: data->x = (int16_t)mp_obj_get_int(dest[1]); break; // converting to lv_coord_t;
                case MP_QSTR_y: data->y = (int16_t)mp_obj_get_int(dest[1]); break; // converting to lv_coord_t;
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_lv_point_t_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct lv_point_t");
}

STATIC const mp_obj_dict_t mp_lv_point_t_locals_dict;

STATIC const mp_obj_type_t mp_lv_point_t_type = {
    { &mp_type_type },
    .name = MP_QSTR_lv_point_t,
    .print = mp_lv_point_t_print,
    .make_new = make_new_lv_struct,
    .attr = mp_lv_point_t_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_lv_point_t_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer }
};

STATIC inline const mp_obj_type_t *get_mp_lv_point_t_type()
{
    return &mp_lv_point_t_type;
}
    

/*
 * Struct lv_indev_proc_types_pointer_t
 */

STATIC inline const mp_obj_type_t *get_mp_lv_indev_proc_types_pointer_t_type();

STATIC inline lv_indev_proc_types_pointer_t* mp_write_ptr_lv_indev_proc_types_pointer_t(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_lv_indev_proc_types_pointer_t_type()));
    return (lv_indev_proc_types_pointer_t*)self->data;
}

#define mp_write_lv_indev_proc_types_pointer_t(struct_obj) *mp_write_ptr_lv_indev_proc_types_pointer_t(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_lv_indev_proc_types_pointer_t(lv_indev_proc_types_pointer_t *field)
{
    return lv_to_mp_struct(get_mp_lv_indev_proc_types_pointer_t_type(), (void*)field);
}

#define mp_read_lv_indev_proc_types_pointer_t(field) mp_read_ptr_lv_indev_proc_types_pointer_t(copy_buffer(&field, sizeof(lv_indev_proc_types_pointer_t)))
#define mp_read_byref_lv_indev_proc_types_pointer_t(field) mp_read_ptr_lv_indev_proc_types_pointer_t(&field)

STATIC void mp_lv_indev_proc_types_pointer_t_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    lv_indev_proc_types_pointer_t *data = (lv_indev_proc_types_pointer_t*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_act_point: dest[0] = mp_read_byref_lv_point_t(data->act_point); break; // converting from lv_point_t;
            case MP_QSTR_last_point: dest[0] = mp_read_byref_lv_point_t(data->last_point); break; // converting from lv_point_t;
            case MP_QSTR_vect: dest[0] = mp_read_byref_lv_point_t(data->vect); break; // converting from lv_point_t;
            case MP_QSTR_drag_sum: dest[0] = mp_read_byref_lv_point_t(data->drag_sum); break; // converting from lv_point_t;
            case MP_QSTR_drag_throw_vect: dest[0] = mp_read_byref_lv_point_t(data->drag_throw_vect); break; // converting from lv_point_t;
            case MP_QSTR_act_obj: dest[0] = ptr_to_mp((void*)data->act_obj); break; // converting from struct _lv_obj_t *;
            case MP_QSTR_last_obj: dest[0] = ptr_to_mp((void*)data->last_obj); break; // converting from struct _lv_obj_t *;
            case MP_QSTR_last_pressed: dest[0] = ptr_to_mp((void*)data->last_pressed); break; // converting from struct _lv_obj_t *;
            case MP_QSTR_gesture_dir: dest[0] = mp_obj_new_int_from_uint(data->gesture_dir); break; // converting from lv_gesture_dir_t;
            case MP_QSTR_gesture_sum: dest[0] = mp_read_byref_lv_point_t(data->gesture_sum); break; // converting from lv_point_t;
            case MP_QSTR_drag_limit_out: dest[0] = mp_obj_new_int_from_uint(data->drag_limit_out); break; // converting from uint8_t;
            case MP_QSTR_drag_in_prog: dest[0] = mp_obj_new_int_from_uint(data->drag_in_prog); break; // converting from uint8_t;
            case MP_QSTR_drag_dir: dest[0] = mp_obj_new_int_from_uint(data->drag_dir); break; // converting from lv_drag_dir_t;
            case MP_QSTR_gesture_sent: dest[0] = mp_obj_new_int_from_uint(data->gesture_sent); break; // converting from uint8_t;
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_act_point: data->act_point = mp_write_lv_point_t(dest[1]); break; // converting to lv_point_t;
                case MP_QSTR_last_point: data->last_point = mp_write_lv_point_t(dest[1]); break; // converting to lv_point_t;
                case MP_QSTR_vect: data->vect = mp_write_lv_point_t(dest[1]); break; // converting to lv_point_t;
                case MP_QSTR_drag_sum: data->drag_sum = mp_write_lv_point_t(dest[1]); break; // converting to lv_point_t;
                case MP_QSTR_drag_throw_vect: data->drag_throw_vect = mp_write_lv_point_t(dest[1]); break; // converting to lv_point_t;
                case MP_QSTR_act_obj: data->act_obj = (void*)mp_to_ptr(dest[1]); break; // converting to struct _lv_obj_t *;
                case MP_QSTR_last_obj: data->last_obj = (void*)mp_to_ptr(dest[1]); break; // converting to struct _lv_obj_t *;
                case MP_QSTR_last_pressed: data->last_pressed = (void*)mp_to_ptr(dest[1]); break; // converting to struct _lv_obj_t *;
                case MP_QSTR_gesture_dir: data->gesture_dir = (uint8_t)mp_obj_get_int(dest[1]); break; // converting to lv_gesture_dir_t;
                case MP_QSTR_gesture_sum: data->gesture_sum = mp_write_lv_point_t(dest[1]); break; // converting to lv_point_t;
                case MP_QSTR_drag_limit_out: data->drag_limit_out = (uint8_t)mp_obj_get_int(dest[1]); break; // converting to uint8_t;
                case MP_QSTR_drag_in_prog: data->drag_in_prog = (uint8_t)mp_obj_get_int(dest[1]); break; // converting to uint8_t;
                case MP_QSTR_drag_dir: data->drag_dir = (uint8_t)mp_obj_get_int(dest[1]); break; // converting to lv_drag_dir_t;
                case MP_QSTR_gesture_sent: data->gesture_sent = (uint8_t)mp_obj_get_int(dest[1]); break; // converting to uint8_t;
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_lv_indev_proc_types_pointer_t_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct lv_indev_proc_types_pointer_t");
}

STATIC const mp_obj_dict_t mp_lv_indev_proc_types_pointer_t_locals_dict;

STATIC const mp_obj_type_t mp_lv_indev_proc_types_pointer_t_type = {
    { &mp_type_type },
    .name = MP_QSTR_lv_indev_proc_types_pointer_t,
    .print = mp_lv_indev_proc_types_pointer_t_print,
    .make_new = make_new_lv_struct,
    .attr = mp_lv_indev_proc_types_pointer_t_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_lv_indev_proc_types_pointer_t_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer }
};

STATIC inline const mp_obj_type_t *get_mp_lv_indev_proc_types_pointer_t_type()
{
    return &mp_lv_indev_proc_types_pointer_t_type;
}
    
typedef __typeof__( ((lv_indev_proc_types_t*)(0))->keypad ) lv_indev_proc_types_keypad_t;

/*
 * Struct lv_indev_proc_types_keypad_t
 */

STATIC inline const mp_obj_type_t *get_mp_lv_indev_proc_types_keypad_t_type();

STATIC inline lv_indev_proc_types_keypad_t* mp_write_ptr_lv_indev_proc_types_keypad_t(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_lv_indev_proc_types_keypad_t_type()));
    return (lv_indev_proc_types_keypad_t*)self->data;
}

#define mp_write_lv_indev_proc_types_keypad_t(struct_obj) *mp_write_ptr_lv_indev_proc_types_keypad_t(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_lv_indev_proc_types_keypad_t(lv_indev_proc_types_keypad_t *field)
{
    return lv_to_mp_struct(get_mp_lv_indev_proc_types_keypad_t_type(), (void*)field);
}

#define mp_read_lv_indev_proc_types_keypad_t(field) mp_read_ptr_lv_indev_proc_types_keypad_t(copy_buffer(&field, sizeof(lv_indev_proc_types_keypad_t)))
#define mp_read_byref_lv_indev_proc_types_keypad_t(field) mp_read_ptr_lv_indev_proc_types_keypad_t(&field)

STATIC void mp_lv_indev_proc_types_keypad_t_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    lv_indev_proc_types_keypad_t *data = (lv_indev_proc_types_keypad_t*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_last_state: dest[0] = mp_obj_new_int_from_uint(data->last_state); break; // converting from lv_indev_state_t;
            case MP_QSTR_last_key: dest[0] = mp_obj_new_int_from_uint(data->last_key); break; // converting from uint32_t;
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_last_state: data->last_state = (uint8_t)mp_obj_get_int(dest[1]); break; // converting to lv_indev_state_t;
                case MP_QSTR_last_key: data->last_key = (uint32_t)mp_obj_get_int(dest[1]); break; // converting to uint32_t;
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_lv_indev_proc_types_keypad_t_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct lv_indev_proc_types_keypad_t");
}

STATIC const mp_obj_dict_t mp_lv_indev_proc_types_keypad_t_locals_dict;

STATIC const mp_obj_type_t mp_lv_indev_proc_types_keypad_t_type = {
    { &mp_type_type },
    .name = MP_QSTR_lv_indev_proc_types_keypad_t,
    .print = mp_lv_indev_proc_types_keypad_t_print,
    .make_new = make_new_lv_struct,
    .attr = mp_lv_indev_proc_types_keypad_t_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_lv_indev_proc_types_keypad_t_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer }
};

STATIC inline const mp_obj_type_t *get_mp_lv_indev_proc_types_keypad_t_type()
{
    return &mp_lv_indev_proc_types_keypad_t_type;
}
    

/*
 * Struct lv_indev_proc_types_t
 */

STATIC inline const mp_obj_type_t *get_mp_lv_indev_proc_types_t_type();

STATIC inline lv_indev_proc_types_t* mp_write_ptr_lv_indev_proc_types_t(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_lv_indev_proc_types_t_type()));
    return (lv_indev_proc_types_t*)self->data;
}

#define mp_write_lv_indev_proc_types_t(struct_obj) *mp_write_ptr_lv_indev_proc_types_t(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_lv_indev_proc_types_t(lv_indev_proc_types_t *field)
{
    return lv_to_mp_struct(get_mp_lv_indev_proc_types_t_type(), (void*)field);
}

#define mp_read_lv_indev_proc_types_t(field) mp_read_ptr_lv_indev_proc_types_t(copy_buffer(&field, sizeof(lv_indev_proc_types_t)))
#define mp_read_byref_lv_indev_proc_types_t(field) mp_read_ptr_lv_indev_proc_types_t(&field)

STATIC void mp_lv_indev_proc_types_t_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    lv_indev_proc_types_t *data = (lv_indev_proc_types_t*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_pointer: dest[0] = mp_read_byref_lv_indev_proc_types_pointer_t(data->pointer); break; // converting from lv_indev_proc_types_pointer_t;
            case MP_QSTR_keypad: dest[0] = mp_read_byref_lv_indev_proc_types_keypad_t(data->keypad); break; // converting from lv_indev_proc_types_keypad_t;
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_pointer: data->pointer = mp_write_lv_indev_proc_types_pointer_t(dest[1]); break; // converting to lv_indev_proc_types_pointer_t;
                case MP_QSTR_keypad: data->keypad = mp_write_lv_indev_proc_types_keypad_t(dest[1]); break; // converting to lv_indev_proc_types_keypad_t;
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_lv_indev_proc_types_t_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct lv_indev_proc_types_t");
}

STATIC const mp_obj_dict_t mp_lv_indev_proc_types_t_locals_dict;

STATIC const mp_obj_type_t mp_lv_indev_proc_types_t_type = {
    { &mp_type_type },
    .name = MP_QSTR_lv_indev_proc_types_t,
    .print = mp_lv_indev_proc_types_t_print,
    .make_new = make_new_lv_struct,
    .attr = mp_lv_indev_proc_types_t_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_lv_indev_proc_types_t_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer }
};

STATIC inline const mp_obj_type_t *get_mp_lv_indev_proc_types_t_type()
{
    return &mp_lv_indev_proc_types_t_type;
}
    

/*
 * Struct lv_indev_proc_t
 */

STATIC inline const mp_obj_type_t *get_mp_lv_indev_proc_t_type();

STATIC inline lv_indev_proc_t* mp_write_ptr_lv_indev_proc_t(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_lv_indev_proc_t_type()));
    return (lv_indev_proc_t*)self->data;
}

#define mp_write_lv_indev_proc_t(struct_obj) *mp_write_ptr_lv_indev_proc_t(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_lv_indev_proc_t(lv_indev_proc_t *field)
{
    return lv_to_mp_struct(get_mp_lv_indev_proc_t_type(), (void*)field);
}

#define mp_read_lv_indev_proc_t(field) mp_read_ptr_lv_indev_proc_t(copy_buffer(&field, sizeof(lv_indev_proc_t)))
#define mp_read_byref_lv_indev_proc_t(field) mp_read_ptr_lv_indev_proc_t(&field)

STATIC void mp_lv_indev_proc_t_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    lv_indev_proc_t *data = (lv_indev_proc_t*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_state: dest[0] = mp_obj_new_int_from_uint(data->state); break; // converting from lv_indev_state_t;
            case MP_QSTR_types: dest[0] = mp_read_byref_lv_indev_proc_types_t(data->types); break; // converting from lv_indev_proc_types_t;
            case MP_QSTR_pr_timestamp: dest[0] = mp_obj_new_int_from_uint(data->pr_timestamp); break; // converting from uint32_t;
            case MP_QSTR_longpr_rep_timestamp: dest[0] = mp_obj_new_int_from_uint(data->longpr_rep_timestamp); break; // converting from uint32_t;
            case MP_QSTR_long_pr_sent: dest[0] = mp_obj_new_int_from_uint(data->long_pr_sent); break; // converting from uint8_t;
            case MP_QSTR_reset_query: dest[0] = mp_obj_new_int_from_uint(data->reset_query); break; // converting from uint8_t;
            case MP_QSTR_disabled: dest[0] = mp_obj_new_int_from_uint(data->disabled); break; // converting from uint8_t;
            case MP_QSTR_wait_until_release: dest[0] = mp_obj_new_int_from_uint(data->wait_until_release); break; // converting from uint8_t;
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_state: data->state = (uint8_t)mp_obj_get_int(dest[1]); break; // converting to lv_indev_state_t;
                case MP_QSTR_types: data->types = mp_write_lv_indev_proc_types_t(dest[1]); break; // converting to lv_indev_proc_types_t;
                case MP_QSTR_pr_timestamp: data->pr_timestamp = (uint32_t)mp_obj_get_int(dest[1]); break; // converting to uint32_t;
                case MP_QSTR_longpr_rep_timestamp: data->longpr_rep_timestamp = (uint32_t)mp_obj_get_int(dest[1]); break; // converting to uint32_t;
                case MP_QSTR_long_pr_sent: data->long_pr_sent = (uint8_t)mp_obj_get_int(dest[1]); break; // converting to uint8_t;
                case MP_QSTR_reset_query: data->reset_query = (uint8_t)mp_obj_get_int(dest[1]); break; // converting to uint8_t;
                case MP_QSTR_disabled: data->disabled = (uint8_t)mp_obj_get_int(dest[1]); break; // converting to uint8_t;
                case MP_QSTR_wait_until_release: data->wait_until_release = (uint8_t)mp_obj_get_int(dest[1]); break; // converting to uint8_t;
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_lv_indev_proc_t_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct lv_indev_proc_t");
}

STATIC const mp_obj_dict_t mp_lv_indev_proc_t_locals_dict;

STATIC const mp_obj_type_t mp_lv_indev_proc_t_type = {
    { &mp_type_type },
    .name = MP_QSTR_lv_indev_proc_t,
    .print = mp_lv_indev_proc_t_print,
    .make_new = make_new_lv_struct,
    .attr = mp_lv_indev_proc_t_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_lv_indev_proc_t_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer }
};

STATIC inline const mp_obj_type_t *get_mp_lv_indev_proc_t_type()
{
    return &mp_lv_indev_proc_t_type;
}
    

/*
 * Struct lv_indev_t
 */

STATIC inline const mp_obj_type_t *get_mp_lv_indev_t_type();

STATIC inline lv_indev_t* mp_write_ptr_lv_indev_t(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_lv_indev_t_type()));
    return (lv_indev_t*)self->data;
}

#define mp_write_lv_indev_t(struct_obj) *mp_write_ptr_lv_indev_t(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_lv_indev_t(lv_indev_t *field)
{
    return lv_to_mp_struct(get_mp_lv_indev_t_type(), (void*)field);
}

#define mp_read_lv_indev_t(field) mp_read_ptr_lv_indev_t(copy_buffer(&field, sizeof(lv_indev_t)))
#define mp_read_byref_lv_indev_t(field) mp_read_ptr_lv_indev_t(&field)

STATIC void mp_lv_indev_t_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    lv_indev_t *data = (lv_indev_t*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_driver: dest[0] = mp_read_byref_lv_indev_drv_t(data->driver); break; // converting from lv_indev_drv_t;
            case MP_QSTR_proc: dest[0] = mp_read_byref_lv_indev_proc_t(data->proc); break; // converting from lv_indev_proc_t;
            case MP_QSTR_cursor: dest[0] = ptr_to_mp((void*)data->cursor); break; // converting from struct _lv_obj_t *;
            case MP_QSTR_group: dest[0] = ptr_to_mp((void*)data->group); break; // converting from struct _lv_group_t *;
            case MP_QSTR_btn_points: dest[0] = mp_read_ptr_lv_point_t((void*)data->btn_points); break; // converting from lv_point_t *;
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_driver: data->driver = mp_write_lv_indev_drv_t(dest[1]); break; // converting to lv_indev_drv_t;
                case MP_QSTR_proc: data->proc = mp_write_lv_indev_proc_t(dest[1]); break; // converting to lv_indev_proc_t;
                case MP_QSTR_cursor: data->cursor = (void*)mp_to_ptr(dest[1]); break; // converting to struct _lv_obj_t *;
                case MP_QSTR_group: data->group = (void*)mp_to_ptr(dest[1]); break; // converting to struct _lv_group_t *;
                case MP_QSTR_btn_points: data->btn_points = (void*)mp_write_ptr_lv_point_t(dest[1]); break; // converting to lv_point_t *;
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_lv_indev_t_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct lv_indev_t");
}

STATIC const mp_obj_dict_t mp_lv_indev_t_locals_dict;

STATIC const mp_obj_type_t mp_lv_indev_t_type = {
    { &mp_type_type },
    .name = MP_QSTR_lv_indev_t,
    .print = mp_lv_indev_t_print,
    .make_new = make_new_lv_struct,
    .attr = mp_lv_indev_t_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_lv_indev_t_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer }
};

STATIC inline const mp_obj_type_t *get_mp_lv_indev_t_type()
{
    return &mp_lv_indev_t_type;
}
    
/* List of structs: ['C_Pointer', 'lv_area_t', 'lv_color16_t', 'lv_color16_ch_t', 'lv_mem_monitor_t', 'lv_task_t', 'lv_indev_drv_t', 'lv_disp_t', 'lv_disp_drv_t', 'lv_disp_buf_t', 'lv_ll_t', 'lv_indev_t', 'lv_indev_proc_t', 'lv_indev_proc_types_t', 'lv_indev_proc_types_pointer_t', 'lv_point_t', 'lv_indev_proc_types_keypad_t'] */
/* Struct C_Pointer contains: [] */

STATIC const mp_rom_map_elem_t mp_C_Pointer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SIZE), MP_ROM_PTR(MP_ROM_INT(sizeof(C_Pointer))) },
    { MP_ROM_QSTR(MP_QSTR_cast), MP_ROM_PTR(&mp_lv_cast_class_method) },
    { MP_ROM_QSTR(MP_QSTR_cast_instance), MP_ROM_PTR(&mp_lv_cast_instance_obj) },
    { MP_ROM_QSTR(MP_QSTR___dereference__), MP_ROM_PTR(&mp_lv_dereference_obj) },
    
};

STATIC MP_DEFINE_CONST_DICT(mp_C_Pointer_locals_dict, mp_C_Pointer_locals_dict_table);
        
/* Struct lv_area_t contains: ['lv_area_copy', 'lv_area_get_width', 'lv_area_get_height', 'lv_area_set', 'lv_area_set_width', 'lv_area_set_height', 'lv_area_get_size'] */

/*
 * st7789_lvgl extension definition for:
 * inline static void lv_area_copy(lv_area_t *dest, const lv_area_t *src)
 */
 
STATIC mp_obj_t mp_lv_area_copy(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_area_t *dest = mp_write_ptr_lv_area_t(mp_args[0]);
    const lv_area_t *src = mp_write_ptr_lv_area_t(mp_args[1]);
    lv_area_copy(dest, src);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_area_copy_obj, 2, mp_lv_area_copy, lv_area_copy);

 

/*
 * st7789_lvgl extension definition for:
 * inline static lv_coord_t lv_area_get_width(const lv_area_t *area_p)
 */
 
STATIC mp_obj_t mp_lv_area_get_width(size_t mp_n_args, const mp_obj_t *mp_args)
{
    const lv_area_t *area_p = mp_write_ptr_lv_area_t(mp_args[0]);
    lv_coord_t _res = lv_area_get_width(area_p);
    return mp_obj_new_int(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_area_get_width_obj, 1, mp_lv_area_get_width, lv_area_get_width);

 

/*
 * st7789_lvgl extension definition for:
 * inline static lv_coord_t lv_area_get_height(const lv_area_t *area_p)
 */
 
STATIC mp_obj_t mp_lv_area_get_height(size_t mp_n_args, const mp_obj_t *mp_args)
{
    const lv_area_t *area_p = mp_write_ptr_lv_area_t(mp_args[0]);
    lv_coord_t _res = lv_area_get_height(area_p);
    return mp_obj_new_int(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_area_get_height_obj, 1, mp_lv_area_get_height, lv_area_get_height);

 

/*
 * st7789_lvgl extension definition for:
 * void lv_area_set(lv_area_t *area_p, lv_coord_t x1, lv_coord_t y1, lv_coord_t x2, lv_coord_t y2)
 */
 
STATIC mp_obj_t mp_lv_area_set(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_area_t *area_p = mp_write_ptr_lv_area_t(mp_args[0]);
    lv_coord_t x1 = (int16_t)mp_obj_get_int(mp_args[1]);
    lv_coord_t y1 = (int16_t)mp_obj_get_int(mp_args[2]);
    lv_coord_t x2 = (int16_t)mp_obj_get_int(mp_args[3]);
    lv_coord_t y2 = (int16_t)mp_obj_get_int(mp_args[4]);
    lv_area_set(area_p, x1, y1, x2, y2);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_area_set_obj, 5, mp_lv_area_set, lv_area_set);

 

/*
 * st7789_lvgl extension definition for:
 * void lv_area_set_width(lv_area_t *area_p, lv_coord_t w)
 */
 
STATIC mp_obj_t mp_lv_area_set_width(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_area_t *area_p = mp_write_ptr_lv_area_t(mp_args[0]);
    lv_coord_t w = (int16_t)mp_obj_get_int(mp_args[1]);
    lv_area_set_width(area_p, w);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_area_set_width_obj, 2, mp_lv_area_set_width, lv_area_set_width);

 

/*
 * st7789_lvgl extension definition for:
 * void lv_area_set_height(lv_area_t *area_p, lv_coord_t h)
 */
 
STATIC mp_obj_t mp_lv_area_set_height(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_area_t *area_p = mp_write_ptr_lv_area_t(mp_args[0]);
    lv_coord_t h = (int16_t)mp_obj_get_int(mp_args[1]);
    lv_area_set_height(area_p, h);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_area_set_height_obj, 2, mp_lv_area_set_height, lv_area_set_height);

 

/*
 * st7789_lvgl extension definition for:
 * uint32_t lv_area_get_size(const lv_area_t *area_p)
 */
 
STATIC mp_obj_t mp_lv_area_get_size(size_t mp_n_args, const mp_obj_t *mp_args)
{
    const lv_area_t *area_p = mp_write_ptr_lv_area_t(mp_args[0]);
    uint32_t _res = lv_area_get_size(area_p);
    return mp_obj_new_int_from_uint(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_area_get_size_obj, 1, mp_lv_area_get_size, lv_area_get_size);

 

STATIC const mp_rom_map_elem_t mp_lv_area_t_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SIZE), MP_ROM_PTR(MP_ROM_INT(sizeof(lv_area_t))) },
    { MP_ROM_QSTR(MP_QSTR_cast), MP_ROM_PTR(&mp_lv_cast_class_method) },
    { MP_ROM_QSTR(MP_QSTR_cast_instance), MP_ROM_PTR(&mp_lv_cast_instance_obj) },
    { MP_ROM_QSTR(MP_QSTR___dereference__), MP_ROM_PTR(&mp_lv_dereference_obj) },
    { MP_ROM_QSTR(MP_QSTR_copy), MP_ROM_PTR(&mp_lv_area_copy_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_width), MP_ROM_PTR(&mp_lv_area_get_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_height), MP_ROM_PTR(&mp_lv_area_get_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_set), MP_ROM_PTR(&mp_lv_area_set_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_width), MP_ROM_PTR(&mp_lv_area_set_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_height), MP_ROM_PTR(&mp_lv_area_set_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_size), MP_ROM_PTR(&mp_lv_area_get_size_obj) },
    
};

STATIC MP_DEFINE_CONST_DICT(mp_lv_area_t_locals_dict, mp_lv_area_t_locals_dict_table);
        
/* Struct lv_color16_t contains: ['lv_color_to1', 'lv_color_to8', 'lv_color_to16', 'lv_color_to32', 'lv_color_mix', 'lv_color_premult', 'lv_color_mix_with_alpha', 'lv_color_brightness', 'lv_color_fill', 'lv_color_lighten', 'lv_color_darken', 'lv_color_to_hsv'] */

/*
 * st7789_lvgl extension definition for:
 * inline static uint8_t lv_color_to1(lv_color_t color)
 */
 
STATIC mp_obj_t mp_lv_color_to1(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_color_t color = mp_write_lv_color16_t(mp_args[0]);
    uint8_t _res = lv_color_to1(color);
    return mp_obj_new_int_from_uint(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_color_to1_obj, 1, mp_lv_color_to1, lv_color_to1);

 

/*
 * st7789_lvgl extension definition for:
 * inline static uint8_t lv_color_to8(lv_color_t color)
 */
 
STATIC mp_obj_t mp_lv_color_to8(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_color_t color = mp_write_lv_color16_t(mp_args[0]);
    uint8_t _res = lv_color_to8(color);
    return mp_obj_new_int_from_uint(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_color_to8_obj, 1, mp_lv_color_to8, lv_color_to8);

 

/*
 * st7789_lvgl extension definition for:
 * inline static uint16_t lv_color_to16(lv_color_t color)
 */
 
STATIC mp_obj_t mp_lv_color_to16(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_color_t color = mp_write_lv_color16_t(mp_args[0]);
    uint16_t _res = lv_color_to16(color);
    return mp_obj_new_int_from_uint(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_color_to16_obj, 1, mp_lv_color_to16, lv_color_to16);

 

/*
 * st7789_lvgl extension definition for:
 * inline static uint32_t lv_color_to32(lv_color_t color)
 */
 
STATIC mp_obj_t mp_lv_color_to32(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_color_t color = mp_write_lv_color16_t(mp_args[0]);
    uint32_t _res = lv_color_to32(color);
    return mp_obj_new_int_from_uint(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_color_to32_obj, 1, mp_lv_color_to32, lv_color_to32);

 

/*
 * st7789_lvgl extension definition for:
 * inline static lv_color_t lv_color_mix(lv_color_t c1, lv_color_t c2, uint8_t mix)
 */
 
STATIC mp_obj_t mp_lv_color_mix(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_color_t c1 = mp_write_lv_color16_t(mp_args[0]);
    lv_color_t c2 = mp_write_lv_color16_t(mp_args[1]);
    uint8_t mix = (uint8_t)mp_obj_get_int(mp_args[2]);
    lv_color_t _res = lv_color_mix(c1, c2, mix);
    return mp_read_lv_color16_t(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_color_mix_obj, 3, mp_lv_color_mix, lv_color_mix);

 

/*
 * st7789_lvgl extension definition for:
 * inline static void lv_color_premult(lv_color_t c, uint8_t mix, uint16_t *out)
 */
 
STATIC mp_obj_t mp_lv_color_premult(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_color_t c = mp_write_lv_color16_t(mp_args[0]);
    uint8_t mix = (uint8_t)mp_obj_get_int(mp_args[1]);
    uint16_t *out = mp_to_ptr(mp_args[2]);
    lv_color_premult(c, mix, out);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_color_premult_obj, 3, mp_lv_color_premult, lv_color_premult);

 

/*
 * st7789_lvgl extension definition for:
 * inline static void lv_color_mix_with_alpha(lv_color_t bg_color, lv_opa_t bg_opa, lv_color_t fg_color, lv_opa_t fg_opa, lv_color_t *res_color, lv_opa_t *res_opa)
 */
 
STATIC mp_obj_t mp_lv_color_mix_with_alpha(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_color_t bg_color = mp_write_lv_color16_t(mp_args[0]);
    lv_opa_t bg_opa = (uint8_t)mp_obj_get_int(mp_args[1]);
    lv_color_t fg_color = mp_write_lv_color16_t(mp_args[2]);
    lv_opa_t fg_opa = (uint8_t)mp_obj_get_int(mp_args[3]);
    lv_color_t *res_color = mp_write_ptr_lv_color16_t(mp_args[4]);
    lv_opa_t *res_opa = mp_to_ptr(mp_args[5]);
    lv_color_mix_with_alpha(bg_color, bg_opa, fg_color, fg_opa, res_color, res_opa);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_color_mix_with_alpha_obj, 6, mp_lv_color_mix_with_alpha, lv_color_mix_with_alpha);

 

/*
 * st7789_lvgl extension definition for:
 * inline static uint8_t lv_color_brightness(lv_color_t color)
 */
 
STATIC mp_obj_t mp_lv_color_brightness(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_color_t color = mp_write_lv_color16_t(mp_args[0]);
    uint8_t _res = lv_color_brightness(color);
    return mp_obj_new_int_from_uint(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_color_brightness_obj, 1, mp_lv_color_brightness, lv_color_brightness);

 

/*
 * st7789_lvgl extension definition for:
 * void lv_color_fill(lv_color_t *buf, lv_color_t color, uint32_t px_num)
 */
 
STATIC mp_obj_t mp_lv_color_fill(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_color_t *buf = mp_write_ptr_lv_color16_t(mp_args[0]);
    lv_color_t color = mp_write_lv_color16_t(mp_args[1]);
    uint32_t px_num = (uint32_t)mp_obj_get_int(mp_args[2]);
    lv_color_fill(buf, color, px_num);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_color_fill_obj, 3, mp_lv_color_fill, lv_color_fill);

 

/*
 * st7789_lvgl extension definition for:
 * lv_color_t lv_color_lighten(lv_color_t c, lv_opa_t lvl)
 */
 
STATIC mp_obj_t mp_lv_color_lighten(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_color_t c = mp_write_lv_color16_t(mp_args[0]);
    lv_opa_t lvl = (uint8_t)mp_obj_get_int(mp_args[1]);
    lv_color_t _res = lv_color_lighten(c, lvl);
    return mp_read_lv_color16_t(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_color_lighten_obj, 2, mp_lv_color_lighten, lv_color_lighten);

 

/*
 * st7789_lvgl extension definition for:
 * lv_color_t lv_color_darken(lv_color_t c, lv_opa_t lvl)
 */
 
STATIC mp_obj_t mp_lv_color_darken(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_color_t c = mp_write_lv_color16_t(mp_args[0]);
    lv_opa_t lvl = (uint8_t)mp_obj_get_int(mp_args[1]);
    lv_color_t _res = lv_color_darken(c, lvl);
    return mp_read_lv_color16_t(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_color_darken_obj, 2, mp_lv_color_darken, lv_color_darken);

 

/*
 * Struct lv_color_hsv_t
 */

STATIC inline const mp_obj_type_t *get_mp_lv_color_hsv_t_type();

STATIC inline lv_color_hsv_t* mp_write_ptr_lv_color_hsv_t(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_lv_color_hsv_t_type()));
    return (lv_color_hsv_t*)self->data;
}

#define mp_write_lv_color_hsv_t(struct_obj) *mp_write_ptr_lv_color_hsv_t(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_lv_color_hsv_t(lv_color_hsv_t *field)
{
    return lv_to_mp_struct(get_mp_lv_color_hsv_t_type(), (void*)field);
}

#define mp_read_lv_color_hsv_t(field) mp_read_ptr_lv_color_hsv_t(copy_buffer(&field, sizeof(lv_color_hsv_t)))
#define mp_read_byref_lv_color_hsv_t(field) mp_read_ptr_lv_color_hsv_t(&field)

STATIC void mp_lv_color_hsv_t_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    lv_color_hsv_t *data = (lv_color_hsv_t*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_h: dest[0] = mp_obj_new_int_from_uint(data->h); break; // converting from uint16_t;
            case MP_QSTR_s: dest[0] = mp_obj_new_int_from_uint(data->s); break; // converting from uint8_t;
            case MP_QSTR_v: dest[0] = mp_obj_new_int_from_uint(data->v); break; // converting from uint8_t;
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_h: data->h = (uint16_t)mp_obj_get_int(dest[1]); break; // converting to uint16_t;
                case MP_QSTR_s: data->s = (uint8_t)mp_obj_get_int(dest[1]); break; // converting to uint8_t;
                case MP_QSTR_v: data->v = (uint8_t)mp_obj_get_int(dest[1]); break; // converting to uint8_t;
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_lv_color_hsv_t_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct lv_color_hsv_t");
}

STATIC const mp_obj_dict_t mp_lv_color_hsv_t_locals_dict;

STATIC const mp_obj_type_t mp_lv_color_hsv_t_type = {
    { &mp_type_type },
    .name = MP_QSTR_lv_color_hsv_t,
    .print = mp_lv_color_hsv_t_print,
    .make_new = make_new_lv_struct,
    .attr = mp_lv_color_hsv_t_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_lv_color_hsv_t_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer }
};

STATIC inline const mp_obj_type_t *get_mp_lv_color_hsv_t_type()
{
    return &mp_lv_color_hsv_t_type;
}
    

/*
 * st7789_lvgl extension definition for:
 * lv_color_hsv_t lv_color_to_hsv(lv_color_t color)
 */
 
STATIC mp_obj_t mp_lv_color_to_hsv(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_color_t color = mp_write_lv_color16_t(mp_args[0]);
    lv_color_hsv_t _res = lv_color_to_hsv(color);
    return mp_read_lv_color_hsv_t(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_color_to_hsv_obj, 1, mp_lv_color_to_hsv, lv_color_to_hsv);

 

STATIC const mp_rom_map_elem_t mp_lv_color16_t_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SIZE), MP_ROM_PTR(MP_ROM_INT(sizeof(lv_color16_t))) },
    { MP_ROM_QSTR(MP_QSTR_cast), MP_ROM_PTR(&mp_lv_cast_class_method) },
    { MP_ROM_QSTR(MP_QSTR_cast_instance), MP_ROM_PTR(&mp_lv_cast_instance_obj) },
    { MP_ROM_QSTR(MP_QSTR___dereference__), MP_ROM_PTR(&mp_lv_dereference_obj) },
    { MP_ROM_QSTR(MP_QSTR_color_to1), MP_ROM_PTR(&mp_lv_color_to1_obj) },
    { MP_ROM_QSTR(MP_QSTR_color_to8), MP_ROM_PTR(&mp_lv_color_to8_obj) },
    { MP_ROM_QSTR(MP_QSTR_color_to16), MP_ROM_PTR(&mp_lv_color_to16_obj) },
    { MP_ROM_QSTR(MP_QSTR_color_to32), MP_ROM_PTR(&mp_lv_color_to32_obj) },
    { MP_ROM_QSTR(MP_QSTR_color_mix), MP_ROM_PTR(&mp_lv_color_mix_obj) },
    { MP_ROM_QSTR(MP_QSTR_color_premult), MP_ROM_PTR(&mp_lv_color_premult_obj) },
    { MP_ROM_QSTR(MP_QSTR_color_mix_with_alpha), MP_ROM_PTR(&mp_lv_color_mix_with_alpha_obj) },
    { MP_ROM_QSTR(MP_QSTR_color_brightness), MP_ROM_PTR(&mp_lv_color_brightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_color_fill), MP_ROM_PTR(&mp_lv_color_fill_obj) },
    { MP_ROM_QSTR(MP_QSTR_color_lighten), MP_ROM_PTR(&mp_lv_color_lighten_obj) },
    { MP_ROM_QSTR(MP_QSTR_color_darken), MP_ROM_PTR(&mp_lv_color_darken_obj) },
    { MP_ROM_QSTR(MP_QSTR_color_to_hsv), MP_ROM_PTR(&mp_lv_color_to_hsv_obj) },
    
};

STATIC MP_DEFINE_CONST_DICT(mp_lv_color16_t_locals_dict, mp_lv_color16_t_locals_dict_table);
        
/* Struct lv_color16_ch_t contains: [] */

STATIC const mp_rom_map_elem_t mp_lv_color16_ch_t_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SIZE), MP_ROM_PTR(MP_ROM_INT(sizeof(lv_color16_ch_t))) },
    { MP_ROM_QSTR(MP_QSTR_cast), MP_ROM_PTR(&mp_lv_cast_class_method) },
    { MP_ROM_QSTR(MP_QSTR_cast_instance), MP_ROM_PTR(&mp_lv_cast_instance_obj) },
    { MP_ROM_QSTR(MP_QSTR___dereference__), MP_ROM_PTR(&mp_lv_dereference_obj) },
    
};

STATIC MP_DEFINE_CONST_DICT(mp_lv_color16_ch_t_locals_dict, mp_lv_color16_ch_t_locals_dict_table);
        
/* Struct lv_mem_monitor_t contains: ['lv_mem_monitor'] */

/*
 * st7789_lvgl extension definition for:
 * void lv_mem_monitor(lv_mem_monitor_t *mon_p)
 */
 
STATIC mp_obj_t mp_lv_mem_monitor(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_mem_monitor_t *mon_p = mp_write_ptr_lv_mem_monitor_t(mp_args[0]);
    lv_mem_monitor(mon_p);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_mem_monitor_obj, 1, mp_lv_mem_monitor, lv_mem_monitor);

 

STATIC const mp_rom_map_elem_t mp_lv_mem_monitor_t_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SIZE), MP_ROM_PTR(MP_ROM_INT(sizeof(lv_mem_monitor_t))) },
    { MP_ROM_QSTR(MP_QSTR_cast), MP_ROM_PTR(&mp_lv_cast_class_method) },
    { MP_ROM_QSTR(MP_QSTR_cast_instance), MP_ROM_PTR(&mp_lv_cast_instance_obj) },
    { MP_ROM_QSTR(MP_QSTR___dereference__), MP_ROM_PTR(&mp_lv_dereference_obj) },
    { MP_ROM_QSTR(MP_QSTR_monitor), MP_ROM_PTR(&mp_lv_mem_monitor_obj) },
    
};

STATIC MP_DEFINE_CONST_DICT(mp_lv_mem_monitor_t_locals_dict, mp_lv_mem_monitor_t_locals_dict_table);
        
/* Struct lv_task_t contains: ['lv_task_del', 'lv_task_set_cb', 'lv_task_set_prio', 'lv_task_set_period', 'lv_task_ready', 'lv_task_set_repeat_count', 'lv_task_reset'] */

/*
 * st7789_lvgl extension definition for:
 * void lv_task_del(lv_task_t *task)
 */
 
STATIC mp_obj_t mp_lv_task_del(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_task_t *task = mp_write_ptr_lv_task_t(mp_args[0]);
    lv_task_del(task);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_task_del_obj, 1, mp_lv_task_del, lv_task_del);

 

/*
 * Callback function lv_task_t_task_cb
 * void lv_task_cb_t(struct _lv_task_t *)
 */

STATIC void lv_task_t_task_cb_callback(lv_task_t * arg0)
{
    mp_obj_t mp_args[1];
    mp_args[0] = mp_read_ptr_lv_task_t((void*)arg0);
    mp_obj_t callbacks = get_callback_dict_from_user_data(arg0->user_data);
    mp_call_function_n_kw(mp_obj_dict_get(callbacks, MP_OBJ_NEW_QSTR(MP_QSTR_lv_task_t_task_cb)) , 1, 0, mp_args);
    return;
}


/*
 * st7789_lvgl extension definition for:
 * void lv_task_set_cb(lv_task_t *task, lv_task_cb_t task_cb)
 */
 
STATIC mp_obj_t mp_lv_task_set_cb(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_task_t *task = mp_write_ptr_lv_task_t(mp_args[0]);
    void *task_cb = mp_lv_callback(mp_args[1], &lv_task_t_task_cb_callback, MP_QSTR_lv_task_t_task_cb, &task->user_data);
    lv_task_set_cb(task, task_cb);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_task_set_cb_obj, 2, mp_lv_task_set_cb, lv_task_set_cb);

 

/*
 * st7789_lvgl extension definition for:
 * void lv_task_set_prio(lv_task_t *task, lv_task_prio_t prio)
 */
 
STATIC mp_obj_t mp_lv_task_set_prio(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_task_t *task = mp_write_ptr_lv_task_t(mp_args[0]);
    lv_task_prio_t prio = (uint8_t)mp_obj_get_int(mp_args[1]);
    lv_task_set_prio(task, prio);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_task_set_prio_obj, 2, mp_lv_task_set_prio, lv_task_set_prio);

 

/*
 * st7789_lvgl extension definition for:
 * void lv_task_set_period(lv_task_t *task, uint32_t period)
 */
 
STATIC mp_obj_t mp_lv_task_set_period(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_task_t *task = mp_write_ptr_lv_task_t(mp_args[0]);
    uint32_t period = (uint32_t)mp_obj_get_int(mp_args[1]);
    lv_task_set_period(task, period);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_task_set_period_obj, 2, mp_lv_task_set_period, lv_task_set_period);

 

/*
 * st7789_lvgl extension definition for:
 * void lv_task_ready(lv_task_t *task)
 */
 
STATIC mp_obj_t mp_lv_task_ready(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_task_t *task = mp_write_ptr_lv_task_t(mp_args[0]);
    lv_task_ready(task);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_task_ready_obj, 1, mp_lv_task_ready, lv_task_ready);

 

/*
 * st7789_lvgl extension definition for:
 * void lv_task_set_repeat_count(lv_task_t *task, int32_t repeat_count)
 */
 
STATIC mp_obj_t mp_lv_task_set_repeat_count(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_task_t *task = mp_write_ptr_lv_task_t(mp_args[0]);
    int32_t repeat_count = (int32_t)mp_obj_get_int(mp_args[1]);
    lv_task_set_repeat_count(task, repeat_count);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_task_set_repeat_count_obj, 2, mp_lv_task_set_repeat_count, lv_task_set_repeat_count);

 

/*
 * st7789_lvgl extension definition for:
 * void lv_task_reset(lv_task_t *task)
 */
 
STATIC mp_obj_t mp_lv_task_reset(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_task_t *task = mp_write_ptr_lv_task_t(mp_args[0]);
    lv_task_reset(task);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_task_reset_obj, 1, mp_lv_task_reset, lv_task_reset);

 

STATIC const mp_rom_map_elem_t mp_lv_task_t_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SIZE), MP_ROM_PTR(MP_ROM_INT(sizeof(lv_task_t))) },
    { MP_ROM_QSTR(MP_QSTR_cast), MP_ROM_PTR(&mp_lv_cast_class_method) },
    { MP_ROM_QSTR(MP_QSTR_cast_instance), MP_ROM_PTR(&mp_lv_cast_instance_obj) },
    { MP_ROM_QSTR(MP_QSTR___dereference__), MP_ROM_PTR(&mp_lv_dereference_obj) },
    { MP_ROM_QSTR(MP_QSTR_del), MP_ROM_PTR(&mp_lv_task_del_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_cb), MP_ROM_PTR(&mp_lv_task_set_cb_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_prio), MP_ROM_PTR(&mp_lv_task_set_prio_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_period), MP_ROM_PTR(&mp_lv_task_set_period_obj) },
    { MP_ROM_QSTR(MP_QSTR_ready), MP_ROM_PTR(&mp_lv_task_ready_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_repeat_count), MP_ROM_PTR(&mp_lv_task_set_repeat_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&mp_lv_task_reset_obj) },
    
};

STATIC MP_DEFINE_CONST_DICT(mp_lv_task_t_locals_dict, mp_lv_task_t_locals_dict_table);
        
/* Struct lv_indev_drv_t contains: ['lv_indev_drv_init', 'lv_indev_drv_register'] */

/*
 * st7789_lvgl extension definition for:
 * void lv_indev_drv_init(lv_indev_drv_t *driver)
 */
 
STATIC mp_obj_t mp_lv_indev_drv_init(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_indev_drv_t *driver = mp_write_ptr_lv_indev_drv_t(mp_args[0]);
    lv_indev_drv_init(driver);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_indev_drv_init_obj, 1, mp_lv_indev_drv_init, lv_indev_drv_init);

 

/*
 * st7789_lvgl extension definition for:
 * lv_indev_t *lv_indev_drv_register(lv_indev_drv_t *driver)
 */
 
STATIC mp_obj_t mp_lv_indev_drv_register(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_indev_drv_t *driver = mp_write_ptr_lv_indev_drv_t(mp_args[0]);
    lv_indev_t * _res = lv_indev_drv_register(driver);
    return mp_read_ptr_lv_indev_t((void*)_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_indev_drv_register_obj, 1, mp_lv_indev_drv_register, lv_indev_drv_register);

 

STATIC const mp_rom_map_elem_t mp_lv_indev_drv_t_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SIZE), MP_ROM_PTR(MP_ROM_INT(sizeof(lv_indev_drv_t))) },
    { MP_ROM_QSTR(MP_QSTR_cast), MP_ROM_PTR(&mp_lv_cast_class_method) },
    { MP_ROM_QSTR(MP_QSTR_cast_instance), MP_ROM_PTR(&mp_lv_cast_instance_obj) },
    { MP_ROM_QSTR(MP_QSTR___dereference__), MP_ROM_PTR(&mp_lv_dereference_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&mp_lv_indev_drv_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_register), MP_ROM_PTR(&mp_lv_indev_drv_register_obj) },
    
};

STATIC MP_DEFINE_CONST_DICT(mp_lv_indev_drv_t_locals_dict, mp_lv_indev_drv_t_locals_dict_table);
        
/* Struct lv_disp_t contains: ['lv_disp_drv_update', 'lv_disp_remove', 'lv_disp_set_default', 'lv_disp_get_hor_res', 'lv_disp_get_ver_res', 'lv_disp_get_antialiasing', 'lv_disp_get_dpi', 'lv_disp_get_size_category', 'lv_disp_get_next', 'lv_disp_get_buf', 'lv_disp_get_inv_buf_size', 'lv_disp_is_double_buf', 'lv_disp_is_true_double_buf'] */

/*
 * st7789_lvgl extension definition for:
 * void lv_disp_drv_update(lv_disp_t *disp, lv_disp_drv_t *new_drv)
 */
 
STATIC mp_obj_t mp_lv_disp_drv_update(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_disp_t *disp = mp_write_ptr_lv_disp_t(mp_args[0]);
    lv_disp_drv_t *new_drv = mp_write_ptr_lv_disp_drv_t(mp_args[1]);
    lv_disp_drv_update(disp, new_drv);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_disp_drv_update_obj, 2, mp_lv_disp_drv_update, lv_disp_drv_update);

 

/*
 * st7789_lvgl extension definition for:
 * void lv_disp_remove(lv_disp_t *disp)
 */
 
STATIC mp_obj_t mp_lv_disp_remove(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_disp_t *disp = mp_write_ptr_lv_disp_t(mp_args[0]);
    lv_disp_remove(disp);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_disp_remove_obj, 1, mp_lv_disp_remove, lv_disp_remove);

 

/*
 * st7789_lvgl extension definition for:
 * void lv_disp_set_default(lv_disp_t *disp)
 */
 
STATIC mp_obj_t mp_lv_disp_set_default(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_disp_t *disp = mp_write_ptr_lv_disp_t(mp_args[0]);
    lv_disp_set_default(disp);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_disp_set_default_obj, 1, mp_lv_disp_set_default, lv_disp_set_default);

 

/*
 * st7789_lvgl extension definition for:
 * lv_coord_t lv_disp_get_hor_res(lv_disp_t *disp)
 */
 
STATIC mp_obj_t mp_lv_disp_get_hor_res(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_disp_t *disp = mp_write_ptr_lv_disp_t(mp_args[0]);
    lv_coord_t _res = lv_disp_get_hor_res(disp);
    return mp_obj_new_int(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_disp_get_hor_res_obj, 1, mp_lv_disp_get_hor_res, lv_disp_get_hor_res);

 

/*
 * st7789_lvgl extension definition for:
 * lv_coord_t lv_disp_get_ver_res(lv_disp_t *disp)
 */
 
STATIC mp_obj_t mp_lv_disp_get_ver_res(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_disp_t *disp = mp_write_ptr_lv_disp_t(mp_args[0]);
    lv_coord_t _res = lv_disp_get_ver_res(disp);
    return mp_obj_new_int(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_disp_get_ver_res_obj, 1, mp_lv_disp_get_ver_res, lv_disp_get_ver_res);

 

/*
 * st7789_lvgl extension definition for:
 * bool lv_disp_get_antialiasing(lv_disp_t *disp)
 */
 
STATIC mp_obj_t mp_lv_disp_get_antialiasing(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_disp_t *disp = mp_write_ptr_lv_disp_t(mp_args[0]);
    bool _res = lv_disp_get_antialiasing(disp);
    return convert_to_bool(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_disp_get_antialiasing_obj, 1, mp_lv_disp_get_antialiasing, lv_disp_get_antialiasing);

 

/*
 * st7789_lvgl extension definition for:
 * lv_coord_t lv_disp_get_dpi(lv_disp_t *disp)
 */
 
STATIC mp_obj_t mp_lv_disp_get_dpi(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_disp_t *disp = mp_write_ptr_lv_disp_t(mp_args[0]);
    lv_coord_t _res = lv_disp_get_dpi(disp);
    return mp_obj_new_int(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_disp_get_dpi_obj, 1, mp_lv_disp_get_dpi, lv_disp_get_dpi);

 

/*
 * st7789_lvgl extension definition for:
 * lv_disp_size_t lv_disp_get_size_category(lv_disp_t *disp)
 */
 
STATIC mp_obj_t mp_lv_disp_get_size_category(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_disp_t *disp = mp_write_ptr_lv_disp_t(mp_args[0]);
    lv_disp_size_t _res = lv_disp_get_size_category(disp);
    return mp_obj_new_int(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_disp_get_size_category_obj, 1, mp_lv_disp_get_size_category, lv_disp_get_size_category);

 

/*
 * st7789_lvgl extension definition for:
 * lv_disp_t *lv_disp_get_next(lv_disp_t *disp)
 */
 
STATIC mp_obj_t mp_lv_disp_get_next(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_disp_t *disp = mp_write_ptr_lv_disp_t(mp_args[0]);
    lv_disp_t * _res = lv_disp_get_next(disp);
    return mp_read_ptr_lv_disp_t((void*)_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_disp_get_next_obj, 1, mp_lv_disp_get_next, lv_disp_get_next);

 

/*
 * st7789_lvgl extension definition for:
 * lv_disp_buf_t *lv_disp_get_buf(lv_disp_t *disp)
 */
 
STATIC mp_obj_t mp_lv_disp_get_buf(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_disp_t *disp = mp_write_ptr_lv_disp_t(mp_args[0]);
    lv_disp_buf_t * _res = lv_disp_get_buf(disp);
    return mp_read_ptr_lv_disp_buf_t((void*)_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_disp_get_buf_obj, 1, mp_lv_disp_get_buf, lv_disp_get_buf);

 

/*
 * st7789_lvgl extension definition for:
 * uint16_t lv_disp_get_inv_buf_size(lv_disp_t *disp)
 */
 
STATIC mp_obj_t mp_lv_disp_get_inv_buf_size(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_disp_t *disp = mp_write_ptr_lv_disp_t(mp_args[0]);
    uint16_t _res = lv_disp_get_inv_buf_size(disp);
    return mp_obj_new_int_from_uint(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_disp_get_inv_buf_size_obj, 1, mp_lv_disp_get_inv_buf_size, lv_disp_get_inv_buf_size);

 

/*
 * st7789_lvgl extension definition for:
 * bool lv_disp_is_double_buf(lv_disp_t *disp)
 */
 
STATIC mp_obj_t mp_lv_disp_is_double_buf(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_disp_t *disp = mp_write_ptr_lv_disp_t(mp_args[0]);
    bool _res = lv_disp_is_double_buf(disp);
    return convert_to_bool(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_disp_is_double_buf_obj, 1, mp_lv_disp_is_double_buf, lv_disp_is_double_buf);

 

/*
 * st7789_lvgl extension definition for:
 * bool lv_disp_is_true_double_buf(lv_disp_t *disp)
 */
 
STATIC mp_obj_t mp_lv_disp_is_true_double_buf(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_disp_t *disp = mp_write_ptr_lv_disp_t(mp_args[0]);
    bool _res = lv_disp_is_true_double_buf(disp);
    return convert_to_bool(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_disp_is_true_double_buf_obj, 1, mp_lv_disp_is_true_double_buf, lv_disp_is_true_double_buf);

 

STATIC const mp_rom_map_elem_t mp_lv_disp_t_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SIZE), MP_ROM_PTR(MP_ROM_INT(sizeof(lv_disp_t))) },
    { MP_ROM_QSTR(MP_QSTR_cast), MP_ROM_PTR(&mp_lv_cast_class_method) },
    { MP_ROM_QSTR(MP_QSTR_cast_instance), MP_ROM_PTR(&mp_lv_cast_instance_obj) },
    { MP_ROM_QSTR(MP_QSTR___dereference__), MP_ROM_PTR(&mp_lv_dereference_obj) },
    { MP_ROM_QSTR(MP_QSTR_drv_update), MP_ROM_PTR(&mp_lv_disp_drv_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&mp_lv_disp_remove_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_default), MP_ROM_PTR(&mp_lv_disp_set_default_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_hor_res), MP_ROM_PTR(&mp_lv_disp_get_hor_res_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_ver_res), MP_ROM_PTR(&mp_lv_disp_get_ver_res_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_antialiasing), MP_ROM_PTR(&mp_lv_disp_get_antialiasing_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_dpi), MP_ROM_PTR(&mp_lv_disp_get_dpi_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_size_category), MP_ROM_PTR(&mp_lv_disp_get_size_category_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_next), MP_ROM_PTR(&mp_lv_disp_get_next_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_buf), MP_ROM_PTR(&mp_lv_disp_get_buf_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_inv_buf_size), MP_ROM_PTR(&mp_lv_disp_get_inv_buf_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_double_buf), MP_ROM_PTR(&mp_lv_disp_is_double_buf_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_true_double_buf), MP_ROM_PTR(&mp_lv_disp_is_true_double_buf_obj) },
    
};

STATIC MP_DEFINE_CONST_DICT(mp_lv_disp_t_locals_dict, mp_lv_disp_t_locals_dict_table);
        
/* Struct lv_disp_drv_t contains: ['lv_disp_drv_init', 'lv_disp_drv_register', 'lv_disp_flush_ready', 'lv_disp_flush_is_last'] */

/*
 * st7789_lvgl extension definition for:
 * void lv_disp_drv_init(lv_disp_drv_t *driver)
 */
 
STATIC mp_obj_t mp_lv_disp_drv_init(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_disp_drv_t *driver = mp_write_ptr_lv_disp_drv_t(mp_args[0]);
    lv_disp_drv_init(driver);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_disp_drv_init_obj, 1, mp_lv_disp_drv_init, lv_disp_drv_init);

 

/*
 * st7789_lvgl extension definition for:
 * lv_disp_t *lv_disp_drv_register(lv_disp_drv_t *driver)
 */
 
STATIC mp_obj_t mp_lv_disp_drv_register(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_disp_drv_t *driver = mp_write_ptr_lv_disp_drv_t(mp_args[0]);
    lv_disp_t * _res = lv_disp_drv_register(driver);
    return mp_read_ptr_lv_disp_t((void*)_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_disp_drv_register_obj, 1, mp_lv_disp_drv_register, lv_disp_drv_register);

 

/*
 * st7789_lvgl extension definition for:
 * void lv_disp_flush_ready(lv_disp_drv_t *disp_drv)
 */
 
STATIC mp_obj_t mp_lv_disp_flush_ready(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_disp_drv_t *disp_drv = mp_write_ptr_lv_disp_drv_t(mp_args[0]);
    lv_disp_flush_ready(disp_drv);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_disp_flush_ready_obj, 1, mp_lv_disp_flush_ready, lv_disp_flush_ready);

 

/*
 * st7789_lvgl extension definition for:
 * bool lv_disp_flush_is_last(lv_disp_drv_t *disp_drv)
 */
 
STATIC mp_obj_t mp_lv_disp_flush_is_last(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_disp_drv_t *disp_drv = mp_write_ptr_lv_disp_drv_t(mp_args[0]);
    bool _res = lv_disp_flush_is_last(disp_drv);
    return convert_to_bool(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_disp_flush_is_last_obj, 1, mp_lv_disp_flush_is_last, lv_disp_flush_is_last);

 

STATIC const mp_rom_map_elem_t mp_lv_disp_drv_t_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SIZE), MP_ROM_PTR(MP_ROM_INT(sizeof(lv_disp_drv_t))) },
    { MP_ROM_QSTR(MP_QSTR_cast), MP_ROM_PTR(&mp_lv_cast_class_method) },
    { MP_ROM_QSTR(MP_QSTR_cast_instance), MP_ROM_PTR(&mp_lv_cast_instance_obj) },
    { MP_ROM_QSTR(MP_QSTR___dereference__), MP_ROM_PTR(&mp_lv_dereference_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&mp_lv_disp_drv_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_register), MP_ROM_PTR(&mp_lv_disp_drv_register_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush_ready), MP_ROM_PTR(&mp_lv_disp_flush_ready_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush_is_last), MP_ROM_PTR(&mp_lv_disp_flush_is_last_obj) },
    
};

STATIC MP_DEFINE_CONST_DICT(mp_lv_disp_drv_t_locals_dict, mp_lv_disp_drv_t_locals_dict_table);
        
/* Struct lv_disp_buf_t contains: ['lv_disp_buf_init'] */

/*
 * st7789_lvgl extension definition for:
 * void lv_disp_buf_init(lv_disp_buf_t *disp_buf, void *buf1, void *buf2, uint32_t size_in_px_cnt)
 */
 
STATIC mp_obj_t mp_lv_disp_buf_init(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_disp_buf_t *disp_buf = mp_write_ptr_lv_disp_buf_t(mp_args[0]);
    void *buf1 = mp_to_ptr(mp_args[1]);
    void *buf2 = mp_to_ptr(mp_args[2]);
    uint32_t size_in_px_cnt = (uint32_t)mp_obj_get_int(mp_args[3]);
    lv_disp_buf_init(disp_buf, buf1, buf2, size_in_px_cnt);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_disp_buf_init_obj, 4, mp_lv_disp_buf_init, lv_disp_buf_init);

 

STATIC const mp_rom_map_elem_t mp_lv_disp_buf_t_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SIZE), MP_ROM_PTR(MP_ROM_INT(sizeof(lv_disp_buf_t))) },
    { MP_ROM_QSTR(MP_QSTR_cast), MP_ROM_PTR(&mp_lv_cast_class_method) },
    { MP_ROM_QSTR(MP_QSTR_cast_instance), MP_ROM_PTR(&mp_lv_cast_instance_obj) },
    { MP_ROM_QSTR(MP_QSTR___dereference__), MP_ROM_PTR(&mp_lv_dereference_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&mp_lv_disp_buf_init_obj) },
    
};

STATIC MP_DEFINE_CONST_DICT(mp_lv_disp_buf_t_locals_dict, mp_lv_disp_buf_t_locals_dict_table);
        
/* Struct lv_ll_t contains: [] */

STATIC const mp_rom_map_elem_t mp_lv_ll_t_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SIZE), MP_ROM_PTR(MP_ROM_INT(sizeof(lv_ll_t))) },
    { MP_ROM_QSTR(MP_QSTR_cast), MP_ROM_PTR(&mp_lv_cast_class_method) },
    { MP_ROM_QSTR(MP_QSTR_cast_instance), MP_ROM_PTR(&mp_lv_cast_instance_obj) },
    { MP_ROM_QSTR(MP_QSTR___dereference__), MP_ROM_PTR(&mp_lv_dereference_obj) },
    
};

STATIC MP_DEFINE_CONST_DICT(mp_lv_ll_t_locals_dict, mp_lv_ll_t_locals_dict_table);
        
/* Struct lv_indev_t contains: ['lv_indev_drv_update', 'lv_indev_get_next'] */

/*
 * st7789_lvgl extension definition for:
 * void lv_indev_drv_update(lv_indev_t *indev, lv_indev_drv_t *new_drv)
 */
 
STATIC mp_obj_t mp_lv_indev_drv_update(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_indev_t *indev = mp_write_ptr_lv_indev_t(mp_args[0]);
    lv_indev_drv_t *new_drv = mp_write_ptr_lv_indev_drv_t(mp_args[1]);
    lv_indev_drv_update(indev, new_drv);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_indev_drv_update_obj, 2, mp_lv_indev_drv_update, lv_indev_drv_update);

 

/*
 * st7789_lvgl extension definition for:
 * lv_indev_t *lv_indev_get_next(lv_indev_t *indev)
 */
 
STATIC mp_obj_t mp_lv_indev_get_next(size_t mp_n_args, const mp_obj_t *mp_args)
{
    lv_indev_t *indev = mp_write_ptr_lv_indev_t(mp_args[0]);
    lv_indev_t * _res = lv_indev_get_next(indev);
    return mp_read_ptr_lv_indev_t((void*)_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_indev_get_next_obj, 1, mp_lv_indev_get_next, lv_indev_get_next);

 

STATIC const mp_rom_map_elem_t mp_lv_indev_t_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SIZE), MP_ROM_PTR(MP_ROM_INT(sizeof(lv_indev_t))) },
    { MP_ROM_QSTR(MP_QSTR_cast), MP_ROM_PTR(&mp_lv_cast_class_method) },
    { MP_ROM_QSTR(MP_QSTR_cast_instance), MP_ROM_PTR(&mp_lv_cast_instance_obj) },
    { MP_ROM_QSTR(MP_QSTR___dereference__), MP_ROM_PTR(&mp_lv_dereference_obj) },
    { MP_ROM_QSTR(MP_QSTR_drv_update), MP_ROM_PTR(&mp_lv_indev_drv_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_next), MP_ROM_PTR(&mp_lv_indev_get_next_obj) },
    
};

STATIC MP_DEFINE_CONST_DICT(mp_lv_indev_t_locals_dict, mp_lv_indev_t_locals_dict_table);
        
/* Struct lv_indev_proc_t contains: [] */

STATIC const mp_rom_map_elem_t mp_lv_indev_proc_t_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SIZE), MP_ROM_PTR(MP_ROM_INT(sizeof(lv_indev_proc_t))) },
    { MP_ROM_QSTR(MP_QSTR_cast), MP_ROM_PTR(&mp_lv_cast_class_method) },
    { MP_ROM_QSTR(MP_QSTR_cast_instance), MP_ROM_PTR(&mp_lv_cast_instance_obj) },
    { MP_ROM_QSTR(MP_QSTR___dereference__), MP_ROM_PTR(&mp_lv_dereference_obj) },
    
};

STATIC MP_DEFINE_CONST_DICT(mp_lv_indev_proc_t_locals_dict, mp_lv_indev_proc_t_locals_dict_table);
        
/* Struct lv_indev_proc_types_t contains: [] */

STATIC const mp_rom_map_elem_t mp_lv_indev_proc_types_t_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SIZE), MP_ROM_PTR(MP_ROM_INT(sizeof(lv_indev_proc_types_t))) },
    { MP_ROM_QSTR(MP_QSTR_cast), MP_ROM_PTR(&mp_lv_cast_class_method) },
    { MP_ROM_QSTR(MP_QSTR_cast_instance), MP_ROM_PTR(&mp_lv_cast_instance_obj) },
    { MP_ROM_QSTR(MP_QSTR___dereference__), MP_ROM_PTR(&mp_lv_dereference_obj) },
    
};

STATIC MP_DEFINE_CONST_DICT(mp_lv_indev_proc_types_t_locals_dict, mp_lv_indev_proc_types_t_locals_dict_table);
        
/* Struct lv_indev_proc_types_pointer_t contains: [] */

STATIC const mp_rom_map_elem_t mp_lv_indev_proc_types_pointer_t_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SIZE), MP_ROM_PTR(MP_ROM_INT(sizeof(lv_indev_proc_types_pointer_t))) },
    { MP_ROM_QSTR(MP_QSTR_cast), MP_ROM_PTR(&mp_lv_cast_class_method) },
    { MP_ROM_QSTR(MP_QSTR_cast_instance), MP_ROM_PTR(&mp_lv_cast_instance_obj) },
    { MP_ROM_QSTR(MP_QSTR___dereference__), MP_ROM_PTR(&mp_lv_dereference_obj) },
    
};

STATIC MP_DEFINE_CONST_DICT(mp_lv_indev_proc_types_pointer_t_locals_dict, mp_lv_indev_proc_types_pointer_t_locals_dict_table);
        
/* Struct lv_point_t contains: [] */

STATIC const mp_rom_map_elem_t mp_lv_point_t_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SIZE), MP_ROM_PTR(MP_ROM_INT(sizeof(lv_point_t))) },
    { MP_ROM_QSTR(MP_QSTR_cast), MP_ROM_PTR(&mp_lv_cast_class_method) },
    { MP_ROM_QSTR(MP_QSTR_cast_instance), MP_ROM_PTR(&mp_lv_cast_instance_obj) },
    { MP_ROM_QSTR(MP_QSTR___dereference__), MP_ROM_PTR(&mp_lv_dereference_obj) },
    
};

STATIC MP_DEFINE_CONST_DICT(mp_lv_point_t_locals_dict, mp_lv_point_t_locals_dict_table);
        
/* Struct lv_indev_proc_types_keypad_t contains: [] */

STATIC const mp_rom_map_elem_t mp_lv_indev_proc_types_keypad_t_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SIZE), MP_ROM_PTR(MP_ROM_INT(sizeof(lv_indev_proc_types_keypad_t))) },
    { MP_ROM_QSTR(MP_QSTR_cast), MP_ROM_PTR(&mp_lv_cast_class_method) },
    { MP_ROM_QSTR(MP_QSTR_cast_instance), MP_ROM_PTR(&mp_lv_cast_instance_obj) },
    { MP_ROM_QSTR(MP_QSTR___dereference__), MP_ROM_PTR(&mp_lv_dereference_obj) },
    
};

STATIC MP_DEFINE_CONST_DICT(mp_lv_indev_proc_types_keypad_t_locals_dict, mp_lv_indev_proc_types_keypad_t_locals_dict_table);
        

/* 
 *
 * Global Module Functions
 *
 */


/*
 * st7789_lvgl extension definition for:
 * inline static lv_color_t lv_color_mix_premult(uint16_t *premult_c1, lv_color_t c2, uint8_t mix)
 */
 
STATIC mp_obj_t mp_lv_color_mix_premult(size_t mp_n_args, const mp_obj_t *mp_args)
{
    uint16_t *premult_c1 = mp_to_ptr(mp_args[0]);
    lv_color_t c2 = mp_write_lv_color16_t(mp_args[1]);
    uint8_t mix = (uint8_t)mp_obj_get_int(mp_args[2]);
    lv_color_t _res = lv_color_mix_premult(premult_c1, c2, mix);
    return mp_read_lv_color16_t(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_color_mix_premult_obj, 3, mp_lv_color_mix_premult, lv_color_mix_premult);

 
/* List of structs: ['lv_color_hsv_t'] */
/* Struct lv_color_hsv_t contains: [] */

STATIC const mp_rom_map_elem_t mp_lv_color_hsv_t_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SIZE), MP_ROM_PTR(MP_ROM_INT(sizeof(lv_color_hsv_t))) },
    { MP_ROM_QSTR(MP_QSTR_cast), MP_ROM_PTR(&mp_lv_cast_class_method) },
    { MP_ROM_QSTR(MP_QSTR_cast_instance), MP_ROM_PTR(&mp_lv_cast_instance_obj) },
    { MP_ROM_QSTR(MP_QSTR___dereference__), MP_ROM_PTR(&mp_lv_dereference_obj) },
    
};

STATIC MP_DEFINE_CONST_DICT(mp_lv_color_hsv_t_locals_dict, mp_lv_color_hsv_t_locals_dict_table);
        

/*
 * st7789_lvgl extension definition for:
 * inline static lv_color_t lv_color_make(uint8_t r, uint8_t g, uint8_t b)
 */
 
STATIC mp_obj_t mp_lv_color_make(size_t mp_n_args, const mp_obj_t *mp_args)
{
    uint8_t r = (uint8_t)mp_obj_get_int(mp_args[0]);
    uint8_t g = (uint8_t)mp_obj_get_int(mp_args[1]);
    uint8_t b = (uint8_t)mp_obj_get_int(mp_args[2]);
    lv_color_t _res = lv_color_make(r, g, b);
    return mp_read_lv_color16_t(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_color_make_obj, 3, mp_lv_color_make, lv_color_make);

 

/*
 * st7789_lvgl extension definition for:
 * inline static lv_color_t lv_color_hex(uint32_t c)
 */
 
STATIC mp_obj_t mp_lv_color_hex(size_t mp_n_args, const mp_obj_t *mp_args)
{
    uint32_t c = (uint32_t)mp_obj_get_int(mp_args[0]);
    lv_color_t _res = lv_color_hex(c);
    return mp_read_lv_color16_t(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_color_hex_obj, 1, mp_lv_color_hex, lv_color_hex);

 

/*
 * st7789_lvgl extension definition for:
 * inline static lv_color_t lv_color_hex3(uint32_t c)
 */
 
STATIC mp_obj_t mp_lv_color_hex3(size_t mp_n_args, const mp_obj_t *mp_args)
{
    uint32_t c = (uint32_t)mp_obj_get_int(mp_args[0]);
    lv_color_t _res = lv_color_hex3(c);
    return mp_read_lv_color16_t(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_color_hex3_obj, 1, mp_lv_color_hex3, lv_color_hex3);

 

/*
 * Callback function lv_log_print_g_cb_t_print_cb
 * void lv_log_print_g_cb_t(lv_log_level_t level, const char *, uint32_t, const char *, const char *)
 */

STATIC void lv_log_print_g_cb_t_print_cb_callback(lv_log_level_t arg0, const char * arg1, uint32_t arg2, const char * arg3, const char * arg4)
{
    mp_obj_t mp_args[5];
    mp_args[0] = mp_obj_new_int(arg0);
    mp_args[1] = convert_to_str((void*)arg1);
    mp_args[2] = mp_obj_new_int_from_uint(arg2);
    mp_args[3] = convert_to_str((void*)arg3);
    mp_args[4] = convert_to_str((void*)arg4);
    mp_obj_t callbacks = get_callback_dict_from_user_data(MP_STATE_PORT(mp_lv_user_data));
    mp_call_function_n_kw(mp_obj_dict_get(callbacks, MP_OBJ_NEW_QSTR(MP_QSTR_lv_log_print_g_cb_t_print_cb)) , 5, 0, mp_args);
    return;
}


/*
 * st7789_lvgl extension definition for:
 * void lv_log_register_print_cb(lv_log_print_g_cb_t print_cb)
 */
 
STATIC mp_obj_t mp_lv_log_register_print_cb(size_t mp_n_args, const mp_obj_t *mp_args)
{
    void *print_cb = mp_lv_callback(mp_args[0], &lv_log_print_g_cb_t_print_cb_callback, MP_QSTR_lv_log_print_g_cb_t_print_cb, &MP_STATE_PORT(mp_lv_user_data));
    lv_log_register_print_cb(print_cb);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_log_register_print_cb_obj, 1, mp_lv_log_register_print_cb, lv_log_register_print_cb);

 

/*
 * st7789_lvgl extension definition for:
 * void *lv_mem_alloc(size_t size)
 */
 
STATIC mp_obj_t mp_lv_mem_alloc(size_t mp_n_args, const mp_obj_t *mp_args)
{
    size_t size = (size_t)mp_obj_get_int(mp_args[0]);
    void * _res = lv_mem_alloc(size);
    return ptr_to_mp((void*)_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_mem_alloc_obj, 1, mp_lv_mem_alloc, lv_mem_alloc);

 

/*
 * st7789_lvgl extension definition for:
 * void lv_mem_free(const void *data)
 */
 
STATIC mp_obj_t mp_lv_mem_free(size_t mp_n_args, const mp_obj_t *mp_args)
{
    const void *data = mp_to_ptr(mp_args[0]);
    lv_mem_free(data);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_mem_free_obj, 1, mp_lv_mem_free, lv_mem_free);

 

/*
 * st7789_lvgl extension definition for:
 * void *lv_mem_realloc(void *data_p, size_t new_size)
 */
 
STATIC mp_obj_t mp_lv_mem_realloc(size_t mp_n_args, const mp_obj_t *mp_args)
{
    void *data_p = mp_to_ptr(mp_args[0]);
    size_t new_size = (size_t)mp_obj_get_int(mp_args[1]);
    void * _res = lv_mem_realloc(data_p, new_size);
    return ptr_to_mp((void*)_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_mem_realloc_obj, 2, mp_lv_mem_realloc, lv_mem_realloc);

 

/*
 * st7789_lvgl extension definition for:
 * void lv_mem_defrag(void)
 */
 
STATIC mp_obj_t mp_lv_mem_defrag(size_t mp_n_args, const mp_obj_t *mp_args)
{
    
    lv_mem_defrag();
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_mem_defrag_obj, 0, mp_lv_mem_defrag, lv_mem_defrag);

 

/*
 * st7789_lvgl extension definition for:
 * lv_res_t lv_mem_test(void)
 */
 
STATIC mp_obj_t mp_lv_mem_test(size_t mp_n_args, const mp_obj_t *mp_args)
{
    
    lv_res_t _res = lv_mem_test();
    return mp_obj_new_int_from_uint(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_mem_test_obj, 0, mp_lv_mem_test, lv_mem_test);

 

/*
 * st7789_lvgl extension definition for:
 * uint32_t lv_task_handler(void)
 */
 
STATIC mp_obj_t mp_lv_task_handler(size_t mp_n_args, const mp_obj_t *mp_args)
{
    
    uint32_t _res = lv_task_handler();
    return mp_obj_new_int_from_uint(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_task_handler_obj, 0, mp_lv_task_handler, lv_task_handler);

 

/*
 * st7789_lvgl extension definition for:
 * lv_task_t *lv_task_create_basic(void)
 */
 
STATIC mp_obj_t mp_lv_task_create_basic(size_t mp_n_args, const mp_obj_t *mp_args)
{
    
    lv_task_t * _res = lv_task_create_basic();
    return mp_read_ptr_lv_task_t((void*)_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_task_create_basic_obj, 0, mp_lv_task_create_basic, lv_task_create_basic);

 

/*
 * Callback function task_xcb
 * void lv_task_cb_t(struct _lv_task_t *)
 */

STATIC void task_xcb_callback(lv_task_t * arg0)
{
    mp_obj_t mp_args[1];
    mp_args[0] = mp_read_ptr_lv_task_t((void*)arg0);
    mp_obj_t callbacks = get_callback_dict_from_user_data(arg0->user_data);
    mp_call_function_n_kw(mp_obj_dict_get(callbacks, MP_OBJ_NEW_QSTR(MP_QSTR_task_xcb)) , 1, 0, mp_args);
    return;
}


/*
 * st7789_lvgl extension definition for:
 * lv_task_t *lv_task_create(lv_task_cb_t task_xcb, uint32_t period, lv_task_prio_t prio, void *user_data)
 */
 
STATIC mp_obj_t mp_lv_task_create(size_t mp_n_args, const mp_obj_t *mp_args)
{
    void *user_data = mp_to_ptr(mp_args[3]);
    uint32_t period = (uint32_t)mp_obj_get_int(mp_args[1]);
    lv_task_prio_t prio = (uint8_t)mp_obj_get_int(mp_args[2]);
    void *task_xcb = mp_lv_callback(mp_args[0], &task_xcb_callback, MP_QSTR_task_xcb, &user_data);
    lv_task_t * _res = lv_task_create(task_xcb, period, prio, user_data);
    return mp_read_ptr_lv_task_t((void*)_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_task_create_obj, 4, mp_lv_task_create, lv_task_create);

 

/*
 * st7789_lvgl extension definition for:
 * void lv_task_enable(bool en)
 */
 
STATIC mp_obj_t mp_lv_task_enable(size_t mp_n_args, const mp_obj_t *mp_args)
{
    bool en = mp_obj_is_true(mp_args[0]);
    lv_task_enable(en);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_task_enable_obj, 1, mp_lv_task_enable, lv_task_enable);

 

/*
 * st7789_lvgl extension definition for:
 * uint8_t lv_task_get_idle(void)
 */
 
STATIC mp_obj_t mp_lv_task_get_idle(size_t mp_n_args, const mp_obj_t *mp_args)
{
    
    uint8_t _res = lv_task_get_idle();
    return mp_obj_new_int_from_uint(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_task_get_idle_obj, 0, mp_lv_task_get_idle, lv_task_get_idle);

 

/*
 * st7789_lvgl extension definition for:
 * void lv_tick_inc(uint32_t tick_period)
 */
 
STATIC mp_obj_t mp_lv_tick_inc(size_t mp_n_args, const mp_obj_t *mp_args)
{
    uint32_t tick_period = (uint32_t)mp_obj_get_int(mp_args[0]);
    lv_tick_inc(tick_period);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_tick_inc_obj, 1, mp_lv_tick_inc, lv_tick_inc);

 

/*
 * st7789_lvgl extension definition for:
 * uint32_t lv_tick_get(void)
 */
 
STATIC mp_obj_t mp_lv_tick_get(size_t mp_n_args, const mp_obj_t *mp_args)
{
    
    uint32_t _res = lv_tick_get();
    return mp_obj_new_int_from_uint(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_tick_get_obj, 0, mp_lv_tick_get, lv_tick_get);

 

/*
 * st7789_lvgl extension definition for:
 * uint32_t lv_tick_elaps(uint32_t prev_tick)
 */
 
STATIC mp_obj_t mp_lv_tick_elaps(size_t mp_n_args, const mp_obj_t *mp_args)
{
    uint32_t prev_tick = (uint32_t)mp_obj_get_int(mp_args[0]);
    uint32_t _res = lv_tick_elaps(prev_tick);
    return mp_obj_new_int_from_uint(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_tick_elaps_obj, 1, mp_lv_tick_elaps, lv_tick_elaps);

 

/*
 * st7789_lvgl extension definition for:
 * lv_color_t lv_color_hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v)
 */
 
STATIC mp_obj_t mp_lv_color_hsv_to_rgb(size_t mp_n_args, const mp_obj_t *mp_args)
{
    uint16_t h = (uint16_t)mp_obj_get_int(mp_args[0]);
    uint8_t s = (uint8_t)mp_obj_get_int(mp_args[1]);
    uint8_t v = (uint8_t)mp_obj_get_int(mp_args[2]);
    lv_color_t _res = lv_color_hsv_to_rgb(h, s, v);
    return mp_read_lv_color16_t(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_color_hsv_to_rgb_obj, 3, mp_lv_color_hsv_to_rgb, lv_color_hsv_to_rgb);

 

/*
 * st7789_lvgl extension definition for:
 * lv_color_hsv_t lv_color_rgb_to_hsv(uint8_t r8, uint8_t g8, uint8_t b8)
 */
 
STATIC mp_obj_t mp_lv_color_rgb_to_hsv(size_t mp_n_args, const mp_obj_t *mp_args)
{
    uint8_t r8 = (uint8_t)mp_obj_get_int(mp_args[0]);
    uint8_t g8 = (uint8_t)mp_obj_get_int(mp_args[1]);
    uint8_t b8 = (uint8_t)mp_obj_get_int(mp_args[2]);
    lv_color_hsv_t _res = lv_color_rgb_to_hsv(r8, g8, b8);
    return mp_read_lv_color_hsv_t(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_color_rgb_to_hsv_obj, 3, mp_lv_color_rgb_to_hsv, lv_color_rgb_to_hsv);

 

/*
 * st7789_lvgl extension definition for:
 * lv_disp_t *lv_disp_get_default(void)
 */
 
STATIC mp_obj_t mp_lv_disp_get_default(size_t mp_n_args, const mp_obj_t *mp_args)
{
    
    lv_disp_t * _res = lv_disp_get_default();
    return mp_read_ptr_lv_disp_t((void*)_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lv_disp_get_default_obj, 0, mp_lv_disp_get_default, lv_disp_get_default);

 

/*
 * st7789_lvgl extension definition for:
 * void st7789_init(void)
 */
 
STATIC mp_obj_t mp_st7789_init(size_t mp_n_args, const mp_obj_t *mp_args)
{
    
    st7789_init();
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_st7789_init_obj, 0, mp_st7789_init, st7789_init);

 

/*
 * st7789_lvgl extension definition for:
 * void st7789_flush(struct _disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
 */
 
STATIC mp_obj_t mp_st7789_flush(size_t mp_n_args, const mp_obj_t *mp_args)
{
    struct _disp_drv_t *drv = mp_write_ptr_lv_disp_drv_t(mp_args[0]);
    const lv_area_t *area = mp_write_ptr_lv_area_t(mp_args[1]);
    lv_color_t *color_map = mp_write_ptr_lv_color16_t(mp_args[2]);
    st7789_flush(drv, area, color_map);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_st7789_flush_obj, 3, mp_st7789_flush, st7789_flush);

 

/*
 * st7789_lvgl extension definition for:
 * void st7789_enable_backlight(bool backlight)
 */
 
STATIC mp_obj_t mp_st7789_enable_backlight(size_t mp_n_args, const mp_obj_t *mp_args)
{
    bool backlight = mp_obj_is_true(mp_args[0]);
    st7789_enable_backlight(backlight);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_st7789_enable_backlight_obj, 1, mp_st7789_enable_backlight, st7789_enable_backlight);

 

/*
 * st7789_lvgl extension definition for:
 * void st7789_set_orientation(uint8_t orientation)
 */
 
STATIC mp_obj_t mp_st7789_set_orientation(size_t mp_n_args, const mp_obj_t *mp_args)
{
    uint8_t orientation = (uint8_t)mp_obj_get_int(mp_args[0]);
    st7789_set_orientation(orientation);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_st7789_set_orientation_obj, 1, mp_st7789_set_orientation, st7789_set_orientation);

 

/*
 * st7789_lvgl extension definition for:
 * void st7789_send_cmd(uint8_t cmd)
 */
 
STATIC mp_obj_t mp_st7789_send_cmd(size_t mp_n_args, const mp_obj_t *mp_args)
{
    uint8_t cmd = (uint8_t)mp_obj_get_int(mp_args[0]);
    st7789_send_cmd(cmd);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_st7789_send_cmd_obj, 1, mp_st7789_send_cmd, st7789_send_cmd);

 

/*
 * st7789_lvgl extension definition for:
 * void disp_driver_init(void)
 */
 
STATIC mp_obj_t mp_disp_driver_init(size_t mp_n_args, const mp_obj_t *mp_args)
{
    
    disp_driver_init();
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_disp_driver_init_obj, 0, mp_disp_driver_init, disp_driver_init);

 

/*
 * st7789_lvgl extension definition for:
 * void disp_driver_flush(struct _disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
 */
 
STATIC mp_obj_t mp_disp_driver_flush(size_t mp_n_args, const mp_obj_t *mp_args)
{
    struct _disp_drv_t *drv = mp_write_ptr_lv_disp_drv_t(mp_args[0]);
    const lv_area_t *area = mp_write_ptr_lv_area_t(mp_args[1]);
    lv_color_t *color_map = mp_write_ptr_lv_color16_t(mp_args[2]);
    disp_driver_flush(drv, area, color_map);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_disp_driver_flush_obj, 3, mp_disp_driver_flush, disp_driver_flush);

 

/*
 * st7789_lvgl extension definition for:
 * void lvgl_driver_init(void)
 */
 
STATIC mp_obj_t mp_lvgl_driver_init(size_t mp_n_args, const mp_obj_t *mp_args)
{
    
    lvgl_driver_init();
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lvgl_driver_init_obj, 0, mp_lvgl_driver_init, lvgl_driver_init);

 

/*
 * st7789_lvgl extension definition for:
 * bool lvgl_spi_driver_init(int host, int miso_pin, int mosi_pin, int sclk_pin, int max_transfer_sz, int dma_channel, int quadwp_pin, int quadhd_pin)
 */
 
STATIC mp_obj_t mp_lvgl_spi_driver_init(size_t mp_n_args, const mp_obj_t *mp_args)
{
    int host = (int)mp_obj_get_int(mp_args[0]);
    int miso_pin = (int)mp_obj_get_int(mp_args[1]);
    int mosi_pin = (int)mp_obj_get_int(mp_args[2]);
    int sclk_pin = (int)mp_obj_get_int(mp_args[3]);
    int max_transfer_sz = (int)mp_obj_get_int(mp_args[4]);
    int dma_channel = (int)mp_obj_get_int(mp_args[5]);
    int quadwp_pin = (int)mp_obj_get_int(mp_args[6]);
    int quadhd_pin = (int)mp_obj_get_int(mp_args[7]);
    bool _res = lvgl_spi_driver_init(host, miso_pin, mosi_pin, sclk_pin, max_transfer_sz, dma_channel, quadwp_pin, quadhd_pin);
    return convert_to_bool(_res);
}

STATIC MP_DEFINE_CONST_LV_FUN_OBJ_VAR(mp_lvgl_spi_driver_init_obj, 8, mp_lvgl_spi_driver_init, lvgl_spi_driver_init);

 

/*
 * Function NOT generated:
 * Missing conversion to lv_mem_buf_arr_t when generating global _lv_mem_buf
 * _lv_mem_buf
 */
    

/*
 * Struct lv_indev_data_t
 */

STATIC inline const mp_obj_type_t *get_mp_lv_indev_data_t_type();

STATIC inline lv_indev_data_t* mp_write_ptr_lv_indev_data_t(mp_obj_t self_in)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(cast(self_in, get_mp_lv_indev_data_t_type()));
    return (lv_indev_data_t*)self->data;
}

#define mp_write_lv_indev_data_t(struct_obj) *mp_write_ptr_lv_indev_data_t(struct_obj)

STATIC inline mp_obj_t mp_read_ptr_lv_indev_data_t(lv_indev_data_t *field)
{
    return lv_to_mp_struct(get_mp_lv_indev_data_t_type(), (void*)field);
}

#define mp_read_lv_indev_data_t(field) mp_read_ptr_lv_indev_data_t(copy_buffer(&field, sizeof(lv_indev_data_t)))
#define mp_read_byref_lv_indev_data_t(field) mp_read_ptr_lv_indev_data_t(&field)

STATIC void mp_lv_indev_data_t_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest)
{
    mp_lv_struct_t *self = MP_OBJ_TO_PTR(self_in);
    lv_indev_data_t *data = (lv_indev_data_t*)self->data;

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        switch(attr)
        {
            case MP_QSTR_point: dest[0] = mp_read_byref_lv_point_t(data->point); break; // converting from lv_point_t;
            case MP_QSTR_key: dest[0] = mp_obj_new_int_from_uint(data->key); break; // converting from uint32_t;
            case MP_QSTR_btn_id: dest[0] = mp_obj_new_int_from_uint(data->btn_id); break; // converting from uint32_t;
            case MP_QSTR_enc_diff: dest[0] = mp_obj_new_int(data->enc_diff); break; // converting from int16_t;
            case MP_QSTR_state: dest[0] = mp_obj_new_int_from_uint(data->state); break; // converting from lv_indev_state_t;
            default: call_parent_methods(self_in, attr, dest); // fallback to locals_dict lookup
        }
    } else {
        if (dest[1])
        {
            // store attribute
            switch(attr)
            {
                case MP_QSTR_point: data->point = mp_write_lv_point_t(dest[1]); break; // converting to lv_point_t;
                case MP_QSTR_key: data->key = (uint32_t)mp_obj_get_int(dest[1]); break; // converting to uint32_t;
                case MP_QSTR_btn_id: data->btn_id = (uint32_t)mp_obj_get_int(dest[1]); break; // converting to uint32_t;
                case MP_QSTR_enc_diff: data->enc_diff = (int16_t)mp_obj_get_int(dest[1]); break; // converting to int16_t;
                case MP_QSTR_state: data->state = (uint8_t)mp_obj_get_int(dest[1]); break; // converting to lv_indev_state_t;
                default: return;
            }

            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC void mp_lv_indev_data_t_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind)
{
    mp_printf(print, "struct lv_indev_data_t");
}

STATIC const mp_obj_dict_t mp_lv_indev_data_t_locals_dict;

STATIC const mp_obj_type_t mp_lv_indev_data_t_type = {
    { &mp_type_type },
    .name = MP_QSTR_lv_indev_data_t,
    .print = mp_lv_indev_data_t_print,
    .make_new = make_new_lv_struct,
    .attr = mp_lv_indev_data_t_attr,
    .locals_dict = (mp_obj_dict_t*)&mp_lv_indev_data_t_locals_dict,
    .buffer_p = { .get_buffer = mp_blob_get_buffer }
};

STATIC inline const mp_obj_type_t *get_mp_lv_indev_data_t_type()
{
    return &mp_lv_indev_data_t_type;
}
    

/*
 * Callback function lv_indev_drv_t_read_cb
 * bool read_cb(struct _lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
 */

STATIC bool lv_indev_drv_t_read_cb_callback(lv_indev_drv_t * arg0, lv_indev_data_t * arg1)
{
    mp_obj_t mp_args[2];
    mp_args[0] = mp_read_ptr_lv_indev_drv_t((void*)arg0);
    mp_args[1] = mp_read_ptr_lv_indev_data_t((void*)arg1);
    mp_obj_t callbacks = get_callback_dict_from_user_data(arg0->user_data);
    mp_obj_t callback_result = mp_call_function_n_kw(mp_obj_dict_get(callbacks, MP_OBJ_NEW_QSTR(MP_QSTR_lv_indev_drv_t_read_cb)) , 2, 0, mp_args);
    return mp_obj_is_true(callback_result);
}


/*
 * Callback function lv_indev_drv_t_feedback_cb
 * void feedback_cb(struct _lv_indev_drv_t *, uint8_t)
 */

STATIC void lv_indev_drv_t_feedback_cb_callback(lv_indev_drv_t * arg0, uint8_t arg1)
{
    mp_obj_t mp_args[2];
    mp_args[0] = mp_read_ptr_lv_indev_drv_t((void*)arg0);
    mp_args[1] = mp_obj_new_int_from_uint(arg1);
    mp_obj_t callbacks = get_callback_dict_from_user_data(arg0->user_data);
    mp_call_function_n_kw(mp_obj_dict_get(callbacks, MP_OBJ_NEW_QSTR(MP_QSTR_lv_indev_drv_t_feedback_cb)) , 2, 0, mp_args);
    return;
}


/*
 * Callback function lv_disp_drv_t_flush_cb
 * void flush_cb(struct _disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
 */

STATIC void lv_disp_drv_t_flush_cb_callback(lv_disp_drv_t * arg0, const lv_area_t * arg1, lv_color_t * arg2)
{
    mp_obj_t mp_args[3];
    mp_args[0] = mp_read_ptr_lv_disp_drv_t((void*)arg0);
    mp_args[1] = mp_read_ptr_lv_area_t((void*)arg1);
    mp_args[2] = mp_read_ptr_lv_color16_t((void*)arg2);
    mp_obj_t callbacks = get_callback_dict_from_user_data(arg0->user_data);
    mp_call_function_n_kw(mp_obj_dict_get(callbacks, MP_OBJ_NEW_QSTR(MP_QSTR_lv_disp_drv_t_flush_cb)) , 3, 0, mp_args);
    return;
}


/*
 * Callback function lv_disp_drv_t_rounder_cb
 * void rounder_cb(struct _disp_drv_t *disp_drv, lv_area_t *area)
 */

STATIC void lv_disp_drv_t_rounder_cb_callback(lv_disp_drv_t * arg0, lv_area_t * arg1)
{
    mp_obj_t mp_args[2];
    mp_args[0] = mp_read_ptr_lv_disp_drv_t((void*)arg0);
    mp_args[1] = mp_read_ptr_lv_area_t((void*)arg1);
    mp_obj_t callbacks = get_callback_dict_from_user_data(arg0->user_data);
    mp_call_function_n_kw(mp_obj_dict_get(callbacks, MP_OBJ_NEW_QSTR(MP_QSTR_lv_disp_drv_t_rounder_cb)) , 2, 0, mp_args);
    return;
}


/*
 * Callback function lv_disp_drv_t_set_px_cb
 * void set_px_cb(struct _disp_drv_t *disp_drv, uint8_t *buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y, lv_color_t color, lv_opa_t opa)
 */

STATIC void lv_disp_drv_t_set_px_cb_callback(lv_disp_drv_t * arg0, uint8_t * arg1, lv_coord_t arg2, lv_coord_t arg3, lv_coord_t arg4, lv_color_t arg5, lv_opa_t arg6)
{
    mp_obj_t mp_args[7];
    mp_args[0] = mp_read_ptr_lv_disp_drv_t((void*)arg0);
    mp_args[1] = ptr_to_mp((void*)arg1);
    mp_args[2] = mp_obj_new_int(arg2);
    mp_args[3] = mp_obj_new_int(arg3);
    mp_args[4] = mp_obj_new_int(arg4);
    mp_args[5] = mp_read_lv_color16_t(arg5);
    mp_args[6] = mp_obj_new_int_from_uint(arg6);
    mp_obj_t callbacks = get_callback_dict_from_user_data(arg0->user_data);
    mp_call_function_n_kw(mp_obj_dict_get(callbacks, MP_OBJ_NEW_QSTR(MP_QSTR_lv_disp_drv_t_set_px_cb)) , 7, 0, mp_args);
    return;
}


/*
 * Callback function lv_disp_drv_t_monitor_cb
 * void monitor_cb(struct _disp_drv_t *disp_drv, uint32_t time, uint32_t px)
 */

STATIC void lv_disp_drv_t_monitor_cb_callback(lv_disp_drv_t * arg0, uint32_t arg1, uint32_t arg2)
{
    mp_obj_t mp_args[3];
    mp_args[0] = mp_read_ptr_lv_disp_drv_t((void*)arg0);
    mp_args[1] = mp_obj_new_int_from_uint(arg1);
    mp_args[2] = mp_obj_new_int_from_uint(arg2);
    mp_obj_t callbacks = get_callback_dict_from_user_data(arg0->user_data);
    mp_call_function_n_kw(mp_obj_dict_get(callbacks, MP_OBJ_NEW_QSTR(MP_QSTR_lv_disp_drv_t_monitor_cb)) , 3, 0, mp_args);
    return;
}


/*
 * Callback function lv_disp_drv_t_wait_cb
 * void wait_cb(struct _disp_drv_t *disp_drv)
 */

STATIC void lv_disp_drv_t_wait_cb_callback(lv_disp_drv_t * arg0)
{
    mp_obj_t mp_args[1];
    mp_args[0] = mp_read_ptr_lv_disp_drv_t((void*)arg0);
    mp_obj_t callbacks = get_callback_dict_from_user_data(arg0->user_data);
    mp_call_function_n_kw(mp_obj_dict_get(callbacks, MP_OBJ_NEW_QSTR(MP_QSTR_lv_disp_drv_t_wait_cb)) , 1, 0, mp_args);
    return;
}


/*
 * Callback function lv_disp_drv_t_gpu_blend_cb
 * void gpu_blend_cb(struct _disp_drv_t *disp_drv, lv_color_t *dest, const lv_color_t *src, uint32_t length, lv_opa_t opa)
 */

STATIC void lv_disp_drv_t_gpu_blend_cb_callback(lv_disp_drv_t * arg0, lv_color_t * arg1, const lv_color_t * arg2, uint32_t arg3, lv_opa_t arg4)
{
    mp_obj_t mp_args[5];
    mp_args[0] = mp_read_ptr_lv_disp_drv_t((void*)arg0);
    mp_args[1] = mp_read_ptr_lv_color16_t((void*)arg1);
    mp_args[2] = mp_read_ptr_lv_color16_t((void*)arg2);
    mp_args[3] = mp_obj_new_int_from_uint(arg3);
    mp_args[4] = mp_obj_new_int_from_uint(arg4);
    mp_obj_t callbacks = get_callback_dict_from_user_data(arg0->user_data);
    mp_call_function_n_kw(mp_obj_dict_get(callbacks, MP_OBJ_NEW_QSTR(MP_QSTR_lv_disp_drv_t_gpu_blend_cb)) , 5, 0, mp_args);
    return;
}


/*
 * Callback function lv_disp_drv_t_gpu_fill_cb
 * void gpu_fill_cb(struct _disp_drv_t *disp_drv, lv_color_t *dest_buf, lv_coord_t dest_width, const lv_area_t *fill_area, lv_color_t color)
 */

STATIC void lv_disp_drv_t_gpu_fill_cb_callback(lv_disp_drv_t * arg0, lv_color_t * arg1, lv_coord_t arg2, const lv_area_t * arg3, lv_color_t arg4)
{
    mp_obj_t mp_args[5];
    mp_args[0] = mp_read_ptr_lv_disp_drv_t((void*)arg0);
    mp_args[1] = mp_read_ptr_lv_color16_t((void*)arg1);
    mp_args[2] = mp_obj_new_int(arg2);
    mp_args[3] = mp_read_ptr_lv_area_t((void*)arg3);
    mp_args[4] = mp_read_lv_color16_t(arg4);
    mp_obj_t callbacks = get_callback_dict_from_user_data(arg0->user_data);
    mp_call_function_n_kw(mp_obj_dict_get(callbacks, MP_OBJ_NEW_QSTR(MP_QSTR_lv_disp_drv_t_gpu_fill_cb)) , 5, 0, mp_args);
    return;
}



/*
 * st7789_lvgl module definitions
 */

STATIC const mp_rom_map_elem_t st7789_lvgl_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_st7789_lvgl) },
    
    { MP_ROM_QSTR(MP_QSTR_lv_color_mix_premult), MP_ROM_PTR(&mp_lv_color_mix_premult_obj) },
    { MP_ROM_QSTR(MP_QSTR_lv_color_make), MP_ROM_PTR(&mp_lv_color_make_obj) },
    { MP_ROM_QSTR(MP_QSTR_lv_color_hex), MP_ROM_PTR(&mp_lv_color_hex_obj) },
    { MP_ROM_QSTR(MP_QSTR_lv_color_hex3), MP_ROM_PTR(&mp_lv_color_hex3_obj) },
    { MP_ROM_QSTR(MP_QSTR_lv_log_register_print_cb), MP_ROM_PTR(&mp_lv_log_register_print_cb_obj) },
    { MP_ROM_QSTR(MP_QSTR_lv_mem_alloc), MP_ROM_PTR(&mp_lv_mem_alloc_obj) },
    { MP_ROM_QSTR(MP_QSTR_lv_mem_free), MP_ROM_PTR(&mp_lv_mem_free_obj) },
    { MP_ROM_QSTR(MP_QSTR_lv_mem_realloc), MP_ROM_PTR(&mp_lv_mem_realloc_obj) },
    { MP_ROM_QSTR(MP_QSTR_lv_mem_defrag), MP_ROM_PTR(&mp_lv_mem_defrag_obj) },
    { MP_ROM_QSTR(MP_QSTR_lv_mem_test), MP_ROM_PTR(&mp_lv_mem_test_obj) },
    { MP_ROM_QSTR(MP_QSTR_lv_task_handler), MP_ROM_PTR(&mp_lv_task_handler_obj) },
    { MP_ROM_QSTR(MP_QSTR_lv_task_create_basic), MP_ROM_PTR(&mp_lv_task_create_basic_obj) },
    { MP_ROM_QSTR(MP_QSTR_lv_task_create), MP_ROM_PTR(&mp_lv_task_create_obj) },
    { MP_ROM_QSTR(MP_QSTR_lv_task_enable), MP_ROM_PTR(&mp_lv_task_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_lv_task_get_idle), MP_ROM_PTR(&mp_lv_task_get_idle_obj) },
    { MP_ROM_QSTR(MP_QSTR_lv_tick_inc), MP_ROM_PTR(&mp_lv_tick_inc_obj) },
    { MP_ROM_QSTR(MP_QSTR_lv_tick_get), MP_ROM_PTR(&mp_lv_tick_get_obj) },
    { MP_ROM_QSTR(MP_QSTR_lv_tick_elaps), MP_ROM_PTR(&mp_lv_tick_elaps_obj) },
    { MP_ROM_QSTR(MP_QSTR_lv_color_hsv_to_rgb), MP_ROM_PTR(&mp_lv_color_hsv_to_rgb_obj) },
    { MP_ROM_QSTR(MP_QSTR_lv_color_rgb_to_hsv), MP_ROM_PTR(&mp_lv_color_rgb_to_hsv_obj) },
    { MP_ROM_QSTR(MP_QSTR_lv_disp_get_default), MP_ROM_PTR(&mp_lv_disp_get_default_obj) },
    { MP_ROM_QSTR(MP_QSTR_st7789_init), MP_ROM_PTR(&mp_st7789_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_st7789_flush), MP_ROM_PTR(&mp_st7789_flush_obj) },
    { MP_ROM_QSTR(MP_QSTR_st7789_enable_backlight), MP_ROM_PTR(&mp_st7789_enable_backlight_obj) },
    { MP_ROM_QSTR(MP_QSTR_st7789_set_orientation), MP_ROM_PTR(&mp_st7789_set_orientation_obj) },
    { MP_ROM_QSTR(MP_QSTR_st7789_send_cmd), MP_ROM_PTR(&mp_st7789_send_cmd_obj) },
    { MP_ROM_QSTR(MP_QSTR_driver_init), MP_ROM_PTR(&mp_disp_driver_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_driver_flush), MP_ROM_PTR(&mp_disp_driver_flush_obj) },
    { MP_ROM_QSTR(MP_QSTR_lvgl_driver_init), MP_ROM_PTR(&mp_lvgl_driver_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_lvgl_spi_driver_init), MP_ROM_PTR(&mp_lvgl_spi_driver_init_obj) },
    
    { MP_ROM_QSTR(MP_QSTR_LV_LOG_LEVEL), MP_ROM_PTR(&mp_ENUM_LV_LOG_LEVEL_type) },
    { MP_ROM_QSTR(MP_QSTR_LV_RES), MP_ROM_PTR(&mp_LV_RES_type) },
    { MP_ROM_QSTR(MP_QSTR_LV_COORD), MP_ROM_PTR(&mp_ENUM_LV_COORD_type) },
    { MP_ROM_QSTR(MP_QSTR_LV_ALIGN), MP_ROM_PTR(&mp_LV_ALIGN_type) },
    { MP_ROM_QSTR(MP_QSTR_LV_TASK_PRIO), MP_ROM_PTR(&mp_LV_TASK_PRIO_type) },
    { MP_ROM_QSTR(MP_QSTR_LV_INDEV_TYPE), MP_ROM_PTR(&mp_LV_INDEV_TYPE_type) },
    { MP_ROM_QSTR(MP_QSTR_LV_INDEV_STATE), MP_ROM_PTR(&mp_LV_INDEV_STATE_type) },
    { MP_ROM_QSTR(MP_QSTR_LV_DRAG_DIR), MP_ROM_PTR(&mp_LV_DRAG_DIR_type) },
    { MP_ROM_QSTR(MP_QSTR_LV_GESTURE_DIR), MP_ROM_PTR(&mp_LV_GESTURE_DIR_type) },
    { MP_ROM_QSTR(MP_QSTR_LV_OPA), MP_ROM_PTR(&mp_LV_OPA_type) },
    { MP_ROM_QSTR(MP_QSTR_LV_DISP_SIZE), MP_ROM_PTR(&mp_LV_DISP_SIZE_type) },
    
    { MP_ROM_QSTR(MP_QSTR_lv_mem_monitor_t), MP_ROM_PTR(&mp_lv_mem_monitor_t_type) },
    { MP_ROM_QSTR(MP_QSTR_lv_point_t), MP_ROM_PTR(&mp_lv_point_t_type) },
    { MP_ROM_QSTR(MP_QSTR_lv_area_t), MP_ROM_PTR(&mp_lv_area_t_type) },
    { MP_ROM_QSTR(MP_QSTR_lv_ll_t), MP_ROM_PTR(&mp_lv_ll_t_type) },
    { MP_ROM_QSTR(MP_QSTR_lv_task_t), MP_ROM_PTR(&mp_lv_task_t_type) },
    { MP_ROM_QSTR(MP_QSTR_lv_indev_data_t), MP_ROM_PTR(&mp_lv_indev_data_t_type) },
    { MP_ROM_QSTR(MP_QSTR_lv_indev_drv_t), MP_ROM_PTR(&mp_lv_indev_drv_t_type) },
    { MP_ROM_QSTR(MP_QSTR_lv_indev_proc_t), MP_ROM_PTR(&mp_lv_indev_proc_t_type) },
    { MP_ROM_QSTR(MP_QSTR_lv_indev_t), MP_ROM_PTR(&mp_lv_indev_t_type) },
    { MP_ROM_QSTR(MP_QSTR_lv_color16_t), MP_ROM_PTR(&mp_lv_color16_t_type) },
    { MP_ROM_QSTR(MP_QSTR_lv_color_hsv_t), MP_ROM_PTR(&mp_lv_color_hsv_t_type) },
    { MP_ROM_QSTR(MP_QSTR_lv_disp_buf_t), MP_ROM_PTR(&mp_lv_disp_buf_t_type) },
    { MP_ROM_QSTR(MP_QSTR_lv_disp_drv_t), MP_ROM_PTR(&mp_lv_disp_drv_t_type) },
    { MP_ROM_QSTR(MP_QSTR_lv_disp_t), MP_ROM_PTR(&mp_lv_disp_t_type) },
    { MP_ROM_QSTR(MP_QSTR_C_Pointer), MP_ROM_PTR(&mp_C_Pointer_type) },
    
    { MP_ROM_QSTR(MP_QSTR_lv_color_t), MP_ROM_PTR(&mp_lv_color16_t_type) },
    
    
    
};


STATIC MP_DEFINE_CONST_DICT (
    mp_module_st7789_lvgl_globals,
    st7789_lvgl_globals_table
);

const mp_obj_module_t mp_module_st7789_lvgl = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_st7789_lvgl_globals
};
MP_REGISTER_MODULE(MP_QSTR_st7789_lvgl, mp_module_st7789_lvgl, 1);
