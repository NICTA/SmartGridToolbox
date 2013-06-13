function J = J(bus, G, B, x)

   [P Q M t] = get(bus, G, B, x);

   N = bus.NPQ + bus.NPV;

   dgdM = sparse(N, bus.NPQ); 
   dgdt = sparse(N, N); 
   dhdM = sparse(N, bus.NPQ); 
   dhdt = sparse(N, N); 
   dhdQ = sparse(bus.NPV, bus.NPV); 

   for (i = 1:bus.NPV)
      dhdQ(i, i) = -1/M(i);
   end

   for (i = 1:bus.NPQ)
      dgdM(i, i) += P(i)/(M(i)^2);
      dhdM(i, i) += Q(i)/(M(i)^2);
   end

   for (i = 1:N)
      for (k = 1:bus.NPQ)
         tik = t(i) - t(k);
         dgdM(i, k) += G(i, k)*cos(tik)+B(i, k)*sin(tik);
         dhdM(i, k) += G(i, k)*sin(tik)-B(i, k)*cos(tik);
      end
   end

   for (i = 1:N)
      for (k = 1:N)
         tik = t(i) - t(k);
         dgdt(i, k) += M(k) * (G(i,k)*sin(tik) - B(i, k)*cos(tik));
         dhdt(i, k) += M(k) * (-G(i,k)*cos(tik) - B(i, k)*sin(tik));
      end
      for (k = 1:N+1) % Last is slack.
         tik = t(i) - t(k);
         dgdt(i, i) += M(k)*(-G(i,k)*sin(tik)+B(i,k)*cos(tik));
         dhdt(i, i) += M(k)*(G(i,k)*cos(tik)+B(i,k)*sin(tik));
      end
   end

   NJ = 2*N;
   J = sparse(NJ, NJ);

   J(N+bus.NPQ+1:N+bus.NPQ+bus.NPV, 2*bus.NPQ+1:2*bus.NPQ+bus.NPV) = dhdQ;
   J(1:N, 1:bus.NPQ) = dgdM;
   J(N+1:2*N, 1:bus.NPQ) = dhdM;
   J(1:N, bus.NPQ+1:2*bus.NPQ) = dgdt(:, 1:bus.NPQ);
   J(1:N, 2*bus.NPQ+bus.NPV+1:end) = dgdt(:, bus.NPQ+1:end);
   J(N+1:2*N, bus.NPQ+1:2*bus.NPQ) = dhdt(:, 1:bus.NPQ);
   J(N+1:2*N, 2*bus.NPQ+bus.NPV+1:end) = dhdt(:, bus.NPQ+1:end);
   %printf('Sparsity of J = %f%%\n', 100 * nnz(J)/prod(size(J)))
end
