function f = nr_f(bus, G, B, x)
   [P Q M t] = nr_get(bus, G, B, x);

   g = zeros(bus.NPQ + bus.NPV, 1);
   h = zeros(bus.NPQ, 1);

   g -= P(bus.iPQPV)./M(bus.iPQPV);
   for (i = bus.i1PQ:bus.i2PV)
      for (k = 1:bus.N) % Last is slack.
         tik = t(i) - t(k);
         g(i) += M(k) * (G(i, k)*cos(tik) + B(i,k)*sin(tik));
      end
   end

   h -= Q(bus.iPQ)./M(bus.iPQ);
   for (i = bus.i1PQ:bus.i2PQ)
      for (k = 1:bus.N) % Last is slack.
         tik = t(i) - t(k);
         h(i) += M(k) * (G(i, k)*sin(tik) - B(i,k)*cos(tik));
      end
   end

   f = [g; h];
end
