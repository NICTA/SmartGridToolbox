function y = eval_pade(a, b, z)
   y = polyval(a(end:-1:1), z) ./ polyval(b(end:-1:1), z);
end
