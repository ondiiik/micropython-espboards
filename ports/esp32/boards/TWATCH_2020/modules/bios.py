# Board bios module for TWatch 2020 by Ondrej Sienczak (OSi)
# See https://github.com/ondiiik/micropython-twatch-2020 for more
import                           axp202
import lvgl                   as lv
import st7789_lvgl            as st7789
import                           ft6x36
import                           bma423
from pcf8563  import PCF8563  as pcf8563
from machine  import             Pin, I2C, PWM
from time     import             sleep_ms, ticks_ms
from uasyncio import sleep_ms as asleep_ms
from i2s      import             I2S
from ustruct  import             unpack




# Display settings
_DISP_V_MIN        = const(2400)                       # Minimal power management backlight display voltage in millivolts
_DISP_V_MAX        = const(3200)                       # Maximal power management backlight display voltage in millivolts
_DISP_BL_FREQ      = const(1000)                       # Display backlight PWM frequency
_DISP_H_RES        = const(240)                        # Display horizontalresolution in pixels
_DISP_V_RES        = const(240)                        # Display vertical resolution in pixels
_DISP_BUFF_SIZE    = const(_DISP_H_RES * 10)           # LVGL display draw buffer size
_DISP_FADE_STEP    = const(1)                          # Backlight fade in/out step delay in milliseconds

# Display byte data indexes
_BD_DISP_BL_LEVEL  = const(0)                          # Index of backlight level
_BD_DISP_MAX       = const(1)                          # Count of display byte data items

