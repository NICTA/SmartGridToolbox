function modify_matpower_case(case_rel_path, scale_q_load, scale_qmax_gen)
    mpc = loadcase([pwd(), '/', case_rel_path]);
    mpc.bus(:, 4) *= scale_q_load;
    mpc.gen(:, 4) *= scale_qmax_gen;
    savecase('demo_input.m', mpc);
end
