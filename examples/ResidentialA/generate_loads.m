dt = 15;
n_in_day = 24 * 60 / dt;
ndays_in_dataset = 28;

dat = load('loads.txt');
dat = dat(:, randperm(6)); % To randomize house sizes.
dat /= 1000; % From W to kW.

% n_data_sets = floor(6 * 365 / ndays_in_dataset);
n_data_sets = 50;

t_vec = (0:15:24*60)'; % Note keep the next point for interpolation.
t_vec_b = (0:5:24*60-5)';
z_vec = zeros(size(t_vec_b));

for i = 1:n_data_sets
   irow = 1 + 96 * floor((i - 1) / 6);
   icol = mod(i - 1, 6) + 1;
   ld_i = dat(irow:irow+96, icol); % Keep the next point for interpolation.
   ld_i_b= interp1(t_vec, ld_i, t_vec_b, 'spline');
   dat_i = [t_vec_b z_vec, z_vec, ld_i_b];
   fname = ['loads/load_', num2str(i), '.txt'];
   fp = fopen(fname, 'w+');
   fprintf(fp, '%d %d %d %f\n', dat_i');
   fclose(fp);
end
