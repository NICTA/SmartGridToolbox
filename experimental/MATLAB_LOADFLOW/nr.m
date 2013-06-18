function [S V bus] = nr(fname); 
   [bus, Y] = init(fname);
   G = real(Y);
   B = imag(Y);
   x = nr_x0(bus);
   maxiter = 50;
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
   [P Q M t] = get(bus, G, B, x);
   %printf('%3d %3d %12f %12f %12f %12f\n', [bus.id, bus.type, P, Q, M, t]');
   S = P + I * Q;
   V = M .* exp(I * t);
   [err_P, err_Q_PQ, err_M_PV, err_V_SL, err_PF] = check_pf(bus, Y, S, V)
end
