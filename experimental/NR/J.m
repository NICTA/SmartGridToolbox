function J = J(NPQ, NPV, bus, G, B, x)
   N = NPQ + NPV;
   dgd1 = zeros(N, N); 
   dhd1 = zeros(N, N); 
   dgd2 = zeros(N, N); 
   dhd2 = zeros(N, N); 

   for (i = 1:NPQ)
      [Pi Qi Mi ti] = get(bus, x, i);
      dgd1(i, i) += Pi/(Mi^2);
      dhd1(i, i) += Qi/(Mi^2);
   end

   for (i = 1:N)
      [Pi Qi Mi ti] = get(bus, x, i);
      for (k = 1:NPQ)
         [Pk Qk Mk tk] = get(bus, x, k);
         dgd1(i, k) += G(i, k)*cos(ti-tk)-B(i, k)*sin(ti-tk);
         dhd1(i, k) += G(i, k)*sin(ti-tk)+B(i, k)*cos(ti-tk);
      end
   end

   for (i = NPQ+1:N)
      [Pi Qi Mi ti] = get(bus, x, i);
      dhd1(i, i) += -1/Mi;
   end

   for (i = 1:N)
      [Pi Qi Mi ti] = get(bus, x, i);
      for (k = 1:N)
         [Pk Qk Mk tk] = get(bus, x, k);
         dgd2(i, k) += Mk * (G(i,k)*sin(ti-tk) + B(i, k)*cos(ti-tk));
         dhd2(i, k) += Mk * (-G(i,k)*cos(ti-tk) + B(i, k)*sin(ti-tk));
      end
      for (k = 1:N+1) % Last is slack.
         [Pk Qk Mk tk] = get(bus, x, k);
         dgd2(i, i) += Mk*(-G(i,k)*sin(ti-tk)-B(i,k)*cos(ti-tk));
         dhd2(i, i) += Mk*(G(i,k)*cos(ti-tk)-B(i,k)*sin(ti-tk));
      end
   end
   J = zeros(2*N, 2*N);
   J(:, 1:2:end-1) = [dgd1; dhd1];
   J(:, 2:2:end) = [dgd2; dhd2];
end
