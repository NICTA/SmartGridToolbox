day = 60*24;

tVec = (0:5:30*day)';
TVec = 60:60:7*day;

lVeci = [];

for i = 1:20
   SMax = rand*3000e-6;          % Maximum value for this particular load.
   xVec = rand(size(TVec));      % Random frequency magnitude.
   pVec = rand(size(TVec))*2*pi; % Phase offset.
   lVec = zeros(size(tVec));     % The load.
   for j = 1:length(TVec)
      lVec += xVec(j)*exp(-(TVec(j)-day)^2/(day)^2)*cos(pVec(j) + 2*pi*tVec/TVec(j));
   end
   lVec -= min(lVec);
   lVec *= -SMax/max(lVec);

   z = zeros(size(lVec));
   dat = [tVec, z, z, lVec];

   fp = fopen(['loads/load_' num2str(i-1) '.txt'], 'w+');
   fprintf(fp, '%6d %10.3e %10.3e %10.3e\n', dat');
   fclose(fp);
   lVeci = [lVeci, lVec];
end
