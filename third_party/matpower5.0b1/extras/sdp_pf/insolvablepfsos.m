function insolvable = insolvablepfsos(mpc,mpopt)
%INSOLVABLEPFSOS A sufficient condition for power flow insolvability
%using sum of squares programming
%
%   [INSOLVABLE] = INSOLVABLEPFSOS(MPC,MPOPT)
%
%   Uses sum of squares programming to generate an infeasibility 
%   certificate for the power flow equations. An infeasibility certificate
%   proves insolvability of the power flow equations.
%
%   Note that absence of an infeasibility certificate does not necessarily
%   mean that a solution does exist (that is, insolvable = 0 does not imply
%   solution existence). See [1] for more details.
%
%   Inputs:
%       MPC : A MATPOWER case specifying the desired power flow equations.
%       MPOPT : A MATPOWER options struct. If not specified, it is
%           assumed to be the default mpoption.
%
%   Outputs:
%       INSOLVABLE : Binary variable. A value of 1 indicates that the
%           specified power flow equations are insolvable, while a value of
%           0 means that the insolvability condition is indeterminant (a
%           solution may or may not exist).
%
%   [1] D.K. Molzahn, V. Dawar, B.C. Lesieutre, and C.L. DeMarco, "Sufficient
%       Conditions for Power Flow Insolvability Considering Reactive Power
%       Limited Generators with Applications to Voltage Stability Margins,"
%       in Bulk Power System Dynamics and Control - IX. Optimization,
%       Security and Control of the Emerging Power Grid, 2013 IREP Symposium,
%       25-30 August 2013.

%   MATPOWER
%   $Id: insolvablepfsos.m 2280 2014-01-17 23:28:37Z ray $
%   by Daniel Molzahn, PSERC U of Wisc, Madison
%   Copyright (c) 2013-2014 by Power System Engineering Research Center (PSERC)
%
%   This file is part of MATPOWER.
%   See http://www.pserc.cornell.edu/matpower/ for more info.
%
%   MATPOWER is free software: you can redistribute it and/or modify
%   it under the terms of the GNU General Public License as published
%   by the Free Software Foundation, either version 3 of the License,
%   or (at your option) any later version.
%
%   MATPOWER is distributed in the hope that it will be useful,
%   but WITHOUT ANY WARRANTY; without even the implied warranty of
%   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
%   GNU General Public License for more details.
%
%   You should have received a copy of the GNU General Public License
%   along with MATPOWER. If not, see <http://www.gnu.org/licenses/>.
%
%   Additional permission under GNU GPL version 3 section 7
%
%   If you modify MATPOWER, or any covered work, to interface with
%   other modules (such as MATLAB code and MEX-files) available in a
%   MATLAB(R) or comparable environment containing parts covered
%   under other licensing terms, the licensors of MATPOWER grant
%   you additional permission to convey the resulting work.

define_constants;

if nargin < 2
    mpopt = mpoption;
end

% Unpack options
ignore_angle_lim    = mpopt.opf.ignore_angle_lim;
verbose             = mpopt.verbose;
enforce_Qlimits     = mpopt.pf.enforce_q_lims;

if enforce_Qlimits > 0
    enforce_Qlimits = 1;
end

if ~have_fcn('yalmip')
    error('insolvablepfsos: The software package YALMIP is required to run insolvablepfsos. See http://users.isy.liu.se/johanl/yalmip/');
end

% set YALMIP options struct in SDP_PF (for details, see help sdpsettings) 
sdpopts = yalmip_options([], mpopt);

%% Handle generator reactive power limits
% If no generator reactive power limits are specified, use this code
% directly. If generator reactive power limits are to be enforced, use a
% function that considers reactive power limited generators.

if enforce_Qlimits
    if verbose > 0
        fprintf('Generator reactive power limits are enforced. Using function insolvablepfsos_limitQ.\n');
    end
    insolvable = insolvablepfsos_limitQ(mpc,mpopt);
    return;
end

%% Load data

mpc = loadcase(mpc);
mpc = ext2int(mpc);

if toggle_dcline(mpc, 'status')
    error('insolvablepfsos: DC lines are not implemented in insolvablepf');
end

if ~ignore_angle_lim && (any(mpc.branch(:,ANGMIN) ~= -360) || any(mpc.branch(:,ANGMAX) ~= 360))
    warning('insolvablepfsos: Angle difference constraints are not implemented. Ignoring angle difference constraints.');
end

nbus = size(mpc.bus,1);
ngen = size(mpc.gen,1);

% Some of the larger system (e.g., case2746wp) have generators 
% corresponding to buses that have bustype == PQ. Change these
% to PV buses.
for i=1:ngen
    busidx = find(mpc.bus(:,BUS_I) == mpc.gen(i,GEN_BUS));
    if isempty(busidx) || ~(mpc.bus(busidx,BUS_TYPE) == PV || mpc.bus(busidx,BUS_TYPE) == REF)
        mpc.bus(busidx,BUS_TYPE) = PV;
        if verbose >= 1
            warning('insolvablepfsos: Bus %s has generator(s) but is listed as a PQ bus. Changing to a PV bus.',int2str(busidx));
        end
    end
end

% Buses may be listed as PV buses without associated generators. Change
% these buses to PQ.
for i=1:nbus
    if mpc.bus(i,BUS_TYPE) == PV
        genidx = find(mpc.gen(:,GEN_BUS) == mpc.bus(i,BUS_I), 1);
        if isempty(genidx)
            mpc.bus(i,BUS_TYPE) = PQ;
            if verbose >= 1
                warning('insolvablepfsos: PV bus %i has no associated generator! Changing these buses to PQ.',i);
            end
        end
    end
