tVec = (0:5:24*60*7)';
TVec = 60*24*1./(1:8);

cVeci = [];
for i = 1:10
   SMax = rand*3000e-6;
   xVec = rand(size(TVec));
   pVec = rand(size(TVec))*2*pi;
   cVec = zeros(size(tVec));
   for j = 1:length(TVec)
      cVec += xVec(j)*cos(pVec(j) + 2*pi*tVec/TVec(j));
   end
   cVec -= min(cVec);
   cVec *= -SMax/max(cVec);

   phase = randi(3);
   z = zeros(size(tVec));
   dat = [tVec, z, z, z, z, z, z, z, z, z];
   dat(:, phase + 7) = cVec;

   fp = fopen(['loads/load_' num2str(i) '.txt'], 'w');
   fprintf(fp, '%6d %10.3e %10.3e %10.3e %10.3e %10.3e %10.3e %10.3e %10.3e %10.3e\n', dat');
   fclose(fp);
   cVeci = [cVeci, cVec];
end
