function f = nr_f(bus, Y, x)
   V = [x(1:NPQ); bus.V(bus.iSL)];
   Vc = [x(NPQ+1:2*NPQ); conj(bus.V(bus.iSL))];
   SPQ = bus.S(bus.iPQ);
   Yc = conj(Y);
   DI = -SPQ ./ VPQ + Yc(iPQ, :) * Vc);
   DIc = -conj(SPQ) ./ VcPQ + Y(iPQ, :) * V;
   f = [DI; DIc];
end
