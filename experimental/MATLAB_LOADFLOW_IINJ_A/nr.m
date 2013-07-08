function [S V bus Y] = nr(fname, maxiter); 
   [bus, Y] = init(fname);
   x = nr_x0(bus);
   tol = 1e-20;
   for i = 1:maxiter
      fx = nr_f(bus, Y, x);
      Jx = nr_J(bus, Y, x);
      x = x - Jx\fx;
      err = max(fx.^2)
      if (err < tol)
         break;
      end
   end
   V = [x(bus.iPQ);bus.V(bus.iSL)];
   S = [bus.S(bus.iPQ);conj(bus.V(bus.iSL)) .* conj(Y(bus.iSL, :)) * V];
end
