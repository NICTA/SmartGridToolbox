function f = nr_f(bus, Y, x)
   i1 = 1; i2 = bus.NPQ; i3 = i2 + 1; i4 = 2 * bus.NPQ;
   PPQ = bus.P(bus.iPQ); QPQ = bus.Q(bus.iPQ);
   G = real(Y); B = imag(Y);
   Vr = [x(i1:i2);real(bus.V(bus.iSL))]; Vi = [x(i3:i4);imag(bus.V(bus.iSL))];
   M2 = Vr(bus.iPQ).^2+Vi(bus.iPQ).^2;
   DR = (-PPQ.*x(i1:i2) - QPQ.*x(i3:i4))./M2 + G(bus.iPQ,:) * Vr - B(bus.iPQ,:) * Vi;
   DI = (-PPQ.*x(i3:i4) + QPQ.*x(i1:i2))./M2 + G(bus.iPQ,:) * Vi + B(bus.iPQ,:) * Vr;
   f = [DR; DI];
end
