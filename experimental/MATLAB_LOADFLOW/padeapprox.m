function [r,a,b,mu,nu,poles,residues] = padeapprox(f,m,n,tol)
%  Input: Function handle f or vector of coefficients f_0,...,f_(m+n).
%         (If f is a function handle, the function must be analytic in a
%         neighborhood of the unit disk since coeffs are computed via FFT.)
%         Numerator and denominator degrees m>=0 and n>=0.
%         An optional 4th argument specifies relative tolerance tol.
%         If omitted, tol=1e-14.  Use tol=0 to turn off robustness.
% Output: Function handle r of exact type (mu,nu) Pade approximant to f
%         with coeff vectors a and b and optional poles and residues.
% P. Gonnet, S. Guettel, and L. N. Trefethen, October 2011

if nargin<4, tol = 1e-14; end                % default rel tolerance 1e-14
if ~isfloat(f)                               % compute coeffs if necessary
  N = 2048; z = exp(2i*pi*(0:N-1)'/N);       % sample at many roots of unity
  f = fft(f(z))/N;                           % Fast Fourier Transform
  tc = 1e-15*norm(f); f(abs(f)<tc) = 0;      % discard near-zero coeffs
  if norm(imag(f),inf)<tc, f = real(f); end  % make real functions real
end
c = [f(:); zeros(m+n+1-length(f),1)];        % make sure c is long enough
c = c(1:m+n+1);                              % but not longer than necessary
ts = tol*norm(c);                            % absolute tolerance
if norm(c(1:m+1),inf)<=tol*norm(c,inf)       % special case r=0
  a = 0; b = 1; mu = -inf; nu = 0;
else
  row = [c(1) zeros(1,n)]; col = c;          % 1st row/col of Toeplitz matrix
  while true                                 % diagonal hopping across block
    if n==0, a = c(1:m+1); b = 1; break, end % special case n=0
    Z = toeplitz(col(1:m+n+1),row(1:n+1));   % Toeplitz matrix
    C = Z(m+2:m+n+1,:);
    rho = sum(svd(C)>ts);                    % numerical rank
    if rho==n, break, end
    m = m-(n-rho); n = rho;                  % decrease m,n if rank-deficient
  end
  if n>0                                     % hopping finished; compute b,a
    [U,S,V] = svd(C,0);
    b = V(:,n+1);                            % null vector gives b
    D = diag(abs(b)+sqrt(eps));              % reweighting preserves zeros better
    [Q,R] = qr((C*D).');                     % so does final computation via QR
    b = D*Q(:,n+1); b = b/norm(b);           % compensate for reweighting
    a = Z(1:m+1,1:n+1)*b;                    % multiplying gives a
    lam = find(abs(b)>tol,1,'first')-1;      % count leading zeros of b
    b = b(lam+1:end); a = a(lam+1:end);      % discard leading zeros of b,a
    b = b(1:find(abs(b)>tol,1,'last'));      % discard trailing zeros of b
  end
  a = a(1:find(abs(a)>ts,1,'last'));         % discard trailing zeros of a
  a = a/b(1); b = b/b(1);                    % normalize 
  mu = length(a)-1; nu = length(b)-1;        % exact numer, denom degrees
end
r = @(z) polyval(a(end:-1:1),z)...           % function handle for r
       ./polyval(b(end:-1:1),z);
if nargout>5                                 % only compute poles if necessary
  poles = roots(b(end:-1:1));                % poles
  t = max(tol,1e-7);                         % perturbation for residue estimate
  residues = t*(r(poles+t)-r(poles-t))/2;    % estimate of residues
end
