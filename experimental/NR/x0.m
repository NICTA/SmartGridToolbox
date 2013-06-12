function x = x0(NPQ, NPV, bus)
   N = NPQ + NPV;
   x = zeros(2 * N, 1);
   x(1:2:2*N-1) = bus.M(N+1);
   x(2:2:2*N) = bus.t(N+1);
   x += 1e-2 * rand(size(x)); % To hopefully avoid degeneracy ...
end
