dat = dlmread('out');
t = dat(:, 1);
price = dat(:, 2) * 0.001;
build_load_inj = dat(:, 3) * 1000;
pv_inj = dat(:, 4) * 1000;
uncontr_inj = build_load_inj + pv_inj;
grid_inj = dat(:, 5) * 1000;
extern_temp = dat(:, 6);
hvac_inj = dat(:, 7) * 1000;
build_temp = dat(:, 8);
batt_inj = dat(:, 9) * 1000;
batt_chg = dat(:, 10); batt_chg *= 100 / max(batt_chg);

clf;
subplot(211);
[ax, h1, h2] = plotyy(t, extern_temp, t, price);
hold on
h3 = plot(ax(1), t, build_temp, 'r'); 
ylabel(ax(1), 'Temperature');
ylabel(ax(2), 'Price ($ per kWh)');
axis(ax(1), [0, 120])
axis(ax(2), [0, 120])
set(ax(1), 'xgrid', 'on');
set(ax(1), 'xminorgrid', 'on');
set(ax(1), 'ygrid', 'on');

subplot(212);
[ax, h1, h2] = plotyy(t, -uncontr_inj, t, batt_chg);
hold on;
h3 = plot(ax(1), t, -hvac_inj, 'r'); 
ylabel(ax(1), 'Power Demand (kW)');
ylabel(ax(2), 'Battery Charge (% of Maximum)');
axis(ax(1), [0, 120])
axis(ax(2), [0, 120])
set(ax(1), 'xgrid', 'on');
set(ax(1), 'xminorgrid', 'on');
set(ax(1), 'ygrid', 'on');
