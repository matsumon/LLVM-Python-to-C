# line 399 hangs if it doesn't run, maybe block?
# not returning i1 for condition in branches
pi = 3.1415
r = 3
v = 0
if 26 <= 28:
    v = pi * r * r
else:
    v = pi * 2 * r
return_value = v
