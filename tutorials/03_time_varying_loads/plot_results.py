import numpy as np
import matplotlib.pyplot as plt

# this a workaround for a numpy bug in file io (http://github.com/numpy/numpy#5655)
def my_loadtxt(*args, **kw):
    if 'dtype' in kw and (kw['dtype'] == 'complex' or kw['dtype'] is np.complex):
        return np.genfromtxt(*args, **kw)
    else:
        return unpatched_loadtxt(*args, **kw)

unpatched_loadtxt = np.loadtxt
np.loadtxt = my_loadtxt

dat = np.loadtxt('out.txt', dtype=np.complex)
def my_fill_between(ax, x, y1, y2, where, **kwargs):
    """
    Plot filled region between `y1` and `y2`.

    This function works exactly the same as matplotlib's fill_between, except
    that it also plots a proxy artist (specifically, a rectangle of 0 size)
    so that it can be added it appears on a legend.
    """
    ax = ax if ax is not None else plt.gca()
    ax.fill_between(x, y1, y2, **kwargs)
    p = plt.Rectangle((0, 0), 0, 0, **kwargs)
    ax.add_patch(p)
    return p


t = np.real(dat[:, 0]) / 24
S_load = dat[:, 1]
S_gen = dat[:, 2]
V_min = np.abs(dat[:, 3])
V_max = np.abs(dat[:, 4])

(fig, (ax1, ax2)) = plt.subplots(2, 1)
ax1.hold(True)
ax2.hold(True)

p1 = my_fill_between(ax1, t, V_min, V_max, where=V_max >= V_min,
                     facecolor='green', alpha=0.65, linewidth=0.0,
                     label='Voltage Envelope')
ax1.axhline(0.96, color='black')
ax1.axhline(1.04, color='black')
ax1.set_xlim([0, 3])
ax1.set_ylim([0.9, 1.1])
ax1.set_ylabel('Voltage Envelope (p.u.)')
ax1.legend(handles=[p1], framealpha=0.75, prop={'size' : 11})

ax2.plot(t, np.real(S_load), label='Real Load (MW)')
ax2.plot(t, np.imag(S_load), label='Reactive Load (MVAR)')
ax2.plot(t, np.real(S_gen), label='Real Gen (MW)')
ax2.plot(t, np.imag(S_gen), label='Reactive Gen (MVAR)')
ax2.set_xlim([0, 3])
ax2.set_xlabel('Time (days)')
ax2.set_ylabel('Power (MW/MVAR)')
ax2.legend(framealpha=0.75, prop={'size' : 11})

plt.show()
