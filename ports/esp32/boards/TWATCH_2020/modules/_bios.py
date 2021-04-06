# Board bios module for TWatch 2020 by Ondrej Sienczak (OSi)
# See https://github.com/ondiiik/micropython-twatch-2020 for more
import                               axp202
import lvgl                       as lv
# import                               lvesp32
import st7789_lvgl                as st7789
import                               ft6x36
from   imagetools import             get_png_info, open_png
import                               bma423
from pcf8563      import PCF8563  as pcf8563
from machine      import             Pin, I2C, PWM, freq
from time         import             sleep_ms, ticks_ms
from uasyncio     import sleep_ms as asleep_ms
from i2s          import             I2S
from ustruct      import             unpack




# Display settings
_DISP_V_MIN        = const(2400)                       # Minimal power management backlight display voltage in millivolts
_DISP_V_MAX        = const(3200)                       # Maximal power management backlight display voltage in millivolts
_DISP_V_RNG        = const(_DISP_V_MAX - _DISP_V_MIN)  # Display voltage range
_DISP_BL_FREQ      = const(120)                        # Display backlight PWM frequency
_DISP_H_RES        = const(240)                        # Display horizontalresolution in pixels
_DISP_V_RES        = const(240)                        # Display vertical resolution in pixels
_DISP_BUFF_SIZE    = const(_DISP_H_RES * 16)           # LVGL display draw buffer size
_DISP_FADE_STEP    = const(1)                          # Backlight fade in/out step delay in milliseconds

# Power management unit bytes module indexes
_BM_PMU_AUDIO_ON   = const(0)                          # Index of audio on status
_BM_PMU_DISPLAY_ON = const(1)                          # Index of display on status
_BM_PMU_DISPLAY_V  = const(2)                          # Index of display voltage in millivolts divided by 100
_BM_PMU_MAX        = const(3)                          # Count of display byte data items

# IMU unit bytes module indexes
_BM_IMU_TAPTAP     = const(0)
_BM_IMU_STEPPER    = const(1)
_BM_IMU_MAX        = const(2)

# Battery management
_CHARG_CURR        = const(300)                        # Battery charging current in milliampers (used 1C for 350mAh battery)

# HW
_I2C_ID            = const(1)
_PIN_SDA           = const(21)
_PIN_SCL           = const(22)
_PIN_PMU_ISR       = const(35)
_PIN_BMA_ISR       = const(39)
_PIN_RTC_ISR       = const(37)
_PIN_BACKLIGHT     = const(12)
_PIN_MOTOR         = const(4)
_PIN_BCK           = const(26) 
_PIN_WS            = const(25)  
_PIN_SDOUT         = const(33)

# WAV decoder
_wav_fmt           = '<IIIIIHHIIHHII'
_WAV_RIFF          = const(0x46464952)
_WAV_WAVE          = const(0x45564157)
_WAV_FMT           = const(0x20746D66)
_WAV_DATA          = const(0x61746164)
_WAV_BUFSIZE       = const(1024)
_I2S_DMA_LEN       = const(512)
_I2S_DMA_CNT       = const(10)
_I2S_OVERLAP       = const(200)




# Board bios class
class Bios:
    def __init__(self):
        # Initialize power management unit first
        i2c      = I2C(_I2C_ID, scl = Pin(_PIN_SCL), sda=Pin(_PIN_SDA))
        self.pmu = Pmu(i2c)
        
        # Run motor for short time to notify that bios
        # is starting up
        self.motor = Motor()
        self.motor.on()
        sleep_ms(50)
        self.motor.off()
        
        # Once PMU is initialized - switch off display back-light
        # power and initialize graphics and other peripherials
        self.pmu.display_on = False
        self.display        = Display(self.pmu)
        self.audio          = Audio()
        self.rtc            = pcf8563(i2c)
        self.imu            = Imu(i2c)
    
    
    @staticmethod
    def pmu_attach_interrupt(callback):
        irq = Pin(_PIN_PMU_ISR, mode = Pin.IN)
        irq.irq(handler = callback, trigger = Pin.IRQ_FALLING)
        return irq
    
    
    @staticmethod
    def bma_attach_interrupt(callback):
        irq = Pin(_PIN_BMA_ISR, mode = Pin.IN)
        irq.irq(handler = callback, trigger = Pin.IRQ_RISING)
        return irq
    
    
    @staticmethod
    def rtc_attach_interrupt(rtc_callback):
        irq = Pin(_PIN_RTC_ISR, mode = Pin.IN)
        irq.irq(handler = rtc_callback, trigger = Pin.IRQ_FALLING)
        return irq



