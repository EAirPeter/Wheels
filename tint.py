from math import *

def u(a, b):
    r = ''
    for i in range(a):
        r = r + b
    return int(r, 16)

x = u(3, '10000100')
y = u(2, 'FDEEEFFE')

print('x + y: ' + hex(x + y))
print('x - y: ' + hex(x - y))
print('x * y: ' + hex(x * y))
