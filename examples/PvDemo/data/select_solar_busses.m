function select_solar_busses(case_rel_path, kVMin, kVMax, MWMin, MWMax)
    mpc = loadcase([pwd(), '/', case_rel_path]);

    define_constants
    ids = mpc.bus(:, BUS_I);
    kVBase = mpc.bus(:, BASE_KV);
    MW = mpc.bus(:, PD);

    sel = kVBase >= kVMin & kVBase <= kVMax & MW >= MWMin & MW <= MWMax;
    ids_sel = ids(sel);
    permute_ids(ids_sel);
end
