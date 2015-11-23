import numpy as np
import matplotlib.pyplot as plt

fig = plt.figure()
ax = plt.gca()

ax.hold(True)
ax.set_yscale('log')
ax.set_xscale('log')

dat = np.loadtxt('benchmark.txt', usecols=(1,2,3,4,5,6,7))

n = dat[:, 0]
ok_mp = dat[:, 1]
t_mp = dat[:, 2]
ok_pol = dat[:, 3]
t_pol = dat[:, 4]
ok_rect = dat[:, 5]
t_rect = dat[:, 6]

t_ok_mp = [x[1] for x in zip(ok_mp, t_mp) if x[0] == 1]
t_bad_mp = [x[1] for x in zip(ok_mp, t_mp) if x[0] == 0]

t_ok_pol = [x[1] for x in zip(ok_pol, t_pol) if x[0] == 1]
t_bad_pol = [x[1] for x in zip(ok_pol, t_pol) if x[0] == 0]

t_ok_rect = [x[1] for x in zip(ok_rect, t_rect) if x[0] == 1]
t_bad_rect = [x[1] for x in zip(ok_rect, t_rect) if x[0] == 0]

print(len(t_bad_mp))
print(len(t_bad_pol))
print(len(t_bad_rect))

ax.scatter(n, t_mp, color='red')
ax.scatter(n, t_pol, color='lightgreen')
ax.scatter(n, t_rect, color='blue')
plt.show()
