function [Q, t] = analy_1pvr(bus, branch)
   assert(size(bus, 1) == 2);

   P = bus(1, 3); 
   M = bus(1, 5); 

   VSL = bus(2, 5); 
   tSL = bus(2, 6); 
   assert(tSL == 0);

   assert(size(branch, 1) == 1);
   g = branch(3);
   b = branch(4);
   assert(b == 0);

   Vr = M^2/VSL - P/(g*VSL);
   Vip = sqrt(M^2 - Vr^2);
   t = arg(Vr+I*Vip);
   Q = -g * VSL * Vip;
end
