function [busdata, branchdata] = from_matpower(ifname, ofname)
   addpath('../../third_party/matpower4.1');
   c = loadcase(ifname);
   busdata = [c.bus(:,[1, 2, 3, 4, 8, 9, 5, 6])];

   nbus = size(busdata, 1);
   ibus = busdata(:, 1);
   maxix = max(ibus);
   map = NA * ones(maxix, 1);
   map(ibus) = 1:nbus;

   busdata(:, 1) = map(ibus);

   sel_PQ = busdata(:, 2) == 1;
   sel_PV = busdata(:, 2) == 2;
   sel_S = busdata(:, 2) == 3;
   assert(sum(sel_S) == 1);
   inew2old = [busdata(sel_PQ, 1); busdata(sel_PV, 1); busdata(sel_S, 1)];
   inew = (1:nbus)';

   busdata = busdata(inew2old, :);
   sel_PQ = busdata(:, 2) == 1;
   sel_PV = busdata(:, 2) == 2;
   sel_S = busdata(:, 2) == 3;
   busdata(:, 1) = 1:nbus;
   iPQ = busdata(sel_PQ, 1);
   iPV = busdata(sel_PV, 1);
   iS = busdata(sel_S, 1);
   busdata(iS, 2) = 0;

   gen = [c.gen(:, [1, 2, 6])];
   gen(:, 1) = map(gen(:, 1));
   [s i] = sort(gen(:, 1));
   gen = gen(i, :);
   ngen = size(gen, 1);
   gen = [(1:ngen)', gen, zeros(ngen, 1)];
   assert(gen(gen(:, 2) == iS, 4) == 1);
   for g = gen()'
      busdata(g(2), [3, 5, 6]) = g(3:5)';
   end

   busdata(iS, 3:4) = NA;
   busdata(iPQ, 5:6) = NA;
   busdata(iPV, [4, 6]) = NA;

   y = 1 ./ (c.branch(:, 3) + I * c.branch(:, 4));
   branchdata = [c.branch(:, [1, 2]), real(y), imag(y)];
   branchdata(:, 1) = map(branchdata(:, 1));
   branchdata(:, 2) = map(branchdata(:, 2));
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
