function matpower2test(casename, outname)
   addpath('../third_party/matpower4.1');

   [MVABase bus gen branch] = runpf(casename);

   MVA = 1e6; % Matpower powers are reported in MVA, not MVABase...

   ibus = bus(:, 1);

   VBase = bus(:, 10) * 1e3;
   VBase(VBase == 0) = 1000;

   V = bus(:,8) .* exp(I * bus(:, 9) * pi / 180);

   Sc = -(bus(:, 3) + I * bus(:,4)) / MVABase;

   Sg = zeros(size(bus, 1), 1);
   Sg(gen(:,1)) = (gen(:, 2) + I * gen(:, 3)) / MVABase;

   out = [ibus, V, Sc, Sg];

   fout = fopen(outname, 'w');
   fprintf(fout, '%e\n', MVABase * 1e6);
   fprintf(fout, '%d\n', size(bus, 1));
   fprintf(fout, '%d %e %e %e %e %e %e %e\n', [ibus, VBase, real(V), imag(V), real(Sc), imag(Sc), real(Sg), imag(Sg)]');
   printf('%e\n', MVABase * 1e6);
   printf('%d\n', size(bus, 1));
   printf('%d %e %e %e %e %e %e %e\n', [ibus, VBase, real(V), imag(V), real(Sc), imag(Sc), real(Sg), imag(Sg)]');
   fclose(fout);
end
