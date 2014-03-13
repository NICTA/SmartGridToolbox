import numpy as np
import matplotlib.pyplot as plt

ncirc = 40 
nperiods = 1 
min_rad = 0.002
max_rad = 0.01
rad_mult = max_rad - min_rad
height_mult = 0.03

plt.axis('off')
ax = plt.gcf().gca()
for i in range(0, ncirc + 1):
    x = float(i)/ncirc
    circ = plt.Circle([x, height_mult*(np.sin(x*2*nperiods*np.pi)) + 0.5],
                      rad_mult*(np.sin(x*2*nperiods*np.pi)**2) + min_rad,
                      color='#8d65d2',
                      clip_on = False)
    ax.add_artist(circ)
    circ = plt.Circle([x, -height_mult*(np.sin(x*2*nperiods*np.pi)) + 0.5],
                      rad_mult*(np.sin(x*2*nperiods*np.pi)**2) + min_rad,
                      color='#5da733',
                      clip_on=False)
    ax.add_artist(circ)
    ax.set_aspect(1.0)
    plt.savefig('circles.pdf')
