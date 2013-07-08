function f = nr_J(bus, Y, x)
   V = [x(1:NPQ); bus.V(bus.iSL)];
   Vc = [x(NPQ+1:2*NPQ); conj(bus.V(bus.iSL))];
   SPQ = bus.S(bus.iPQ);
   ScPQ = conj(SPQ);
   Yc = conj(Y);
   J = [Y, Sc./transpose(Vc); S./transpose(V), Yc];
end
