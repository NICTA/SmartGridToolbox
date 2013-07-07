function [S V bus Y] = nr(fname, maxiter); 
   [bus, Y] = init(fname);
   G = real(Y);
   B = imag(Y);
   x = nr_x0(bus);
   tol = 1e-20;
   for i = 1:maxiter
      fx = nr_f(bus, G, B, x);
      Jx = nr_J(bus, G, B, x);
      x = x - Jx\fx;
      err = max(fx.^2);
      if (err < tol)
         break;
      end
   end
   [P Q M t] = nr_get(bus, G, B, x);
   V = M .* exp(I * t);
   % Make the power satisfy the PF equations exactly, rather than making
   % the bus controls exact.
   S = S_of_V(V, bus, Y);
   [err_P, err_Q_PQ, err_M_PV] = check_pf(bus, Y, S, V)
end
