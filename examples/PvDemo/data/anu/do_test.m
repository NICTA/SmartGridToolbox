define_constants;

mpc_orig = loadcase("anu.m");
sel = mpc_orig.branch(:, TAP) == 0;

mpc = mpc_orig;
    
mpc.branch(sel, RATE_A) = 0;
mpc.branch(sel, RATE_B) = 0;
mpc.branch(sel, RATE_C) = 0;

mpc.branch(sel, BR_X) *= 2;

mult_factors = -5:0.5:5;

result_Q = [];
for mult_factor = mult_factors
    mpc.bus(:, QD) = mpc_orig.bus(:, QD) * mult_factor; 
    mpc = runpf(mpc);
    min_V = min(mpc.bus(:, VM))
    max_V = max(mpc.bus(:, VM))
    result_Q = [result_Q; min_V, max_V];
end
mpc.bus(:, QD) = mpc_orig.bus(:, QD);

result_P = [];
for mult_factor = mult_factors
    mpc.bus(:, PD) = mpc_orig.bus(:, PD) * mult_factor; 
    mpc = runpf(mpc);
    min_V = min(mpc.bus(:, VM))
    max_V = max(mpc.bus(:, VM))
    result_P = [result_P; min_V, max_V];
end
mpc.bus(:, PD) = mpc_orig.bus(:, PD);

result_PQ = [];
for mult_factor = mult_factors
    mpc.bus(:, [PD, QD]) = mpc_orig.bus(:, [PD, QD]) * mult_factor; 
    mpc = runpf(mpc);
    min_V = min(mpc.bus(:, VM))
    max_V = max(mpc.bus(:, VM))
    result_PQ = [result_PQ; min_V, max_V];
end
mpc.bus(:, PD) = mpc_orig.bus(:, PD);

plot(  mult_factors, result_P, 'b.-' ...
     , mult_factors, result_Q, 'g.-' ...
     , mult_factors, result_PQ, 'r.-');
