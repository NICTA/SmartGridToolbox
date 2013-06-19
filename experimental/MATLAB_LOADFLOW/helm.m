function [S V bus c d] = helm(fname, niter)
   [bus, Y] = init(fname);
   N = bus.NPQ + bus.NPV;
   G = real(Y);
   B = imag(Y);
   V0 = bus.M(bus.iSL);
   M2PV = bus.M(bus.iPV).^2;
   gs = bus.gs;
   bs = bus.bs;

   GQQ = G(bus.iPQ, bus.iPQ);
   GQV = G(bus.iPQ, bus.iPV);
   GVQ = G(bus.iPV, bus.iPQ);
   GVV = G(bus.iPV, bus.iPV);
   BQQ = B(bus.iPQ, bus.iPQ);
   BAA = B(bus.iPQPV, bus.iPQPV);
   A = V0 * [[-GQQ;-GVQ;-BQQ], [BAA;[-GQQ,-GQV]]];
   Ai = eye(size(A))/A;

   c = zeros(N, niter);
   d = zeros(N, niter);

   % Indices into x.
   ixPQc = 1:bus.NPQ;
   ixd = (bus.NPQ+1):(bus.NPQ+N);
   % Indices into c, d.
   iPQ = 1:bus.NPQ;
   iPV = bus.NPQ+1:bus.NPQ+bus.NPV;
   iPQPV = 1:N;

   c(iPV, 1) = c(iPV, 1) + (M2PV - V0^2)/(2*V0);
   for n = 1:niter
      temp = zeros(bus.NPV, 1);
      for m = 1:n-1
         temp = temp - (c(iPV, m).*c(iPV, n-m) + d(iPV, m).*d(iPV, n-m)); 
      end
      c(iPV, n) = c(iPV, n) + temp/(2*V0);

      % Calculate rhs.
      rP = zeros(N, 1);
      rQ = zeros(bus.NPQ, 1);
      if (bus.NPV > 0)
         rP = rP + V0 * G(bus.iPQPV, bus.iPV) * c(iPV, n);
         rQ = rQ + V0 * B(bus.iPQ, bus.iPV) * c(iPV, n);
      end
      if (n == 1)
         rP = rP - bus.P(bus.iPQPV) - V0*bus.IcR(bus.iPQPV);
         rQ = rQ + bus.Q(bus.iPQ) - V0*bus.IcI(bus.iPQ);
      end
      if (n > 1)
         rP = rP + c(:,n-1).*(V0*gs(bus.iPQPV)-bus.IcR(bus.iPQPV)) ...
             + d(:,n-1).*(-V0*bs(bus.iPQPV)-bus.IcI(bus.iPQPV));
         rQ = rQ + c(iPQ,n-1).*( ...
                  V0*bs(bus.iPQ)-bus.IcI(bus.iPQ)) ...
             + d(iPQ,n-1).*( ...
                  V0*gs(bus.iPQ)+bus.IcR(bus.iPQ));
      end
      for m = 1:n-1
         gc_m_bd = gs(bus.iPQPV).*c(:,m) - bs(bus.iPQPV).*d(:,m);
         bc_p_gd = bs(bus.iPQPV).*c(:,m) + gs(bus.iPQPV).*d(:,m);
         Gc_m_Bd = G(bus.iPQPV,bus.iPQPV)*c(:,m) ...
                 - B(bus.iPQPV,bus.iPQPV)*d(:,m);
         Bc_p_Gd = B(bus.iPQPV,bus.iPQPV)*c(:,m) ...
                 + G(bus.iPQPV,bus.iPQPV)*d(:,m);

         rP = rP + c(:,n-m).*(gc_m_bd+Gc_m_Bd);
         rP = rP + d(:,n-m).*(bc_p_gd+Bc_p_Gd);
         rQ = rQ + c(iPQ,n-m).* ...
               (bc_p_gd(iPQ,:)+Bc_p_Gd(iPQ,:));
         rQ = rQ - d(iPQ,n-m).* ...
               (gc_m_bd(iPQ,:)+Gc_m_Bd(iPQ,:));
      end
      r = [rP;rQ];
      x = A\r;
      c(iPQ, n) = x(ixPQc);
      d(:, n) = x(ixd);
   end
   
   % Now compute the Pade approximation:
   V = zeros(bus.N, 1);
   for i = 1:N
      McL = transpose([V0, c(i, :)]);
      N = (length(McL) - 1) / 2;
      [ca, cb] = padeapprox(McL, N, N);
      McL = [0, d(i, :)]';
      [da, db] = padeapprox(McL, N, N);
      V(i) = eval_pade(ca, cb, 1) + 1i * eval_pade(da, db, 1);
   end
   V(bus.N) = V0;

   S = S_of_V(V, Y);

   [err_P, err_Q_PQ, err_M_PV] = check_pf(bus, Y, S, V)
end
