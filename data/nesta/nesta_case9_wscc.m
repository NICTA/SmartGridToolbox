%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%                                                                  %%%%%
%%%%      NICTA Energy System Test Case Archive (NESTA) - v0.1.5      %%%%%
%%%%              Optimal Power Flow - Typical Operation              %%%%%
%%%%                       30 - October - 2014                        %%%%%
%%%%                                                                  %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Power flow data for the Western System Coordinating Council (WSCC) 9 bus, 3 generator case.
%   
%   Electric Power Research Institute
%   "Power System Dynamic Analysis-Phase I"
%   EPRI Report EL-484, July 1977.
%   
%   Original source of line thermal limits, generator costs remains unknown.
%
function mpc = nesta_case9_wscc
mpc.version = '2';
mpc.baseMVA = 100.0;

%% area data
%	area	refbus
mpc.areas = [
	1	 5;
];

%% bus data
%	bus_i	type	Pd	Qd	Gs	Bs	area	Vm	Va	baseKV	zone	Vmax	Vmin
mpc.bus = [
	1	 3	 0.0	 0.0	 0.0	 0.0	 1	     1.0000	     0.0000	 345.0	 1	     1.1000	     0.9000;
	2	 2	 0.0	 0.0	 0.0	 0.0	 1	     1.0000	     0.0000	 345.0	 1	     1.1000	     0.9000;
	3	 2	 0.0	 0.0	 0.0	 0.0	 1	     1.0000	     0.0000	 345.0	 1	     1.1000	     0.9000;
	4	 1	 0.0	 0.0	 0.0	 0.0	 1	     1.0000	     0.0000	 345.0	 1	     1.1000	     0.9000;
	5	 1	 90.0	 30.0	 0.0	 0.0	 1	     1.0000	     0.0000	 345.0	 1	     1.1000	     0.9000;
	6	 1	 0.0	 0.0	 0.0	 0.0	 1	     1.0000	     0.0000	 345.0	 1	     1.1000	     0.9000;
	7	 1	 100.0	 35.0	 0.0	 0.0	 1	     1.0000	     0.0000	 345.0	 1	     1.1000	     0.9000;
	8	 1	 0.0	 0.0	 0.0	 0.0	 1	     1.0000	     0.0000	 345.0	 1	     1.1000	     0.9000;
	9	 1	 125.0	 50.0	 0.0	 0.0	 1	     1.0000	     0.0000	 345.0	 1	     1.1000	     0.9000;
];

%% generator data
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf
mpc.gen = [
	1	 0.0	 0.0	 300.0	 -300.0	 1.0	 100.0	 1	 250.0	 10.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0;
	2	 163.0	 0.0	 300.0	 -300.0	 1.0	 100.0	 1	 300.0	 10.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0;
	3	 85.0	 0.0	 300.0	 -300.0	 1.0	 100.0	 1	 270.0	 10.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0;
];

%% generator cost data
%	2	startup	shutdown	n	c(n-1)	...	c0
mpc.gencost = [
	2	 1500.0	 0.0	 3	   0.110000	   5.000000	 150.000000;
	2	 2000.0	 0.0	 3	   0.085000	   1.200000	 600.000000;
	2	 3000.0	 0.0	 3	   0.122500	   1.000000	 335.000000;
];

%% branch data
%	fbus	tbus	r	x	b	rateA	rateB	rateC	ratio	angle	status	angmin	angmax
mpc.branch = [
	1	 4	 0.0	 0.0576	 0.0	 250.0	 250.0	 250.0	 0.0	 0.0	 1	 -30.0	 30.0;
	4	 5	 0.017	 0.092	 0.158	 250.0	 250.0	 250.0	 0.0	 0.0	 1	 -30.0	 30.0;
	5	 6	 0.039	 0.17	 0.358	 150.0	 150.0	 150.0	 0.0	 0.0	 1	 -30.0	 30.0;
	3	 6	 0.0	 0.0586	 0.0	 300.0	 300.0	 300.0	 0.0	 0.0	 1	 -30.0	 30.0;
	6	 7	 0.0119	 0.1008	 0.209	 150.0	 150.0	 150.0	 0.0	 0.0	 1	 -30.0	 30.0;
	7	 8	 0.0085	 0.072	 0.149	 250.0	 250.0	 250.0	 0.0	 0.0	 1	 -30.0	 30.0;
	8	 2	 0.0	 0.0625	 0.0	 250.0	 250.0	 250.0	 0.0	 0.0	 1	 -30.0	 30.0;
	8	 9	 0.032	 0.161	 0.306	 250.0	 250.0	 250.0	 0.0	 0.0	 1	 -30.0	 30.0;
	9	 4	 0.01	 0.085	 0.176	 250.0	 250.0	 250.0	 0.0	 0.0	 1	 -30.0	 30.0;
];

% INFO    : === Translation Options ===
% INFO    : Phase Angle Bound:           30.0 (deg.)
% INFO    : 
% INFO    : === Writing Matpower Case File Notes ===
