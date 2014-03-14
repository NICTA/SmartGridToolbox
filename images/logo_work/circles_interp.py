import numpy as np
import matplotlib.pyplot as plt

def sigmoid(t, w):
    return 1.0/(1.0 + np.exp(-t/w))

ncirc = 20 
nperiods = 0.5 
min_rad = 0.003
max_rad = 0.015
rad_mult = max_rad - min_rad
height_mult = 0.02

nicta_green='#5da733'
nicta_purple='#8d65d2'

rng = np.arange(0, ncirc + 1)

nicta_green_hex = np.array([0x5D/0xFF, 0xA7/0xFF, 0x33/0xFF, 1.0])
nicta_purple_hex = np.array([0x8D/0xFF, 0x65/0xFF, 0xD2/0xFF, 1.0])

interp = [i*nicta_green_hex/ncirc + (ncirc - i)*nicta_purple_hex/ncirc for i in rng]

plt.axis('off')
ax = plt.gcf().gca()

for i in range(0, ncirc + 1):
    j = ncirc - i

    fi = sigmoid(i/ncirc-0.5, 0.05)
    fj = 1.0 - fi
    print(fi, fj)

    col1 = fi * nicta_green_hex + fj * nicta_purple_hex
    col2 = fj * nicta_green_hex + fi * nicta_purple_hex

    x = float(i)/ncirc
    circ = plt.Circle([x, height_mult*(np.sin(x*2*nperiods*np.pi)) + 0.5],
                      rad_mult*(np.sin(x*2*nperiods*np.pi)**2) + min_rad,
                      color=col1,
                      clip_on = False)
    ax.add_artist(circ)
    circ = plt.Circle([x, -height_mult*(np.sin(x*2*nperiods*np.pi)) + 0.5],
                      rad_mult*(np.sin(x*2*nperiods*np.pi)**2) + min_rad,
                      color=col2,
                      clip_on=False)
    ax.add_artist(circ)
    
    ax.set_aspect(1.0)

plt.savefig('circles.pdf')
