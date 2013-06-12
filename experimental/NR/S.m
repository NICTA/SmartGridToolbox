function [P Q] = S(NPQ, NPV, bus, G, B, M, t)
   Mc = M .* cos(t);
   Ms = M .* sin(t);
   i1PV = NPQ+1;
   i2PV = NPQ+NPV;
   iS = NPQ+NPV+1;
   QPV = Mc(i1PV:i2PV) .* (-GPV(i1PV:i2PV,:)*Ms - B(i1PV:i2PV,:)*Mc) ...
       + Ms(i1PV:i2PV) .* ( GPV(i1PV:i2PV,:)*Mc - B(i1PV:i2PV,:)*Ms);
   PS = Mc(iS) .* (GPV(iS,:)*Mc - B(iS,:)*Ms) ...
      + Ms(is) .* (GPV(iS,:)*Ms + B(i1PV:i2PV,:)*Mc);
   P = bus.P(1:NPQ+NPV);
   Q = [bus.Q(1:NPQ);QPV];
end
