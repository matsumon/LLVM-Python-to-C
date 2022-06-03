# line 399 hangs if it doesn't run, maybe block?
# not returning i1 for condition in branches
a = True
b = False
x = 7
y = 1
if a:
    x = 5
    if b:
        y = 4
    else:
        y = 2

z = (x * 3 * 7) / y
