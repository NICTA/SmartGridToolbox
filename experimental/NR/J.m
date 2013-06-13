function J = J(bus, G, B, x)

   [P Q M t] = get(bus, G, B, x);

   N = bus.NPQ + bus.NPV;
   dgdM = zeros(N, bus.NPQ); 
   dgdt = zeros(N, N); 
   dhdM = zeros(bus.NPQ, bus.NPQ); 
   dhdt = zeros(bus.NPQ, N); 

   for (i = 1:bus.NPQ)
      dgdM(i, i) += P(i)/(M(i)^2);
      dhdM(i, i) += Q(i)/(M(i)^2);
   end

   for (i = 1:N)
      for (k = 1:bus.NPQ)
         tik = t(i) - t(k);
         dgdM(i, k) += G(i, k)*cos(tik)+B(i, k)*sin(tik);
      end
   end

   for (i = 1:bus.NPQ)
      for (k = 1:bus.NPQ)
         tik = t(i) - t(k);
         dhdM(i, k) += G(i, k)*sin(tik)-B(i, k)*cos(tik);
      end
   end

   for (i = 1:N)
      for (k = 1:N)
         tik = t(i) - t(k);
         dgdt(i, k) += M(k) * (G(i,k)*sin(tik) - B(i, k)*cos(tik));
      end
      for (k = 1:N+1) % Last is slack.
         tik = t(i) - t(k);
         dgdt(i, i) += M(k)*(-G(i,k)*sin(tik)+B(i,k)*cos(tik));
      end
   end

   for (i = 1:bus.NPQ)
      for (k = 1:N)
         tik = t(i) - t(k);
         dhdt(i, k) += M(k) * (-G(i,k)*cos(tik) - B(i, k)*sin(tik));
      end
      for (k = 1:N+1) % Last is slack.
         tik = t(i) - t(k);
         dhdt(i, i) += M(k)*(G(i,k)*cos(tik)+B(i,k)*sin(tik));
      end
   end

   J = zeros(2*bus.NPQ+bus.NPV, 2*bus.NPQ+bus.NPV);
   J(1:N, 1:bus.NPQ) = dgdM;
   J(1:N, bus.NPQ+1:end) = dgdt;
   J(N+1:N+bus.NPQ, 1:bus.NPQ) = dhdM;
   J(N+1:N+bus.NPQ, bus.NPQ+1:end) = dhdt;
end