# splash screen
class Splash(lv.obj):
    def __init__(self, scr):
        super().__init__(scr)
        
        try:
            with open('splash.png' ,'rb') as f:
                decoder         = lv.img.decoder_create()
                decoder.info_cb = get_png_info
                decoder.open_cb = open_png
                
                png_data        = f.read()
                png_img_dsc     = lv.img_dsc_t( { 'data_size': len(png_data),
                                                  'data'     : png_data } )
                
                img  = lv.img(self)
                img.align(scr, lv.ALIGN.IN_TOP_LEFT, 0, 0)
                img.set_src(png_img_dsc)
        except:
            img = lv.obj(self)
        
        self._lbl = lv.label(img)
        self._lbl.set_style_local_text_color(self.PART.MAIN,
                                             lv.STATE.DEFAULT,
                                             lv.color_hex3(0xFF8))
        self.label = 'Loading bios ...'
    
    
    @property
    def label(self):
        return self._txt
    
    
    @label.setter
    def label(self, txt):
        print(txt)
        self._txt = txt
        self._lbl.set_text(txt)
        self._lbl.align(self, lv.ALIGN.IN_BOTTOM_MID, 0, -12)
    
    
    
# display interface
class Display:
    def __init__(self, pmu):
        self._brightness = 0
        self._backlight  = PWM(Pin(_PIN_BACKLIGHT, Pin.OUT), freq = _DISP_BL_FREQ, duty = 0)
        
        # Initilaize LVGL first
        lv.init()
        
        
        # Initializes display
        st7789.lvgl_driver_init()
        
        disp_buf1         = st7789.lv_disp_buf_t()
        buf1_1            = bytes(_DISP_BUFF_SIZE)
        disp_buf1.init(buf1_1, None, len(buf1_1) // 4)
        
        disp_drv          = st7789.lv_disp_drv_t()
        disp_drv.init()
        disp_drv.buffer   = disp_buf1
        disp_drv.flush_cb = st7789.driver_flush
        disp_drv.hor_res  = _DISP_H_RES
        disp_drv.ver_res  = _DISP_V_RES
        
        self.disp         = disp_drv.register()
        
        
        # inmitializes touch screen
        ft6x36.lvgl_touch_init()
        
        indev_drv         = ft6x36.lv_indev_drv_t()
        indev_drv.init()
        indev_drv.type    = lv.INDEV_TYPE.POINTER
        indev_drv.read_cb = ft6x36.touch_driver_read
        
        self.indev        =  indev_drv.register()
        
        
        # And load splash-screen if there is some found
        self.brightness    = 0
        pmu.display_power  = 1
        pmu.display_on     = True
        
        self.splash        = Splash(None)
        lv.scr_load(self.splash)
        self.refresh(10)
        
        self.fade(0.1)
    
    
    
    @staticmethod
    def refresh(rate : int = 10):
        lv.tick_inc(rate)
        lv.task_handler()
    
    
    async def afade(self,
                    v : float):
        rng, dval = self._blf_range(v)
        
        if rng is None:
            return
        
        for i in rng:
            self._backlight.duty(i)
            await asleep_ms(_DISP_FADE_STEP)
        
        self._backlight.duty(dval)
    
    
    def fade(self,
             v : float):
        rng, dval = self._blf_range(v)
        
        if rng is None:
            return
        
        for i in rng:
            self._backlight.duty(i)
            sleep_ms(_DISP_FADE_STEP)
        
        self._backlight.duty(dval)
    
    
    def _blf_range(self, v):
        val  = min(1, max(0, v))
        dval = int(1023 * val)
        prev = int(1023 * self._brightness)
        
        if dval == prev:
            return None, None, None
        
        if dval > prev:
            rng = range(prev, dval) 
        if dval < prev:
            rng = reversed(range(dval, prev))
        
        self._brightness = val
        return rng, dval
    
    
    @property
    def brightness(self) -> float:
        return self._brightness
    
    
    @brightness.setter
    def brightness(self, val : float):
        val = min(1, max(0, val))
        self._brightness = val
        self._backlight.duty(int(1023 * val))
    
    
    def off(self):
        st7789.st7789_send_cmd(0x10)
    
    def sleep(self):
        st7789.st7789_send_cmd(0x10)
    
    def wakeup(self):
        st7789.st7789_send_cmd(0x11)



# PMU interface
class Pmu:
    def __init__(self, i2c):
        self._drv = axp202.PMU(i2c)
        self._bm  = bytearray(_BM_PMU_MAX)
        
        self._drv.setPowerOutPut(axp202.AXP202_LDO2, False)
        self._drv.setPowerOutPut(axp202.AXP202_LDO3, False)
        
        self._drv.setShutdownTime(axp202.AXP_POWER_OFF_TIME_4S)
        self._drv.setChgLEDMode(axp202.AXP20X_LED_OFF)
        self._drv.setPowerOutPut(axp202.AXP202_EXTEN, False)
        self._drv.setChargeControlCur(_CHARG_CURR)
    
    
    def power_off(self):
        self._drv.setPowerOutPut(axp202.AXP202_EXTEN, False)
        self._drv.setPowerOutPut(axp202.AXP202_LDO4,  False)
        self._drv.setPowerOutPut(axp202.AXP202_DCDC2, False)
        self.audio_on   = False
        self.display_on = False
    
    
    @property
    def freq(self):
        return freq()
    
    
    @property
    def audio_on(self) -> bool:
        return bool(self._bm[_BM_PMU_AUDIO_ON])
    
    
    @audio_on.setter
    def audio_on(self,
                 val : bool):
        self._bm[_BM_PMU_AUDIO_ON] = int(val)
        self._drv.setLDO3Mode(1)
        self._drv.setPowerOutPut(axp202.AXP202_LDO3, val)
    
    
    @property
    def display_on(self) -> bool:
        return bool(self._bm[_BM_PMU_DISPLAY_ON])
    
    
    @display_on.setter
    def display_on(self,
                   val : bool):
        self._bm[_BM_PMU_DISPLAY_ON] = int(val)
        self._drv.setPowerOutPut(axp202.AXP202_LDO2, val)
    
    
    @property
    def display_power(self) -> float:
        return self._bm[_BM_PMU_DISPLAY_V] / 255
    
    
    @display_power.setter
    def display_power(self,
                     val : float):
        nv = max(0, min(255, int(val * 255)))
        
        if not nv == self._bm[_BM_PMU_DISPLAY_V]:
            self._bm[_BM_PMU_DISPLAY_V] = nv
            self._drv.setLDO2Voltage(_DISP_V_MIN + nv * _DISP_V_RNG // 255)



# Motor interface
class Motor:
    def __init__(self):
        self._drv = PWM(Pin(_PIN_MOTOR, Pin.OUT), freq = 1000, duty = 0)
    
    
    def on(self):
        self._drv.duty(5)
    
    
    def off(self):
        self._drv.duty(0)
    
    
    def set_strength(self, strength):
        self._drv.duty(5 * strength / 100)
    
    
    def set_freq(self, freq):
        self._drv.freq(freq)



# Audio interface
class Audio:
    def __init__(self):
        self._bck    = Pin(_PIN_BCK)
        self._ws     = Pin(_PIN_WS)  
        self._sdout  = Pin(_PIN_SDOUT)
    
    
    def play_wav(self,
                 file : str):
        with open(file, 'rb') as wav:
            audio, ms = self._init_wav(wav)
            buff      = memoryview(bytearray(_WAV_BUFSIZE))
            cnt       = 1
            end       = ms + _I2S_OVERLAP + ticks_ms()
            
            while cnt > 0:
                cnt     = wav.readinto(buff)
                written = 0
                
                while written < cnt:
                    written += audio.write(buff[written:cnt], timeout = 0)
            
        sleep_ms(max(0, end - ticks_ms()))
        audio.deinit()
        bios.pmu.audio_on = False
    
    
    async def aplay_wav(self,
                        file : str):
        with open(file, 'rb') as wav:
            audio, ms = self._init_wav(wav)
            buff      = memoryview(bytearray(_WAV_BUFSIZE))
            cnt       = 1
            end       = ms + _I2S_OVERLAP + ticks_ms()
            
            while cnt > 0:
                cnt     = wav.readinto(buff)
                written = 0
                
                while written < cnt:
                    written += audio.write(buff[written:cnt], timeout = 0)
                    await asleep_ms(0)
            
        await asleep_ms(max(0, end - ticks_ms()))
        audio.deinit()
        bios.pmu.audio_on = False
    
    
    def _init_wav(self, wav):
        header = wav.read(44)
        riff, \
        _, \
        wave, \
        fmt, \
        _, _, \
        channels, \
        rate, \
        _, _, \
        bits, \
        data, \
        size = unpack(_wav_fmt, header)
        
        if (not _WAV_WAVE == wave) or \
           (not _WAV_RIFF == riff) or \
           (not _WAV_FMT  == fmt)  or \
           (not _WAV_DATA == data):
            raise ValueError('Invalid WAV signature')
        
        if not bits == 16:
            raise ValueError('Only 16 bit WAV supported')
        
        bios.pmu.audio_on = True
        audio             = I2S(I2S.NUM0, 
                                bck           = self._bck,
                                ws            = self._ws,
                                sdout         = self._sdout, 
                                standard      = I2S.PHILIPS, 
                                mode          = I2S.MASTER_TX,
                                dataformat    = I2S.B16,
                                channelformat = I2S.ONLY_LEFT,
                                samplerate    = rate,
                                dmacount      = _I2S_DMA_CNT,
                                dmalen        = _I2S_DMA_LEN)
        
        return audio, (8000 * size) // (rate * channels * bits)



class Imu:
    DIRECTION_TOP_EDGE    = bma423.DIRECTION_TOP_EDGE
    DIRECTION_BOTTOM_EDGE = bma423.DIRECTION_BOTTOM_EDGE
    DIRECTION_LEFT_EDGE   = bma423.DIRECTION_LEFT_EDGE
    DIRECTION_RIGHT_EDGE  = bma423.DIRECTION_RIGHT_EDGE
    DIRECTION_DISP_UP     = bma423.DIRECTION_DISP_UP
    DIRECTION_DISP_DOWN   = bma423.DIRECTION_DISP_DOWN
    
    
    def __init__(self, i2c):
        self._drv = bma423
        self._bm  = bytearray(_BM_IMU_MAX)
        bma423.init(i2c, irq = True)
    
    
    @property
    def accel(self):
        return self._drv.accel()
    
    
    @property
    def direction(self):
        return self._drv.accel()
    
    
    @property
    def temp(self):
        return self._drv.temp()
    
    
    @property
    def steps(self):
        return self._drv.step_count()
    
    
    def reset(self):
        self._drv.step_reset()
    
    
    def wait_taptap(self):
        while not self._test(_BM_IMU_TAPTAP):
            sleep_ms(10)
    
    
    async def await_taptap(self):
        while not self._test(_BM_IMU_TAPTAP):
            await asleep_ms(10)
    
    
    def wait_step(self):
        while not self._test(_BM_IMU_STEPPER):
            sleep_ms(10)
        
        return self._drv.step_count()
    
    
    async def await_step(self):
        while not self._test(_BM_IMU_STEPPER):
            await asleep_ms(10)
        
        return self._drv.step_count()
    
    
    def _test(self, flag):
        v = bios.imu._drv.irq_read()
        
        if   v == bios.imu._drv.IRQ_DOUBLE_WAKEUP:
            self._bm[_BM_IMU_TAPTAP]  = 1
        elif v == bios.imu._drv.IRQ_STEP_COUNTER:
            self._bm[_BM_IMU_STEPPER] = 1
        
        v = self._bm[flag]
        self._bm[flag] = 0
        return v




# Board bios instance
bios = Bios()
bios.motor.off()

