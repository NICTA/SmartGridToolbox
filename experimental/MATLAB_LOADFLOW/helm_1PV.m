function [S V bus Y c] = helm_1PV(fname, niter)
   tic()
   [bus, Y] = init(fname);
   V0 = bus.M(bus.iSL);
   M = bus.M(bus.iPV);
   P = bus.P(bus.iPV);
   y = Y(bus.iPV, bus.iPV);
   g = real(y);
   b = imag(y);

   c = zeros(1, niter);
   d = zeros(1, niter);

   c(1) = (M^2 - V0^2) / (2 * V0);
   d(1) = -P / (b * V0) + (g * (M^2 - V0^2))/(b * V0);
   for n = 1:niter
      temp = 0;
      for (m = 1:n - 1)
         temp = temp + (c(m) * c(n-m) + d(m) * d(n-m));
      end
      c(n) = c(n) - temp / (2 * V0);
      d(n) -= c(n) * g / b;
   end

   % Now compute the Pade approximation:
   V = 0;
   McL = transpose([V0, c]);
   NMC = (length(McL) - 1) / 2;
   [a, b] = padeapprox(McL, NMC, NMC);
   V += eval_pade(a, b, 1);
   McL = transpose([V0, d]);
   NMC = (length(McL) - 1) / 2;
   [a, b] = padeapprox(McL, NMC, NMC);
   V += 1i * eval_pade(a, b, 1);

   V = [V;V0];

   S = S_of_V(V, Y);

   [err_P, err_Q_PQ] = check_pf(bus, Y, S, V)
   elapsed_time = toc();
   printf('Elapsed time = %e\n', elapsed_time);
end
