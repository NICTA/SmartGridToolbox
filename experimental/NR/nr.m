% Expects bus, branch to be defined.
NBUS = size(bus, 1);

NPQ = 0;
NPV = 0;
NSL = 0;
NOTHER = 0;
type_prev = 0;
nbus_prev = 0;
for i = 1:NBUS
   nbus = bus(i, 1);
   assert(nbus == nbus_prev + 1);

   type = bus(i, 2);

   if (type == 1)
      NPQ += 1;
   elseif (type == 2)
      NPV += 1;
   elseif (type == 0)
      NSL += 1;
   else
     NOTHER += 1;
   end

   nbus_prev = nbus;
   type_prev = type;
end
assert(NSL == 1);
assert(NOTHER == 0);

Y = zeros(NBUS, NBUS);
for i = 1:size(branch, 1)
   k = branch(i, 1);
   l = branch(i, 2);
   assert(l > k);
   ykl = branch(i, 3) + I * branch(1, 4);
   Y(k, l) -= ykl;
   Y(l, k) -= ykl;
   Y(k, k) += ykl;
   Y(l, l) += ykl;
end
G = real(Y);
B = imag(Y);


x = x0(NPQ, NPV, bus);

maxiter = 10;
tol = 1e-20;
for i = 1:maxiter
   fx = f(NPQ, NPV, bus, G, B, x);
   Jx = J(NPQ, NPV, bus, G, B, x);
   x = x - Jx\fx;
   err = max(fx.^2)
   if (err < tol)
      break;
   end
end
x
i
