function [busdata, branchdata] = from_matpower(ifname, ofname)
   addpath('../../third_party/matpower4.1');
   c = loadcase(ifname);
   busdata = [c.bus(:,[1, 2, 3, 4, 8, 9, 5, 6])];
   ibus = busdata(:, 1);
   nbus = size(busdata, 1);
   map(ibus) = 1:nbus;

   gen = [c.gen(:, [1, 2, 6])]; % id, P, M
   busdata(map(gen(1)), [3, 5]) = gen(2:3)';

   iPQ = busdata(:, 2) == 1;
   iPV = busdata(:, 2) == 2;
   iS = busdata(:, 2) == 3;

   busdata(iS, 3:4) = NaN;
   busdata(iPQ, 5:6) = NaN;
   busdata(iPV, [4, 6]) = NaN;

   y = 1 ./ (c.branch(:, 3) + I * c.branch(:, 4));
   branchdata = [c.branch(:, [1, 2]), real(y), imag(y)];
   % Reorder if necessary so second idx is greater than first.
   sel = branchdata(:, 1) > branchdata(:, 2);
   branchdata(sel, 1:2) = [branchdata(sel, 2), branchdata(sel, 1)];

   busdata(:, 3) /= c.baseMVA;
   busdata(:, 4) /= c.baseMVA;

   f = fopen(ofname, 'w');
   fprintf(f, '%% Bus Data\n');
   fprintf(f, '%%\tID\tTYPE\tP\tQ\tM\ttheta\tgs\tbs\n');
   fprintf(f, 'busdata = [ ...\n');
   fprintf(f, '\t%-4d\t%-4d\t%-.4f\t%-.4f\t%-.4f\t%-.4f\t%-.4f\t%-.4f\n', ...
           busdata');
   fprintf(f, '];\n');
   fprintf(f, '\n');

   fprintf(f, '%% Branch Data\n');
   fprintf(f, '%%\tFROM\tTO\tg\tb\n');
   fprintf(f, 'branchdata = [ ...\n');
   fprintf(f, '\t%-4d\t%-4d\t%-.4f\t%-.4f\n', branchdata');
   fprintf(f, '];\n');
   fclose(ofname);
end
