# Copyright 2020 LeMaRiva|tech lemariva.com
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#         http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
from machine import Pin, SPI
from sx127x  import SX127x
from ssd1306 import Display


class Lora(SX127x):
    pins_config = { 'miso'  : 19,
                    'mosi'  : 27,
                    'ss'    : 18,
                    'sck'   : 5,
                    'dio_0' : 26,
                    'reset' : 14,
                    'led'   : 2, 
    }
    
    
    lora_config = { 'frequency'       : 868E6, 
                    'tx_power_level'  : 2, 
                    'signal_bandwidth': 125E3, 
                    'spreading_factor': 8, 
                    'coding_rate'     : 5, 
                    'preamble_length' : 8,
                    'implicit_header' : False, 
                    'sync_word'       : 0x12, 
                    'enable_CRC'      : False,
                    'invert_IQ'       : False, }


    def __init__(self,
                 parameters = lora_config,
                 pins       = pins_config):
        self._spi = SPI(baudrate = 10000000, 
                        polarity = 0,
                        phase    = 0,
                        bits     = 8,
                        firstbit = SPI.MSB,
                        sck      = Pin(pins['sck'],  Pin.OUT, Pin.PULL_DOWN),
                        mosi     = Pin(pins['mosi'], Pin.OUT, Pin.PULL_UP),
                        miso     = Pin(pins['miso'], Pin.IN,  Pin.PULL_UP))
        super().__init__(self._spi, pins, parameters)


class Bios:
    def __init__(self,
                 lora_config = Lora.lora_config):
        self.lora    = Lora(lora_config)
        self.display = Display()
