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

# Chip Address
AXP202_SLAVE_ADDRESS             = const(0x35)
AXP192_SLAVE_ADDRESS             = const(0x34)

# Chip ID
AXP202_CHIP_ID                   = const(0x41)
AXP192_CHIP_ID                   = const(0x03)

# REG MAP
AXP202_STATUS                    = const(0x00)
AXP202_MODE_CHGSTATUS            = const(0x01)
AXP202_OTG_STATUS                = const(0x02)
AXP202_IC_TYPE                   = const(0x03)
AXP202_DATA_BUFFER1              = const(0x04)
AXP202_DATA_BUFFER2              = const(0x05)
AXP202_DATA_BUFFER3              = const(0x06)
AXP202_DATA_BUFFER4              = const(0x07)
AXP202_DATA_BUFFER5              = const(0x08)
AXP202_DATA_BUFFER6              = const(0x09)
AXP202_DATA_BUFFER7              = const(0x0A)
AXP202_DATA_BUFFER8              = const(0x0B)
AXP202_DATA_BUFFER9              = const(0x0C)
AXP202_DATA_BUFFERA              = const(0x0D)
AXP202_DATA_BUFFERB              = const(0x0E)
AXP202_DATA_BUFFERC              = const(0x0F)
AXP202_LDO234_DC23_CTL           = const(0x12)
AXP202_DC2OUT_VOL                = const(0x23)
AXP202_LDO3_DC2_DVM              = const(0x25)
AXP202_DC3OUT_VOL                = const(0x27)
AXP202_LDO24OUT_VOL              = const(0x28)
AXP202_LDO3OUT_VOL               = const(0x29)
AXP202_IPS_SET                   = const(0x30)
AXP202_VOFF_SET                  = const(0x31)
AXP202_OFF_CTL                   = const(0x32)
AXP202_CHARGE1                   = const(0x33)
AXP202_CHARGE2                   = const(0x34)
AXP202_BACKUP_CHG                = const(0x35)
AXP202_POK_SET                   = const(0x36)
AXP202_DCDC_FREQSET              = const(0x37)
AXP202_VLTF_CHGSET               = const(0x38)
AXP202_VHTF_CHGSET               = const(0x39)
AXP202_APS_WARNING1              = const(0x3A)
AXP202_APS_WARNING2              = const(0x3B)
AXP202_TLTF_DISCHGSET            = const(0x3C)
AXP202_THTF_DISCHGSET            = const(0x3D)
AXP202_DCDC_MODESET              = const(0x80)
AXP202_ADC_EN1                   = const(0x82)
AXP202_ADC_EN2                   = const(0x83)
AXP202_ADC_SPEED                 = const(0x84)
AXP202_ADC_INPUTRANGE            = const(0x85)
AXP202_ADC_IRQ_RETFSET           = const(0x86)
AXP202_ADC_IRQ_FETFSET           = const(0x87)
AXP202_TIMER_CTL                 = const(0x8A)
AXP202_VBUS_DET_SRP              = const(0x8B)
AXP202_HOTOVER_CTL               = const(0x8F)
AXP202_GPIO0_CTL                 = const(0x90)
AXP202_GPIO0_VOL                 = const(0x91)
AXP202_GPIO1_CTL                 = const(0x92)
AXP202_GPIO2_CTL                 = const(0x93)
AXP202_GPIO012_SIGNAL            = const(0x94)
AXP202_GPIO3_CTL                 = const(0x95)
AXP202_INTEN1                    = const(0x40)
AXP202_INTEN2                    = const(0x41)
AXP202_INTEN3                    = const(0x42)
AXP202_INTEN4                    = const(0x43)
AXP202_INTEN5                    = const(0x44)
AXP202_INTSTS1                   = const(0x48)
AXP202_INTSTS2                   = const(0x49)
AXP202_INTSTS3                   = const(0x4A)
AXP202_INTSTS4                   = const(0x4B)
AXP202_INTSTS5                   = const(0x4C)

