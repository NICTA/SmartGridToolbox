function aggregate_loads(case_rel_path)

    poiss_lambda = 5;
    sdev_factor = 0.1; % 1 -> stddev = mean
    mpc = loadcase([pwd(), '/', case_rel_path]);

    nbus = rows(mpc.bus);

    SStatic = -mpc.bus(:, 3) - I * mpc.bus(:, 4);

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
        Si = li(:, 2);

        Si = Si - mean(Si);
        Si = Si / std(Si);
        Si = Si - max(Si);
        Si = SStatic(i) * sdev_factor * Si;
        SiTot = SStatic(i) + Si;

        fname = ['aggregate_loads/load_', num2str(i), '.txt'];                                                    
        fp = fopen(fname, 'w+');
        z = zeros(size(ti));
        printArg = [ ...
            ti, ...
            z, z, ...
            z, z, ...
            real(Si), imag(Si)];
        fprintf(fp, '%d %f+%fj %f+%fj %f+%fj\n', printArg');                                                             
        fclose(fp);                                

        fname = ['aggregate_loads/tot_load_', num2str(i), '.txt'];                                                    
        fp = fopen(fname, 'w+');
        z = zeros(size(ti));
        printArg = [ ...
            ti, ...
            z, z, ...
            z, z, ...
            real(SiTot), imag(SiTot)];
        fprintf(fp, '%d %f+%fj %f+%fj %f+%fj\n', printArg');                                                             
        fclose(fp);                                
    end
