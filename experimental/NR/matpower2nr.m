function [busdata, branchdata] = matpower2nr(infname)
   c = loadcase(infname);
   busdata = [c.bus(:,[1, 2, 3, 4, 8, 9])];
   nbus = size(busdata, 1);

   sel_PQ = busdata(:, 2) == 1;
   sel_PV = busdata(:, 2) == 2;
   sel_S = busdata(:, 2) == 3;
   assert(sum(sel_S) == 1);
   inew2old = [busdata(sel_PQ, 1); busdata(sel_PV, 1); busdata(sel_S, 1)];
   inew = (1:nbus)';
   [s, i] = sort(inew2old);
   iold2new = inew(i);
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
   gen(:, 1) = iold2new(gen(:, 1));
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
   branchdata(:, 1) = iold2new(branchdata(:, 1));
   branchdata(:, 2) = iold2new(branchdata(:, 2));
   sel = branchdata(:, 1) > branchdata(:, 2);
   branchdata(sel, 1:2) = [branchdata(sel, 2), branchdata(sel, 1)];

   busdata(:, 3) /= c.baseMVA;
   busdata(:, 4) /= c.baseMVA;
end
