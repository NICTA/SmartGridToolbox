function [S V bus Y c] = helm_PQ(fname, niter)
   tic()
   [bus, Y] = init(fname);
   V0 = bus.M(bus.iSL);

   c = zeros(bus.NPQ, niter);
   e = zeros(bus.NPQ, niter);

   S = bus.P(bus.iPQ) + 1i * bus.Q(bus.iPQ);

   A = Y(bus.iPQ, bus.iPQ);

   for n = 1:niter
      % Calculate rhs.
      r = zeros(bus.NPQ, 1);
      if (n == 1)
         r = r + conj(S(bus.iPQ))/V0;
      end
      if (n > 1)
         r = r + conj(S(bus.iPQ)).*conj(e(:,n-1));
      end
      c(:,n) = A\r;
      e(:,n) = -c(:,n)/V0^2;
      for (m = 1:n-1)
         e(:,n) -= c(:,n-m).*e(:,m)/V0;
      end
   end
   
   % Now compute the Pade approximation:
   V = zeros(bus.N, 1);
   for i = 1:bus.NPQ
      McL = transpose([V0, real(c(i, :))]);
      NMC = (length(McL) - 1) / 2;
      [a, b] = padeapprox(McL, NMC, NMC);
      V(i) += eval_pade(a, b, 1);
      McL = transpose([0, imag(c(i, :))]);
      NMC = (length(McL) - 1) / 2;
      [a, b] = padeapprox(McL, NMC, NMC);
      V(i) += 1i * eval_pade(a, b, 1);
   end

   V(bus.N) = V0;

   S = S_of_V(V, Y);

   [err_P, err_Q_PQ] = check_pf(bus, Y, S, V)
   elapsed_time = toc();
   printf('Elapsed time = %e\n', elapsed_time);
end
