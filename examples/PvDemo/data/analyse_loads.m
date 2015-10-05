function loads = analyse_loads(case_rel_path, max_factor = 1)
    mpc = loadcase([pwd(), '/', case_rel_path]);
    define_constants;
    n_bus = size(mpc.bus, 1);
    PStatic = mpc.bus(:, PD);
    QStatic = mpc.bus(:, QD);
    SStatic = PStatic + I * QStatic;
    sel = SStatic != 0;
    idx_sel = (1:n_bus)'(sel);
    id_sel = mpc.bus(:, BUS_I)(sel);
    
    PSel = PStatic(sel);
    n_house = PSel * 1000 / 1.5;
    penetration = 0.2 * ones(size(PSel));
    ave_area = 15;
    a = n_house .* penetration .* ave_area;
    MVA_max = n_house .* penetration .* 0.002;
    
    rand('state', 1234); p = randperm(size(id_sel));
    id_perm = id_sel(p);
    a_perm = a(p);
    MVA_max_perm = MVA_max(p);

    printf('    n_load_busses: %d\n', length(idx_sel));
    print_int_vec('    load_busses', id_sel);
    printf('    n_solar_busses: %d\n', length(idx_sel));
    print_int_vec('    solar_busses', id_perm);
    print_vec('    solar_area', a_perm);
    print_vec('    solar_S_mag_max', MVA_max_perm);
