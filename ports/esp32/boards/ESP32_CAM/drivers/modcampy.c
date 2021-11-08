#include "modcampy.h"

#include "esp_camera.h"
#include "esp_log.h"

#include "py/binary.h"
#include "py/mpprint.h"
#include "py/nlr.h"
#include "py/objstr.h"
#include "py/runtime.h"

#include "mp.h"
#include <stdarg.h>
#include <string.h>


/**
 * @brief   Active camera singleton pointer
 */
static struct campy_Camera* _activeCamera;


#define TAG "camera"

//WROVER-KIT PIN Map
#define CAM_PIN_PWDN    32 //power down is not used
#define CAM_PIN_RESET   -1 //software reset will be performed
#define CAM_PIN_XCLK     0
#define CAM_PIN_SIOD    26 // SDA
#define CAM_PIN_SIOC    27 // SCL

#define CAM_PIN_D7      35
#define CAM_PIN_D6      34
#define CAM_PIN_D5      39
#define CAM_PIN_D4      36
#define CAM_PIN_D3      21
#define CAM_PIN_D2      19
#define CAM_PIN_D1      18
#define CAM_PIN_D0       5
#define CAM_PIN_VSYNC   25
#define CAM_PIN_HREF    23
#define CAM_PIN_PCLK    22



static camera_config_t camera_config =
{
    .pin_pwdn     = CAM_PIN_PWDN,
    .pin_reset    = CAM_PIN_RESET,
    .pin_xclk     = CAM_PIN_XCLK,
    .pin_sscb_sda = CAM_PIN_SIOD,
    .pin_sscb_scl = CAM_PIN_SIOC,
    
    .pin_d7       = CAM_PIN_D7,
    .pin_d6       = CAM_PIN_D6,
    .pin_d5       = CAM_PIN_D5,
    .pin_d4       = CAM_PIN_D4,
    .pin_d3       = CAM_PIN_D3,
    .pin_d2       = CAM_PIN_D2,
    .pin_d1       = CAM_PIN_D1,
    .pin_d0       = CAM_PIN_D0,
    .pin_vsync    = CAM_PIN_VSYNC,
    .pin_href     = CAM_PIN_HREF,
    .pin_pclk     = CAM_PIN_PCLK,
    
    //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
    .xclk_freq_hz = 20000000,
    .ledc_timer   = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,
    
    .pixel_format = PIXFORMAT_JPEG,//YUV422,GRAYSCALE,RGB565,JPEG
    .frame_size   = FRAMESIZE_UXGA,//QQVGA-UXGA Do not use sizes above QVGA when not JPEG
    
    .jpeg_quality = 12,  //0-63 lower number means higher quality
};



#include "esp_system.h"
#include "esp_spi_flash.h"




/*
 *******************************************************************************
 * Helpers
 *******************************************************************************
 */
#define ENUM2QSTR_CASE(_prefix_, _key_) case _prefix_##_##_key_ : return MP_ROM_QSTR(MP_QSTR_##_key_)
#define QSTR2ENUM_CASE(_prefix_, _key_) case MP_QSTR_##_key_    : val = _prefix_##_##_key_; break


STATIC mp_obj_t _framesize2qstr(framesize_t aFrameSize)
{
    switch(aFrameSize)
    {
        ENUM2QSTR_CASE( FRAMESIZE, 96X96   );
        ENUM2QSTR_CASE( FRAMESIZE, QQVGA   );
        ENUM2QSTR_CASE( FRAMESIZE, QCIF    );
        ENUM2QSTR_CASE( FRAMESIZE, HQVGA   );
        ENUM2QSTR_CASE( FRAMESIZE, 240X240 );
        ENUM2QSTR_CASE( FRAMESIZE, QVGA    );
        ENUM2QSTR_CASE( FRAMESIZE, CIF     );
        ENUM2QSTR_CASE( FRAMESIZE, HVGA    );
        ENUM2QSTR_CASE( FRAMESIZE, VGA     );
        ENUM2QSTR_CASE( FRAMESIZE, SVGA    );
        ENUM2QSTR_CASE( FRAMESIZE, XGA     );
        ENUM2QSTR_CASE( FRAMESIZE, HD      );
        ENUM2QSTR_CASE( FRAMESIZE, SXGA    );
        ENUM2QSTR_CASE( FRAMESIZE, UXGA    );
        ENUM2QSTR_CASE( FRAMESIZE, FHD     );
        ENUM2QSTR_CASE( FRAMESIZE, P_HD    );
        ENUM2QSTR_CASE( FRAMESIZE, P_3MP   );
        ENUM2QSTR_CASE( FRAMESIZE, QXGA    );
        ENUM2QSTR_CASE( FRAMESIZE, QHD     );
        ENUM2QSTR_CASE( FRAMESIZE, WQXGA   );
        ENUM2QSTR_CASE( FRAMESIZE, P_FHD   );
        ENUM2QSTR_CASE( FRAMESIZE, QSXGA   );
        default: return MP_ROM_QSTR(MP_QSTR_unknown);
    }
}


