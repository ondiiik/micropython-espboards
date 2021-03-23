# -*- coding: utf-8 -*-
# Port of the official ttgo library for the LilyGo TTGO T-Watch 2020.
# Author: Nikita Selin (Anodev)[https://github.com/OPHoperHPO]
import gc
import _thread
import axp202
import lvgl as lv
import st7789_lvgl
import ft6x36
import bma423 as BMA423
from pcf8563 import PCF8563
from machine import Pin, I2C, PWM


class Watch:
    def __init__(self, fastboot=False):
        self.__i2c__ = I2C(1, scl=Pin(22), sda=Pin(21))
        self.pmu = axp202.PMU(self.__i2c__)
        self.tft = self.__init_display__()
        self.touch = self.__init_touch__()
        self.motor = None
        self.rtc = None
        self.bma = None
        self.ticker = None
        self.__fastboot__ = fastboot
        if fastboot:
            _thread.start_new_thread(self.__init_prep__, ())
        else:
            self.__init_prep__()

    def __init_prep__(self):
        self.init_power()
        self.motor = Motor()
        self.rtc = PCF8563(self.__i2c__)
        self.bma = self.__init_bma__()

    def __init_touch__(self):
        ft6x36.lvgl_touch_init()
        return ft6x36

    def __init_bma__(self):
        BMA423.init(self.__i2c__, irq=True)
        return BMA423

    def __init_display__(self):
        return Display(self.pmu)

    @staticmethod
    def pmu_attach_interrupt(callback):
        irq = Pin(35, mode=Pin.IN)
        irq.irq(handler=callback, trigger=Pin.IRQ_FALLING)
        return irq

    @staticmethod
    def bma_attach_interrupt(callback):
        irq = Pin(39, mode=Pin.IN)
        irq.irq(handler=callback, trigger=Pin.IRQ_RISING)
        return irq

    @staticmethod
    def rtc_attach_interrupt(rtc_callback):
        irq = Pin(37, mode=Pin.IN)
        irq.irq(handler=rtc_callback, trigger=Pin.IRQ_FALLING)
        return irq

    def enable_audio_power(self, en=True):
        self.pmu.setLDO3Mode(1)
        self.pmu.setPowerOutPut(axp202.AXP202_LDO3, en)

    def lvgl_begin(self):
        import lvesp32
        lv.init()
        self.ticker = lvesp32
        disp_buf1 = st7789_lvgl.lv_disp_buf_t()
        buf1_1 = bytes(240 * 10)
        disp_buf1.init(buf1_1, None, len(buf1_1) // 4)
        disp_drv = st7789_lvgl.lv_disp_drv_t()
        disp_drv.init()
        disp_drv.buffer = disp_buf1
        disp_drv.flush_cb = st7789_lvgl.driver_flush
        disp_drv.hor_res = 240
        disp_drv.ver_res = 240
        disp_drv.register()
        indev_drv = ft6x36.lv_indev_drv_t()
        indev_drv.init()
        indev_drv.type = lv.INDEV_TYPE.POINTER
        indev_drv.read_cb = ft6x36.touch_driver_read
        indev_drv.register()

    def init_power(self):
        # Change the button boot time to 4 seconds
        self.pmu.setShutdownTime(axp202.AXP_POWER_OFF_TIME_4S)
        # Turn off the charging instructions, there should be no
        self.pmu.setChgLEDMode(axp202.AXP20X_LED_OFF)
        # Turn off external enable
        self.pmu.setPowerOutPut(axp202.AXP202_EXTEN, False)
        # axp202 allows maximum charging current of 1800mA, minimum 300mA
        self.pmu.setChargeControlCur(300)

    def power_off(self):
        self.pmu.setPowerOutPut(axp202.AXP202_EXTEN, False)
        self.pmu.setPowerOutPut(axp202.AXP202_LDO4, False)
        self.pmu.setPowerOutPut(axp202.AXP202_DCDC2, False)
        self.pmu.setPowerOutPut(axp202.AXP202_LDO3, False)
        self.pmu.setPowerOutPut(axp202.AXP202_LDO2, False)


class Display:
    """Display wrapper"""

    def __init__(self, pmu):
        """Inits display"""
        # Init display
        tft = st7789_lvgl
        tft.lvgl_driver_init()
        self.tft = tft
        self.pmu = pmu
        self.set_backlight_level(0)  # Turn backlight off
        self.backlight_level = 0
        self.bl_pin = Pin(12, Pin.OUT)
        self.backlight(1)  # Enable power on backlight

    def backlight_fade(self, val=100):
        if val > self.backlight_level:
            data = 0
            for i in range(self.backlight_level, val):
                data = i
                self.set_backlight_level(i)
            self.backlight_level = data
            return True
        elif val < self.backlight_level:
            data = 0
            for i in reversed(range(val, self.backlight_level)):
                data = i
                self.set_backlight_level(i)
            self.backlight_level = data
            return True

    def switch_scene(self):
        level = self.backlight_level
        self.backlight_fade(0)
        self.backlight_fade(level)

    def set_backlight_level(self, percent):
        if 0 <= percent <= 100:
            voltage = 800 * percent / 100
            self.__set_lcd_backlight_voltage__(2400 + voltage)
            self.backlight_level = percent

    def display_off(self):
        self.tft.st7789_send_cmd(0x10)

    def display_sleep(self):
        self.tft.st7789_send_cmd(0x10)

    def display_wakeup(self):
        self.tft.st7789_send_cmd(0x11)

    def backlight(self, val):
        self.bl_pin.value(val)
        self.__turn_lcd_backlight__(val)

    def __turn_lcd_backlight__(self, val):
        if val == 1:
            self.pmu.setPowerOutPut(axp202.AXP202_LDO2, True)
        else:
            self.pmu.setPowerOutPut(axp202.AXP202_LDO2, False)

    def __set_lcd_backlight_voltage__(self, voltage=3200):
        if voltage >= 3200:
            self.pmu.setLDO2Voltage(3200)
        elif voltage <= 2400:
            self.pmu.setLDO2Voltage(2400)
        else:
            self.pmu.setLDO2Voltage(voltage)


class Motor:
    def __init__(self):
        self.pwm = PWM(Pin(4, Pin.OUT), freq=1000, duty=0)

    def on(self):
        self.pwm.duty(5)

    def off(self):
        self.pwm.duty(0)

    def set_strength(self, strength):
        self.pwm.duty(5 * strength / 100)

    def set_freq(self, freq):
        self.pwm.freq(freq)