# Irq control register
AXP192_INTEN1                    = const(0x40)
AXP192_INTEN2                    = const(0x41)
AXP192_INTEN3                    = const(0x42)
AXP192_INTEN4                    = const(0x43)
AXP192_INTEN5                    = const(0x4A)

# Irq status register
AXP192_INTSTS1                   = const(0x44)
AXP192_INTSTS2                   = const(0x45)
AXP192_INTSTS3                   = const(0x46)
AXP192_INTSTS4                   = const(0x47)
AXP192_INTSTS5                   = const(0x4D)

AXP192_DC1_VLOTAGE               = const(0x26)

# axp 20 adc data register
AXP202_BAT_AVERVOL_H8            = const(0x78)
AXP202_BAT_AVERVOL_L4            = const(0x79)
AXP202_BAT_AVERCHGCUR_H8         = const(0x7A)
AXP202_BAT_AVERCHGCUR_L4         = const(0x7B)
AXP202_BAT_VOL_H8                = const(0x50)
AXP202_BAT_VOL_L4                = const(0x51)
AXP202_ACIN_VOL_H8               = const(0x56)
AXP202_ACIN_VOL_L4               = const(0x57)
AXP202_ACIN_CUR_H8               = const(0x58)
AXP202_ACIN_CUR_L4               = const(0x59)
AXP202_VBUS_VOL_H8               = const(0x5A)
AXP202_VBUS_VOL_L4               = const(0x5B)
AXP202_VBUS_CUR_H8               = const(0x5C)
AXP202_VBUS_CUR_L4               = const(0x5D)
AXP202_INTERNAL_TEMP_H8          = const(0x5E)
AXP202_INTERNAL_TEMP_L4          = const(0x5F)
AXP202_TS_IN_H8                  = const(0x62)
AXP202_TS_IN_L4                  = const(0x63)
AXP202_GPIO0_VOL_ADC_H8          = const(0x64)
AXP202_GPIO0_VOL_ADC_L4          = const(0x65)
AXP202_GPIO1_VOL_ADC_H8          = const(0x66)
AXP202_GPIO1_VOL_ADC_L4          = const(0x67)

AXP202_BAT_AVERDISCHGCUR_H8      = const(0x7C)
AXP202_BAT_AVERDISCHGCUR_L5      = const(0x7D)
AXP202_APS_AVERVOL_H8            = const(0x7E)
AXP202_APS_AVERVOL_L4            = const(0x7F)
AXP202_INT_BAT_CHGCUR_H8         = const(0xA0)
AXP202_INT_BAT_CHGCUR_L4         = const(0xA1)
AXP202_EXT_BAT_CHGCUR_H8         = const(0xA2)
AXP202_EXT_BAT_CHGCUR_L4         = const(0xA3)
AXP202_INT_BAT_DISCHGCUR_H8      = const(0xA4)
AXP202_INT_BAT_DISCHGCUR_L4      = const(0xA5)
AXP202_EXT_BAT_DISCHGCUR_H8      = const(0xA6)
AXP202_EXT_BAT_DISCHGCUR_L4      = const(0xA7)
AXP202_BAT_CHGCOULOMB3           = const(0xB0)
AXP202_BAT_CHGCOULOMB2           = const(0xB1)
AXP202_BAT_CHGCOULOMB1           = const(0xB2)
AXP202_BAT_CHGCOULOMB0           = const(0xB3)
AXP202_BAT_DISCHGCOULOMB3        = const(0xB4)
AXP202_BAT_DISCHGCOULOMB2        = const(0xB5)
AXP202_BAT_DISCHGCOULOMB1        = const(0xB6)
AXP202_BAT_DISCHGCOULOMB0        = const(0xB7)
AXP202_COULOMB_CTL               = const(0xB8)
AXP202_BAT_POWERH8               = const(0x70)
AXP202_BAT_POWERM8               = const(0x71)
AXP202_BAT_POWERL8               = const(0x72)

