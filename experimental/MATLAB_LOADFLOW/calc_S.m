function S = calc_S(V, Y)
   Vc = conj(V);
   Yc = conj(Y);
   S = V .* (Yc * Vc);
end
