function [maxerr_P, maxerr_Q_PQ, maxerr_M_PV] = ...
      check_pf(bus, Y, S, V)
   P = real(S);
   Q = imag(S);
   M = abs(V);
   t = angle(V);

   maxerr_P = max(abs(P(bus.iPQPV) - bus.P(bus.iPQPV)));
   maxerr_Q_PQ = max(abs(Q(bus.iPQ) - bus.Q(bus.iPQ)));
   maxerr_M_PV = max(abs(M(bus.iPV) - bus.M(bus.iPV)));
end