AXP202_VREF_TEM_CTRL             = const(0xF3)
AXP202_BATT_PERCENTAGE           = const(0xB9)

# AXP202   bit definitions for AXP events irq event
AXP202_IRQ_USBLO                 = const(1)
AXP202_IRQ_USBRE                 = const(2)
AXP202_IRQ_USBIN                 = const(3)
AXP202_IRQ_USBOV                 = const(4)
AXP202_IRQ_ACRE                  = const(5)
AXP202_IRQ_ACIN                  = const(6)
AXP202_IRQ_ACOV                  = const(7)

AXP202_IRQ_TEMLO                 = const(8)
AXP202_IRQ_TEMOV                 = const(9)
AXP202_IRQ_CHAOV                 = const(10)
AXP202_IRQ_CHAST                 = const(11)
AXP202_IRQ_BATATOU               = const(12)
AXP202_IRQ_BATATIN               = const(13)
AXP202_IRQ_BATRE                 = const(14)
AXP202_IRQ_BATIN                 = const(15)

AXP202_IRQ_POKLO                 = const(16)
AXP202_IRQ_POKSH                 = const(17)
AXP202_IRQ_LDO3LO                = const(18)
AXP202_IRQ_DCDC3LO               = const(19)
AXP202_IRQ_DCDC2LO               = const(20)
AXP202_IRQ_CHACURLO              = const(22)
AXP202_IRQ_ICTEMOV               = const(23)

AXP202_IRQ_EXTLOWARN2            = const(24)
AXP202_IRQ_EXTLOWARN1            = const(25)
AXP202_IRQ_SESSION_END           = const(26)
AXP202_IRQ_SESS_AB_VALID         = const(27)
AXP202_IRQ_VBUS_UN_VALID         = const(28)
AXP202_IRQ_VBUS_VALID            = const(29)
AXP202_IRQ_PDOWN_BY_NOE          = const(30)
AXP202_IRQ_PUP_BY_NOE            = const(31)

AXP202_IRQ_GPIO0TG               = const(32)
AXP202_IRQ_GPIO1TG               = const(33)
AXP202_IRQ_GPIO2TG               = const(34)
AXP202_IRQ_GPIO3TG               = const(35)
AXP202_IRQ_PEKFE                 = const(37)
AXP202_IRQ_PEKRE                 = const(38)
AXP202_IRQ_TIMER                 = const(39)

# Signal Capture
AXP202_BATT_VOLTAGE_STEP         = 1.1
AXP202_BATT_DISCHARGE_CUR_STEP   = 0.5
AXP202_BATT_CHARGE_CUR_STEP      = 0.5
AXP202_ACIN_VOLTAGE_STEP         = 1.7
AXP202_ACIN_CUR_STEP             = 0.625
AXP202_VBUS_VOLTAGE_STEP         = 1.7
AXP202_VBUS_CUR_STEP             = 0.375
AXP202_INTENAL_TEMP_STEP         = 0.1
AXP202_APS_VOLTAGE_STEP          = 1.4
AXP202_TS_PIN_OUT_STEP           = 0.8
AXP202_GPIO0_STEP                = 0.5
AXP202_GPIO1_STEP                = 0.5

# axp202 power channel
AXP202_EXTEN                     = const(0)
AXP202_DCDC3                     = const(1)
AXP202_LDO2                      = const(2)
AXP202_LDO4                      = const(3)
AXP202_DCDC2                     = const(4)
AXP202_LDO3                      = const(6)

# axp192 power channel
AXP192_DCDC1                     = const(0)
AXP192_DCDC3                     = const(1)
AXP192_LDO2                      = const(2)
AXP192_LDO3                      = const(3)
AXP192_DCDC2                     = const(4)
AXP192_EXTEN                     = const(6)

# AXP202 ADC channel
AXP202_ADC1                      = const(1)
AXP202_ADC2                      = const(2)

