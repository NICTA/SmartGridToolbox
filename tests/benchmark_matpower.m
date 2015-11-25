#!/usr/bin/env octave -q

arg_list = argv();
c = arg_list{1};
define_constants;
mpc = loadcase(c);
n = size(mpc.bus, 1)

mpc.bus(:, VM) = 1; 
mpc.bus(:, VA) = 0; 

mpc.gen(:, QG) = 0;

sel_sl = mpc.bus(:, 2) == 3;
i_sl = mpc.bus(sel_sl, 1);
sel_sl_gen = ismember(mpc.gen(:, GEN_BUS), i_sl);
mpc.gen(sel_sl_gen, PG) = 0;

[results, success] = runpf(mpc, mpoption('verbose', 2, 'pf.nr.max_it', 100));

printf('%d %d %f\n', n, success, results.et)
