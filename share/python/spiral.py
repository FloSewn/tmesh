import numpy as np
from matplotlib import pyplot as plt
from numpy import pi, sin, cos


def main():

    a = 2.5
    b = -3.4
    c = 6.5
    t = np.linspace(0.0, 5.*pi, 5000)
    x = (a + b * t) * cos(t) + c * sin(40*t)
    y = (a + b * t) * sin(t) + c * cos(40*t)
    fig, ax = plt.subplots(1,1,figsize=(10,10))
    ax.plot(x,y,lw=2.,c='r',ls='',marker='x')
    ax.set_xlim([-50,55])
    ax.set_ylim([-55,50])
    plt.show()

if __name__ == '__main__': main()