# axp202 adc1 args
AXP202_BATT_VOL_ADC1             = const(7)
AXP202_BATT_CUR_ADC1             = const(6)
AXP202_ACIN_VOL_ADC1             = const(5)
AXP202_ACIN_CUR_ADC1             = const(4)
AXP202_VBUS_VOL_ADC1             = const(3)
AXP202_VBUS_CUR_ADC1             = const(2)
AXP202_APS_VOL_ADC1              = const(1)
AXP202_TS_PIN_ADC1               = const(0)

# axp202 adc2 args
AXP202_TEMP_MONITORING_ADC2      = const(7)
AXP202_GPIO1_FUNC_ADC2           = const(3)
AXP202_GPIO0_FUNC_ADC2           = const(2)

# AXP202 IRQ1
AXP202_VBUS_VHOLD_LOW_IRQ        = const(1 << 1)
AXP202_VBUS_REMOVED_IRQ          = const(1 << 2)
AXP202_VBUS_CONNECT_IRQ          = const(1 << 3)
AXP202_VBUS_OVER_VOL_IRQ         = const(1 << 4)
AXP202_ACIN_REMOVED_IRQ          = const(1 << 5)
AXP202_ACIN_CONNECT_IRQ          = const(1 << 6)
AXP202_ACIN_OVER_VOL_IRQ         = const(1 << 7)

# AXP202 IRQ2
AXP202_BATT_LOW_TEMP_IRQ         = const(1 << 8)
AXP202_BATT_OVER_TEMP_IRQ        = const(1 << 9)
AXP202_CHARGING_FINISHED_IRQ     = const(1 << 10)
AXP202_CHARGING_IRQ              = const(1 << 11)
AXP202_BATT_EXIT_ACTIVATE_IRQ    = const(1 << 12)
AXP202_BATT_ACTIVATE_IRQ         = const(1 << 13)
AXP202_BATT_REMOVED_IRQ          = const(1 << 14)
AXP202_BATT_CONNECT_IRQ          = const(1 << 15)

# AXP202 IRQ3
AXP202_PEK_LONGPRESS_IRQ         = const(1 << 16)
AXP202_PEK_SHORTPRESS_IRQ        = const(1 << 17)
AXP202_LDO3_LOW_VOL_IRQ          = const(1 << 18)
AXP202_DC3_LOW_VOL_IRQ           = const(1 << 19)
AXP202_DC2_LOW_VOL_IRQ           = const(1 << 20)
AXP202_CHARGE_LOW_CUR_IRQ        = const(1 << 21)
AXP202_CHIP_TEMP_HIGH_IRQ        = const(1 << 22)

# AXP202 IRQ4
AXP202_APS_LOW_VOL_LEVEL2_IRQ    = const(1 << 24)
APX202_APS_LOW_VOL_LEVEL1_IRQ    = const(1 << 25)
AXP202_VBUS_SESSION_END_IRQ      = const(1 << 26)
AXP202_VBUS_SESSION_AB_IRQ       = const(1 << 27)
AXP202_VBUS_INVALID_IRQ          = const(1 << 28)
AXP202_VBUS_VAILD_IRQ            = const(1 << 29)
AXP202_NOE_OFF_IRQ               = const(1 << 30)
AXP202_NOE_ON_IRQ                = const(1 << 31)
AXP202_ALL_IRQ                   = const(0xFFFF)

# AXP202 LDO3 Mode
AXP202_LDO3_LDO_MODE             = const(0)
AXP202_LDO3_DCIN_MODE            = const(1)

