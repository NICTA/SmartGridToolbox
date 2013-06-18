function [S V bus] = helm(fname); 
   [bus, Y] = init(fname);
   N = bus.NPQ + bus.NPV;
   G = real(Y);
   B = imag(Y);
   V0 = bus.M(bus.iSL);
   M2PV = bus.M(bus.iPV).^2;
   I0R = zeros(bus.N, 1); % TODO: get proper values for shunts.
   I0I = zeros(bus.N, 1); % TODO: get proper values for shunts.
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

   % Indices into x array.
   ixPQc = 1:bus.NPQ;
   ixd = (bus.NPQ+1):(bus.NPQ+N);

   niter = 10;
   c = zeros(N, niter);
   d = zeros(N, niter);
   c(bus.iPV, 1) = c(bus.iPV, 1) + (M2PV - V0^2)/(2*V0);
   for n = 1:niter
      % Set c_n for PV busses:
      temp = zeros(bus.NPV, 1);
      for m = 1:n-1
         temp = temp - (c(bus.iPV, m).*c(bus.iPV, n-m) ...
                        + d(bus.iPV, m).*d(bus.iPV, n-m)); 
      end
      c(bus.iPV, n) = c(bus.iPV, n) + temp/(2*V0);

      % Calculate rhs.
      rP = zeros(N, 1);
      rQ = zeros(bus.NPQ, 1);
      if (bus.NPV > 0)
         rP = rP + V0 * G(bus.iPQPV, bus.iPV) * c(bus.iPV, n);
         rQ = rQ + V0 * B(bus.iPQ, bus.iPV) * c(bus.iPV, n);
      end
      if (n == 1)
         rP = rP - bus.P(bus.iPQPV) - V0*I0R(bus.iPQPV);
         rQ = rQ + bus.Q(bus.iPQ) - V0*I0I(bus.iPQ);
      end
      if (n > 1)
         rP = rP + c(:,n-1).*(V0*gs(1:N)-I0R(1:N)) ...
             + d(:,n-1).*(-V0*bs(1:N)-I0I(1:N));
         rQ = rQ + c(bus.iPQ,n-1).*( ...
                  V0*bs(bus.iPQ)-I0I(bus.iPQ)) ...
             + d(bus.iPQ,n-1).*( ...
                  V0*gs(bus.iPQ)+I0R(bus.iPQ));
      end
      for m = 1:n-1
         gc_m_bd = gs(1:N).*c(:,m) - bs(1:N).*d(:,m);
         bc_p_gd = bs(1:N).*c(:,m) + gs(1:N).*d(:,m);
         Gc_m_Bd = G(1:N,1:N)*c(:,m) - B(1:N,1:N)*d(:,m);
         Bc_p_Gd = B(1:N,1:N)*c(:,m) + G(1:N,1:N)*d(:,m);

         rP = rP + c(:,n-m).*(gc_m_bd+Gc_m_Bd);
         rP = rP + d(:,n-m).*(bc_p_gd+Bc_p_Gd);
         rQ = rQ + c(bus.iPQ,n-m).* ...
               (bc_p_gd(bus.iPQ)+Bc_p_Gd(bus.iPQ));
         rQ = rQ - d(bus.iPQ,n-m).* ...
               (gc_m_bd(bus.iPQ)+Gc_m_Bd(bus.iPQ));
      end
      r = [rP;rQ];
      x = A\r;
      c(bus.iPQ, n) = x(ixPQc);
      d(:, n) = x(ixd);
   end
   % Now compute the Pade approximation:
   V = zeros(bus.N, 1);
   for i = 1:N
      McL = transpose([V0, c(i, :) + 1i * d(i, :)]);
      N = (length(McL) - 1) / 2;
      [a, b] = padeapprox(McL, N, N);
      V(i) = eval_pade(a, b, 1);
   end
   V(bus.N) = V0

   S = S_of_V(V, Y);
   [err_P, err_Q_PQ, err_M_PV, err_V_SL, err_PF] = check_pf(bus, Y, S, V)
end
