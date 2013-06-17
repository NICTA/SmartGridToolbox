function [P Q] = S(bus, G, B, M, t)
   Mc = M .* cos(t);
   Ms = M .* sin(t);
   QPV = Mc(bus.iPV) .* (-G(bus.iPV,:)*Ms - B(bus.iPV,:)*Mc) ...
       + Ms(bus.iPV) .* ( G(bus.iPV,:)*Mc - B(bus.iPV,:)*Ms);
   PSL = Mc(bus.iSL) .* ( G(bus.iSL,:)*Mc - B(bus.iSL,:)*Ms) ...
       + Ms(bus.iSL) .* ( G(bus.iSL,:)*Ms + B(bus.iSL,:)*Mc);
   QSL = Mc(bus.iSL) .* (-G(bus.iSL,:)*Ms - B(bus.iSL,:)*Mc) ...
       + Ms(bus.iSL) .* ( G(bus.iSL,:)*Mc - B(bus.iSL,:)*Ms);
   P = [bus.P(bus.iPQPV);PSL];
   Q = [bus.Q(bus.iPQ);QPV;QSL];
end
