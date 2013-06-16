function J = J(bus, G, B, x)

   [P Q M t] = get(bus, G, B, x);

   N = bus.NPQ + bus.NPV;
   dgdM = sparse(N, bus.NPQ); 
   dgdt = sparse(N, N); 
   dhdM = sparse(bus.NPQ, bus.NPQ); 
   dhdt = sparse(bus.NPQ, N); 

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

   NJ = 2*bus.NPQ+bus.NPV;
   J = sparse(NJ, NJ);
   J(1:N, 1:bus.NPQ) = dgdM;
   J(1:N, bus.NPQ+1:end) = dgdt;
   J(N+1:N+bus.NPQ, 1:bus.NPQ) = dhdM;
   J(N+1:N+bus.NPQ, bus.NPQ+1:end) = dhdt;
   %printf('Sparsity of J = %f%%\n', 100 * nnz(J)/prod(size(J)))
end