# AXP202 LDO4 voltage setting args
AXP202_LDO4_1250MV               = const(0)
AXP202_LDO4_1300MV               = const(1)
AXP202_LDO4_1400MV               = const(2)
AXP202_LDO4_1500MV               = const(3)
AXP202_LDO4_1600MV               = const(4)
AXP202_LDO4_1700MV               = const(5)
AXP202_LDO4_1800MV               = const(6)
AXP202_LDO4_1900MV               = const(7)
AXP202_LDO4_2000MV               = const(8)
AXP202_LDO4_2500MV               = const(9)
AXP202_LDO4_2700MV               = const(10)
AXP202_LDO4_2800MV               = const(11)
AXP202_LDO4_3000MV               = const(12)
AXP202_LDO4_3100MV               = const(13)
AXP202_LDO4_3200MV               = const(14)
AXP202_LDO4_3300MV               = const(15)

# Boot time setting
AXP202_STARTUP_TIME_128MS        = const(0)
AXP202_STARTUP_TIME_3S           = const(1)
AXP202_STARTUP_TIME_1S           = const(2)
AXP202_STARTUP_TIME_2S           = const(3)

# Long button time setting
AXP202_LONGPRESS_TIME_1S         = const(0)
AXP202_LONGPRESS_TIME_1S5        = const(1)
AXP202_LONGPRESS_TIME_2S         = const(2)
AXP202_LONGPRESS_TIME_2S5        = const(3)

# Shutdown duration setting
AXP202_SHUTDOWN_TIME_4S          = const(0)
AXP202_SHUTDOWN_TIME_6S          = const(1)
AXP202_SHUTDOWN_TIME_8S          = const(2)
AXP202_SHUTDOWN_TIME_10S         = const(3)

# REG 33H: Charging control 1 Charging target-voltage setting
AXP202_TARGET_VOL_4_1V           = const(0)
AXP202_TARGET_VOL_4_15V          = const(1)
AXP202_TARGET_VOL_4_2V           = const(2)
AXP202_TARGET_VOL_4_36V          = const(3)

# AXP202 LED CONTROL
AXP20X_LED_OFF                   = const(0)
AXP20X_LED_BLINK_1HZ             = const(1)
AXP20X_LED_BLINK_4HZ             = const(2)
AXP20X_LED_LOW_LEVEL             = const(3)


def FORCED_OPEN_DCDC3(x):
    x |= (AXP202_ON << AXP202_DCDC3)
    return x


def BIT_MASK(x):
    return 1 << x


def IS_OPEN(reg, channel):
    return bool(reg & BIT_MASK(channel))


RISING                           = const(0x01)
FALLING                          = const(0x02)

AXP_PASS                         = const(0)
AXP_FAIL                         = const(-1)
AXP_INVALID                      = const(-2)
AXP_NOT_INIT                     = const(-3)
AXP_NOT_SUPPORT                  = const(-4)
AXP_ARG_INVALID                  = const(-5)

AXP173_SLAVE_ADDRESS             = const(0x34)


AXP173_CHIP_ID                   = const(0xAD)  # !Axp173 does not have a chip ID, given a custom ID

# ! Logic states
AXP202_ON                        = const(1)
AXP202_OFF                       = const(0)

AXP192_LDO23OUT_VOL              = const(0x28)
AXP192_GPIO0_CTL                 = const(0x90)
AXP192_GPIO0_VOL                 = const(0x91)
AXP192_GPIO1_CTL                 = const(0x92)
AXP192_GPIO2_CTL                 = const(0x93)
AXP192_GPIO012_SIGNAL            = const(0x94)
AXP192_GPIO34_CTL                = const(0x95)

# AXP192 only
AXP202_GPIO2_STEP                = 0.5
AXP202_GPIO3_STEP                = 0.5

# AXP173
AXP173_EXTEN_DC2_CTL             = const(0x10)
AXP173_CTL_DC2_BIT               = const(0)
AXP173_CTL_EXTEN_BIT             = const(2)
AXP173_DC1_VLOTAGE               = const(0x26)
AXP173_LDO4_VLOTAGE              = const(0x27)

AXP202_OUTPUT_MAX                = const(5)


AXP192_OUTPUT_MAX                = const(5)

