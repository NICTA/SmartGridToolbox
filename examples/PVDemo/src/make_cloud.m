day = 24 * 60;
t = 0:5:2*24*60;
T = [0.15*day, 0.38*day, 2.4*day, 1.7*day, 2*day];
d = [0.1*day, 1.2*day, 3.2*day, 0.9*day, 0.3*day];
f = [0.2, 0.3, 0.6, 0.4, 0.1];

c = 0;
for i = 1:size(T, 2)
    c += f(i)*sin(2*pi*(t + d(i))./T(i));
end
maxc = 0.95;
minc = 0.1;
c *= (maxc - minc)/(max(c) - min(c));
c += minc - min(c);
dat = [t;c]';
save -ascii cloud.txt dat
