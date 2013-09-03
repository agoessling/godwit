#!/usr/bin/python

import re

f = open('st7529_board.h')
s = f.read()
f.close()

pin_map = re.findall('ST7529_D(\d)_PIN\s+AVR32_PIN_P([AB])(\d{2})', s)

f = open('st7529_board_lut.h','wb')

f.write('/*************************************************\r\n')
f.write(' * FILE:    st7529_board_lut.h\r\n')
f.write(' * AUTHOR:  ADG\r\n')
f.write(' * DESC:    ST7529 Pin Mapping LUT\r\n')
f.write(' *************************************************/\r\n')

f.write('\r\nconst struct{uint32_t porta; uint32_t portb;} st7529_pin_map_lut')
f.write('[256] = {')

for i in range(256):
    if i%3 == 0:
        f.write('\r\n    ')

    porta_val = 0
    portb_val = 0

    for pin in pin_map:
        val = ((i>>int(pin[0]))&0x01)<<int(pin[2])

        if pin[1] == 'A':
            porta_val |= val
        else:
            portb_val |= val

    f.write('{{0x{:08X},0x{:08X}}},'.format(porta_val, portb_val))

f.write('\r\n};')

f.close()
