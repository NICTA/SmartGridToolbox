
input_files = glob('pylon_converted/*_pylon.m');
for input_file = input_files'
   input_file = input_file{1}
   mpc = loadcase(input_file);
   mpc.bus = [mpc.bus(:, 1:end-4)];
   size(mpc.bus)
   mpc.gen = [mpc.gen(:, 1:10), zeros(size(mpc.gen, 1), 11), mpc.gen(:, 11:end)];
   mpc.gencost(:,1) = 2;
   output_file = [input_file(1:end-2), '_mp_normalised.m']
   savecase(output_file, mpc);
end
