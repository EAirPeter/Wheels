from math import *

def u(a, b):
    r = ''
    for i in range(a):
        r = r + b
    return int(r, 16)

xi = raw_input().split(' ')
yi = raw_input().split(' ')

x = u(int(xi[0]), str(xi[1]))
y = u(int(yi[0]), str(yi[1]))

print('x + y: ' + hex(x + y))
print('x - y: ' + hex(x - y))
print('x * y: ' + hex(x * y))
print('x / y: ' + hex(x / y))
