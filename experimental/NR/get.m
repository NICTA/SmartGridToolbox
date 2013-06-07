function [P Q M t] = get(bus, x, i)
   if (bus(i, 2) == 1)
      P = bus(i, 3);
      Q = bus(i, 4);
      M = x(2*i-1);
      t = x(2*i);
   elseif (bus(i, 2) == 2)
      P = bus(i, 3);
      Q = x(2*i-1);
      M = bus(i, 5); 
      t = x(2*i);
   elseif (bus(i, 2) == 0)
      P = NA;
      Q = NA;
      M = bus(i, 5);
      t = bus(i, 6);
   end
end
