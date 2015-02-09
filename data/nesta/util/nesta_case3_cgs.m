%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%                                                                  %%%%%
%%%%      NICTA Energy System Test Case Archive (NESTA) - v0.1.5      %%%%%
%%%%              Optimal Power Flow - Typical Operation              %%%%%
%%%%                       30 - October - 2014                        %%%%%
%%%%                                                                  %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   The base 3 bus example case from the NESTA report.
%
function mpc = nesta_case3_cgs
mpc.version = '2';
mpc.baseMVA = 100.0;

%% bus data
%	bus_i	type	Pd	Qd	Gs	Bs	area	Vm	Va	baseKV	zone	Vmax	Vmin
mpc.bus = [
	1	 3	 0.0	 0.0	 0.0	 0.0	 1	     1.0000	     0.0000	 240.0	 1	     1.1000	     0.9000;
	2	 1	 0.0	 0.0	 0.0	 0.0	 1	     1.0000	     0.0000	 240.0	 1	     1.1000	     0.9000;
	3	 2	 100.0	 0.0	 0.0	 0.0	 1	     1.0000	     0.0000	 240.0	 1	     1.1000	     0.9000;
];

%% generator data
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf
mpc.gen = [
	1	 0.0	 0.0	 10000.0	 -10000.0	 1.0	 100.0	 1	 10000.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0;
	3	 0.0	 0.0	 0.0	 0.0	 1.0	 100.0	 1	 10000.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0;
];

%% generator cost data
%	2	startup	shutdown	n	c(n-1)	...	c0
mpc.gencost = [
	2	 0.0	 0.0	 3	   0.000000	   1.000000	   0.000000;
	2	 0.0	 0.0	 3	   0.000000	  10.000000	   0.000000;
];

%% branch data
%	fbus	tbus	r	x	b	rateA	rateB	rateC	ratio	angle	status	angmin	angmax
mpc.branch = [
	1	 2	 0.0	 0.05	 0.0	 999.0	 0.0	 0.0	 0.0	 0.0	 1	 -30.0	 30.0;
	1	 3	 0.1	 0.1	 0.0	 999.0	 0.0	 0.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 3	 0.0	 0.05	 0.0	 999.0	 0.0	 0.0	 0.0	 0.0	 1	 -30.0	 30.0;
];

% INFO    : === Translation Options ===
% INFO    : Phase Angle Bound:           30.0 (deg.)
% INFO    : 
% INFO    : === Writing Matpower Case File Notes ===
