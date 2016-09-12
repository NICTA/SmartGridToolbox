% Load raw data:
dat_ld = dlmread('../PvDemo/data/loads/load_1.txt');
for i = 2:10
    dat_ld(:, 2) = dat_ld(:, 2) + dlmread(['../PvDemo/data/loads/load_', num2str(i), '.txt'])(:, 2);
end
t_ld = dat_ld(:, 1);

% Price raw data:
pr = csvread('DATA201508_NSW1.csv')(:, 4);
t_pr = (0:30:30 * length(pr) - 1)';

% Upper bound on time:
t_minimax = min(max(t_ld), max(t_pr));
ld_sel = t_ld <= t_minimax;
pr_sel = t_pr <= t_minimax;

% Load data:
t_ld = t_ld(ld_sel);
P_ld = real(dat_ld(ld_sel, 2));
Q_ld = imag(dat_ld(ld_sel, 2));
z = zeros(size(t_ld));
f = fopen('load_build.txt', 'w+');
fprintf(f, '%d %f%+fj %f%+fj %f%+fj\n', [t_ld, z, z, z, z, P_ld, Q_ld]');
fclose(f);

% Price data:
t_pr = t_pr(pr_sel);
pr = pr(pr_sel);
f = fopen('price.txt', 'w+');
fprintf(f, '%d %f\n', [t_pr, pr]');
fclose(f);

t = 0:5:t_minimax;
day = 24 * 60;

% Cloud data:
T = [0.15, 0.38, 1, 2.4, 1.7, 2] * day;
d = [0.1, 1.2, 0.4, 3.2, 0.9, 0.3] * day;
f = [0.2, 0.3, 0.9, 0.6, 0.4, 0.1];
x = 0;
for i = 1:size(T, 2)
    x += f(i)*sin(2*pi*(t + d(i))./T(i));
end
max_x = 0.95;
min_x = 0.1;
x *= (max_x - min_x)/(max(x) - min(x));
x += min_x - min(x);
f = fopen('cloud.txt', 'w+');
fprintf(f, '%d %f %f %f\n', [t; x; 0.3 * x; 0.3 * x]);
fclose(f);

% Building heat data:
T = [0.1, 0.2, 1, 7] * day;
d = [0.1, 0.6, 0.6, 0.9] * day;
f = [0.2, 0.3, 0.6, 0.4, 0.1];
x = 0;
for i = 1:size(T, 2)
    x += f(i)*sin(2*pi*(t + d(i))./T(i));
end
max_x = 5000;
min_x = 1000;
x *= (max_x - min_x)/(max(x) - min(x));
x += min_x - min(x);
f = fopen('internal_thermal_power.txt', 'w+');
fprintf(f, '%d %f\n', [t; x]);
fclose(f);
