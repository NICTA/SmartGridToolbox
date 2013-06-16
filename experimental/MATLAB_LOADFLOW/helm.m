function [S V bus branch] = helm(cname); 
   [bus, Y] = init(cname);
   G = real(Y);
   B = imag(Y);
   V0 = bus.M(bus.i1SL);
   M2PV = bus.M(bus.i1PV:bus.i2PV).^2;

   GQQ = G(bus.i1PQ:bus.i2PQ, bus.i1PQ:bus.i2PQ);
   GQV = G(bus.i1PQ:bus.i2PQ, bus.i1PV:bus.i2PV);
   GVQ = G(bus.i1PV:bus.i2PV, bus.i1PQ:bus.i2PQ);
   BQQ = B(bus.i1PQ:bus.i2PQ, bus.i1PQ:bus.i2PQ);
   BAA = B(1:bus.i2PV, 1:bus.i2PV);
   A = [[GQQ;GVQ], BAA; BQQ, GQQ, GQV];
   A *= -bus.M(bus.i1SL);
   Ai = eye(size(A))/A;

   niter = 10;
   cPV = [];
   x = [];
   i1xQc = 1;
   i2xQc = i1xQc + bus.NPQ;
   i1xQd = i2xQc + 1;
   i1xQd = i2xQc + bus.NPQ;
   for n = 1:niter
      x( = M2PV-V0^2;
      for m = 1:n-1
         cPV -= (cPV(:, m).*c(:, n-m) + d(:, m).*d(:, n-m)); 
      end
   end
end