STATIC framesize_t _qstr2framesize(mp_obj_t aStr)
{
    qstr        fs  = mp_obj_str_get_qstr(aStr);
    framesize_t val = FRAMESIZE_INVALID;
    
    switch(fs)
    {
        QSTR2ENUM_CASE( FRAMESIZE, 96X96   );
        QSTR2ENUM_CASE( FRAMESIZE, QQVGA   );
        QSTR2ENUM_CASE( FRAMESIZE, QCIF    );
        QSTR2ENUM_CASE( FRAMESIZE, HQVGA   );
        QSTR2ENUM_CASE( FRAMESIZE, 240X240 );
        QSTR2ENUM_CASE( FRAMESIZE, QVGA    );
        QSTR2ENUM_CASE( FRAMESIZE, CIF     );
        QSTR2ENUM_CASE( FRAMESIZE, HVGA    );
        QSTR2ENUM_CASE( FRAMESIZE, VGA     );
        QSTR2ENUM_CASE( FRAMESIZE, SVGA    );
        QSTR2ENUM_CASE( FRAMESIZE, XGA     );
        QSTR2ENUM_CASE( FRAMESIZE, HD      );
        QSTR2ENUM_CASE( FRAMESIZE, SXGA    );
        QSTR2ENUM_CASE( FRAMESIZE, UXGA    );
        QSTR2ENUM_CASE( FRAMESIZE, FHD     );
        QSTR2ENUM_CASE( FRAMESIZE, P_HD    );
        QSTR2ENUM_CASE( FRAMESIZE, P_3MP   );
        QSTR2ENUM_CASE( FRAMESIZE, QXGA    );
        QSTR2ENUM_CASE( FRAMESIZE, QHD     );
        QSTR2ENUM_CASE( FRAMESIZE, WQXGA   );
        QSTR2ENUM_CASE( FRAMESIZE, P_FHD   );
        QSTR2ENUM_CASE( FRAMESIZE, QSXGA   );
        default: mp_raise_ValueError(MP_ERROR_TEXT("Unsupported frame size"));
    }
    
    return val;
}


STATIC mp_obj_t _format2qstr(pixformat_t aFormat)
{
    switch(aFormat)
    {
    ENUM2QSTR_CASE( PIXFORMAT, RGB565    );
    ENUM2QSTR_CASE( PIXFORMAT, YUV422    );
    ENUM2QSTR_CASE( PIXFORMAT, GRAYSCALE );
    ENUM2QSTR_CASE( PIXFORMAT, JPEG      );
    ENUM2QSTR_CASE( PIXFORMAT, RGB888    );
    ENUM2QSTR_CASE( PIXFORMAT, RAW       );
    ENUM2QSTR_CASE( PIXFORMAT, RGB444    );
    ENUM2QSTR_CASE( PIXFORMAT, RGB555    );
    default: return MP_ROM_QSTR(MP_QSTR_unknown);
    }
}


