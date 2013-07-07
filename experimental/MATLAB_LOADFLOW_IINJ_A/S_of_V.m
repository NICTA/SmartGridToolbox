function S = S_of_V(V, bus, Y)
   Ic = bus.IcR + 1i * bus.IcI;
   S = conj(conj(V) .* (-Ic + Y * V));
end
