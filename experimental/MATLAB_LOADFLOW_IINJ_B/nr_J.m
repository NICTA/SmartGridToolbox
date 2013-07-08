function J = nr_J(bus, Y, x)
   i1 = 1; i2 = bus.NPQ; i3 = i2 + 1; i4 = 2 * bus.NPQ;
   PPQ = bus.P(bus.iPQ); QPQ = bus.Q(bus.iPQ);
   G = real(Y); B = imag(Y);
   Vr = x(i1:i2); Vi = x(i3:i4);
   M2 = Vr(bus.iPQ).^2+Vi(bus.iPQ).^2;
   M4 = M2.^2;
   J11diag = diag(-PPQ./M2 + 2 * Vr .* (PPQ.*Vr + QPQ.* Vi)./M4);
   J12diag = diag(-QPQ./M2 + 2 * Vi .* (PPQ.*Vr + QPQ.* Vi)./M4);
   J21diag = diag(QPQ./M2 + 2 * Vr .* (PPQ.*Vi - QPQ.* Vr)./M4);
   J22diag = diag(-PPQ./M2 + 2 * Vi .* (PPQ.*Vi - QPQ.* Vr)./M4);
   J = [J11diag + G(bus.iPQ, bus.iPQ), J12diag - B(bus.iPQ, bus.iPQ); ...
        J21diag + B(bus.iPQ, bus.iPQ), J22diag + G(bus.iPQ, bus.iPQ)];
end
