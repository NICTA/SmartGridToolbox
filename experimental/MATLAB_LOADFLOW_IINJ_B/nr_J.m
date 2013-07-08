function J = nr_J(bus, Y, x)
   [V, Vc] = nr_get(bus, Y, x);
   Yc = conj(Y);
   SPQ = bus.S(bus.iPQ);
   SPQc = conj(SPQ);
   J = [Y(bus.iPQ, bus.iPQ), bsxfun(@rdivide, SPQc, transpose(Vc(bus.iPQ))); ...
        bsxfun(@rdivide, SPQ, transpose(V(bus.iPQ))), Yc(bus.iPQ, bus.iPQ)];
end
