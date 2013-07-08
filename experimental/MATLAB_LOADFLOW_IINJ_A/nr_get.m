function [V, Vc] = nr_get(bus, Y, x)
   V = [x(1:bus.NPQ); bus.V(bus.iSL)];
   Vc = [x(bus.NPQ+1:2*bus.NPQ); conj(bus.V(bus.iSL))];
end
