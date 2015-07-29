dat = 1e-3 * load('loads_processed.txt');
n_days = size(dat, 1)/1440;
n_split = 1440 * 7; % 4 week chunks
dat_b = [];
t = (0:n_split-1)';
i = 1;
while size(dat, 1) >= n_split
    dat_i = [t, dat(1:n_split, :)];
    dat = dat(n_split+1:end, :);
    if (any(any(isnan(dat_i))))
        continue
    end

    fname = ['loads/load_', num2str(i), '.txt'];
    fp = fopen(fname, 'w+');
    fprintf(fp, '%d %f %f\n', dat_i');
    fclose(fp);
    
    i = i + 1;
end
