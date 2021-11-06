/*
 * mp_namespace.h
 *
 *  Created on: Jun 14, 2020
 *      Author: ondiiik
 */
#pragma once



#define MP_NAMESPACE2(_parent_, _member_)              mpy__##_parent_##__##_member_
#define MP_NAMESPACE3(_parent_, _member_, _submember_) mpy__##_parent_##__##_member_##__##_submember_


#define MP_MODULE_BEGIN(_name_) \
    const mp_rom_map_elem_t mpy__##_name_##_members_table[] = \
    { \
        { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_##_name_) },
        
        
#define MP_MODULE_END(_name_) \
    }; \
    \
    STATIC MP_DEFINE_CONST_DICT(_name_##_members, mpy__##_name_##_members_table); \
    \
    const mp_obj_module_t mp_module_##_name_ = \
    { \
        .base    = { &mp_type_module }, \
        .globals = (mp_obj_dict_t*)&_name_##_members, \
    }; \


#define MP_CLASS(_parent_,_name_) \
    const mp_obj_type_t mpy__##_parent_##__##_name_ = \
    { \
        { &mp_type_type }, \
        .name     = MP_QSTR_##_name_, \
        .make_new = mpy__##_parent_##__##_name_##____init__, \
        .print    = mpy__##_parent_##__##_name_##____str__, \
        .attr     = mpy__##_parent_##__##_name_##____attr__, \
    };


#define MP_ATTR_PROPERTY(_parent_, _name_, _self_) \
            case MP_QSTR_##_name_: \
                aDestination[0] = mpy__##_parent_##__##_name_(_self_); \
                break;


#define MP_ATTR_METHOD(_parent_, _name_, _self_) \
            case MP_QSTR_##_name_: \
                aDestination[0] = (mp_obj_t)&mpy__##_parent_##__##_name_; \
                aDestination[1] = _self_; \
                break;


#define MP_MEMBER(_parent_, _member_) \
    { MP_ROM_QSTR(MP_QSTR_##_member_), MP_ROM_PTR(&mpy__##_parent_##__##_member_) },


#define MP_FN_0(_parent_, _member_) \
    STATIC mp_obj_t                                                 mpy__##_parent_##__##_member_##_##_F(); \
    STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy__##_parent_##__##_member_, mpy__##_parent_##__##_member_##_##_F); \
    mp_obj_t                                                        mpy__##_parent_##__##_member_##_##_F()


#define MP_FN_1(_parent_, _member_, _a1_) \
    STATIC mp_obj_t                                                 mpy__##_parent_##__##_member_##_##_F(const mp_obj_t _a1_); \
    STATIC MP_DEFINE_CONST_FUN_OBJ_1(mpy__##_parent_##__##_member_, mpy__##_parent_##__##_member_##_##_F); \
    mp_obj_t                                                        mpy__##_parent_##__##_member_##_##_F(const mp_obj_t _a1_)


#define MP_FN_2(_parent_, _member_, _a1_, _a2_) \
    STATIC mp_obj_t                                                 mpy__##_parent_##__##_member_##_##_F(const mp_obj_t _a1_, const mp_obj_t _a2_); \
    STATIC MP_DEFINE_CONST_FUN_OBJ_2(mpy__##_parent_##__##_member_, mpy__##_parent_##__##_member_##_##_F); \
    mp_obj_t                                                        mpy__##_parent_##__##_member_##_##_F(const mp_obj_t _a1_, const mp_obj_t _a2_)
