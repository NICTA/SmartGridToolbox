tvec = (0:5:24*60*7)';
Tvec = 60*24*1./(1:8);

for i = 1:10
   xvec = rand(size(Tvec));
   pvec = rand(size(Tvec))*2*pi;
   cvec = zeros(size(tvec));
   for j = 1:length(Tvec)
      cvec += xvec(j)*cos(pvec(j) + 2*pi*tvec/Tvec(j));
   end
   cvec -= min(cvec);
   cvec /= max(cvec);

   phase = randi(3);
   z = zeros(size(tvec));
   dat = [tvec, z, z, z, z, z, z, z, z, z];
   dat(:, phase + 7) = cvec;

   fp = fopen(['load_' num2str(i) '.txt'], 'w');
   fprintf(fp, '%6d %5.3f %5.3f %5.3f %5.3f %5.3f %5.3f %5.3f %5.3f %5.3f\n', dat');
   fclose(fp);
end
