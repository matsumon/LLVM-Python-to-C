# line 399 hangs if it doesn't run, maybe block?
# not returning i1 for condition in branches
n = 8
if 2 > 1:
    n = n * 2
return_value = n
