function [S V bus] = nr(fname); 
   [bus, Y] = init(fname);
   G = real(Y);
   B = imag(Y);
   x = x0(bus);
   maxiter = 50;
   tol = 1e-20;
   for i = 1:maxiter
      fx = f(bus, G, B, x);
      Jx = J(bus, G, B, x);
      x = x - Jx\fx;
      err = max(fx.^2)
      if (err < tol)
         break;
      end
   end
   [P Q M t] = get(bus, G, B, x);
   %printf('%3d %3d %12f %12f %12f %12f\n', [bus.id, bus.type, P, Q, M, t]');
   %check_pf(bus, G, B, P, Q, M, t)
   S = P + I * Q;
   V = M .* (cos(t) + I * sin(t));
end