STATIC pixformat_t _qstr2format(mp_obj_t aStr)
{
    qstr        format = mp_obj_str_get_qstr(aStr);
    pixformat_t val    = FRAMESIZE_INVALID;
    
    switch(format)
    {
        QSTR2ENUM_CASE( PIXFORMAT, RGB565    );
        QSTR2ENUM_CASE( PIXFORMAT, YUV422    );
        QSTR2ENUM_CASE( PIXFORMAT, GRAYSCALE );
        QSTR2ENUM_CASE( PIXFORMAT, JPEG      );
        QSTR2ENUM_CASE( PIXFORMAT, RGB888    );
        QSTR2ENUM_CASE( PIXFORMAT, RAW       );
        QSTR2ENUM_CASE( PIXFORMAT, RGB444    );
        QSTR2ENUM_CASE( PIXFORMAT, RGB555    );
        default: mp_raise_ValueError(MP_ERROR_TEXT("Unsupported frame size"));
    }
    
    return val;
}


STATIC int _camclamp(mp_float_t aVal)
{
    int nv = (int)(aVal * 2.0 + 0.5F);
    return (nv > 2) ? 2 : (nv < -2) ? -2 : nv;
}


/*
 *******************************************************************************
 * campy.FrameBuffer class
 *******************************************************************************
 */
STATIC mp_obj_t MP_NAMESPACE3(campy, FrameBuffer, __init__)(const mp_obj_type_t* aType,
                                                            size_t               aArgsCnt,
                                                            size_t               aKw,
                                                            const mp_obj_t*      aArgs)
{
    mp_raise_NotImplementedError(MP_ERROR_TEXT("Object shall be created by <campy.Camera> class only"));
    return mp_const_none;
}



STATIC void MP_NAMESPACE3(campy, FrameBuffer, __str__)(const mp_print_t* aPrint,
                                                       mp_obj_t          aSelf,
                                                       mp_print_kind_t   aKind)
{
    struct campy_FrameBuffer* self = MP_OBJ_TO_PTR(aSelf);
    mp_printf(aPrint,
              "<FrameBuffer %q:%dx%d:%d>",
              MP_OBJ_QSTR_VALUE(_format2qstr(self->format)),
              self->width,
              self->height,
              self->len);
}



STATIC mp_obj_t MP_NAMESPACE3(campy__FrameBuffer, data, __load__)(mp_obj_t aSelf)
{
    struct campy_FrameBuffer* self  = MP_OBJ_TO_PTR(aSelf);
    
    if (NULL == self->buf)
    {
        mp_raise_ValueError(MP_ERROR_TEXT("Frame buffer invalidated (called reducto)"));
    }
    
    mp_obj_str_t* bytes = m_new_obj(mp_obj_str_t);
    bytes->base.type    = &mp_type_bytes;
    bytes->data         = (const byte*)self->buf;
    bytes->len          = self->len;
    bytes->hash         = qstr_compute_hash(bytes->data, bytes->len);
    
    return bytes;
}



STATIC mp_obj_t MP_NAMESPACE3(campy__FrameBuffer, width, __load__)(mp_obj_t  aSelf)
{
    struct campy_FrameBuffer* self = MP_OBJ_TO_PTR(aSelf);
    return MP_OBJ_NEW_SMALL_INT(self->width);
}



STATIC mp_obj_t MP_NAMESPACE3(campy__FrameBuffer, height, __load__)(mp_obj_t  aSelf)
{
    struct campy_FrameBuffer* self = MP_OBJ_TO_PTR(aSelf);
    return MP_OBJ_NEW_SMALL_INT(self->height);
}



STATIC mp_obj_t MP_NAMESPACE3(campy__FrameBuffer, format, __load__)(mp_obj_t  aSelf)
{
    struct campy_FrameBuffer* self = MP_OBJ_TO_PTR(aSelf);
    return _format2qstr(self->format);
}



MP_FN_1(campy__FrameBuffer, reducto, aSelf)
{
    struct campy_FrameBuffer* self  = MP_OBJ_TO_PTR(aSelf);
    
    if (self->len != 0)
    {
        m_free(self->buf);
        self->buf    = NULL;
        self->len    = 0;
        self->width  = 0;
        self->height = 0;
    }
    
    return mp_const_none;
}



STATIC void MP_NAMESPACE3(campy, FrameBuffer, __attr__)(mp_obj_t  aSelf,
                                                        qstr      aAttribute,
                                                        mp_obj_t* aDestination)
{
    MP_LOAD
    {
        MP_ATTR_PROPERTY( campy, FrameBuffer, data    );
        MP_ATTR_METHOD(   campy, FrameBuffer, reducto );
        MP_ATTR_PROPERTY( campy, FrameBuffer, width   );
        MP_ATTR_PROPERTY( campy, FrameBuffer, height  );
        MP_ATTR_PROPERTY( campy, FrameBuffer, format  );
    }
}



