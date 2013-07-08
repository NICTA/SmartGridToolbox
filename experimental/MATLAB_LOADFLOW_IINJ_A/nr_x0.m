function x = nr_x0(bus)
   x(1:bus.NPQ) = bus.V(bus.V);
   x(bus.NPQ+1:2*bus.NPQ) = conj(bus.V);
end
