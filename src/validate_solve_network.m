function [okSgt, VSESgt, okMp, VSEMp] = validate_solve_network(filename)
   okSgt = true;
   okMp = true;
   filename = make_absolute_filename(filename)
   status = system(['./solve_network ', filename, ' solve_network >/dev/null']);
   if (status != 0)
      printf('Could not solve %s; status = %d\n', filename, status);
      okSgt = false;
      return;
   end

   bus = load('solve_network.bus');
   branch = load('solve_network.branch');

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
   
   VSgt = bus(:,1) .* bus(:, 2) .* exp(I * bus(:, 3) * pi / 180);
   SSgt = bus(:, 4) + I * bus(:, 5) - bus(:, 6) - I * bus(:, 7);
   errSgt = validate(VSgt, SSgt, yShunt, IZip, Y);
   
   result = runpf(filename, mpoption('out.all', 0));
   busMp = result.bus;
   genMp = result.gen;

   % Sometimes, Matpower has a non-zero angle for the slack voltage, I guess due to transformer phase shift.
   % Adjust this.
   iSlack = find(busMp(:, 2) == 3);
   angOff = busMp(iSlack, 9);
   busMp(:, 9) -= angOff;

   VMp = bus(:, 1) .* busMp(:, 8) .* exp(I * busMp(:, 9) * pi / 180);
   SMp = - busMp(:, 3) - I * busMp(:, 4);
   for (row = genMp')
      busIdx = find(busMp(:, 1) == row(1));
      SMp(busIdx) += row(2) + I * row(3);
   end

   errMp = validate(VMp, SMp, yShunt, IZip, Y);

   VSESgt = [VSgt, SSgt, errSgt];
   VSEMp = [VMp, SMp, errMp];
   if (any(abs(VSESgt(:,3)) > 0.01))
      okSgt = false;
   end
   if (any(abs(VSEMp(:,3)) > 0.01))
      okMp = false;
   end
end

function err = validate(V, S, yShunt, IConst, Y)
   err = S - V .* conj(Y * V);
end
