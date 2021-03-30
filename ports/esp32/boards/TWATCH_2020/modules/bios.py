# Board bios module for TWatch 2020 by Ondrej Sienczak (OSi)
# See https://github.com/ondiiik/micropython-twatch-2020 for more
import                           axp202
import lvgl                   as lv
import st7789_lvgl            as st7789
import                           ft6x36
import                           bma423
from pcf8563  import PCF8563  as pcf8563
from machine  import             Pin, I2C, PWM
from time     import             sleep_ms
from uasyncio import sleep_ms as asleep_ms



# Display settings
_DISP_V_MIN        = const(2400)                       # Minimal power management backlight display voltage in millivolts
_DISP_V_MAX        = const(3200)                       # Maximal power management backlight display voltage in millivolts
_DISP_V_RANGE      = const(_DISP_V_MAX - _DISP_V_MIN)  # Difference between minimal and maximal display backlight voltage
_DISP_H_RES        = const(240)                        # Display horizontalresolution in pixels
_DISP_V_RES        = const(240)                        # Display vertical resolution in pixels
_DISP_BUFF_SIZE    = const(_DISP_H_RES * 10)           # LVGL display draw buffer size
_DISP_FADE_STEP    = const(8)                          # Backlight fade in/out step (larger means faster)

# Display byte data indexes
_BD_DISP_BL_LEVEL  = const(0)                          # Index of backlight level
_BD_DISP_MAX       = const(1)                          # Count of display byte data items

# Power management unit byte data indexes
_BD_PMU_AUDIO_ON   = const(0)                          # Index of audio on status
_BD_PMU_DISPLAY_ON = const(1)                          # Index of display on status
_BD_PMU_DISPLAY_V  = const(2)                          # Index of display voltage in millivolts divided by 100
_BD_PMU_MAX        = const(3)                          # Count of display byte data items

# Battery management
_CHARG_CURR        = const(300)                        # Battery charging current in milliampers

# HW
_I2C_ID            = const(1)
_PIN_SDA           = const(21)
_PIN_SCL           = const(22)
_PIN_PMU_ISR       = const(35)
_PIN_BMA_ISR       = const(39)
_PIN_RTC_ISR       = const(37)
_PIN_BACKLIGHT     = const(12)
_PIN_MOTOR         = const(4)



# Board bios class
class Bios:
    def __init__(self, fastboot = False):
        self._i2c    = I2C(_I2C_ID, scl = Pin(_PIN_SCL), sda=Pin(_PIN_SDA))
        self.pmu     = Pmu(self._i2c)
        self.motor   = Motor()
        self.display = Display(self.pmu)
        ft6x36.lvgl_touch_init()
        self.touch   = ft6x36
        self._init_lvgl()
        self.rtc     = None
        self.bma     = None
        self.ticker  = None
        if fastboot:
            import _thread
            _thread.start_new_thread(self._init_slow, ())
        else:
            self._init_slow()
    
    def _init_lvgl(self):
        import lvesp32
        
        lv.init()
        self.ticker       = lvesp32
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
        self.rtc = pcf8563(self._i2c)
        bma423.init(self._i2c, irq=True)
        self.bma = bma423
    
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
    def __init__(self, pmu):
        st7789.lvgl_driver_init()
        self.driver            = st7789
        self.pmu               = pmu
        self._bdata            = bytearray(_BD_DISP_MAX)
        self._bl               = Pin(_PIN_BACKLIGHT, Pin.OUT)
        self.backlight_percent = 0
        self.pmu.display_on    = True
    
    
    async def afade(self,
                    val : int):
        rng, dval = self._blf_range(val)
        
        if rng is None:
            return
        
        for i in rng:
            self.pmu.display_volt = i
            await asleep_ms(0)
        
        self.pmu.display_volt          = dval
        self._bdata[_BD_DISP_BL_LEVEL] = val
    
    
    def fade(self,
             val : int):
        rng, dval = self._blf_range(val)
        
        if rng is None:
            return
        
        for i in rng:
            self.pmu.display_volt = i
            sleep_ms(0)
        
        self.pmu.display_volt          = dval
        self._bdata[_BD_DISP_BL_LEVEL] = val
    
    
    def _blf_range(self, val):
        val  = min(100, max(0, val))
        prev = self._bdata[_BD_DISP_BL_LEVEL]
        
        if val == prev:
            return None, None
        
        dval = _DISP_V_MIN + _DISP_V_RANGE * val  // 100
        
        if val > prev:
            rng = range(_DISP_V_MIN + _DISP_V_RANGE * prev // 100, dval, _DISP_FADE_STEP) 
        if val < prev:
            rng = reversed(range(dval, _DISP_V_MIN + _DISP_V_RANGE * prev // 100, _DISP_FADE_STEP))
         
        return rng, dval
    
    
    @property
    def percent(self) -> int:
        return self._bdata[_BD_DISP_BL_LEVEL]
    
    
    @percent.setter
    def percent(self, val : int):
        val = min(100, max(0, val))
        self._bdata[_BD_DISP_BL_LEVEL] = val
        self.pmu.display_volt = val / 1000
    
    
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



# Board bios instance
bios = Bios()
bios.motor.off()

