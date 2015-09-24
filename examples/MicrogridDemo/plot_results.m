dat = dlmread('out');
t = dat(:, 1);
price = dat(:, 2);
build_load_inj = dat(:, 3);
pv_inj = dat(:, 4);
uncontr_inj = build_load_inj + pv_inj;
grid_inj = dat(:, 5);
extern_temp = dat(:, 6);
hvac_inj = dat(:, 7);
build_temp = dat(:, 8);
batt_inj = dat(:, 9);
batt_chg = dat(:, 10);

clf;
subplot(211);
ax = plotyy(t, price, t, extern_temp);
ylabel(ax(1), 'Price');
ylabel(ax(2), 'External Temperature');
grid on;
axis([100, 120])
subplot(212);
plot(t, [uncontr_inj, batt_inj, hvac_inj]);
ylabel('Injections');
grid on;
axis([100, 120])
%subplot(413);
%plot(t, batt_chg);
%ylabel('Battery charge');
%subplot(414);
%plot(t, [extern_temp, build_temp]);
%ylabel('Temperatures');
%legend({'External', 'Internal'});
