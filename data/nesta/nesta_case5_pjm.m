%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%                                                                  %%%%%
%%%%      NICTA Energy System Test Case Archive (NESTA) - v0.1.5      %%%%%
%%%%              Optimal Power Flow - Typical Operation              %%%%%
%%%%                       30 - October - 2014                        %%%%%
%%%%                                                                  %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   CASE5  Power flow data for modified 5 bus, 5 gen case based on PJM 5-bus system
%   Please see CASEFORMAT for details on the case file format.
%
%   Based on data from ...
%     F.Li and R.Bo, "Small Test Systems for Power System Economic Studies",
%     Proceedings of the 2010 IEEE Power & Energy Society General Meeting
%
%   Created by Rui Bo in 2006, modified in 2010, 2014.
%
function mpc = nesta_case5_pjm
mpc.version = '2';
mpc.baseMVA = 100.0;

%% area data
%	area	refbus
mpc.areas = [
	1	 4;
];

%% bus data
%	bus_i	type	Pd	Qd	Gs	Bs	area	Vm	Va	baseKV	zone	Vmax	Vmin
mpc.bus = [
	1	 2	 0.0	 0.0	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.1000	     0.9000;
	2	 1	 300.0	 98.61	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.1000	     0.9000;
	3	 2	 300.0	 98.61	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.1000	     0.9000;
	4	 3	 400.0	 131.47	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.1000	     0.9000;
	5	 2	 0.0	 0.0	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.1000	     0.9000;
];

%% generator data
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf
mpc.gen = [
	1	 40.0	 0.0	 30.0	 -30.0	 1.0	 100.0	 1	 40.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0;
	1	 170.0	 0.0	 127.5	 -127.5	 1.0	 100.0	 1	 170.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0;
	3	 323.49	 0.0	 390.0	 -390.0	 1.0	 100.0	 1	 520.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0;
	4	 0.0	 0.0	 150.0	 -150.0	 1.0	 100.0	 1	 200.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0;
	5	 466.51	 0.0	 450.0	 -450.0	 1.0	 100.0	 1	 600.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0;
];

%% generator cost data
%	2	startup	shutdown	n	c(n-1)	...	c0
mpc.gencost = [
	2	 0.0	 0.0	 3	   0.000000	  14.000000	   0.000000;
	2	 0.0	 0.0	 3	   0.000000	  15.000000	   0.000000;
	2	 0.0	 0.0	 3	   0.000000	  30.000000	   0.000000;
	2	 0.0	 0.0	 3	   0.000000	  40.000000	   0.000000;
	2	 0.0	 0.0	 3	   0.000000	  10.000000	   0.000000;
];

%% branch data
%	fbus	tbus	r	x	b	rateA	rateB	rateC	ratio	angle	status	angmin	angmax
mpc.branch = [
	1	 2	 0.00281	 0.0281	 0.00712	 400.0	 400.0	 400.0	 0.0	 0.0	 1	 -30.0	 30.0;
	1	 4	 0.00304	 0.0304	 0.00658	 425	 425	 425	 0.0	 0.0	 1	 -30.0	 30.0;
	1	 5	 0.00064	 0.0064	 0.03126	 425	 425	 425	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 3	 0.00108	 0.0108	 0.01852	 425	 425	 425	 0.0	 0.0	 1	 -30.0	 30.0;
	3	 4	 0.00297	 0.0297	 0.00674	 425	 425	 425	 0.0	 0.0	 1	 -30.0	 30.0;
	4	 5	 0.00297	 0.0297	 0.00674	 240.0	 240.0	 240.0	 0.0	 0.0	 1	 -30.0	 30.0;
];

% INFO    : === Translation Options ===
% INFO    : Phase Angle Bound:           30.0 (deg.)
% INFO    : Line Capacity Model:         stat
% INFO    : 
% INFO    : === Line Capacity Stat Model Notes ===
% INFO    : Updated Thermal Rating: on line 1-4 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 425
% INFO    : Updated Thermal Rating: on line 1-5 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 425
% INFO    : Updated Thermal Rating: on line 2-3 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 425
% INFO    : Updated Thermal Rating: on line 3-4 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 425
% INFO    : 
% INFO    : === Writing Matpower Case File Notes ===
