input_files = glob('psse/*.raw');
for input_file = input_files'
   input_file = input_file{1}
   output_file = ['mp_converted/', input_file(6:end-4), '_mp_converted.m']

   [records, sections] = psse_read(input_file);
   data = psse_parse(records, sections);
   [mpc, warnings] = psse_convert([], data);

   savecase(output_file, mpc);
   warnings
end
