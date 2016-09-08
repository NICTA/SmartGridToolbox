function loads = aggregate_loads(case_rel_path)
    define_constants;
    mpc = loadcase([pwd(), '/', case_rel_path]);
    mpc.bus(:, PD) = 0.0;
    mpc.bus(:, QD) = 0.0;
    [dirname, fname, ext] = fileparts(case_rel_path);
    fname_mod = fullfile(dirname, [fname, '_mod', ext]);
    savecase(fname_mod, mpc);
end
