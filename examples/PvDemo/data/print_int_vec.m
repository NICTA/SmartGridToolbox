function print_int_vec(name, v)
    printf('%s: [', name);
    printf('%d, ', v(1:end-1));
    printf('%d]\n', v(end));
