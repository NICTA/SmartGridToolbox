function [V, S, err] = validate_solve_network(filename)
   system(['./solve_network ', filename, ' solve_network']);

   bus = load('solve_network.bus');
   branch = load('solve_network.branch');

   VSgt = bus(:,1) .* bus(:, 2) .* exp(I * bus(:, 3) * pi / 180);
   Sg = bus(:, 4) + I * bus(:, 5);
   Sl = bus(:, 6) + I * bus(:, 7);

   yShunt = bus(:, 8) + I * bus(:, 9);
   IZip = bus(:, 10) + I * bus(:, 11);

   nbus = rows(bus);
   Y = sparse(nbus, nbus);
   for (row = branch')
      ibus = row(1);
      jbus = row(2);
      YRow = [row(3) + I * row(4), row(5) + I * row(6); row(7) + I * row(8), row(9) + I * row(10)];
      Y([ibus, jbus], [ibus, jbus]) += YRow;
   end
   for i = 1:rows(yShunt)
       Y(i, i) += yShunt(i);
   end

   SSgt = Sg - Sl;
   errSgt = SSgt - VSgt .* conj(Y * VSgt);

   result = runpf(filename);
   busMp = result.bus;
   genMp = result.gen;
   VMp = bus(:, 1) .* busMp(:, 8) .* exp(I * busMp(:, 9) * pi / 180);
   SMp = - busMp(:, 3) - I * busMp(:, 4);

   for (row = genMp')
      SMp(row(1)) += row(2) + I * row(3);
   end
   
   errMp = SMp - VMp .* conj(Y * VMp);

   V = [VSgt, VMp];
   S = [SSgt, SMp];
   err = [errSgt, errMp];
end
