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

n_ok_mp = [x[1] for x in zip(ok_mp, n) if x[0] == 1]
t_ok_mp = [x[1] for x in zip(ok_mp, t_mp) if x[0] == 1]
n_bad_mp = [x[1] for x in zip(ok_mp, n) if x[0] == 0]
t_bad_mp = [x[1] for x in zip(ok_mp, t_mp) if x[0] == 0]

n_ok_pol = [x[1] for x in zip(ok_pol, n) if x[0] == 1]
t_ok_pol = [x[1] for x in zip(ok_pol, t_pol) if x[0] == 1]
n_bad_pol = [x[1] for x in zip(ok_pol, n) if x[0] == 0]
t_bad_pol = [x[1] for x in zip(ok_pol, t_pol) if x[0] == 0]

n_ok_rect = [x[1] for x in zip(ok_rect, n) if x[0] == 1]
t_ok_rect = [x[1] for x in zip(ok_rect, t_rect) if x[0] == 1]
n_bad_rect = [x[1] for x in zip(ok_rect, n) if x[0] == 0]
t_bad_rect = [x[1] for x in zip(ok_rect, t_rect) if x[0] == 0]

print('# bad matpower = ' + str(len(t_bad_mp)))
print('# bad nr_rect  = ' + str(len(t_bad_pol)))
print('# bad nr_pol   = ' + str(len(t_bad_rect)))

ax.scatter(n_ok_mp, t_ok_mp, color='red', marker='.')
ax.scatter(n_bad_mp, t_bad_mp, color='red', marker='x')
ax.scatter(n_ok_pol, t_ok_pol, color='blue', marker='.')
ax.scatter(n_bad_pol, t_bad_pol, color='blue', marker='x')
ax.scatter(n_ok_rect, t_ok_rect, color='lightgreen', marker='.')
ax.scatter(n_bad_rect, t_bad_rect, color='lightgreen', marker='x')
plt.show()
