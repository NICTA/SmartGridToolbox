function loads = aggregate_loads(case_rel_path, max_factor = 1)
    mpc = loadcase([pwd(), '/', case_rel_path]);
    n_bus = size(mpc.bus, 1);
    SStatic = mpc.bus(:, 3) + I * mpc.bus(:, 4);

    mpc.bus(:, 3) = 0.0;
    mpc.bus(:, 4) = 0.0;
    [dirname, fname, ext] = fileparts(case_rel_path);
    fname_mod = fullfile(dirname, [fname, '_mod', ext])
    savecase(fname_mod, mpc);

    sel = abs(SStatic) > 0;
    id_sel = [1:n_bus](sel);
    n_sel = length(id_sel);
    SStaticSel = SStatic(sel);
    printf('size = %d\n', n_sel);
    printf('[');
    printf('%d, ', id_sel(1:end-1));
    printf('%d]\n', id_sel(end));

    files = glob('loads/load_[0-9][0-9]*.txt');
    n_load = size(files, 1);
    l = [];
    for i = 1:n_load
        fname = fullfile('loads', ['load_', num2str(i), '.txt']);
        l_i = load_load(fname);
        if (i == 1)
            t = l_i(:, 1);
        end
        l = [l, l_i(:, 2)];
    end

    mean_l = mean(l, 2);
    noise_l = bsxfun(@minus, l, mean_l);
    mean_var_noise_l = mean(var(noise_l, 1));
    l_max = max(real(mean_l)) * max_factor;

    loads = t;
    for i = 1:n_sel
        % Now take the sum of n (>0) randomly choosen loads.
        n = round(real(SStaticSel(i)) / l_max);
        assert(n > 0); % TODO: if none, scale one load.
        i_load = randi(n_load, n, 1);
        h_load = hist(i_load, 1:n_load);
        noise_l_i = zeros(length(t), 1);
        for j = 1: n_load
            h_load_j = h_load(j);
            if (h_load_j > 0)
                noise_l_i += h_load_j * noise_l(:, j);
            end
        end

        % We only have a limited sample of all possible loads to choose from.
        % This introduces sampling errors. We handle this by scaling the noise.
        noise_l_i = noise_l_i * sqrt(n * mean_var_noise_l / var(noise_l_i));
        l_i = n * mean_l + noise_l_i;

        loads = [loads, l_i];
        printf('static = %f+%fj, n = %d, mean = %f, min = %f, max = %f\n', ...
            real(SStaticSel(i)), imag(SStaticSel(i)), n, mean(real(l_i)), ...
            min(real(l_i)), max(real(l_i)))
        fflush(stdout);

        loadsdirname = fullfile(dirname, 'loads');
        mkdir(loadsdirname);

        fname = fullfile(loadsdirname, ['load_', num2str(id_sel(i)), '.txt']);
        fp = fopen(fname, 'w+');
        z = zeros(size(t));
        printArg = [ ...
            t, ...
            z, z, ...
            z, z, ...
            real(l_i), imag(l_i)];
        fprintf(fp, '%d %f+%fj %f+%fj %f+%fj\n', printArg');
        fclose(fp);
    end
end

function l = load_load(filename)
    l = load(filename);
    l = [l(:, 1), l(:, 2) + I * l(:, 3)];
end
