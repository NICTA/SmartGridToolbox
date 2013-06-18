function [err_P, err_Q_PQ, err_M_PV, err_V_SL, err_PF] = check_pf(bus, Y, S, V)
   P = real(S);
   Q = imag(S);
   M = abs(V);
   t = angle(V);

   SPF = S_of_V(V, Y);

   err_P = P(bus.iPQPV) - bus.P(bus.iPQPV);
   err_Q_PQ = Q(bus.iPQ) - bus.Q(bus.iPQ);
   err_M_PV = M(bus.iPV) - bus.M(bus.iPV);
   bus_V_SL = bus.M(bus.iSL) .* exp(I * bus.t(bus.iSL));
   err_V_SL = V(bus.iSL) - bus_V_SL;
   err_PF = norm(S - SPF);
end
