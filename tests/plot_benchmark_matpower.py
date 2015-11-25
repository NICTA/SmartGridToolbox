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
print('# bad nr_pol  = ' + str(len(t_bad_pol)))
print('# bad nr_rect   = ' + str(len(t_bad_rect)))

s1 = ax.scatter(n_ok_mp, t_ok_mp, color='red', marker='.')
s2 = ax.scatter(n_bad_mp, t_bad_mp, color='red', marker='x')
s3 = ax.scatter(n_ok_pol, t_ok_pol, color='blue', marker='.')
s4 = ax.scatter(n_bad_pol, t_bad_pol, color='blue', marker='x')
s5 = ax.scatter(n_ok_rect, t_ok_rect, color='lightgreen', marker='.')
s6 = ax.scatter(n_bad_rect, t_bad_rect, color='lightgreen', marker='x')

ax.set_xlabel('Number of busses')
ax.set_ylabel('Walltime (s)')
ax.legend((s1, s2, s3, s4, s5, s6), 
          ['matpower', 'matpower (not converged)',
           'NR pol', 'NR pol (not converged)',
           'NR rect', 'NR rect (not converged)'],
          loc=4,
          framealpha=0.75,
          prop={'size' : 11})

plt.show()
