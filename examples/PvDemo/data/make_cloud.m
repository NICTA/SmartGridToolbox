day = 24 * 60;
t = 0:5:3*day;

T = [0.07, 0.15, 0.38, 1, 2.4, 1.7, 2] * day;
d = [0.5, 0.1, 1.2, 0.4, 3.2, 0.9, 0.3] * day;
f = [0.35, 0.67, 0.72, 0.8, 0.6, 0.4, 0.15];
x = 0;
for i = 1:size(T, 2)
    x += f(i)*sin(2*pi*(t + d(i))./T(i));
end
max_x = 0.95;
min_x = 0.1;
x *= (max_x - min_x)/(max(x) - min(x));
x += min_x - min(x);
x = x.^2;

f = fopen('cloud.txt', 'w+');
fprintf(f, '%d %f\n', [t; x]);
fclose(f);
