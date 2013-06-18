% Function is cos(x)
% Expanded about origin.
% M = 6
% N = 6
% Need McLaurin series up to 12.
M = 6;
N = 6;
McL = zeros(1, M + N + 1);
for i = 0:2:(N+N)
   McL(i+1) = (-1).^(i/2)/factorial(i);
end
McL = McL';

[a, b] = padeapprox(McL, M, N);

z = linspace(0, 2*pi, 100);

fAnaly = cos(z);
fPad = polyval(a(end:-1:1), z) ./polyval(b(end:-1:1), z);
fMc = polyval(McL(end:-1:1), z);

plot(z, fMc, z, fPad, z, fAnaly);
