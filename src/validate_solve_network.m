function [V, S, err] = validate_solve_network(filename)
   filename = make_absolute_filename(filename)
   status = system(['./solve_network ', filename, ' solve_network >/dev/null']);
   if (status != 0)
      printf('Could not solve %s; status = %d\n', filename, status);
      return
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
   VMp = bus(:, 1) .* busMp(:, 8) .* exp(I * busMp(:, 9) * pi / 180);
   SMp = - busMp(:, 3) - I * busMp(:, 4);
   for (row = genMp')
      busIdx = find(busMp(:, 1) == row(1));
      SMp(busIdx) += row(2) + I * row(3);
   end
   errMp = validate(VMp, SMp, yShunt, IZip, Y);

   V = [VSgt, VMp];
   S = [SSgt, SMp];
   err = [errSgt, errMp];
   iCheck = find(abs(err(:, 1)) > 100)(1)
   err(iCheck, :)

   busI = zeros(size(bus(:,1)), 2);
   for (branch_row = branch')
      ibus = branch_row(1); 
      jbus = branch_row(2);
      if (ibus == iCheck || jbus == iCheck)
         ibus, jbus
         Y_ = [branch_row(3) + I * branch_row(4), branch_row(5) + I * branch_row(6); 
               branch_row(7) + I * branch_row(8), branch_row(9) + I * branch_row(10)];

         VSgt_ = [VSgt(ibus); VSgt(jbus)];
         VMp_ = [VMp(ibus); VMp(jbus)];

         ISgt_ = -Y_ * VSgt_
         IMp_ = -Y_ * VMp_

         busI(ibus, 1) += ISgt_(1);
         busI(ibus, 2) += IMp_(1);
         busI(jbus, 1) += ISgt_(2);
         busI(jbus, 2) += IMp_(2);
      end
   end
   yShunt_ = yShunt(iCheck);

   VSgt_ = VSgt(iCheck);
   SSgt_ = SSgt(iCheck);
   VMp_ = VMp(iCheck);
   SMp_ = SMp(iCheck);

   ISgt_ = conj(SSgt_) ./ conj(VSgt_) - yShunt(iCheck) .* VSgt_;
   IMp_ = conj(SMp_) ./ conj(VMp_) - yShunt(iCheck) .* VMp_;

   busI(iCheck, 1) += ISgt_;
   busI(iCheck, 2) += IMp_;

   busI(iCheck, :)
end

function err = validate(V, S, yShunt, IConst, Y)
   err = S - V .* conj(Y * V);
end
