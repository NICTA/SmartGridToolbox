function x = x0(bus)
   % 1:NPQ = M of PQ
   % NPQ+1:2*NPQ = t of PQ
   % 2*NPQ+1:2*NPQ+NPV = t of PV
   x = zeros(2 * bus.NPQ + bus.NPV, 1);
   x(1:bus.NPQ) = bus.M(bus.i1SL); % M of PQ busses.
   x += 1e-2 * rand(size(x)); % To hopefully avoid degeneracy ...
end
