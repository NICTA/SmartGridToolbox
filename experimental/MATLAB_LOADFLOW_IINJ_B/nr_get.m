function V = nr_get(bus, Y, x)
   V = [x(1:bus.NPQ); bus.V(bus.iSL)];
end
