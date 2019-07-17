#!/bin/env python

import sys

def convert_to_utf(value):
    result = 0
    if value > 0x10FFFF:
        raise ValueError("Cannot encode value {}, it is too large to fit.".format(value))
    elif value > 0xFFFF:
        a = ((value >> 18) & 0x07) | 0xF0
        b = ((value >> 12) & 0x3F) | 0x80
        c = ((value >>  6) & 0x3F) | 0x80
        d = ((value >>  0) & 0x3F) | 0x80
        result = (a << 24) | (b << 16) | (c << 8) | d
    elif value > 0x7FF:
        a = ((value >> 12) & 0x0F) | 0xE0
        b = ((value >>  6) & 0x3F) | 0x80
        c = ((value >>  0) & 0x3F) | 0x80
        result = (a << 16) | (b << 8) | c
    elif value > 0x7F:
        a = ((value >>  6) & 0x1F) | 0xC0
        b = ((value >>  0) & 0x3F) | 0x80
        result = (a << 8) | b
    else:
        a = (value >> 0) & 0x7F
        assert(a == value)
        result = value
    return result


if __name__ == '__main__':
    value = 0xBE20
    if len(sys.argv) > 1:
        value = int(sys.argv[1], 0)
    print(hex(convert_to_utf(value)))
