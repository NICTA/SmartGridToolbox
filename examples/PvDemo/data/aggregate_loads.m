function loads = aggregate_loads(case_rel_path, max_factor = 1, Q_factor = 1, as_const_Z = false)
    define_constants;
    mpc = loadcase([pwd(), '/', case_rel_path]);
    n_bus = size(mpc.bus, 1);

    SStatic = mpc.bus(:, PD) + I * mpc.bus(:, QD);

    mpc.bus(:, PD) = 0.0;
    mpc.bus(:, QD) = 0.0;

    [dirname, fname, ext] = fileparts(case_rel_path);
    fname_mod = fullfile(dirname, [fname, '_mod', ext])
    savecase(fname_mod, mpc);

    sel = abs(SStatic) > 0;
    idx_sel = [1:n_bus](sel);
    id_sel = mpc.bus(sel, BUS_I);
    KvBaseSel = mpc.bus(sel, BASE_KV);
    SStaticSel = SStatic(sel);

    n_sel = length(idx_sel);

    printf('size = %d\n', n_sel);
    printf('[');
    printf('%d, ', id_sel(1:end-1));
    printf('%d]\n', id_sel(end));

    files = glob('loads/load_[0-9][0-9]*.txt');
    n_load = size(files, 1);
    lds = [];
    for i = 1:n_load
        fname = fullfile('loads', ['load_', num2str(i), '.txt']);
        ld = load_load(fname);
        if (i == 1)
            t = ld(:, 1);
        end
        lds = [lds, ld(:, 2)];
    end

    mean_lds = mean(lds, 2);
    noise_lds = bsxfun(@minus, lds, mean_lds);
    mean_var_noise_lds = mean(var(noise_lds, 1));
    max_mean_lds = max(real(mean_lds)) * max_factor;

    loads = t;
    for i = 1:n_sel
        % Now take the sum of n (>0) randomly choosen loads.
        n = round(real(SStaticSel(i)) / max_mean_lds);
        assert(n > 0); % TODO: if none, scale one load.
        i_ld = randi(n_load, n, 1);
        h_ld = hist(i_ld, 1:n_load);
        noise = zeros(length(t), 1);
        for j = 1: n_load
            h_ld_j = h_ld(j);
            if (h_ld_j > 0)
                noise += h_ld_j * noise_lds(:, j);
            end
        end

        % We only have a limited sample of all possible loads to choose from.
        % This introduces sampling errors. We handle this by scaling the noise.
        noise = noise * sqrt(n * mean_var_noise_lds / var(noise));
        ld = n * mean_lds + noise;
        ld = real(ld) + I * Q_factor * imag(ld);

        loads = [loads, ld];
        printf('static = %f+%fj, n = %d, mean = %f, min = %f, max = %f\n', ...
            real(SStaticSel(i)), imag(SStaticSel(i)), n, mean(real(ld)), ...
            min(real(ld)), max(real(ld)))
        fflush(stdout);

        loadsdirname = fullfile(dirname, 'loads');
        mkdir(loadsdirname);

        fname = fullfile(loadsdirname, ['load_', num2str(id_sel(i)), '.txt']);
        fp = fopen(fname, 'w+');
        z = zeros(size(t));
        if (as_const_Z)
            Y_const = conj(ld) / KvBaseSel(i)^2;
            printArg = [ ...
                t, ...
                real(Y_const), imag(Y_const), ...
                z, z, ...
                z, z];
        else
            S_const = -ld;
            printArg = [ ...
                t, ...
                z, z, ...
                z, z, ...
                real(S_const), imag(S_const)];
        end
        fprintf(fp, '%d %f%+fj %f%+fj %f%+fj\n', printArg');
        fclose(fp);
    end
end

function l = load_load(filename)
    l = load(filename);
    l = [l(:, 1), l(:, 2) + I * l(:, 3)];
end
