function [bus, Y] = init(fname);
   eval(fname);

   bus.N = length(busdata(:, 1));
   bus.id = busdata(:, 1);
   bus.type = busdata(:, 2);
   bus.P = busdata(:, 3);
   bus.Q = busdata(:, 4);
   bus.S = bus.P + 1i * bus.Q;
   bus.M = busdata(:, 5);
   bus.t = busdata(:, 6);
   bus.V = bus.M .* exp(1i * bus.t);
   bus.gs = busdata(:, 7);
   bus.bs = busdata(:, 8);
   bus.ys = bus.gs + 1i * bus.bs;
   bus.IcR = busdata(:, 9);
   bus.IcI = busdata(:, 10);
   bus.Ic = bus.IcR + bus.IcI;

   map(bus.id) = 1:bus.N;
   
   bus.NPQ = 0;
   bus.NPV = 0;
   bus.NSL = 0;
   NOTHER = 0;
   for i = 1:bus.N
      type = bus.type(i);
      if (type == 1)
         bus.NPQ = bus.NPQ + 1;
      elseif (type == 2)
         bus.NPV = bus.NPV + 1;
      elseif (type == 3)
         bus.NSL = bus.NSL + 1;
      else
        NOTHER = NOTHER + 1;
      end
   end
   assert(bus.NSL == 1);
   assert(bus.NPV == 0);
   assert(NOTHER == 0);

   bus.iPQ = bus.type == 1;
   bus.iSL = bus.type == 3;
   bus.i1PQ = 1;
   bus.i2PQ = bus.NPQ;
   bus.i1SL = bus.NPQ + 1;
   bus.i1SL = bus.NPQ + bus.NSL;

   from = branchdata(:, 1);
   to = branchdata(:, 2);
   g = branchdata(:, 3);
   b = branchdata(:, 4);
   nbranch = size(from);
   sparsity = nbranch/bus.N^2;
   if (sparsity <= 0.10)
      Y = sparse(bus.N, bus.N);
   else
      Y = zeros(bus.N, bus.N);
   end
   for i = 1:nbranch
      k = from(i);
      l = to(i);
      assert(l > k);
      k = map(k);
      l = map(l);
      ykl = g(i) + 1i * b(i);
      Y(k, l) = Y(k, l) - ykl;
      Y(l, k) = Y(l, k) - ykl;
      Y(k, k) = Y(k, k) + ykl;
      Y(l, l) = Y(l, l) + ykl;
   end
   %printf('Sparsity of Y = %f%%\n', 100 * nnz(Y)/prod(size(Y)))
end
