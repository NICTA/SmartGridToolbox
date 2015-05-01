mins_in_day = 24 * 60;

n_days = 7;
dt = 15;
dt_b = 5;

n_in_dataset = n_days * mins_in_day / dt;

dat = load('loads.txt');
rand('state', 0);
dat = dat(:, randperm(6)); % To randomize house sizes.
dat /= 1e6; % From W to MW.

n_data_sets = 50;

t_vec = (0:dt:n_days*mins_in_day)'; % Note keep the next point for interpolation.
t_vec_b = (0:dt_b:n_days*mins_in_day-dt_b)';
z_vec = zeros(size(t_vec_b));

for i = 1:n_data_sets
   irow = 1 + n_in_dataset * floor((i - 1) / 6);
   icol = mod(i - 1, 6) + 1;
   ld_i = dat(irow:irow+n_in_dataset, icol); % Keep the next point for interpolation.
   ld_i_b = -0.5 * interp1(t_vec, ld_i, t_vec_b, 'spline');
   dat_i = [t_vec_b, z_vec, z_vec, ld_i_b];
   fname = ['loads/load_', num2str(i-1), '.txt'];
   fp = fopen(fname, 'w+');
   fprintf(fp, '%d %d %d %f\n', dat_i');
   fclose(fp);
end
