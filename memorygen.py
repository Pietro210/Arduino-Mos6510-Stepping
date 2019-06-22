#!/usr/bin/env python3
# usage: cat blink.o | ./memorygen.py > 6510-Stepping/memory.h

import sys

hexbytes = []

for byte in sys.stdin.buffer.read():
    hexbytes.append('0x%02x' % byte)

print('#ifndef MEMORY_H')
print('#define MEMORY_H')
print('uint8_t memory[%d] = {%s};' % (len(hexbytes), ','.join(hexbytes)))
print('#endif')
