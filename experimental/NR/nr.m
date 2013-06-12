[busdata, branchdata] = matpower2nr("case2PV.m")

bus.id = busdata(:, 1);
bus.type = busdata(:, 2);
bus.P = busdata(:, 3);
bus.Q = busdata(:, 4);
bus.M = busdata(:, 5);
bus.t = busdata(:, 6);
NBUS = size(bus.id, 1);

branch.from = branchdata(:, 1);
branch.to = branchdata(:, 2);
branch.g = branchdata(:, 3);
branch.b = branchdata(:, 4);
NBRANCH = size(branch.from);

NPQ = 0;
NPV = 0;
NSL = 0;
NOTHER = 0;
type_prev = 0;
nbus_prev = 0;
for i = 1:NBUS
   nbus = bus.id(i);
   assert(nbus == nbus_prev + 1);
   type = bus.type(i);
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
for i = 1:NBRANCH
   k = branch.from(i);
   l = branch.to(i);
   assert(l > k);
   ykl = branch.g(i) + I * branch.b(i);
   Y(k, l) -= ykl;
   Y(l, k) -= ykl;
   Y(k, k) += ykl;
   Y(l, l) += ykl;
end
G = real(Y);
B = imag(Y);

x = x0(NPQ, NPV, bus)

maxiter = 100;
tol = 1e-20;
for i = 1:maxiter
   fx = f(NPQ, NPV, bus, G, B, x)
   Jx = J(NPQ, NPV, bus, G, B, x);
   x = x - Jx\fx;
   err = max(fx.^2)
   if (err < tol)
      break;
   end
end
x
i
