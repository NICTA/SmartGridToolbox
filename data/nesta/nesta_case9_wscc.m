%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%                                                                  %%%%%
%%%%      NICTA Energy System Test Case Archive (NESTA) - v0.3.0      %%%%%
%%%%              Optimal Power Flow - Typical Operation              %%%%%
%%%%                        30 - April - 2015                         %%%%%
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
	1	 3	 0.0	 0.0	 0.0	 0.0	 1	    1.10000	    0.00000	 345.0	 1	    1.10000	    0.90000;
	2	 2	 0.0	 0.0	 0.0	 0.0	 1	    1.09736	    4.89363	 345.0	 1	    1.10000	    0.90000;
	3	 2	 0.0	 0.0	 0.0	 0.0	 1	    1.08662	    3.24953	 345.0	 1	    1.10000	    0.90000;
	4	 1	 0.0	 0.0	 0.0	 0.0	 1	    1.09422	   -2.46292	 345.0	 1	    1.10000	    0.90000;
	5	 1	 90.0	 30.0	 0.0	 0.0	 1	    1.08445	   -3.98200	 345.0	 1	    1.10000	    0.90000;
	6	 1	 0.0	 0.0	 0.0	 0.0	 1	    1.10000	    0.60287	 345.0	 1	    1.10000	    0.90000;
	7	 1	 100.0	 35.0	 0.0	 0.0	 1	    1.08949	   -1.19628	 345.0	 1	    1.10000	    0.90000;
	8	 1	 0.0	 0.0	 0.0	 0.0	 1	    1.10000	    0.90562	 345.0	 1	    1.10000	    0.90000;
	9	 1	 125.0	 50.0	 0.0	 0.0	 1	    1.07176	   -4.61523	 345.0	 1	    1.10000	    0.90000;
];

%% generator data
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf
mpc.gen = [
	1	 89.799	 12.966	 300.0	 -300.0	 1.1	 100.0	 1	 250.0	 10.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0;
	2	 134.321	 0.032	 300.0	 -300.0	 1.09736	 100.0	 1	 300.0	 10.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0;
	3	 94.187	 -22.634	 300.0	 -300.0	 1.08662	 100.0	 1	 270.0	 10.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0;
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
% INFO    : AC OPF Solution File:        nesta_case9_wscc.dat.opf.sol
% INFO    : 
% INFO    : === Voltage Setpoint Replacement Notes ===
% INFO    : Bus 1	: V=1.0, theta=0.0 -> V=1.1, theta=0.0
% INFO    : Bus 2	: V=1.0, theta=0.0 -> V=1.09736, theta=4.89363
% INFO    : Bus 3	: V=1.0, theta=0.0 -> V=1.08662, theta=3.24953
% INFO    : Bus 4	: V=1.0, theta=0.0 -> V=1.09422, theta=-2.46292
% INFO    : Bus 5	: V=1.0, theta=0.0 -> V=1.08445, theta=-3.982
% INFO    : Bus 6	: V=1.0, theta=0.0 -> V=1.1, theta=0.60287
% INFO    : Bus 7	: V=1.0, theta=0.0 -> V=1.08949, theta=-1.19628
% INFO    : Bus 8	: V=1.0, theta=0.0 -> V=1.1, theta=0.90562
% INFO    : Bus 9	: V=1.0, theta=0.0 -> V=1.07176, theta=-4.61523
% INFO    : 
% INFO    : === Generator Setpoint Replacement Notes ===
% INFO    : Gen at bus 1	: Pg=0.0, Qg=0.0 -> Pg=89.799, Qg=12.966
% INFO    : Gen at bus 1	: Vg=1.0 -> Vg=1.1
% INFO    : Gen at bus 2	: Pg=163.0, Qg=0.0 -> Pg=134.321, Qg=0.032
% INFO    : Gen at bus 2	: Vg=1.0 -> Vg=1.09736
% INFO    : Gen at bus 3	: Pg=85.0, Qg=0.0 -> Pg=94.187, Qg=-22.634
% INFO    : Gen at bus 3	: Vg=1.0 -> Vg=1.08662
% INFO    : 
% INFO    : === Writing Matpower Case File Notes ===
