from random import *

N = 20 # randint(1, 10)
M = 10  # randint(1, 10)

print(N)
print(M)

E = []

i = 0
while i < M:
    x = randint(1, N)
    y = randint(1, N)
    if x != y and (x, y) not in E:
        print(x, y)
        i += 1
