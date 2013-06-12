function f = f(NPQ, NPV, bus, G, B, x)
   N = NPQ + NPV;
   g = zeros(N, 1);
   h = zeros(N, 1);
   [P Q M t] = get(NPQ, NPV, bus, x)
   for (i = 1:N)
      [Mi ti] = get(NPQ, bus, x, i);
      [Pi Qi] = S(NPQ, NPV, bus, G, B, x);
      g(i) -= Pi/Mi;
      h(i) -= Qi/Mi;
      for (k = 1:N+1) % Last is slack.
         [Pk Qk Mk tk] = get(bus, x, k);
         g(i) += Mk * (G(i, k)*cos(ti-tk) + B(i,k)*sin(ti-tk));
         h(i) += Mk * (G(i, k)*sin(ti-tk) - B(i,k)*cos(ti-tk));
      end
   end

   f = [g; h];
end
