function matpower2test(casename, outname)
   addpath('../third_party/matpower4.1');

   [MVABase bus gen branch] = runpf(casename);

   SBase = 1e6; % Matpower powers are reported in MVA, not MVABase...

   ibus = bus(:, 1);

   VBase = bus(:, 10) * 1e3;
   VBase(VBase == 0) = 1;

   V = VBase .* bus(:,8) .* exp(I * bus(:, 9) * pi / 180);

   Sc = -SBase * (bus(:, 3) + I * bus(:,4));

   Sg = zeros(size(bus, 1), 1);
   Sg(gen(:,1)) = SBase * (gen(:, 2) + I * gen(:, 3));

   out = [ibus, V, Sc, Sg];

   fout = fopen(outname, 'w');
   fprintf(fout, '%e\n', MVABase * 1e6);
   fprintf(fout, '%d\n', size(bus, 1));
   fprintf(fout, '%d %e %e %e %e %e %e %e\n', [ibus, VBase, real(V), imag(V), real(Sc), imag(Sc), real(Sg), imag(Sg)]');
   fclose(fout);
end
