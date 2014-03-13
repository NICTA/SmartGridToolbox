import numpy as np
import matplotlib.pyplot as plt

ncirc = 20 
nperiods = 0.5 
min_rad = 0.003
max_rad = 0.015
rad_mult = max_rad - min_rad
height_mult = 0.02

nicta_green='#5da733'
nicta_purple='#8d65d2'

plt.axis('off')
ax = plt.gcf().gca()
for i in range(0, ncirc + 1):
    x = float(i)/ncirc
    circ = plt.Circle([x, height_mult*(np.sin(x*2*nperiods*np.pi)) + 0.5],
                      rad_mult*(np.sin(x*2*nperiods*np.pi)**2) + min_rad,
                      color=nicta_green,
                      clip_on = False)
    ax.add_artist(circ)
    circ = plt.Circle([x, -height_mult*(np.sin(x*2*nperiods*np.pi)) + 0.5],
                      rad_mult*(np.sin(x*2*nperiods*np.pi)**2) + min_rad,
                      color=nicta_purple,
                      clip_on=False)
    ax.add_artist(circ)
    
    ax.set_aspect(1.0)

plt.savefig('circles.pdf')
