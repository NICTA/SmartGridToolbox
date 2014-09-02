function t_insolvablepfsos(quiet)
%T_INSOLVABLEPF  Test for power flow insolvability condition

%   MATPOWER
%   $Id: t_insolvablepfsos.m 2272 2014-01-17 14:15:47Z ray $
%   by Daniel Molzahn, PSERC U of Wisc, Madison
%   and Ray Zimmerman, PSERC Cornell
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

if nargin < 1
    quiet = 0;
end

num_tests = 2;

t_begin(num_tests, quiet);

[PQ, PV, REF, NONE, BUS_I, BUS_TYPE, PD, QD, GS, BS, BUS_AREA, VM, ...
    VA, BASE_KV, ZONE, VMAX, VMIN, LAM_P, LAM_Q, MU_VMAX, MU_VMIN] = idx_bus;
[GEN_BUS, PG, QG, QMAX, QMIN, VG, MBASE, GEN_STATUS, PMAX, PMIN, ...
    MU_PMAX, MU_PMIN, MU_QMAX, MU_QMIN, PC1, PC2, QC1MIN, QC1MAX, ...
    QC2MIN, QC2MAX, RAMP_AGC, RAMP_10, RAMP_30, RAMP_Q, APF] = idx_gen;
[F_BUS, T_BUS, BR_R, BR_X, BR_B, RATE_A, RATE_B, RATE_C, ...
    TAP, SHIFT, BR_STATUS, PF, QF, PT, QT, MU_SF, MU_ST, ...
    ANGMIN, ANGMAX, MU_ANGMIN, MU_ANGMAX] = idx_brch;
[PW_LINEAR, POLYNOMIAL, MODEL, STARTUP, SHUTDOWN, NCOST, COST] = idx_cost;

casefile = 't_case9mod_opf';
if quiet
    verbose = 0;
else
    verbose = 0;
end

t0 = 'INSOLVABLEPF : ';

%% test an insolvable case
load soln9mod_opf;     %% defines bus_soln, gen_soln, branch_soln, Vslack_min_soln, sigma_soln, etacomp_soln

res = loadcase(casefile);
res.bus = bus_soln;
res.gen = gen_soln;
res.branch = branch_soln;

% Make the case insolvable
mult = 10;
res.bus(:,PD) = mult*res.bus(:,PD);
res.bus(:,QD) = mult*res.bus(:,QD);
res.gen(:,PG) = mult*res.gen(:,PG);

mpopt = mpoption('out.all', 0, 'verbose', verbose);

%% get test results for insolvable case
t = [t0 '(insolvable case) :'];
insolvable = insolvablepfsos(res,mpopt);
t_ok(insolvable, [t ' insolvable']);


%% test a solvable case
load soln9mod_opf;     %% defines bus_soln, gen_soln, branch_soln, Vslack_min_soln, sigma_soln, etacomp_soln

res = loadcase(casefile);
res.bus = bus_soln;
res.gen = gen_soln;
res.branch = branch_soln;

%% get test results for insolvable case
t = [t0 '(solvable case) :'];
insolvable = insolvablepfsos(res,mpopt);
t_ok(~insolvable, [t ' solvable']);

t_end;
