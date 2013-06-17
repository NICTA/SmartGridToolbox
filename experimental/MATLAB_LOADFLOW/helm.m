function [S V bus] = helm(cname); 
   [bus, Y] = init(cname);
   NQV = bus.NPQ + bus.NPV;
   G = real(Y);
   B = imag(Y);
   V0 = bus.M(bus.i1SL);
   M2PV = bus.M(bus.i1PV:bus.i2PV).^2;
   I0R = zeros(bus.N, 1); % TODO: get proper values for shunts.
   I0I = zeros(bus.N, 1); % TODO: get proper values for shunts.
   gs = bus.gs;
   bs = bus.bs;

   GQQ = G(bus.i1PQ:bus.i2PQ, bus.i1PQ:bus.i2PQ);
   GQV = G(bus.i1PQ:bus.i2PQ, bus.i1PV:bus.i2PV);
   GVQ = G(bus.i1PV:bus.i2PV, bus.i1PQ:bus.i2PQ);
   BQQ = B(bus.i1PQ:bus.i2PQ, bus.i1PQ:bus.i2PQ);
   BAA = B(1:bus.i2PV, 1:bus.i2PV);
   A = [[GQQ;GVQ], BAA; BQQ, GQQ, GQV];
   A *= -V0;
   Ai = eye(size(A))/A;

   i1xQc = 1;
   i2xQc = bus.NPQ;
   i1xQd = i2xQc + 1;
   i2xQd = i2xQc + bus.NPQ;
   i1xVd = i2xQd + 1;
   i2xVd = i2xQd + bus.NPV;
   Nx = i2xVd;

   niter = 20;
   c = zeros(NQV, niter);
   d = zeros(NQV, niter);

   c(bus.i1PV:bus.i2PV, 1) += (M2PV - V0^2)/(2*V0);
   for n = 1:niter
      % Set c_n for PV busses:
      temp = zeros(bus.NPV, 1);
      for m = 1:n-1
         temp -= (c(bus.i1PV:bus.i2PV, m).*c(bus.i1PV:bus.i2PV, n-m) ...
                  + d(bus.i1PV:bus.i2PV, m).*d(bus.i1PV:bus.i2PV, n-m)); 
      end
      c(bus.i1PV:bus.i2PV, n) += temp/(2*V0);

      % Calculate rhs.
      rP = zeros(NQV, 1);
      rQ = zeros(bus.NPQ, 1);
      if (n == 1)
         rP += -bus.P(bus.i1PQ:bus.i2PV) - V0*I0R(bus.i1PQ:bus.i2PV);
         rQ += bus.Q(bus.i1PQ:bus.i2PQ) - V0*I0I(bus.i1PQ:bus.i2PQ);
      end
      if (n > 1)
         rP += c(:,n-1).*(V0*gs(1:NQV)-I0R(1:NQV)) ...
             + d(:,n-1).*(-V0*bs(1:NQV)-I0I(1:NQV));
         rQ += c(bus.i1PQ:bus.i2PQ,n-1).*( ...
                  V0*bs(bus.i1PQ:bus.i2PQ)-I0I(bus.i1PQ:bus.i2PQ)) ...
             + d(bus.i1PQ:bus.i2PQ,n-1).*( ...
                  V0*gs(bus.i1PQ:bus.i2PQ)+I0R(bus.i1PQ:bus.i2PQ));
      end
      for m = 1:n-1
         gc_m_bd = gs(1:NQV).*c(:,m) - bs(1:NQV).*d(:,m);
         bc_p_gd = bs(1:NQV).*c(:,m) + gs(1:NQV).*d(:,m);
         Gc_m_Bd = G(1:NQV,1:NQV)*c(:,m) - B(1:NQV,1:NQV)*d(:,m);
         Bc_p_Gd = B(1:NQV,1:NQV)*c(:,m) + G(1:NQV,1:NQV)*d(:,m);

         rP += c(:,n-m).*(gc_m_bd+Gc_m_Bd);
         rP += d(:,n-m).*(bc_p_gd+Bc_p_Gd);
         rQ += c(bus.i1PQ:bus.i2PQ,n-m).* ...
               (bc_p_gd(bus.i1PQ:bus.i2PQ)+Bc_p_Gd(bus.i1PQ:bus.i2PQ));
         rQ -= d(bus.i1PQ:bus.i2PQ,n-m).* ...
               (gc_m_bd(bus.i1PQ:bus.i2PQ)+Gc_m_Bd(bus.i1PQ:bus.i2PQ));
      end
      r = [rP;rQ];
      x = A\r;
      c(bus.i1PQ:bus.i2PQ, n) = x(i1xQc:i2xQc);
      d(:, n) = x(i1xQd:i2xVd);
   end
   V = [V0 + sum(c, 2) + I * sum(d, 2);V0];
   S = calc_S(V, Y);
end
