function [P Q M t] = get(bus, x, i)
   if (bus.type(i) == 1)
      P = bus.P(i);
      Q = bus.Q(i);
      M = x(2*i-1);
      t = x(2*i);
   elseif (bus.type(i) == 2)
      P = bus.P(i);
      Q = x(2*i-1);
      M = bus.M(i); 
      t = x(2*i);
   elseif (bus.type(i) == 0)
      P = NA;
      Q = NA;
      M = bus.M(i);
      t = bus.t(i);
   end
end
