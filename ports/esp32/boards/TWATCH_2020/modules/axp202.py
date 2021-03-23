'''
MIT License

Copyright (c) 2019 lewis he

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

axp20x.py - MicroPython library for X-Power AXP202 chip.
Created by Lewis he on June 24, 2019.
github:https://github.com/lewisxhe/AXP202X_Libraries
Updated by Anodev https://github.com/OPHoperHPO
'''
import time

from ustruct import unpack
from machine import I2C
from axp_constants import *

default_pin_scl = 22
default_pin_sda = 21
default_pin_intr = 35
default_chip_type = AXP202_CHIP_ID


class PMU(object):
    startupParams = [
        0b00000000,
        0b01000000,
        0b10000000,
        0b11000000
    ]

    longPressParams = [
        0b00000000,
        0b00010000,
        0b00100000,
        0b00110000]

    shutdownParams = {
        0b00000000,
        0b00000001,
        0b00000010,
        0b00000011
    }

    targetVolParams = [
        0b00000000,
        0b00100000,
        0b01000000,
        0b01100000]

    def __init__(self, i2c:I2C, address=None):
        self.device = None
        self._chip_id = default_chip_type
        self.address = address if address else AXP202_SLAVE_ADDRESS

        self._outputReg = None
        self.buffer = bytearray(16)
        self.bytebuf = memoryview(self.buffer[0:1])
        self.wordbuf = memoryview(self.buffer[0:2])
        self._irq = memoryview(self.buffer[0:5])
        self.bus = i2c
        self.init_device()

    def write_byte(self, reg, val):
        self.bytebuf[0] = val
        self.bus.writeto_mem(self.address, reg, self.bytebuf)

    def read_byte(self, reg, buffer_full=False):
        if buffer_full is True:
            buffer = bytearray(16)
            self.bus.readfrom_mem_into(self.address, reg, buffer)
            return buffer
        else:
            self.bus.readfrom_mem_into(self.address, reg, self.bytebuf)
            return self.bytebuf[0]

    def read_word(self, reg):
        self.bus.readfrom_mem_into(self.address, reg, self.wordbuf)
        return unpack('>H', self.wordbuf)[0]

    def read_word2(self, reg):
        self.bus.readfrom_mem_into(self.address, reg, self.wordbuf)
        return unpack('>h', self.wordbuf)[0]

    def init_device(self):
        print('* initializing mpu')
        self._chip_id = self.read_byte(AXP202_IC_TYPE)
        if self._chip_id == AXP202_CHIP_ID:
            self._chip_id = AXP202_CHIP_ID
            print("Detect PMU Type is AXP202")
            self._outputReg = self.read_byte(AXP202_LDO234_DC23_CTL)
            print("OUTPUT Register 0x%x\n", self._outputReg)
        elif self._chip_id == AXP192_CHIP_ID:
            print("Detect PMU Type is AXP192")
            self._chip_id = AXP192_CHIP_ID
        else:
            raise Exception("Invalid Chip ID!")

    def isExtenEnable(self):
        if self._chip_id == AXP202_CHIP_ID:
            return IS_OPEN(self._outputReg, AXP202_EXTEN)
        else:
            return False

    def isLDO2Enable(self):
        if self._chip_id == AXP202_CHIP_ID:
            return IS_OPEN(self._outputReg, AXP202_LDO2)

    def isLDO3Enable(self):
        if self._chip_id == AXP202_CHIP_ID:
            return IS_OPEN(self._outputReg, AXP202_LDO3)
        else:
            return False

    def isLDO4Enable(self):
        if self._chip_id == AXP202_CHIP_ID:
            return IS_OPEN(self._outputReg, AXP202_LDO4)
        else:
            return False

    def isDCDC2Enable(self):
        if self._chip_id == AXP202_CHIP_ID:
            return IS_OPEN(self._outputReg, AXP202_DCDC2)
        else:
            return False

    def isDCDC3Enable(self):
        if self._chip_id == AXP202_CHIP_ID:
            return IS_OPEN(self._outputReg, AXP202_DCDC3)
        else:
            return False

    def setPowerOutPut(self, ch, en):
        data = self.read_byte(AXP202_LDO234_DC23_CTL)
        if (en):
            data |= (1 << ch)
        else:
            data &= (~(1 << ch))

        if self._chip_id == AXP202_CHIP_ID:
            FORCED_OPEN_DCDC3(data)  # ! Must be forced open in T-Watch

        self.write_byte(AXP202_LDO234_DC23_CTL, data)
        time.sleep(1)
        val = self.read_byte(AXP202_LDO234_DC23_CTL)
        if data == val:
            _outputReg = val
            return AXP_PASS
        return AXP_FAIL

    def enablePower(self, ch):
        data = self.read_byte(AXP202_LDO234_DC23_CTL)
        data = data | (1 << ch)
        if self._chip_id == AXP202_CHIP_ID:
            FORCED_OPEN_DCDC3(data)  # ! Must be forced open in T-Watch
        self.write_byte(AXP202_LDO234_DC23_CTL, data)
        time.sleep(1)
        val = self.read_byte(AXP202_LDO234_DC23_CTL)
        if data == val:
            self._outputReg = val
            return AXP_PASS
        return AXP_FAIL

    def disablePower(self, ch):
        data = self.read_byte(AXP202_LDO234_DC23_CTL)
        data &= ~(1 << ch)
        self.write_byte(AXP202_LDO234_DC23_CTL, data)

    def __BIT_MASK(self, mask):
        return 1 << mask

    def _getRegistH8L5(self, regh8, regl5):
        hv = self.read_byte(regh8)
        lv = self.read_byte(regl5)
        return (hv << 5) | (lv & 0x1F)

    def _getRegistResult(self, regh8, regl4):
        hv = self.read_byte(regh8)
        lv = self.read_byte(regl4)
        return (hv << 4) | (lv & 0x0F)

    def isChargeing(self):
        reg = self.read_byte(AXP202_MODE_CHGSTATUS)
        return IS_OPEN(reg, 6)

    def isBatteryConnect(self):
        data = self.read_byte(AXP202_MODE_CHGSTATUS)
        return IS_OPEN(data, 5)

    def getAcinCurrent(self):
        data = self._getRegistResult(AXP202_ACIN_CUR_H8, AXP202_ACIN_CUR_L4)
        return data * AXP202_ACIN_CUR_STEP

    def getAcinVoltage(self):
        data = self._getRegistResult(AXP202_ACIN_VOL_H8, AXP202_ACIN_VOL_L4)
        return data * AXP202_ACIN_VOLTAGE_STEP

    def getVbusVoltage(self):
        data = self._getRegistResult(AXP202_VBUS_VOL_H8, AXP202_VBUS_VOL_L4)
        return data * AXP202_VBUS_VOLTAGE_STEP

    def getVbusCurrent(self):
        data = self._getRegistResult(AXP202_VBUS_CUR_H8, AXP202_VBUS_CUR_L4)
        return data * AXP202_VBUS_CUR_STEP

    def getTemp(self):
        data = self._getRegistResult(AXP202_INTERNAL_TEMP_H8, AXP202_INTERNAL_TEMP_L4) * AXP202_INTERNAL_TEMP_STEP
        return data

    def getTSTemp(self):
        data = self._getRegistResult(AXP202_TS_IN_H8, AXP202_TS_IN_L4)
        return data * AXP202_TS_PIN_OUT_STEP

    def getGPIO0Voltage(self):
        data = self._getRegistResult(AXP202_GPIO0_VOL_ADC_H8,
                                     AXP202_GPIO0_VOL_ADC_L4)
        return data * AXP202_GPIO0_STEP

    def getGPIO1Voltage(self):
        data = self._getRegistResult(AXP202_GPIO1_VOL_ADC_H8,
                                     AXP202_GPIO1_VOL_ADC_L4)
        return data * AXP202_GPIO1_STEP

    def getBattInpower(self):
        h8 = self.read_byte(AXP202_BAT_POWERH8)
        m8 = self.read_byte(AXP202_BAT_POWERM8)
        l8 = self.read_byte(AXP202_BAT_POWERL8)
        data = (h8 << 16) | (m8 << 8) | l8
        return 2 * data * 1.1 * 0.5 / 1000

    def getBattVoltage(self):
        data = self._getRegistResult(AXP202_BAT_AVERVOL_H8, AXP202_BAT_AVERVOL_L4)
        return data * AXP202_BATT_VOLTAGE_STEP

    def getBattChargeCurrent(self):
        data = 0
        if (self._chip_id == AXP202_CHIP_ID):
            data = self._getRegistResult(
                AXP202_BAT_AVERCHGCUR_H8, AXP202_BAT_AVERCHGCUR_L4) * AXP202_BATT_CHARGE_CUR_STEP
        elif self._chip_id == AXP192_CHIP_ID:
            data = self._getRegistH8L5(
                AXP202_BAT_AVERCHGCUR_H8, AXP202_BAT_AVERCHGCUR_L4) * AXP202_BATT_CHARGE_CUR_STEP
        return data

    def getBattDischargeCurrent(self):
        data = self._getRegistResult(
            AXP202_BAT_AVERDISCHGCUR_H8, AXP202_BAT_AVERDISCHGCUR_L5) * AXP202_BATT_DISCHARGE_CUR_STEP
        return data

    def getSysIPSOUTVoltage(self):
        return self._getRegistResult(AXP202_APS_AVERVOL_H8, AXP202_APS_AVERVOL_L4)

    def getBattChargeCoulomb(self):
        buffer = self.read_byte(0xB0, True)
        return (buffer[0] << 24) + (buffer[1] << 16) + (buffer[2] << 8) + buffer[3]

    def getBattDischargeCoulomb(self):
        buffer = self.read_byte(0xB4, True)
        return (buffer[0] << 24) + (buffer[1] << 16) + (buffer[2] << 8) + buffer[3]

    def getCoulombData(self):
        charge = self.getBattChargeCoulomb()
        discharge = self.getBattDischargeCoulomb()
        rate = self.getAdcSamplingRate()
        result = 65536.0 * 0.5 * (charge - discharge) / 3600.0 / rate
        return result

    # //-------------------------------------------------------
    # // New Coulomb functions  by MrFlexi
    # //-------------------------------------------------------
    def getCoulombRegister(self):
        return self.read_byte(AXP202_COULOMB_CTL)

    def setCoulombRegister(self, val):
        self.write_byte(AXP202_COULOMB_CTL, val)
        return AXP_PASS

    def EnableCoulombcounter(self):
        val = 0x80
        self.write_byte(AXP202_COULOMB_CTL, val)

    def DisableCoulombcounter(self):
        val = 0x00
        self.write_byte(AXP202_COULOMB_CTL, val)
        return AXP_PASS

    def StopCoulombcounter(self):
        self.write_byte(AXP202_COULOMB_CTL, 0xB8)
        return AXP_PASS

    def ClearCoulombcounter(self):
        self.write_byte(AXP202_COULOMB_CTL, 0xA0)
        return AXP_PASS

    def getAdcSamplingRate(self):
        # //axp192 same axp202 aregister address 0x84
        val = self.read_byte(AXP202_ADC_SPEED)
        return 25 * int(pow(2, (val & 0xC0) >> 6))

    def setAdcSamplingRate(self, rate):
        if (rate > AXP_ADC_SAMPLING_RATE_200HZ):
            return AXP_FAIL
        val = self.read_byte(AXP202_ADC_SPEED)
        rw = rate
        val &= 0x3F
        val |= (rw << 6)
        self.write_byte(AXP202_ADC_SPEED, val)
        return AXP_PASS

    def setTSfunction(self, func):
        if (func > AXP_TS_PIN_FUNCTION_ADC):
            return AXP_FAIL
        val = self.read_byte(AXP202_ADC_SPEED)
        rw = func
        val &= 0xFA
        val |= (rw << 2)
        self.write_byte(AXP202_ADC_SPEED, val)
        return AXP_PASS

    def setTScurrent(self, current):
        if current > AXP_TS_PIN_CURRENT_80UA:
            return AXP_FAIL
        val = self.read_byte(AXP202_ADC_SPEED)
        rw = current
        val &= 0xCF
        val |= (rw << 4)
        self.write_byte(AXP202_ADC_SPEED, val)
        return AXP_PASS

    def setTSmode(self, mode):
        if (mode > AXP_TS_PIN_MODE_ENABLE):
            return AXP_FAIL
        val = self.read_byte(AXP202_ADC_SPEED)
        rw = mode
        val &= 0xFC
        val |= rw
        self.write_byte(AXP202_ADC_SPEED, val)
        if mode == AXP_TS_PIN_MODE_DISABLE:
            self.adc1Enable(AXP202_TS_PIN_ADC1, False)
        else:
            self.adc1Enable(AXP202_TS_PIN_ADC1, True)
        return AXP_PASS

    def adc1Enable(self, params, en):
        val = self.read_byte(AXP202_ADC_EN1)
        if (en):
            val |= params
        else:
            val &= ~(params)
        self.write_byte(AXP202_ADC_EN1, val)
        return AXP_PASS

    def adc2Enable(self, params, en):
        val = self.read_byte(AXP202_ADC_EN2)
        if (en):
            val |= params
        else:
            val &= ~(params)
        self.write_byte(AXP202_ADC_EN2, val)
        return AXP_PASS

    def enableADC(self, ch, val):
        if (ch == 1):
            data = self.read_byte(AXP202_ADC_EN1)
            data = data | (1 << val)
            self.write_byte(AXP202_ADC_EN1, data)
        elif (ch == 2):
            data = self.read_byte(AXP202_ADC_EN2)
            data = data | (1 << val)
            self.write_byte(AXP202_ADC_EN1, data)
        else:
            return

    def disableADC(self, ch, val):
        if (ch == 1):
            data = self.read_byte(AXP202_ADC_EN1)
            data = data & (~(1 << val))
            self.write_byte(AXP202_ADC_EN1, data)
        elif (ch == 2):
            data = self.read_byte(AXP202_ADC_EN2)
            data = data & (~(1 << val))
            self.write_byte(AXP202_ADC_EN1, data)
        else:
            return

    def enableIRQ(self, params, en):
        if (params & 0xFF):
            val1 = params & 0xFF
            val = self.read_byte(AXP202_INTEN1)
            if (en):
                val |= val1
            else:
                val &= ~(val1)
            self.write_byte(AXP202_INTEN1, val)

        if (params & 0xFF00):
            val1 = params >> 8
            val = self.read_byte(AXP202_INTEN2)
            if (en):
                val |= val1
            else:
                val &= ~(val1)
            self.write_byte(AXP202_INTEN2, val)

        if (params & 0xFF0000):
            val1 = params >> 16
            val = self.read_byte(AXP202_INTEN3)
            if (en):
                val |= val1
            else:
                val &= ~(val1)
            self.write_byte(AXP202_INTEN3, val)

        if (params & 0xFF000000):
            val1 = params >> 24
            val = self.read_byte(AXP202_INTEN4)
            if (en):
                val |= val1
            else:
                val &= ~(val1)
            self.write_byte(AXP202_INTEN4, val)

        if (params & 0xFF00000000):
            val1 = params >> 32
            val = self.read_byte(AXP202_INTEN5)
            if (en):
                val |= val1
            else:
                val &= ~(val1)
            self.write_byte(AXP202_INTEN5, val)
        return AXP_PASS

    def readIRQ(self):
        if self._chip_id == AXP202_CHIP_ID:
            for i in range(5):
                self._irq[i] = self.read_byte(AXP202_INTSTS1 + i)
            return AXP_PASS
        return AXP_FAIL

    def clearIRQ(self):
        if self._chip_id == AXP202_CHIP_ID:
            for i in range(5):
                self.write_byte(AXP202_INTSTS1 + i, 0xFF)
        for i in range(len(self._irq)):
            self._irq[i] = 0

    def isAcinOverVoltageIRQ(self):
        return bool(self._irq[0] & BIT_MASK(7))

    def isAcinPlugInIRQ(self):
        return bool((self._irq[0] & BIT_MASK(6)))

    def isAcinRemoveIRQ(self):
        return bool(self._irq[0] & BIT_MASK(5))

    def isVbusOverVoltageIRQ(self):
        return bool(self._irq[0] & BIT_MASK(4))

    def isVbusPlugInIRQ(self):
        return bool(self._irq[0] & BIT_MASK(3))

    def isVbusRemoveIRQ(self):
        return bool(self._irq[0] & BIT_MASK(2))

    def isVbusLowVHOLDIRQ(self):
        return bool(self._irq[0] & BIT_MASK(1))

    def isBattPlugInIRQ(self):
        return bool(self._irq[1] & BIT_MASK(7))

    def isBattRemoveIRQ(self):
        return bool(self._irq[1] & BIT_MASK(6))

    def isBattEnterActivateIRQ(self):
        return bool(self._irq[1] & BIT_MASK(5))

    def isBattExitActivateIRQ(self):
        return bool(self._irq[1] & BIT_MASK(4))

    def isChargingIRQ(self):
        return bool((self._irq[1] & BIT_MASK(3)))

    def isChargingDoneIRQ(self):
        return bool(self._irq[1] & BIT_MASK(2))

    def isBattTempLowIRQ(self):
        return bool(self._irq[1] & BIT_MASK(1))

    def isBattTempHighIRQ(self):
        return bool(self._irq[1] & BIT_MASK(0))

    def isPEKShortPressIRQ(self):
        return bool(self._irq[2] & BIT_MASK(1))

    def isPEKLongtPressIRQ(self):
        return bool(self._irq[2] & BIT_MASK(0))

    def isTimerTimeoutIRQ(self):
        return bool(self._irq[4] & BIT_MASK(7))

    def isVBUSPlug(self):
        reg = self.read_byte(AXP202_STATUS)
        return IS_OPEN(reg, 5)

    def setDCDC2Voltage(self, mv):
        if (mv < 700):
            mv = 700
        elif (mv > 2275):
            mv = 2275
        val = (mv - 700) / 25
        self.write_byte(AXP202_DC2OUT_VOL, int(val))
        return AXP_PASS

    def getDCDC2Voltage(self):
        val = 0
        val = self.read_byte(AXP202_DC2OUT_VOL)
        return val * 25 + 700

    def getDCDC3Voltage(self):
        val = 0
        val = self.read_byte(AXP202_DC3OUT_VOL)
        return val * 25 + 700

    def setDCDC3Voltage(self, mv):
        if (mv < 700):
            mv = 700
        elif (mv > 3500):
            mv = 3500
        val = (mv - 700) / 25
        self.write_byte(AXP202_DC3OUT_VOL, int(val))
        return AXP_PASS

    def setLDO2Voltage(self, mv):
        if (mv < 1800):
            mv = 1800
        elif (mv > 3300):
            mv = 3300
        val = (mv - 1800) / 100
        prev = self.read_byte(AXP202_LDO24OUT_VOL)
        prev &= 0x0F
        prev |= int(val) << 4
        self.write_byte(AXP202_LDO24OUT_VOL, int(prev))
        return AXP_PASS

    def getLDO2Voltage(self):
        rval = self.read_byte(AXP202_LDO24OUT_VOL)
        rval &= 0xF0
        rval >>= 4
        return rval * 100 + 1800

    def setLDO3Voltage(self, mv):
        if self._chip_id == AXP202_CHIP_ID and mv < 700:
            mv = 700
        elif self._chip_id == AXP192_CHIP_ID and mv < 1800:
            mv = 1800

        if self._chip_id == AXP202_CHIP_ID and mv > 3500:
            mv = 3500
        elif self._chip_id == AXP192_CHIP_ID and mv > 3300:
            mv = 3300

        if self._chip_id == AXP202_CHIP_ID:
            val = (mv - 700) / 25
            prev = self.read_byte(AXP202_LDO3OUT_VOL)
            prev &= 0x80
            prev |= int(val)
            self.write_byte(AXP202_LDO3OUT_VOL, int(prev))
            return AXP_PASS
            # self.write_byte(AXP202_LDO3OUT_VOL, int(val))
        elif self._chip_id == AXP192_CHIP_ID:
            val = (mv - 1800) / 100
            prev = self.read_byte(AXP192_LDO23OUT_VOL)
            prev &= 0xF0
            prev |= int(val)
            self.write_byte(AXP192_LDO23OUT_VOL, int(prev))

    def getLDO3Voltage(self):
        rVal = self.read_byte(AXP202_LDO3OUT_VOL)
        if rVal & 0x80:
            # //! According to the hardware N_VBUSEN Pin selection
            return self.getVbusVoltage() * 1000
        else:
            return (rVal & 0x7F) * 25 + 700

    def setLDO4Voltage(self, arg):
        data = self.read_byte(AXP202_LDO24OUT_VOL)
        data = data & 0xF0
        data = data | arg
        self.write_byte(AXP202_LDO24OUT_VOL, data)
        return AXP_PASS

    def getLDO4Voltage(self):
        ldo4_table = [1250, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, 2500, 2700, 2800, 3000, 3100, 3200, 3300]
        val = self.read_byte(AXP202_LDO24OUT_VOL)
        val &= 0xF
        return ldo4_table[val]

    def setLDO3Mode(self, mode):
        if mode > AXP202_LDO3_MODE_DCIN:
            return
        data = self.read_byte(AXP202_LDO3OUT_VOL)
        if mode:
            data = data | self.__BIT_MASK(7)
        else:
            data = data & (~self.__BIT_MASK(7))
        self.write_byte(AXP202_LDO3OUT_VOL, data)
        return AXP_PASS

    def setStartupTime(self, val):
        startupParams = (
            0b00000000,
            0b01000000,
            0b10000000,
            0b11000000)
        if (val > AXP202_STARTUP_TIME_2S):
            return
        data = self.read_byte(AXP202_POK_SET)
        data = data & (~startupParams[3])
        data = data | startupParams[val]
        self.write_byte(AXP202_POK_SET, data)
        return AXP_PASS

    def setlongPressTime(self, val):
        longPressParams = (
            0b00000000,
            0b00010000,
            0b00100000,
            0b00110000)
        if (val > AXP202_LONGPRESS_TIME_2S5):
            return
        data = self.read_byte(AXP202_POK_SET)
        data = data & (~longPressParams[3])
        data = data | longPressParams[val]
        self.write_byte(AXP202_POK_SET, data)
        return AXP_PASS

    def setShutdownTime(self, val):
        shutdownParams = (
            0b00000000,
            0b00000001,
            0b00000010,
            0b00000011)
        if (val > AXP202_SHUTDOWN_TIME_10S):
            return
        data = self.read_byte(AXP202_POK_SET)
        data = data & (~shutdownParams[3])
        data = data | shutdownParams[val]
        self.write_byte(AXP202_POK_SET, data)
        return AXP_PASS

    def setTimeOutShutdown(self, en):
        data = self.read_byte(AXP202_POK_SET)
        if (en):
            data = data | self.__BIT_MASK(3)
        else:
            data = data | (~self.__BIT_MASK(3))
        self.write_byte(AXP202_POK_SET, data)
        return AXP_PASS

    def shutdown(self):
        data = self.read_byte(AXP202_OFF_CTL)
        data = data | self.__BIT_MASK(7)
        self.write_byte(AXP202_OFF_CTL, data)
        return AXP_PASS

    def getSettingChargeCurrent(self):
        data = self.read_byte(AXP202_CHARGE1)
        data = data & 0b00000111
        curr = 300 + data * 100
        return curr

    def isChargeingEnable(self):
        data = self.read_byte(AXP202_CHARGE1)
        if (data & self.__BIT_MASK(7)):
            return True
        return False

    def enableChargeing(self):
        data = self.read_byte(AXP202_CHARGE1)
        data = data | self.__BIT_MASK(7)
        self.write_byte(AXP202_CHARGE1, data)

    def setChargingTargetVoltage(self, val):
        targetVolParams = (
            0b00000000,
            0b00100000,
            0b01000000,
            0b01100000)
        if (val > AXP202_TARGET_VOL_4_36V):
            return
        data = self.read_byte(AXP202_CHARGE1)
        data = data & (~targetVolParams[3])
        data = data | targetVolParams[val]
        self.write_byte(AXP202_CHARGE1, data)

    def getBattPercentage(self):
        if self.isBatteryConnect() is False:
            return 0

        val = self.read_byte(AXP202_BATT_PERCENTAGE)
        if val & BIT_MASK(7) is False:
            return val & (~BIT_MASK(7))
        return 0

    def setChgLEDChgControl(self):
        data = self.read_byte(AXP202_OFF_CTL)
        data = data & 0b111110111
        self.write_byte(AXP202_OFF_CTL, data)

    def setChgLEDMode(self, mode):
        data = self.read_byte(AXP202_OFF_CTL)
        data |= self.__BIT_MASK(3)
        if (mode == AXP20X_LED_OFF):
            data = data & 0b11001111
        elif (mode == AXP20X_LED_BLINK_1HZ):
            data = data & 0b11001111
            data = data | 0b00010000
        elif (mode == AXP20X_LED_BLINK_4HZ):
            data = data & 0b11001111
            data = data | 0b00100000
        elif (mode == AXP20X_LED_LOW_LEVEL):
            data = data & 0b11001111
            data = data | 0b00110000
        self.write_byte(AXP202_OFF_CTL, data)

    def limitingOff(self):
        val = self.read_byte(AXP202_IPS_SET)
        val |= 0x03
        self.write_byte(AXP202_IPS_SET, val)

    def setTimer(self, minutes):
        if (minutes > 63):
            return AXP_ARG_INVALID
        self.write_byte(AXP202_TIMER_CTL, minutes)
        return AXP_PASS

    def offTimer(self):
        minutes = 0x80
        self.write_byte(AXP202_TIMER_CTL, minutes)
        return AXP_PASS

    def clearTimerStatus(self):
        val = self.read_byte(AXP202_TIMER_CTL)
        val |= 0x80
        self.write_byte(AXP202_TIMER_CTL, val)
        return AXP_PASS

    def _axp202_gpio_0_select(self, mode):
        return {
            mode == AXP_IO_OUTPUT_LOW_MODE: 0,
            mode == AXP_IO_OUTPUT_HIGH_MODE: 1,
            mode == AXP_IO_INPUT_MODE: 2,
            mode == AXP_IO_LDO_MODE: 3,
            mode == AXP_IO_ADC_MODE: 4,
        }[True]

    def _axp202_gpio_1_select(self, mode):
        return {
            mode == AXP_IO_OUTPUT_LOW_MODE: 0,
            mode == AXP_IO_OUTPUT_HIGH_MODE: 1,
            mode == AXP_IO_INPUT_MODE: 2,
            mode == AXP_IO_ADC_MODE: 4,
        }[True]

    def _axp202_gpio_2_select(self, mode):
        return {
            mode == AXP_IO_OUTPUT_LOW_MODE: 0,
            mode == AXP_IO_FLOATING_MODE: 1,
            mode == AXP_IO_INPUT_MODE: 2,
        }[True]

    def _axp202_gpio_3_select(self, mode):
        return {
            mode == AXP_IO_OPEN_DRAIN_OUTPUT_MODE: 0,
            mode == AXP_IO_INPUT_MODE: 1,
        }[True]

    def _axp202_gpio_set(self, gpio, mode):
        if gpio == AXP_GPIO_0:
            rslt = self._axp202_gpio_0_select(mode)
            if (rslt < 0): return rslt
            val = self.read_byte(AXP202_GPIO0_CTL)
            val &= 0b11111000
            val |= rslt
            self.write_byte(AXP202_GPIO0_CTL, val)
            return AXP_PASS
        elif gpio == AXP_GPIO_1:
            rslt = self._axp202_gpio_1_select(mode)
            if (rslt < 0): return rslt
            val = self.read_byte(AXP202_GPIO1_CTL)
            val &= 0b11111000
            val |= rslt
            self.write_byte(AXP202_GPIO1_CTL, val)
            return AXP_PASS
        elif gpio == AXP_GPIO_2:
            rslt = self._axp202_gpio_2_select(mode)
            if (rslt < 0): return rslt
            val = self.read_byte(AXP202_GPIO2_CTL)
            val &= 0b11111000
            val |= rslt
            self.write_byte(AXP202_GPIO2_CTL, val)
            return AXP_PASS
        elif gpio == AXP_GPIO_2:
            rslt = self._axp202_gpio_3_select(mode)
            if (rslt < 0): return rslt
            val = self.read_byte(AXP202_GPIO3_CTL)
            if rslt == 1:
                val = val | self.__BIT_MASK(2)
            else:
                val = val & (~BIT_MASK(2))
            self.write_byte(AXP202_GPIO3_CTL, val)
            return AXP_PASS

    def setGPIOMode(self, gpio, mode):
        return self._axp202_gpio_set(gpio, mode)

    def _axp_irq_mask(self, irq):
        return {
            irq == AXP_IRQ_NONE: 0,
            irq == AXP_IRQ_RISING: BIT_MASK(7),
            irq == AXP_IRQ_FALLING: BIT_MASK(6),
            irq == AXP_IRQ_DOUBLE_EDGE: 0b1100000,
        }[True]

    def _axp202_gpio_irq_set(self, gpio, irq):
        mask = self._axp_irq_mask(irq)
        if mask < 0: return mask
        reg = {
            gpio == AXP_GPIO_0: AXP202_GPIO0_CTL,
            gpio == AXP_GPIO_1: AXP202_GPIO1_CTL,
            gpio == AXP_GPIO_2: AXP202_GPIO2_CTL,
            gpio == AXP_GPIO_3: AXP202_GPIO3_CTL,
        }[True]
        val = self.read_byte(reg)
        if mask == 0:
            val = (val & 0b00111111)
        else:
            val = val | mask
        self.write_byte(reg, val)
        return AXP_PASS

    def setGPIOIrq(self, gpio, irq):
        return self._axp202_gpio_irq_set(gpio, irq)

    def setLDO5Voltage(self, vol):
        params = [
            0b11111000,  # 1.8V
            0b11111001,  # //2.5V
            0b11111010,  # //2.8V
            0b11111011,  # //3.0V
            0b11111100,  # //3.1V
            0b11111101,  # //3.3V
            0b11111110,  # //3.4V
            0b11111111,  # //3.5V
        ]
        if vol > params.__sizeof__() / params[0].__sizeof__():
            return AXP_ARG_INVALID
        val = self.read_byte(AXP202_GPIO0_VOL)
        val &= 0b11111000
        val |= params[vol]
        self.write_byte(AXP202_GPIO0_VOL, val)
        return AXP_PASS

    def _axp202_gpio_write(self, gpio, val):
        reg = {
            gpio == AXP_GPIO_0: AXP202_GPIO0_CTL,
            gpio == AXP_GPIO_1: AXP202_GPIO1_CTL,
            gpio == AXP_GPIO_2: AXP202_GPIO2_CTL,
            gpio == AXP_GPIO_3: AXP202_GPIO3_CTL,
        }[True]
        if reg == AXP202_GPIO2_CTL:
            if val: return AXP_NOT_SUPPORT
        if reg == AXP202_GPIO3_CTL:
            if val: return AXP_NOT_SUPPORT
            wval = self.read_byte(AXP202_GPIO3_CTL)
            wval &= 0b11111101
            self.write_byte(AXP202_GPIO3_CTL, wval)
            return AXP_PASS
        wVal = self.read_byte(reg)
        wVal = val if val(wVal | 1) else (wVal & 0b11111000)
        self.write_byte(reg, wVal)

    def _axp202_gpio_read(self, gpio):
        reg = AXP202_GPIO012_SIGNAL
        offset = {
            gpio == AXP_GPIO_0: 4,
            gpio == AXP_GPIO_1: 5,
            gpio == AXP_GPIO_2: 6,
            gpio == AXP_GPIO_3: 0,
        }[True]
        if offset == 0:
            reg = AXP202_GPIO3_CTL
        val = self.read_byte(reg)
        if val & BIT_MASK(offset):
            return 1
        else:
            return 0

    def gpioWrite(self, gpio, val):
        return self._axp202_gpio_write(gpio, val)

    def gpioRead(self, gpio):
        return self._axp202_gpio_read(gpio)

    def getChargeControlCur(self):
        val = self.read_byte(AXP202_CHARGE1)
        val &= 0x0F
        cur = val * 100 + 300
        if cur > 1800 or cur < 300: return 0
        return cur

    def setChargeControlCur(self, mA):
        val = self.read_byte(AXP202_CHARGE1)
        val &= 0b11110000
        mA -= 300
        val |= int((mA / 100))
        self.write_byte(AXP202_CHARGE1, val)
        return AXP_PASS