MP_CLASS(campy, FrameBuffer)



/*
 *******************************************************************************
 * campy.Camera class
 *******************************************************************************
 */
STATIC mp_obj_t MP_NAMESPACE3(campy, Camera, __init__)(const mp_obj_type_t* aType,
                                                       size_t               aArgsCnt,
                                                       size_t               aKw,
                                                       const mp_obj_t*      aArgs)
{
    if (NULL != _activeCamera)
    {
        esp_camera_deinit();
        _activeCamera = NULL;
    }
    
    if (aArgsCnt >= 1)
    {
        camera_config.frame_size = _qstr2framesize(aArgs[0]);
    }
    
    if (aArgsCnt >= 2)
    {
        camera_config.pixel_format = _qstr2format(aArgs[1]);
    }
    
    struct campy_Camera*   activeCamera = campy_Camera_new(&camera_config);
    campy_Camera_init(     activeCamera);
    
    activeCamera->sensor = esp_camera_sensor_get();
    
    _activeCamera        = activeCamera;
    return MP_OBJ_FROM_PTR(activeCamera);
}


STATIC struct campy_Camera* _get_camera(mp_obj_t aSelf)
{
    struct campy_Camera* self = MP_OBJ_TO_PTR(aSelf);
    
    if (_activeCamera != self)
    {
        MP_RAISE(Exception, "This camera was replaced by another active camera");
    }
    
    return self;
}


STATIC void MP_NAMESPACE3(campy, Camera, __str__)(const mp_print_t* aPrint,
                                                  mp_obj_t          aSelf,
                                                  mp_print_kind_t   aKind)
{
    struct campy_Camera* self = _get_camera(aSelf);
    mp_printf(aPrint, "<Camera %s>", self->model);
}



STATIC mp_obj_t MP_NAMESPACE3(campy__Camera, model, __load__)(mp_obj_t  aSelf)
{
    struct campy_Camera* self = _get_camera(aSelf);
    return mp_obj_new_str(self->model, strlen(self->model));
}



STATIC mp_obj_t MP_NAMESPACE3(campy__Camera, jpeg_quality, __load__)(mp_obj_t aSelf)
{
    struct campy_Camera* self = _get_camera(aSelf);
    return MP_OBJ_NEW_SMALL_INT(self->config.jpeg_quality);
}



STATIC mp_obj_t MP_NAMESPACE3(campy__Camera, jpeg_quality, __store__)(mp_obj_t aSelf,
                                                                      mp_obj_t aWhat)
{
    struct campy_Camera* self = _get_camera(aSelf);
    
    if (PIXFORMAT_JPEG != self->config.pixel_format)
    {
        MP_RAISE(Exception, "quality can be set only for JPEG");
    }
    
    int q = mp_obj_get_float(aWhat) * 60 + 4;
    
    esp_camera_deinit();
    self->config.jpeg_quality = (q < 4) ? 4 : (q > 64) ? 64 : q;
    campy_Camera_init(self);
    
    return mp_const_none;
}


STATIC mp_obj_t MP_NAMESPACE3(campy__Camera, frame_size, __load__)(mp_obj_t  aSelf)
{
    struct campy_Camera* self = _get_camera(aSelf);
    return _framesize2qstr(self->config.frame_size);
}


STATIC void MP_NAMESPACE3(campy__Camera, frame_size, __store__)(mp_obj_t  aSelf,
                                                                mp_obj_t  aWhat)
{
    struct campy_Camera* self = _get_camera(aSelf);
    framesize_t          val  = _qstr2framesize(aWhat);
    
    esp_camera_deinit();
    self->config.frame_size = val;
    campy_Camera_init(self);
}


STATIC mp_obj_t MP_NAMESPACE3(campy__Camera, contrast, __load__)(mp_obj_t  aSelf)
{
    struct campy_Camera* self = _get_camera(aSelf);
    return mp_obj_new_float((mp_float_t)(self->sensor->status.contrast) / 2);
}


