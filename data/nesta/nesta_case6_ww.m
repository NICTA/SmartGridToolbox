%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%                                                                  %%%%%
%%%%      NICTA Energy System Test Case Archive (NESTA) - v0.1.5      %%%%%
%%%%              Optimal Power Flow - Typical Operation              %%%%%
%%%%                       30 - October - 2014                        %%%%%
%%%%                                                                  %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Power flow data for 6 bus, 3 gen case from Wood & Wollenberg.
%
%   This is the 6 bus example from pp. 104, 112, 119, 123-124, 549 of
%   "Power Generation, Operation, and Control, 2nd Edition",
%   by Allen. J. Wood and Bruce F. Wollenberg, John Wiley & Sons, NY, Jan 1996.
%
function mpc = nesta_case6_ww
mpc.version = '2';
mpc.baseMVA = 100.0;

%% bus data
%	bus_i	type	Pd	Qd	Gs	Bs	area	Vm	Va	baseKV	zone	Vmax	Vmin
mpc.bus = [
	1	 3	 0.0	 0.0	 0.0	 0.0	 1	     1.0500	     0.0000	 230.0	 1	     1.0500	     1.0500;
	2	 2	 0.0	 0.0	 0.0	 0.0	 1	     1.0500	     0.0000	 230.0	 1	     1.0500	     1.0500;
	3	 2	 0.0	 0.0	 0.0	 0.0	 1	     1.0700	     0.0000	 230.0	 1	     1.0700	     1.0700;
	4	 1	 70.0	 70.0	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.0500	     0.9500;
	5	 1	 70.0	 70.0	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.0500	     0.9500;
	6	 1	 70.0	 70.0	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.0500	     0.9500;
];

%% generator data
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf
mpc.gen = [
	1	 0.0	 0.0	 100.0	 -100.0	 1.05	 100.0	 1	 200.0	 50.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0;
	2	 50.0	 0.0	 100.0	 -100.0	 1.05	 100.0	 1	 150.0	 37.5	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0;
	3	 60.0	 0.0	 100.0	 -100.0	 1.07	 100.0	 1	 180.0	 45.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0;
];

%% generator cost data
%	2	startup	shutdown	n	c(n-1)	...	c0
mpc.gencost = [
	2	 0.0	 0.0	 3	   0.005330	  11.669000	 213.100000;
	2	 0.0	 0.0	 3	   0.008890	  10.333000	 200.000000;
	2	 0.0	 0.0	 3	   0.007410	  10.833000	 240.000000;
];

%% branch data
%	fbus	tbus	r	x	b	rateA	rateB	rateC	ratio	angle	status	angmin	angmax
mpc.branch = [
	1	 2	 0.1	 0.2	 0.04	 40.0	 40.0	 40.0	 0.0	 0.0	 1	 -30.0	 30.0;
	1	 4	 0.05	 0.2	 0.04	 60.0	 60.0	 60.0	 0.0	 0.0	 1	 -30.0	 30.0;
	1	 5	 0.08	 0.3	 0.06	 40.0	 40.0	 40.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 3	 0.05	 0.25	 0.06	 40.0	 40.0	 40.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 4	 0.05	 0.1	 0.02	 60.0	 60.0	 60.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 5	 0.1	 0.3	 0.04	 30.0	 30.0	 30.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 6	 0.07	 0.2	 0.05	 90.0	 90.0	 90.0	 0.0	 0.0	 1	 -30.0	 30.0;
	3	 5	 0.12	 0.26	 0.05	 70.0	 70.0	 70.0	 0.0	 0.0	 1	 -30.0	 30.0;
	3	 6	 0.02	 0.1	 0.02	 80.0	 80.0	 80.0	 0.0	 0.0	 1	 -30.0	 30.0;
	4	 5	 0.2	 0.4	 0.08	 20.0	 20.0	 20.0	 0.0	 0.0	 1	 -30.0	 30.0;
	5	 6	 0.1	 0.3	 0.06	 40.0	 40.0	 40.0	 0.0	 0.0	 1	 -30.0	 30.0;
];

% INFO    : === Translation Options ===
% INFO    : Phase Angle Bound:           30.0 (deg.)
% INFO    : 
% INFO    : === Writing Matpower Case File Notes ===
