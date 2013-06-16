function [Vp, Vm] = analy_1pqr(bus, branch)
   assert(size(bus, 1) == 2);

   P = bus(1, 3); 
   Q = bus(1, 4); 
   assert(Q == 0);

   VSL = bus(2, 5); 
   tSL = bus(2, 6); 
   assert(tSL == 0);

   assert(size(branch, 1) == 1);
   g = branch(3);
   b = branch(4);
   assert(b == 0);

   sig = P/(g * VSL^2); 
   Vp = 0.5 + 0.5 * sqrt(1 + 4 * sig)
   Vm = 0.5 - 0.5 * sqrt(1 + 4 * sig)
end
