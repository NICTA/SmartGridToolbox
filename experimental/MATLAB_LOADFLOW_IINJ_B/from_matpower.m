function [busdata, branchdata] = from_matpower(ifname, ofname)
   addpath('../../third_party/matpower4.1');

   c = loadcase(ifname);

   busdata = [c.bus(:,[1, 2, 3, 4, 8, 9, 5, 6])];
   nbus = size(busdata, 1);
   busdata = [busdata, zeros(nbus, 2)];
   %  1     2     3     4     5     6     7     8     9     10
   %  id    type  P     Q     M     theta gs    bs    IcR   IcI

   % Sort by type.
   iPQ = busdata(:, 2) == 1;
   iPV = busdata(:, 2) == 2;
   iSL = busdata(:, 2) == 3;
   busdata = [busdata(iPQ, :); busdata(iPV, :); busdata(iSL,:)];

   % Find index vectors.
   iPQ = busdata(:, 2) == 1;
   iPV = busdata(:, 2) == 2;
   iSL = busdata(:, 2) == 3;

   % Map from id to index.
   map(busdata(:, 1)) = 1:nbus;

   % Apply generator data.
   gen = [c.gen(:, [1, 2, 3, 6])]; 
   %  1     2     3     4
   %  id    P     Q     M
   busdata(map(gen(:, 1)), 3:4) = busdata(map(gen(:, 1)), 3:4) - gen(:, 2:3);
   busdata(map(gen(:, 1)), 5) = gen(:, 4);

   busdata(:, 3:4) = -busdata(:, 3:4); % Convert demand to injection.

   busdata(iPQ, 5:6) = NaN;
   busdata(iPV, [4, 6]) = NaN;
   busdata(iSL, [3, 4]) = NaN;

   y = 1 ./ (c.branch(:, 3) + I * c.branch(:, 4));
   branchdata = [c.branch(:, [1, 2]), real(y), imag(y)];
   % Reorder if necessary so second idx is greater than first.
   sel = branchdata(:, 1) > branchdata(:, 2);
   branchdata(sel, 1:2) = [branchdata(sel, 2), branchdata(sel, 1)];

   % Scale powers.
   busdata(:, 3) = busdata(:, 3) / c.baseMVA;
   busdata(:, 4) = busdata(:, 4) / c.baseMVA;

   f = fopen(ofname, 'w');
   fprintf(f, '%% Bus Data\n');
   fprintf(f, '%%\tID\tTYPE\tP\tQ\tM\ttheta\tgs\tbs\tIcR\tIcI\n');
   fprintf(f, 'busdata = [ ...\n');
   fprintf(f, '\t%-4d\t%-4d\t%-.4f\t%-.4f\t%-.4f\t%-.4f\t%-.4f\t%-.4f\t%-.4f\t%-.4f\n', busdata');
   fprintf(f, '];\n');
   fprintf(f, '\n');

   fprintf(f, '%% Branch Data\n');
   fprintf(f, '%%\tFROM\tTO\tg\tb\n');
   fprintf(f, 'branchdata = [ ...\n');
   fprintf(f, '\t%-4d\t%-4d\t%-.4f\t%-.4f\n', branchdata');
   fprintf(f, '];\n');
   fclose(ofname);
end
