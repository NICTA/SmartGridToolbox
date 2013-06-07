function x = x0(NPQ, NPV, bus)
   N = NPQ + NPV;
   x = zeros(2 * N, 1);
   x(1:2:2*NPQ-1) = bus(end, 5);
   x(2:2:2*N) = bus(end, 6);
   x += 1e-2 * rand(size(x)); % To hopefully avoid degeneracy ...
end
