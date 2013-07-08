function f = nr_f(bus, Y, x)
   [V, Vc] = nr_get(bus, Y, x);
   Yc = conj(Y);
   SPQ = bus.S(bus.iPQ);
   SPQc = conj(SPQ);
   DI = -SPQc ./ Vc(bus.iPQ) + Y(bus.iPQ, :) * V;
   DIc = -SPQ ./ V(bus.iPQ) + Yc(bus.iPQ, :) * Vc;
   f = [DI; DIc];
end
