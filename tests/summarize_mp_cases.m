files = glob('../data/matpower_cases/*.m')';
for file = files;
   file = make_absolute_filename(file{})
   [outDir, outName] = fileparts(file)
   outf = fopen(['mp_compare/', outName, '.compare'], 'w+');
   result = runpf(file, mpoption('out.all', 0));
   bus = result.bus;
   gen = result.gen;

   % Sometimes, Matpower has a non-zero angle for the slack voltage, I guess due to transformer phase shift.
   % Adjust this.
   iSlack = find(bus(:, 2) == 3);
   angOff = bus(iSlack, 9);
   bus(:, 9) -= angOff;

   V = bus(:, 8) .* exp(I * bus(:, 9) * pi / 180);
   S = - bus(:, 3) - I * bus(:, 4);
   for (row = gen')
      busIdx = find(bus(:, 1) == row(1));
      S(busIdx) += row(2) + I * row(3);
   end
   dat = [real(V), imag(V), real(S), imag(S)]';
   fprintf(outf, '%15.10f %15.10f %15.10f %15.10f\n', dat)
   fclose(outf);
end
