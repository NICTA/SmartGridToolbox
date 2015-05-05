function aggregate_loads(case_rel_path, poiss_lambda, sd_factor, max_factor)

    mpc = loadcase([pwd(), '/', case_rel_path]);

    nbus = rows(mpc.bus);

    SStatic = -mpc.bus(:, 3) - I * mpc.bus(:, 4);

    mkdir('aggregate_loads');
    for i = 1:rows(SStatic)
        n_load = poissrnd(poiss_lambda);
        for j = 1:n_load
            lij = load(['loads/load_', num2str(randi([0, 49])), '.txt'])(:, [1, 4]);
            if (j == 1)
                li = lij;
            else
                li(:, 2) += lij(:, 2);
            end
        end

        ti = li(:, 1);

        Si = -li(:, 2); % Now +ve draw rather than -ve injection.
        Si /= max(Si); % Note: Si is real. Max is now 1.
        Si = sd_factor * (Si - mean(Si)); % Reduce fluctuations if desired.
        Si = Si + 1 - max(Si); % Max is now 1 again.
        Si *= SStatic(i) * max_factor; % Scale so max load is max_factor * SStatic.
        SiFluct = Si - SStatic(i);

        printf('static = %f, mean = %f, min = %f, max = %f, sdev = %f\n', ...
               SStatic(i), mean(real(Si)), min(real(Si)), max(real(Si)), std(real(Si)));
        fflush(stdout);

        fname = ['aggregate_loads/load_', num2str(i), '.txt'];                                                    
        fp = fopen(fname, 'w+');
        z = zeros(size(ti));
        printArg = [ ...
            ti, ...
            z, z, ...
            z, z, ...
            real(SiFluct), imag(SiFluct)];
        fprintf(fp, '%d %f+%fj %f+%fj %f+%fj\n', printArg');                                                             
        fclose(fp);                                

        fname = ['aggregate_loads/tot_load_', num2str(i), '.txt'];                                                    
        fp = fopen(fname, 'w+');
        z = zeros(size(ti));
        printArg = [ ...
            ti, ...
            z, z, ...
            z, z, ...
            real(Si), imag(Si)];
        fprintf(fp, '%d %f+%fj %f+%fj %f+%fj\n', printArg');                                                             
        fclose(fp);                                
    end
