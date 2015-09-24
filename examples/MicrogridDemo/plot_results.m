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
[ax, h1, h2] = plotyy(t, price, t, extern_temp);
hold on
h3 = plot(ax(2), t, build_temp, 'r'); 
ylabel(ax(1), 'Price');
ylabel(ax(2), 'Temperature');
grid on;
axis(ax(1), [80, 120])
axis(ax(2), [80, 120])

subplot(212);
[ax, h1, h2] = plotyy(t, uncontr_inj, t, batt_chg);
hold on;
h3 = plot(ax(1), t, hvac_inj, 'r'); 
ylabel(ax(1), 'Injections');
ylabel(ax(2), 'Battery Charge');
grid on;
axis(ax(1), [80, 120])
axis(ax(2), [80, 120])
%legend([h1, h3, h2], {'Uncontrolled', 'HVAC', 'Charge'}, 'location', 'northeastoutside'); 
%subplot(413);
%plot(t, batt_chg);
%ylabel('Battery charge');
%subplot(414);
%plot(t, [extern_temp, build_temp]);
%ylabel('Temperatures');
%legend({'External', 'Internal'});
