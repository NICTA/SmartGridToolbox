function f = f(bus, G, B, x)
   [P Q M t] = get(bus, G, B, x);

   g = zeros(bus.NPQ + bus.NPV, 1);
   h = zeros(bus.NPQ + bus.NPV, 1);

   for (i = bus.i1PQ:bus.i2PV)
      g(i) -= P(i)/M(i);
      h(i) -= Q(i)/M(i);
      for (k = 1:bus.N) % Last is slack.
         tik = t(i) - t(k);
         g(i) += M(k) * (G(i, k)*cos(tik) + B(i,k)*sin(tik));
         h(i) += M(k) * (G(i, k)*sin(tik) - B(i,k)*cos(tik));
      end
   end

   f = [g; h];
end
