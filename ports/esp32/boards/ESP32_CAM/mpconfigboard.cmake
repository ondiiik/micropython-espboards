set(SDKCONFIG_DEFAULTS
    boards/sdkconfig.base4m
    boards/sdkconfig.ble
    boards/sdkconfig.spiram
)

if(NOT MICROPY_FROZEN_MANIFEST)
    set(MICROPY_FROZEN_MANIFEST ${MICROPY_PORT_DIR}/boards/manifest.py)
endif()
