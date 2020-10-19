from decimal import *

def calc(in_val, perc, tol=float(0)):
    print("tol {}/{}".format(((tol)/100), tol))
    return (in_val * perc * (1-float(tol)/100)/100)

o_val = calc(100, 100, 10)
print("O_val is {}".format(o_val))