STATIC void MP_NAMESPACE3(campy__Camera, contrast, __store__)(mp_obj_t  aSelf,
                                                              mp_obj_t  aWhat)
{
    struct campy_Camera* self = _get_camera(aSelf);
    self->sensor->set_contrast(self->sensor, _camclamp(mp_obj_get_float(aWhat)));
}


STATIC mp_obj_t MP_NAMESPACE3(campy__Camera, brightness, __load__)(mp_obj_t  aSelf)
{
    struct campy_Camera* self = _get_camera(aSelf);
    return mp_obj_new_float((mp_float_t)(self->sensor->status.brightness) / 2);
}


STATIC void MP_NAMESPACE3(campy__Camera, brightness, __store__)(mp_obj_t  aSelf,
                                                                mp_obj_t  aWhat)
{
    struct campy_Camera* self = _get_camera(aSelf);
    self->sensor->set_brightness(self->sensor, _camclamp(mp_obj_get_float(aWhat)));
}


STATIC mp_obj_t MP_NAMESPACE3(campy__Camera, saturation, __load__)(mp_obj_t  aSelf)
{
    struct campy_Camera* self = _get_camera(aSelf);
    return mp_obj_new_float((mp_float_t)(self->sensor->status.saturation) / 2);
}


STATIC void MP_NAMESPACE3(campy__Camera, saturation, __store__)(mp_obj_t  aSelf,
                                                                mp_obj_t  aWhat)
{
    struct campy_Camera* self = _get_camera(aSelf);
    self->sensor->set_saturation(self->sensor, _camclamp(mp_obj_get_float(aWhat)));
}


STATIC mp_obj_t MP_NAMESPACE3(campy__Camera, exposure, __load__)(mp_obj_t  aSelf)
{
    struct campy_Camera* self = _get_camera(aSelf);
    return mp_obj_new_float((mp_float_t)(self->sensor->status.ae_level) / 2);
}


STATIC void MP_NAMESPACE3(campy__Camera, exposure, __store__)(mp_obj_t  aSelf,
                                                              mp_obj_t  aWhat)
{
    struct campy_Camera* self = _get_camera(aSelf);
    self->sensor->set_ae_level(self->sensor, _camclamp(mp_obj_get_float(aWhat)));
}



MP_FN_1(campy__Camera, capture, aSelf)
{
    struct campy_Camera* self = _get_camera(aSelf);
    return (mp_obj_t)esp_camera_fb_get();
}



STATIC void MP_NAMESPACE3(campy, Camera, __attr__)(mp_obj_t  aSelf,
                                                   qstr      aAttribute,
                                                   mp_obj_t* aDestination)
{
    MP_LOAD
    {
        MP_ATTR_METHOD(       campy, Camera, capture      )
        MP_ATTR_PROPERTY(     campy, Camera, model        )
        MP_ATTR_PROPERTY(     campy, Camera, jpeg_quality )
        MP_ATTR_PROPERTY(     campy, Camera, frame_size   )
        MP_ATTR_PROPERTY(     campy, Camera, contrast     )
        MP_ATTR_PROPERTY(     campy, Camera, brightness   )
        MP_ATTR_PROPERTY(     campy, Camera, saturation   )
        MP_ATTR_PROPERTY(     campy, Camera, exposure     )
    }
    MP_STORE
    {
        MP_ATTR_PROPERTY_SET( campy, Camera, jpeg_quality )
        MP_ATTR_PROPERTY_SET( campy, Camera, frame_size   )
        MP_ATTR_PROPERTY_SET( campy, Camera, contrast     )
        MP_ATTR_PROPERTY_SET( campy, Camera, brightness   )
        MP_ATTR_PROPERTY_SET( campy, Camera, saturation   )
        MP_ATTR_PROPERTY_SET( campy, Camera, exposure     )
    }
}




MP_CLASS(campy, Camera)




/*
 *******************************************************************************
 * campy module
 *******************************************************************************
 */
MP_MODULE_BEGIN( campy         )
MP_MEMBER(       campy, Camera )
MP_MODULE_END(   campy         )
MP_REGISTER_MODULE(MP_QSTR_campy, mp_module_campy, 1);