AXP173_DCDC1                     = const(0)
AXP173_LDO4                      = const(1)
AXP173_LDO2                      = const(2)
AXP173_LDO3                      = const(3)
AXP173_DCDC2                     = const(4)
AXP173_EXTEN                     = const(6)
AXP173_OUTPUT_MAX                = const(5)


AXP192_STARTUP_TIME_128MS        = const(0)
AXP192_STARTUP_TIME_512MS        = const(1)
AXP192_STARTUP_TIME_1S           = const(2)
AXP192_STARTUP_TIME_2S           = const(3)

AXP_LONGPRESS_TIME_1S            = const(0)
AXP_LONGPRESS_TIME_1S5           = const(1)
AXP_LONGPRESS_TIME_2S            = const(2)
AXP_LONGPRESS_TIME_2S5           = const(3)

AXP_POWER_OFF_TIME_4S            = const(0)
AXP_POWER_OFF_TIME_65            = const(1)
AXP_POWER_OFF_TIME_8S            = const(2)
AXP_POWER_OFF_TIME_16S           = const(3)

AXP202_LDO3_MODE_LDO             = const(0)
AXP202_LDO3_MODE_DCIN            = const(1)


# ! IRQ5 REG 44H
AXP202_GPIO0_EDGE_TRIGGER_IRQ    = 1 << 32  # GPIO0 input edge trigger IRQ enable
AXP202_GPIO1_EDGE_TRIGGER_IRQ    = 1 << 33  # GPIO1input edge trigger or ADC input IRQ enable
AXP202_GPIO2_EDGE_TRIGGER_IRQ    = 1 << 34  # GPIO2input edge trigger IRQ enable
AXP202_GPIO3_EDGE_TRIGGER_IRQ    = 1 << 35  # GPIO3 input edge trigger IRQ enable
# **Reserved and unchangeable BIT 4
AXP202_PEK_FALLING_EDGE_IRQ      = 1 << 37  # PEK press falling edge IRQ enable
AXP202_PEK_RISING_EDGE_IRQ       = 1 << 38  # PEK press rising edge IRQ enable
AXP202_TIMER_TIMEOUT_IRQ         = 1 << 39  # Timer timeout IRQ enable
AXP202_LDO4_MAX                  = const(16)

