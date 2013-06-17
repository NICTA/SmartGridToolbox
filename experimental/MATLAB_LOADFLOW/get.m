function [P, Q, M, t] = get(bus, G, B, x)
   M = [x(bus.iPQ); bus.M(bus.iPV); ...
        bus.M(bus.iSL)];
   t = [x(bus.NPQ+1:end); 0];
   [P, Q] = S(bus, G, B, M, t);
end
