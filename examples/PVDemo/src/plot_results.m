d = dlmread("out");
plot(d(:,1), real(d(:,2:4)), ['r'; 'b'; 'g'], 'LineWidth', 2, ...
     d(:,1), imag(d(:,2:4)), ['r'; 'b'; 'g'], 'LineWidth', 1);
legend({'real load', 'real gen', 'real solar gen', 'react load', 'react gen', 'react solar gen'})
