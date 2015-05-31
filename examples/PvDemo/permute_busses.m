function permute_busses(case_rel_path, kVb_min, kVb_max)
    mpc = loadcase([pwd(), '/', case_rel_path]);
    ids = mpc.bus(:, 1);
    kVb = mpc.bus(:, 10);
    ids = ids(kVb >= kVb_min & kVb <= kVb_max); 
    p = randperm(size(ids));
    ids_perm = ids(p);
    printf('size = %d\n', size(ids_perm, 1));
    printf('[');
    printf('%d, ', ids_perm(1:end-1));
    printf('%d]\n', ids_perm(end));
