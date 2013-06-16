function [P, Q, M, t] = get(bus, G, B, x)
   M = [x(bus.i1PQ:bus.i2PQ); bus.M(bus.i1PV:bus.i2PV); ...
        bus.M(bus.i1SL:bus.i2SL)];
   t = [x(bus.NPQ+1:end); 0];
   [P, Q] = S(bus, G, B, M, t);
end
