define_constants;

mpc = loadcase("anu.m");
sel = mpc.branch(:, TAP) == 0;

mpc.branch(sel, RATE_A) = 0;
mpc.branch(sel, RATE_B) = 0;
mpc.branch(sel, RATE_C) = 0;

% mpc.branch(sel, BR_X) *= 2;

mpc = runpf(mpc);
savecase("anu_a.m", mpc);