AXP202_LDO5_1800MV               = const(0)
AXP202_LDO5_2500MV               = const(1)
AXP202_LDO5_2800MV               = const(2)
AXP202_LDO5_3000MV               = const(3)
AXP202_LDO5_3100MV               = const(4)
AXP202_LDO5_3300MV               = const(5)
AXP202_LDO5_3400MV               = const(6)
AXP202_LDO5_3500MV               = const(7)
AXP202_INTERNAL_TEMP_STEP        = 0.1
AXP_ADC_SAMPLING_RATE_25HZ       = const(0)
AXP_ADC_SAMPLING_RATE_50HZ       = const(1)
AXP_ADC_SAMPLING_RATE_100HZ      = const(2)
AXP_ADC_SAMPLING_RATE_200HZ      = const(3)
AXP_TS_PIN_CURRENT_20UA          = const(0)
AXP_TS_PIN_CURRENT_40UA          = const(1)
AXP_TS_PIN_CURRENT_60UA          = const(2)
AXP_TS_PIN_CURRENT_80UA          = const(3)
AXP_TS_PIN_FUNCTION_BATT         = const(0)
AXP_TS_PIN_FUNCTION_ADC          = const(1)
AXP_TS_PIN_MODE_DISABLE          = const(0)
AXP_TS_PIN_MODE_CHARGING         = const(1)
AXP_TS_PIN_MODE_SAMPLING         = const(2)
AXP_TS_PIN_MODE_ENABLE           = const(3)
AXP_GPIO_0                       = const(0)
AXP_GPIO_1                       = const(1)
AXP_GPIO_2                       = const(2)
AXP_GPIO_3                       = const(3)
AXP_GPIO_4                       = const(4)
AXP_IO_OUTPUT_LOW_MODE           = const(0)
AXP_IO_OUTPUT_HIGH_MODE          = const(1)
AXP_IO_INPUT_MODE                = const(2)
AXP_IO_LDO_MODE                  = const(3)
AXP_IO_ADC_MODE                  = const(4)
AXP_IO_FLOATING_MODE             = const(5)
AXP_IO_OPEN_DRAIN_OUTPUT_MODE    = const(6)
AXP_IO_PWM_OUTPUT_MODE           = const(7)
AXP_IO_EXTERN_CHARGING_CTRL_MODE = const(8)
AXP_IRQ_NONE                     = const(0)
AXP_IRQ_RISING                   = const(1)
AXP_IRQ_FALLING                  = const(2)
AXP_IRQ_DOUBLE_EDGE              = const(3)
AXP192_GPIO_1V8                  = const(0)
AXP192_GPIO_1V9                  = const(1)
AXP192_GPIO_2V0                  = const(2)
AXP192_GPIO_2V1                  = const(3)
AXP192_GPIO_2V2                  = const(4)
AXP192_GPIO_2V3                  = const(5)
AXP192_GPIO_2V4                  = const(6)
AXP192_GPIO_2V5                  = const(7)
AXP192_GPIO_2V6                  = const(8)
AXP192_GPIO_2V7                  = const(9)
AXP192_GPIO_2V8                  = const(10)
AXP192_GPIO_2V9                  = const(11)
AXP192_GPIO_3V0                  = const(12)
AXP192_GPIO_3V1                  = const(13)
AXP192_GPIO_3V2                  = const(14)
AXP192_GPIO_3V3                  = const(15)
AXP1XX_CHARGE_CUR_100MA          = const(0)
AXP1XX_CHARGE_CUR_190MA          = const(1)
AXP1XX_CHARGE_CUR_280MA          = const(2)
AXP1XX_CHARGE_CUR_360MA          = const(3)
AXP1XX_CHARGE_CUR_450MA          = const(4)
AXP1XX_CHARGE_CUR_550MA          = const(5)
AXP1XX_CHARGE_CUR_630MA          = const(6)
AXP1XX_CHARGE_CUR_700MA          = const(7)
AXP1XX_CHARGE_CUR_780MA          = const(8)
AXP1XX_CHARGE_CUR_880MA          = const(9)
AXP1XX_CHARGE_CUR_960MA          = const(10)
AXP1XX_CHARGE_CUR_1000MA         = const(11)
AXP1XX_CHARGE_CUR_1080MA         = const(12)
AXP1XX_CHARGE_CUR_1160MA         = const(13)
AXP1XX_CHARGE_CUR_1240MA         = const(14)
AXP1XX_CHARGE_CUR_1320MA         = const(15)




class PMU(object):
    def __init__(self,
                 i2c     : I2C,
                 address : int = None):
        self.device     = None
        self._chip_id   = AXP202_CHIP_ID
        self.address    = address if address else AXP202_SLAVE_ADDRESS

        self._outputReg = None
        self.buffer     = bytearray(16)
        self.bytebuf    = memoryview(self.buffer[0:1])
        self.wordbuf    = memoryview(self.buffer[0:2])
        self._irq       = memoryview(self.buffer[0:5])
        self.bus        = i2c
        
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
        self._chip_id = self.read_byte(AXP202_IC_TYPE)
        if self._chip_id == AXP202_CHIP_ID:
            self._chip_id = AXP202_CHIP_ID
            self._outputReg = self.read_byte(AXP202_LDO234_DC23_CTL)
        elif self._chip_id == AXP192_CHIP_ID:
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
        ldo4_table = (1250, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, 2500, 2700, 2800, 3000, 3100, 3200, 3300)
        val        = self.read_byte(AXP202_LDO24OUT_VOL)
        val       &= 0xF
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
        startupParams = (0b00000000,
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
        longPressParams = (0b00000000,
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
        shutdownParams = (0b00000000,
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
        targetVolParams = (0b00000000,
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
