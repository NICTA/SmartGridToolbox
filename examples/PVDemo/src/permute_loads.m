function permute_busses(case_rel_path)
    mpc = loadcase([pwd(), '/', case_rel_path]);
    ids = mpc.bus(:, 1);
    p = randperm(size(ids));
    ids_perm = ids(p);
    printf('[');
    printf('%d, ', ids_perm(1:end-1));
    printf('%d]\n', ids_perm(end));