end

pq = find(mpc.bus(:,2) == PQ);
pv = find(mpc.bus(:,2) == PV);
ref = find(mpc.bus(:,2) == REF);

refpv = sort([ref; pv]);
pvpq = sort([pv; pq]);

Y = makeYbus(mpc);
G = real(Y);
B = imag(Y);

% Make vectors of power injections and voltage magnitudes
S = makeSbus(mpc.baseMVA, mpc.bus, mpc.gen);
P = real(S);
Q = imag(S);

Vmag = zeros(nbus,1);
Vmag(mpc.gen(:,GEN_BUS)) = mpc.gen(:,VG);


%% Create voltage variables
% We need a Vd and Vq for each bus

if verbose >= 2
    fprintf('Creating variables.\n');
end

Vd = sdpvar(nbus,1);
Vq = sdpvar(nbus,1);

V = [1; Vd; Vq];

%% Create polynomials

if verbose >= 2
    fprintf('Creating polynomials.\n');
end

deg = 0;

% Polynomials for active power
for i=1:nbus-1
    [l,lc] = polynomial(V,deg,0);
    lambda(i) = l;
    clambda{i} = lc;
end

% Polynomials for reactive power
for i=1:length(pq)
    [g,gc] = polynomial(V,deg,0);
    gamma(i) = g;
    cgamma{i} = gc;
end

% Polynomials for voltage magnitude
for i=1:length(refpv)
    [m,mc] = polynomial(V,deg,0);
    mu(i) = m;
    cmu{i} = mc;
end

% Polynomial for reference angle
[delta, cdelta] = polynomial(V,deg,0);


%% Create power flow equation polynomials

if verbose >= 2
    fprintf('Creating power flow equations.\n');
end

% Make P equations
for k=1:nbus-1
    i = pvpq(k);
    
    % Take advantage of sparsity in forming polynomials
%     fp(k) = Vd(i) * sum(G(:,i).*Vd - B(:,i).*Vq) + Vq(i) * sum(B(:,i).*Vd + G(:,i).*Vq);
    
    y = find(G(:,i) | B(:,i));
    for m=1:length(y)
        if exist('fp','var') && length(fp) == k
            fp(k) = fp(k) + Vd(i) * (G(y(m),i)*Vd(y(m)) - B(y(m),i)*Vq(y(m))) + Vq(i) * (B(y(m),i)*Vd(y(m)) + G(y(m),i)*Vq(y(m)));
        else
            fp(k) = Vd(i) * (G(y(m),i)*Vd(y(m)) - B(y(m),i)*Vq(y(m))) + Vq(i) * (B(y(m),i)*Vd(y(m)) + G(y(m),i)*Vq(y(m)));
        end
    end

end

% Make Q equations
for k=1:length(pq)
    i = pq(k);
    
    % Take advantage of sparsity in forming polynomials
%     fq(k) = Vd(i) * sum(-B(:,i).*Vd - G(:,i).*Vq) + Vq(i) * sum(G(:,i).*Vd - B(:,i).*Vq);
    
    y = find(G(:,i) | B(:,i));
    for m=1:length(y)
        if exist('fq','var') && length(fq) == k
            fq(k) = fq(k) + Vd(i) * (-B(y(m),i)*Vd(y(m)) - G(y(m),i)*Vq(y(m))) + Vq(i) * (G(y(m),i)*Vd(y(m)) - B(y(m),i)*Vq(y(m)));
        else
            fq(k) = Vd(i) * (-B(y(m),i)*Vd(y(m)) - G(y(m),i)*Vq(y(m))) + Vq(i) * (G(y(m),i)*Vd(y(m)) - B(y(m),i)*Vq(y(m)));
        end
    end
end

% Make V equations
for k=1:length(refpv)
    i = refpv(k);
    fv(k) = Vd(i)^2 + Vq(i)^2;
end


%% Form the test polynomial

if verbose >= 2
    fprintf('Forming the test polynomial.\n');
end

sospoly = 1;

% Add active power terms
for i=1:nbus-1
    sospoly = sospoly + lambda(i) * (fp(i) - P(pvpq(i)));
end

% Add reactive power terms
for i=1:length(pq)
    sospoly = sospoly + gamma(i) * (fq(i) - Q(pq(i)));
end

% Add voltage magnitude terms
for i=1:length(refpv)
    sospoly = sospoly + mu(i) * (fv(i) - Vmag(refpv(i))^2);
end

% Add angle reference term
sospoly = sospoly + delta * Vq(ref);

sospoly = -sospoly;

%% Solve the SOS problem

if verbose >= 2
    fprintf('Solving the sum of squares problem.\n');
end

sosopts = sdpsettings(sdpopts,'sos.newton',1,'sos.congruence',1);

constraints = sos(sospoly);

pvec = cdelta(:).';

for i=1:length(lambda)
    pvec = [pvec clambda{i}(:).'];
end

for i=1:length(gamma)
    pvec = [pvec cgamma{i}(:).'];
end

for i=1:length(mu)
    pvec = [pvec cmu{i}(:).'];
end

% Preserve warning settings
S = warning;

sol = solvesos(constraints,[],sosopts,pvec);

warning(S);

if verbose >= 2
    fprintf('Solver exit message: %s\n',sol.info);
end

if sol.problem
    % Power flow equations may have a solution
    insolvable = 0;
elseif sol.problem == 0
    % Power flow equations do not have a solution.
    insolvable = 1;
end