# Power management unit byte data indexes
_BD_PMU_AUDIO_ON   = const(0)                          # Index of audio on status
_BD_PMU_DISPLAY_ON = const(1)                          # Index of display on status
_BD_PMU_DISPLAY_V  = const(2)                          # Index of display voltage in millivolts divided by 100
_BD_PMU_MAX        = const(3)                          # Count of display byte data items

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
    def __init__(self, fastboot = False):
        self._i2c    = I2C(_I2C_ID, scl = Pin(_PIN_SCL), sda=Pin(_PIN_SDA))
        self.display = Display()
        ft6x36.lvgl_touch_init()
        self.touch   = ft6x36
        self._init_lvgl()
        self.audio   = Audio()
        self.pmu     = None
        self.motor   = None
        self.rtc     = None
        self.bma     = None
        if fastboot:
            import _thread
            _thread.start_new_thread(self._init_slow, ())
        else:
            self._init_slow()
    
    
    def _init_lvgl(self):
        import lvesp32
        
        lv.init()
        disp_buf1         = st7789.lv_disp_buf_t()
        buf1_1            = bytes(_DISP_BUFF_SIZE)
        disp_buf1.init(buf1_1, None, len(buf1_1) // 4)
        
        disp_drv          = st7789.lv_disp_drv_t()
        disp_drv.init()
        disp_drv.buffer   = disp_buf1
        disp_drv.flush_cb = st7789.driver_flush
        disp_drv.hor_res  = _DISP_H_RES
        disp_drv.ver_res  = _DISP_V_RES
        disp_drv.register()
        
        indev_drv         = ft6x36.lv_indev_drv_t()
        indev_drv.init()
        indev_drv.type    = lv.INDEV_TYPE.POINTER
        indev_drv.read_cb = ft6x36.touch_driver_read
        indev_drv.register()
    
    
    def _init_slow(self):
        self.pmu              = Pmu(self._i2c)
        self.motor            = Motor()
        
        self.pmu.display_on   = True
        self.pmu.display_volt = _DISP_V_MAX
        self.display.percent  = 0
        
        self.rtc              = pcf8563(self._i2c)
        self.bma              = bma423
        
        bma423.init(self._i2c, irq = True)
    
    
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



# display interface
class Display:
    def __init__(self):
        st7789.lvgl_driver_init()
        self.driver            = st7789
        self._bdata            = bytearray(_BD_DISP_MAX)
        self._bl               = PWM(Pin(_PIN_BACKLIGHT, Pin.OUT), freq = _DISP_BL_FREQ, duty = 0)
        self.backlight_percent = 0
    
    
    async def afade(self,
                    val : int):
        rng, dval = self._blf_range(val)
        
        if rng is None:
            return
        
        for i in rng:
            self._bl.duty(i)
            await asleep_ms(_DISP_FADE_STEP)
        
        self._bl.duty(dval)
        self._bdata[_BD_DISP_BL_LEVEL] = val
    
    
    def fade(self,
             val : int):
        rng, dval = self._blf_range(val)
        
        if rng is None:
            return
        
        for i in rng:
            self._bl.duty(i)
            sleep_ms(_DISP_FADE_STEP)
        
        self._bl.duty(dval)
        self._bdata[_BD_DISP_BL_LEVEL] = val
    
    
    def _blf_range(self, val):
        val  = min(100, max(0, val))
        prev = self._bdata[_BD_DISP_BL_LEVEL]
        
        if val == prev:
            return None, None
        
        dval = 1023 * val  // 100
        
        if val > prev:
            rng = range(1023 * prev // 100, dval) 
        if val < prev:
            rng = reversed(range(dval, 1023 * prev // 100))
         
        return rng, dval
    
    
    @property
    def percent(self) -> int:
        return self._bdata[_BD_DISP_BL_LEVEL]
    
    
    @percent.setter
    def percent(self, val : int):
        val = min(100, max(0, val))
        self._bdata[_BD_DISP_BL_LEVEL] = val
        self._bl.duty(val * 10)
    
    
    def off(self):
        self.driver.st7789_send_cmd(0x10)

    def sleep(self):
        self.driver.st7789_send_cmd(0x10)

    def wakeup(self):
        self.driver.st7789_send_cmd(0x11)



# PMU interface
class Pmu:
    def __init__(self, i2c):
        self.driver = axp202.PMU(i2c)
        self._bdata = bytearray(_BD_PMU_MAX)
        
        self.driver.setShutdownTime(axp202.AXP_POWER_OFF_TIME_4S)
        self.driver.setChgLEDMode(axp202.AXP20X_LED_OFF)
        self.driver.setPowerOutPut(axp202.AXP202_EXTEN, False)
        self.driver.setChargeControlCur(_CHARG_CURR)
    
    
    def power_off(self):
        self.driver.setPowerOutPut(axp202.AXP202_EXTEN, False)
        self.driver.setPowerOutPut(axp202.AXP202_LDO4,  False)
        self.driver.setPowerOutPut(axp202.AXP202_DCDC2, False)
        self.audio_on   = False
        self.display_on = False
    
    
    @property
    def audio_on(self) -> bool:
        return bool(self._bdata[_BD_PMU_AUDIO_ON])
    
    
    @audio_on.setter
    def audio_on(self,
                 val : bool):
        self._bdata[_BD_PMU_AUDIO_ON] = int(val)
        self.driver.setLDO3Mode(1)
        self.driver.setPowerOutPut(axp202.AXP202_LDO3, val)
    
    
    @property
    def display_on(self) -> bool:
        return bool(self._bdata[_BD_PMU_DISPLAY_ON])
    
    
    @display_on.setter
    def display_on(self,
                   val : bool):
        self._bdata[_BD_PMU_DISPLAY_ON] = int(val)
        self.driver.setPowerOutPut(axp202.AXP202_LDO2, val)
    
    
    @property
    def display_volt(self) -> float:
        return bool(self._bdata[_BD_PMU_DISPLAY_V] * 100)
    
    
    @display_volt.setter
    def display_volt(self,
                     val : float):
        nv = int(val // 100)
        self._bdata[_BD_PMU_DISPLAY_V] = nv
        self.driver.setLDO2Voltage(min(_DISP_V_MAX, max(_DISP_V_MIN, nv * 100)))



# Motor interface
class Motor:
    def __init__(self):
        self.pwm = PWM(Pin(_PIN_MOTOR, Pin.OUT), freq = 1000, duty = 0)

    def on(self):
        self.pwm.duty(5)

    def off(self):
        self.pwm.duty(0)

    def set_strength(self, strength):
        self.pwm.duty(5 * strength / 100)

    def set_freq(self, freq):
        self.pwm.freq(freq)



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




# Board bios instance
bios = Bios()
bios.motor.off()

