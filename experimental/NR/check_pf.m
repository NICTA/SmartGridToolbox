function f = check_pf(bus, G, B, P, Q, M, t)
   err_PQ_P = sum((P(bus.i1PQ:bus.i2PQ) - bus.P(bus.i1PQ:bus.i2PQ)).^2)
   assert(err_PQ_P == 0) 
   err_PQ_Q = sum((Q(bus.i1PQ:bus.i2PQ) - bus.Q(bus.i1PQ:bus.i2PQ)).^2)
   assert(err_PQ_Q == 0) 
   err_PV_P = sum((P(bus.i1PV:bus.i2PV) - bus.P(bus.i1PV:bus.i2PV)).^2)
   assert(err_PV_P == 0) 
   err_PV_M = sum((M(bus.i1PV:bus.i2PV) - bus.M(bus.i1PV:bus.i2PV)).^2)
   assert(err_PV_M == 0) 
   err_SL_M = sum((M(bus.i1SL:bus.i2SL) - bus.M(bus.i1SL:bus.i2SL)).^2)
   assert(err_SL_M == 0) 
   err_SL_t = sum((t(bus.i1SL:bus.i2SL) - bus.t(bus.i1SL:bus.i2SL)).^2)
   assert(err_SL_t == 0) 

   V = M .* (cos(t) + I * sin(t));
   Vc = conj(V);
   Yc = G - I * B;
   S = P + I * Q;
   S1 = V .* (Yc * Vc);
   [S S1]
   norm(S - S1)
end
