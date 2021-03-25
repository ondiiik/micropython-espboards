#!/bin/bash
BUILD=build-TWATCH_2020
TOP=../..
mkdir -p ${BUILD}/espidfmod

ESPIDFMOD_SOURCE=${TOP}/lib/lv_bindings/driver/esp32/espidf.h



ESPIDFMOD_MODULE=${BUILD}/espidfmod/mp_espidf.c
ESPIDFMOD_PP=${BUILD}/espidfmod/mp_espidf.pp.c

echo "Build ${ESPIDFMOD_MODULE} by $(LVGL_BINDING_DIR)/gen/gen_mpy.py"
    $(Q)$(CPP) $(ESPIDFMOD_CFLAGS) -DPYCPARSER -I $(LVGL_BINDING_DIR)/pycparser/utils/fake_libc_include $(INC) $(INC_ESPCOMP) $(ESPIDFMOD_SOURCE) > $(ESPIDFMOD_PP)
    $(Q)$(PYTHON) $(LVGL_BINDING_DIR)/gen/gen_mpy.py -M espidf -E $(ESPIDFMOD_PP) $(ESPIDFMOD_SOURCE) > ${ESPIDFMOD_MODULE}

LIB_SRC_C += \
    lib/lv_bindings/driver/esp32/modlvesp32.c \
    lib/lv_bindings/driver/esp32/modrtch.c \
    lib/lv_bindings/driver/esp32/espidf.c \
    $(ESPIDFMOD_MODULE) \
    lib/lv_bindings/driver/esp32/modxpt2046.c \
    lib/lv_bindings/driver/esp32/modILI9341.c \

DRIVERS_SRC_C = $(addprefix drivers/,\
    bus/softspi.c \
    dht/dht.c \
    )

OBJ_MP =
OBJ_MP += $(PY_O)
OBJ_MP += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ_MP += $(addprefix $(BUILD)/, $(EXTMOD_SRC_C:.c=.o))
OBJ_MP += $(addprefix $(BUILD)/, $(LIB_SRC_C:.c=.o))
OBJ_MP += $(addprefix $(BUILD)/, $(DRIVERS_SRC_C:.c=.o))

# Only enable this for the MicroPython source: ignore warnings from esp-idf.
$(OBJ_MP): CFLAGS += -Wdouble-promotion -Wfloat-conversion
