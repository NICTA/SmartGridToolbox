dat = dlmread('../PvDemo/data/loads/load_1.txt');
for i = 2:10
    dat = dat + dlmread(['../PvDemo/data/loads/load_', num2str(i), '.txt']);
end
t = dat(:, 1);
z = zeros(size(t));
PLd = dat(:, 2);
QLd = dat(:, 3);
arg = [t, z, z, z, z, -PLd, -QLd];
f = fopen('load_build.txt', 'w+');
fprintf(f, '%d %f%+fj %f%+fj %f%+fj\n', arg');
fclose(f);

price = csvread('DATA201508_NSW1.csv')(:, 4);
t_price = (0:30:30 * length(price) - 1)';
f = fopen('price.txt', 'w+');
fprintf(f, '%d %f\n', [t_price, price]');
fclose(f);
