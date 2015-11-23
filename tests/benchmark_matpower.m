#!/usr/bin/env octave -q

arg_list = argv();
c = arg_list{1};
define_constants;
mpc = loadcase(c);
n = size(mpc.bus, 1)
mpc.bus(:, VM) = 1; 
mpc.bus(:, VA) = 0; 
[results, success] = runpf(mpc, mpoption('verbose', 0));

printf('%d %d %f\n', n, success, results.et)
