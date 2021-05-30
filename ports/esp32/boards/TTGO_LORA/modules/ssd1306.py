# https://learn.adafruit.com/micropython-hardware-ssd1306-oled-display/software
from   machine import SoftI2C, Pin
import time
import framebuf


# register definitions
SET_CONTRAST        = const(0x81)
SET_ENTIRE_ON       = const(0xa4)
SET_NORM_INV        = const(0xa6)
SET_DISP            = const(0xae)
SET_MEM_ADDR        = const(0x20)
SET_COL_ADDR        = const(0x21)
SET_PAGE_ADDR       = const(0x22)
SET_DISP_START_LINE = const(0x40)
SET_SEG_REMAP       = const(0xa0)
SET_MUX_RATIO       = const(0xa8)
SET_COM_OUT_DIR     = const(0xc0)
SET_DISP_OFFSET     = const(0xd3)
SET_COM_PIN_CFG     = const(0xda)
SET_DISP_CLK_DIV    = const(0xd5)
SET_PRECHARGE       = const(0xd9)
SET_VCOM_DESEL      = const(0xdb)
SET_CHARGE_PUMP     = const(0x8d)


class SSD1306:
    def __init__(self, width, height, external_vcc):
        self.width = width
        self.height = height
        self.external_vcc = external_vcc
        self.pages = self.height // 8
        self.buffer = bytearray(self.pages * self.width)
        fb = framebuf.FrameBuffer(self.buffer, self.width, self.height, framebuf.MONO_VLSB)
        self.framebuf = fb
        # Provide methods for accessing FrameBuffer graphics primitives. This is a
        # workround because inheritance from a native class is currently unsupported.
        # http://docs.micropython.org/en/latest/pyboard/library/framebuf.html
        self.fill = fb.fill
        self.pixel = fb.pixel
        self.hline = fb.hline
        self.vline = fb.vline
        self.line = fb.line
        self.rect = fb.rect
        self.fill_rect = fb.fill_rect
        self.text = fb.text
        self.scroll = fb.scroll
        self.blit = fb.blit
        self.poweron()
        self.init_display()

    def init_display(self):
        for cmd in (
            SET_DISP | 0x00, # off
            # address setting
            SET_MEM_ADDR, 0x00, # horizontal
            # resolution and layout
            SET_DISP_START_LINE | 0x00,
            SET_SEG_REMAP | 0x01, # column addr 127 mapped to SEG0
            SET_MUX_RATIO, self.height - 1,
            SET_COM_OUT_DIR | 0x08, # scan from COM[N] to COM0
            SET_DISP_OFFSET, 0x00,
            SET_COM_PIN_CFG, 0x02 if self.height == 32 else 0x12,
            # timing and driving scheme
            SET_DISP_CLK_DIV, 0x80,
            SET_PRECHARGE, 0x22 if self.external_vcc else 0xf1,
            SET_VCOM_DESEL, 0x30, # 0.83*Vcc
            # display
            SET_CONTRAST, 0xff, # maximum
            SET_ENTIRE_ON, # output follows RAM contents
            SET_NORM_INV, # not inverted
            # charge pump
            SET_CHARGE_PUMP, 0x10 if self.external_vcc else 0x14,
            SET_DISP | 0x01): # on
            self.write_cmd(cmd)
        self.fill(0)
        self.show()

    def poweroff(self):
        self.write_cmd(SET_DISP | 0x00)

    def contrast(self, contrast):
        self.write_cmd(SET_CONTRAST)
        self.write_cmd(contrast)

    def invert(self, invert):
        self.write_cmd(SET_NORM_INV | (invert & 1))

    def show(self):
        x0 = 0
        x1 = self.width - 1
        if self.width == 64:
            # displays with width of 64 pixels are shifted by 32
            x0 += 32
            x1 += 32
        self.write_cmd(SET_COL_ADDR)
        self.write_cmd(x0)
        self.write_cmd(x1)
        self.write_cmd(SET_PAGE_ADDR)
        self.write_cmd(0)
        self.write_cmd(self.pages - 1)
        self.write_data(self.buffer)


class SSD1306_I2C(SSD1306):
    def __init__(self, width, height, i2c, addr=0x3c, external_vcc=False):
        self.i2c = i2c
        self.addr = addr
        self.temp = bytearray(2)
        super().__init__(width, height, external_vcc)

    def write_cmd(self, cmd):
        self.temp[0] = 0x80 # Co=1, D/C#=0
        self.temp[1] = cmd
        self.i2c.writeto(self.addr, self.temp)

    def write_data(self, buf):
        self.temp[0] = self.addr << 1
        self.temp[1] = 0x40 # Co=0, D/C#=1
        self.i2c.start()
        self.i2c.write(self.temp)
        self.i2c.write(buf)
        self.i2c.stop()

    def poweron(self):
        pass


class Display:

    def __init__(self,
                 width = 128, height = 64,
                 scl_pin_id = 15, sda_pin_id = 4,
                 freq = 400000):

        self.width = width
        self.height = height
        self.poweron()
        self.i2c = SoftI2C(scl  = Pin(scl_pin_id, Pin.OUT),
                           sda  = Pin(sda_pin_id),
                           freq = freq)
        self.display = SSD1306_I2C(width, height, self.i2c)
        self.show = self.display.show

    def poweron(self, pin=16):
        pin_reset = Pin(pin, mode=Pin.OUT)
        pin_reset.value(0)
        time.sleep_ms(50)
        pin_reset.value(1)

    def poweroff(self, pin=16):
        pin_reset = Pin(pin, mode=Pin.OUT)
        pin_reset.value(0)

    def clear(self):
        self.display.fill(0)
        self.display.show()


    def show_text(self, text, x = 0, y = 0, clear_first = True, show_now = True, hold_seconds = 0):
        if clear_first: self.display.fill(0)
        self.display.text(text, x, y)
        if show_now:
            self.display.show()
            if hold_seconds > 0: time.sleep(hold_seconds)


    def wrap(self, text, start_line = 0,
             height_per_line = 8, width_per_char = 8,
             start_pixel_each_line = 0):

        chars_per_line = self.width//width_per_char
        max_lines = self.height//height_per_line - start_line
        lines = [(text[chars_per_line*line: chars_per_line*(line+1)], start_pixel_each_line, height_per_line*(line+start_line))
                 for line in range(max_lines)]

        return lines


    def show_text_wrap(self, text,
                       start_line = 0, height_per_line = 8, width_per_char = 8, start_pixel_each_line = 0,
                       clear_first = True, show_now = True, hold_seconds = 0):

        if clear_first: self.clear()

        for line, x, y in self.wrap(text, start_line, height_per_line, width_per_char, start_pixel_each_line):
            self.show_text(line, x, y, clear_first = False, show_now = False)

        if show_now:
            self.display.show()
            if hold_seconds > 0: time.sleep(hold_seconds)


    def show_datetime(self, year, month, day, hour, minute, second):
        datetime = [year, month, day, hour, minute, second]
        datetime_str = ["{0:0>2}".format(d) for d in datetime]

        self.show_text(text = '-'.join(datetime_str[:3]),
                        x = 0, y = 0, clear_first = True, show_now = False)
        self.show_text(text = ':'.join(datetime_str[3:6]),
                        x = 0, y = 10, clear_first = False, show_now = True)


    def show_time(self, year, month, day, hour, minute, second):
        self.show_datetime(year, month, day, hour, minute, second)
