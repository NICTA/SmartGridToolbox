function [bus, Y] = init(fname);
   eval(fname)
   bus.id = busdata(:, 1);
   bus.type = busdata(:, 2);
   bus.P = busdata(:, 3);
   bus.Q = busdata(:, 4);
   bus.M = busdata(:, 5);
   bus.t = busdata(:, 6);
   bus.gs = busdata(:, 7);
   bus.bs = busdata(:, 8);
   bus.N = size(bus.id, 1);
   
   branch.from = branchdata(:, 1);
   branch.to = branchdata(:, 2);
   branch.g = branchdata(:, 3);
   branch.b = branchdata(:, 4);
   branch.N = size(branch.from);
   
   bus.NPQ = 0;
   bus.NPV = 0;
   bus.NSL = 0;
   NOTHER = 0;
   type_prev = 0;
   nbus_prev = 0;
   for i = 1:bus.N
      nbus = bus.id(i);
      assert(nbus == nbus_prev + 1);
      type = bus.type(i);
      if (type == 1)
         bus.NPQ = bus.NPQ + 1;
      elseif (type == 2)
         bus.NPV = bus.NPV + 1;
      elseif (type == 0)
         bus.NSL = bus.NSL + 1;
      else
        NOTHER = NOTHER + 1;
      end
   
      nbus_prev = nbus;
      type_prev = type;
   end
   assert(bus.NSL == 1);
   assert(NOTHER == 0);
   
   bus.iPQ = bus.type == 1;
   bus.iPV = bus.type == 2;
   bus.iSL = bus.type == 0;
   bus.iPQPV = bus.iPQ | bus.iPV;
   bus.i1PQ = 1;
   bus.i2PQ = bus.NPQ;
   bus.i1PV = bus.NPQ + 1;
   bus.i2PV = bus.NPQ + bus.NPV;
   bus.i1SL = bus.NPQ + bus.NPV + 1;
   bus.i1SL = bus.NPQ + bus.NPV + bus.NSL;
  
   sparsity = branch.N/bus.N^2;
   if (sparsity <= 0.10)
      Y = sparse(bus.N, bus.N);
   else
      Y = zeros(bus.N, bus.N);
   end
   for i = 1:branch.N
      k = branch.from(i);
      l = branch.to(i);
      assert(l > k);
      ykl = branch.g(i) + 1i * branch.b(i);
      Y(k, l) = Y(k, l) - ykl;
      Y(l, k) = Y(l, k) - ykl;
      Y(k, k) = Y(k, k) + ykl;
      Y(l, l) = Y(l, l) + ykl;
   end
   %printf('Sparsity of Y = %f%%\n', 100 * nnz(Y)/prod(size(Y)))
end
