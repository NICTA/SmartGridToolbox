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
   BQQ = B(bus.iPQ, bus.iPQ);
   BAA = B(bus.iPQPV, bus.iPQPV);
   A = [[GQQ;GVQ], BAA; BQQ, GQQ, GQV];
   A *= -V0
   Ai = eye(size(A))/A;

   i1xQc = 1;
   i2xQc = bus.NPQ;
   i1xQd = i2xQc + 1;
   i2xQd = i2xQc + bus.NPQ;
   i1xVd = i2xQd + 1;
   i2xVd = i2xQd + bus.NPV;
   Nx = i2xVd;

   niter = 20;
   c = zeros(N, niter);
   d = zeros(N, niter);

   c(bus.iPV, 1) += (M2PV - V0^2)/(2*V0);
   for n = 1:niter
      % Set c_n for PV busses:
      temp = zeros(bus.NPV, 1);
      for m = 1:n-1
         temp -= (c(bus.iPV, m).*c(bus.iPV, n-m) ...
                  + d(bus.iPV, m).*d(bus.iPV, n-m)); 
      end
      c(bus.iPV, n) += temp/(2*V0);

      % Calculate rhs.
      rP = zeros(N, 1);
      rQ = zeros(bus.NPQ, 1);
      if (n == 1)
         rP += -bus.P(bus.iPQPV) - V0*I0R(bus.iPQPV);
         rQ += bus.Q(bus.iPQ) - V0*I0I(bus.iPQ);
      end
      if (n > 1)
         rP += c(:,n-1).*(V0*gs(1:N)-I0R(1:N)) ...
             + d(:,n-1).*(-V0*bs(1:N)-I0I(1:N));
         rQ += c(bus.iPQ,n-1).*( ...
                  V0*bs(bus.iPQ)-I0I(bus.iPQ)) ...
             + d(bus.iPQ,n-1).*( ...
                  V0*gs(bus.iPQ)+I0R(bus.iPQ));
      end
      for m = 1:n-1
         gc_m_bd = gs(1:N).*c(:,m) - bs(1:N).*d(:,m);
         bc_p_gd = bs(1:N).*c(:,m) + gs(1:N).*d(:,m);
         Gc_m_Bd = G(1:N,1:N)*c(:,m) - B(1:N,1:N)*d(:,m);
         Bc_p_Gd = B(1:N,1:N)*c(:,m) + G(1:N,1:N)*d(:,m);

         rP += c(:,n-m).*(gc_m_bd+Gc_m_Bd);
         rP += d(:,n-m).*(bc_p_gd+Bc_p_Gd);
         rQ += c(bus.iPQ,n-m).* ...
               (bc_p_gd(bus.iPQ)+Bc_p_Gd(bus.iPQ));
         rQ -= d(bus.iPQ,n-m).* ...
               (gc_m_bd(bus.iPQ)+Gc_m_Bd(bus.iPQ));
      end
      r = [rP;rQ];
      x = A\r;
      c(bus.iPQ, n) = x(i1xQc:i2xQc);
      d(:, n) = x(i1xQd:i2xVd);
   end
   V = [V0 + sum(c, 2) + I * sum(d, 2);V0];
   S = calc_S(V, Y);
end
