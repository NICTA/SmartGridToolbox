%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%                                                                  %%%%%
%%%%      NICTA Energy System Test Case Archive (NESTA) - v0.1.5      %%%%%
%%%%              Optimal Power Flow - Typical Operation              %%%%%
%%%%                       30 - October - 2014                        %%%%%
%%%%                                                                  %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   CASE4GS  Power flow data for 4 bus, 2 gen case from Grainger & Stevenson.
%   Please see CASEFORMAT for details on the case file format.
%
%   This is the 4 bus example from pp. 337-338 of "Power System Analysis",
%   by John Grainger, Jr., William Stevenson, McGraw-Hill, 1994.
%
function mpc = nesta_case4_gs
mpc.version = '2';
mpc.baseMVA = 100.0;

%% bus data
%	bus_i	type	Pd	Qd	Gs	Bs	area	Vm	Va	baseKV	zone	Vmax	Vmin
mpc.bus = [
	1	 3	 50.0	 30.99	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.1000	     0.9000;
	2	 1	 170.0	 105.35	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.1000	     0.9000;
	3	 1	 200.0	 123.94	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.1000	     0.9000;
	4	 2	 80.0	 49.58	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.1000	     0.9000;
];

%% generator data
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf
mpc.gen = [
	4	 318.0	 0.0	 150.0	 -150.0	 1.02	 100.0	 1	 419	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	1	 0.0	 0.0	 150.0	 -150.0	 1.0	 100.0	 1	 1220	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NUC
];

%% generator cost data
%	2	startup	shutdown	n	c(n-1)	...	c0
mpc.gencost = [
	2	 0.0	 0.0	 3	   0.000000	   1.074863	   0.000000; % NG
	2	 0.0	 0.0	 3	   0.000000	   0.219975	   0.000000; % NUC
];

%% branch data
%	fbus	tbus	r	x	b	rateA	rateB	rateC	ratio	angle	status	angmin	angmax
mpc.branch = [
	1	 2	 0.01008	 0.0504	 0.1025	 250.0	 250.0	 250.0	 0.0	 0.0	 1	 -30.0	 30.0;
	1	 3	 0.00744	 0.0372	 0.0775	 250.0	 250.0	 250.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 4	 0.00744	 0.0372	 0.0775	 250.0	 250.0	 250.0	 0.0	 0.0	 1	 -30.0	 30.0;
	3	 4	 0.01272	 0.0636	 0.1275	 250.0	 250.0	 250.0	 0.0	 0.0	 1	 -30.0	 30.0;
];

% INFO    : === Translation Options ===
% INFO    : Phase Angle Bound:           30.0 (deg.)
% INFO    : Gen Active Capacity Model:   stat
% INFO    : Gen Active Cost Model:       stat
% WARNING : No active generation at the slack bus, assigning type - NUC
% INFO    : 
% INFO    : === Generator Classification Notes ===
% INFO    : NUC    1   -     0.00
% INFO    : NG     1   -   100.00
% INFO    : 
% INFO    : === Generator Active Capacity Stat Model Notes ===
% INFO    : Gen at bus 4 - NG	: Pg=318.0, Pmax=318.0 -> Pmax=419   samples: 1
% INFO    : Gen at bus 1 - NUC	: Pg=0.0, Pmax=300.0 -> Pmax=1220   samples: 1
% INFO    : 
% INFO    : === Generator Active Cost Stat Model Notes ===
% INFO    : Updated Generator Cost: NG - 0.0 20.0 0.0314 -> 0 1.07486321374 0
% INFO    : Updated Generator Cost: NUC - 0.0 40.0 0.01 -> 0 0.21997539603 0
% INFO    : 
% INFO    : === Writing Matpower Case File Notes ===
