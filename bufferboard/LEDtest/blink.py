#!/usr/bin/python3

import sys
from time import sleep
from quick2wire.gpio import Pin

pin = Pin(int(sys.argv[1]) if len(sys.argv) > 1 else 12, Pin.Out)
print pin

try:
    pin.value = 1
    while True:
        sleep(1)
        pin.value = 1 - pin.value
except KeyboardInterrupt:
    pin.value = 0
    pin.unexport()

