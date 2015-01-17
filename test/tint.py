from math import *

def u(a, b):
    r = ''
    for i in range(a):
        r = r + b
    return int(r, 16)

xi = int(raw_input(), 16)#.split(' ')
yi = int(raw_input(), 16)#.split(' ')

x = xi#u(int(xi[0]), str(xi[1]))
y = yi#u(int(yi[0]), str(yi[1]))

print('x + y: ' + hex(x + y))
print('x - y: ' + hex(x - y))
print('x * y: ' + hex(x * y))
print('x / y: ' + hex(x / y))
