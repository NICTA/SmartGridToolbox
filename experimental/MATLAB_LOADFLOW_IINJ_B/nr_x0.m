function x = nr_x0(bus)
   x = zeros(2*bus.NPQ, 1);
   x(1:bus.NPQ) = bus.V(bus.iSL);
   x(bus.NPQ+1:2*bus.NPQ) = conj(bus.V(bus.iSL));
end
