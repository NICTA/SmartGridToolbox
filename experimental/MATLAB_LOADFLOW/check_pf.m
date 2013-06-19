function [maxerr_P, maxerr_Q_PQ, maxerr_M_PV, maxerr_V_SL] = ...
      check_pf(bus, Y, S, V)
   P = real(S);
   Q = imag(S);
   M = abs(V);
   t = angle(V);

   bus_V_SL = bus.M(bus.iSL) .* exp(1i * bus.t(bus.iSL));

   maxerr_P = max(abs(P(bus.iPQPV) - bus.P(bus.iPQPV)));
   maxerr_Q_PQ = max(abs(Q(bus.iPQ) - bus.Q(bus.iPQ)));
   maxerr_M_PV = max(abs(M(bus.iPV) - bus.M(bus.iPV)));
   maxerr_V_SL = max(abs(V(bus.iSL) - bus_V_SL));
end
