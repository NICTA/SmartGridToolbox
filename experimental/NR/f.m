function f = f(NPQ, NPV, bus, G, B, x)
   N = NPQ + NPV;
   g = zeros(N, 1);
   h = zeros(N, 1);
   for (i = 1:N)
      [Pi Qi Mi ti] = get(bus, x, i);

      g(i) -= Pi/Mi;
      h(i) -= Qi/Mi;
      for (k = 1:N+1) % Last is slack.
         [Pk Qk Mk tk] = get(bus, x, k);
         g(i) += Mk * (G(i, k)*cos(ti-tk) - B(i,k)*sin(ti-tk));
         h(i) += Mk * (G(i, k)*sin(ti-tk) + B(i,k)*cos(ti-tk));
      end
   end

   f = [g; h];
end
