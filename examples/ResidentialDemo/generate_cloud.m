tvec = 0:5:24*60*30;
Tvec = 60:120:60*24;
xvec = rand(size(Tvec)).*exp(-((Tvec-18*60)/(24*60)).^2);
pvec = rand(size(Tvec))*2*pi;
cvec = zeros(size(tvec));
for i = 1:length(Tvec)
   cvec += xvec(i)*cos(pvec(i) + 2*pi*tvec/Tvec(i));
end
cvec -= min(cvec);
cvec /= max(cvec);
cvec = 1-(1-cvec).^4;

dat = [tvec', cvec'];

fp = fopen('cloud_cover.txt', 'w');
fprintf(fp, '%d %5.3f\n', dat');
fclose(fp);
