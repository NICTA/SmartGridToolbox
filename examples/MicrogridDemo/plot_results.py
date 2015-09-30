import numpy as np
import matplotlib.pyplot as plt

dat = np.loadtxt('out')

t = dat[:, 0] / 24
price = dat[:, 1] * 0.001
build_load_inj = dat[:, 2] * 1000
pv_inj = dat[:, 3] * 1000
uncontr_inj = build_load_inj + pv_inj
grid_inj = dat[:, 4] * 1000
extern_temp = dat[:, 5]
hvac_inj = dat[:, 6] * 1000
build_temp = dat[:, 7]
batt_inj = dat[:, 8] * 1000
batt_chg = dat[:, 9]; batt_chg *= 100 / max(batt_chg)

fig, axes = plt.subplots(2,1)

ax11 = axes[0]
l1 = ax11.plot(t, price, 'b-')
# ax11.set_xlabel('Time (days)')
ax11.set_ylabel('Price ($/kWh)', color='b')
ax11.xaxis.grid('on')

ax12 = ax11.twinx()
ax12.hold(True)
l2 = ax12.plot(t, extern_temp, 'r-')
l3 = ax12.plot(t, build_temp, 'r:')
ax12.set_ylabel('Temperature (C)', color='r')
ax12.legend(l1 + l2 + l3, 
            ['Price ($/kWh)', 'Extern. Temp. (C)', 'Intern. Temp. (C)'],
            loc=1,
            framealpha=0.75,
            prop={'size' : 11})

ax21 = axes[1]
l1 = ax21.plot(t, -hvac_inj, 'b-')
ax21.hold(True)
l2 = ax21.plot(t, -uncontr_inj, 'b:')
ax21.set_xlabel('Time (days)')
ax21.set_ylabel('Power (kW)', color='b')
ax21.xaxis.grid('on')

ax22 = ax21.twinx()
l3 = ax22.plot(t, batt_chg, 'r-')
ax22.set_ylabel('Stored Energy (kWh)', color='r')
ax22.legend(l1 + l2 + l3, 
            ['HVAC Power (kW)', 'Purchased Power (kW)', 'Battery Charge (kWh)'],
            loc=1,
            framealpha=0.75,
            prop={'size' : 11})

plt.savefig('microgrid_results.png')
plt.show()
