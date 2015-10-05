function print_vec(name, v)
    printf('%s: [', name);
    printf('%.1f, ', v(1:end-1));
    printf('%.1f]\n', v(end));
