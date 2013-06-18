function [f] = test_pade(N, vec)
   z = 0;
   McL = zeros(2*N,1);
   for i=0:2*N-1
      McL(i+1) = 1/factorial(i);
   end

   fMcL = polyval(McL(end:-1:1), vec);

   [a, b]=pade(McL(2:end));
   f =eval_pade(vec,a,b);
   plot(vec/I, sin(vec/I), 'ro', vec/I, imag(f), 'b-', vec/I, imag(fMcL),'g+')
end
