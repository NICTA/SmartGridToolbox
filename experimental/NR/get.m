function [P, Q, M, t] = get(NPQ, NPV, bus, G, B, x)
   M = [x(1:NPQ); bus.M(NPQ+1:NPQ+NPV); bus.M(end)];
   t = x(NPQ+1:end; 0);
   [P, Q] = S(NPQ, NPV, bus, G, B, M, t);
end
