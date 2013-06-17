function [bus, Y] = init(fname);
   addpath('~/matpower4.1');
   [busdata, branchdata] = from_matpower(cname);
   load(fname);

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
         bus.NPQ += 1;
      elseif (type == 2)
         bus.NPV += 1;
      elseif (type == 0)
         bus.NSL += 1;
      else
        NOTHER += 1;
      end
   
      nbus_prev = nbus;
      type_prev = type;
   end
   assert(bus.NSL == 1);
   assert(NOTHER == 0);
   
   bus.i1PQ = 1;
   bus.i2PQ = bus.NPQ;
   bus.i1PV = bus.i2PQ + 1;
   bus.i2PV = bus.i2PQ + bus.NPV;
   bus.i1SL = bus.i2PV + 1;
   bus.i2SL = bus.i2PV + bus.NSL;
   
   Y = sparse(bus.N, bus.N);
   for i = 1:branch.N
      k = branch.from(i);
      l = branch.to(i);
      assert(l > k);
      ykl = branch.g(i) + I * branch.b(i);
      Y(k, l) -= ykl;
      Y(l, k) -= ykl;
      Y(k, k) += ykl;
      Y(l, l) += ykl;
   end
   printf('Sparsity of Y = %f%%\n', 100 * nnz(Y)/prod(size(Y)))
end
