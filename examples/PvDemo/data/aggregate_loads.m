function loads = aggregate_loads(case_rel_path, as_const_Z = false)
    define_constants;
    mpc = loadcase([pwd(), '/', case_rel_path]);
    n_bus = size(mpc.bus, 1);

    S_static = mpc.bus(:, PD) + I * mpc.bus(:, QD);
    mean_S_static = mean(S_static);
    S_static_factor = imag(mean_S_static) / real(mean_S_static);

    sel = real(S_static) > 0;

    mpc.bus(sel, PD) = 0.0;
    mpc.bus(sel, QD) = 0.0;
    [dirname, fname, ext] = fileparts(case_rel_path);
    fname_mod = fullfile(dirname, [fname, '_mod', ext]);
    savecase(fname_mod, mpc);

    idx_sel = [1:n_bus](sel);
    id_sel = mpc.bus(sel, BUS_I);
    Kv_base_sel = mpc.bus(sel, BASE_KV);
    S_static_sel = S_static(sel);

    n_sel = length(idx_sel);

    n_load = size(glob('loads/load_[0-9][0-9]*.txt'), 1);
    lds = [];
    for i = 1:n_load
        ld = dlmread(fullfile('loads', ['load_', num2str(i), '.txt']));
        if (i == 1)
            t = ld(:, 1);
        end
        lds = [lds, ld(:, 2)];
    end

    mean_lds = mean(lds, 2);

    mean_mean_lds = mean(mean_lds);
    mean_mean_lds_factor = imag(mean_mean_lds) / real(mean_mean_lds);
    lds = complex(real(lds), imag(lds) * S_static_factor / mean_mean_lds_factor);
    mean_lds = complex(real(mean_lds), imag(mean_lds) * S_static_factor / mean_mean_lds_factor);

    noise_lds = bsxfun(@minus, lds, mean_lds);

    mean_var_noise_lds = mean(var(noise_lds, 1));
    max_mean_lds = max(real(mean_lds));

    loads = t;
    for i = 1:n_sel
        % Now take the sum of n (>0) randomly choosen loads.
        n = round(real(S_static_sel(i)) / max_mean_lds);
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

        loads = [loads, ld];
        printf('static = %f+%fj, n = %d, mean = %f, min = %f, max = %f\n', ...
            real(S_static_sel(i)), imag(S_static_sel(i)), n, mean(real(ld)), ...
            min(real(ld)), max(real(ld)))
        fflush(stdout);

        loadsdirname = fullfile(dirname, 'loads');
        mkdir(loadsdirname);

        fname = fullfile(loadsdirname, ['load_', num2str(id_sel(i)), '.txt']);
        fp = fopen(fname, 'w+');
        z = zeros(size(t));
        if (as_const_Z)
            Y_const = conj(ld) / Kv_base_sel(i)^2;
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
