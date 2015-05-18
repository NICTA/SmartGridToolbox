%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%                                                                  %%%%%
%%%%      NICTA Energy System Test Case Archive (NESTA) - v0.3.0      %%%%%
%%%%              Optimal Power Flow - Typical Operation              %%%%%
%%%%                        30 - April - 2015                         %%%%%
%%%%                                                                  %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   6-bus system
%
%   From in problem 3.6 in book 'Computational
%   Methods for Electric Power Systems' by Mariesa Crow
%   created by Rui Bo on 2007/11/12
%
function mpc = nesta_case6_c
mpc.version = '2';
mpc.baseMVA = 100.0;

%% bus data
%	bus_i	type	Pd	Qd	Gs	Bs	area	Vm	Va	baseKV	zone	Vmax	Vmin
mpc.bus = [
	1	 3	 25.0	 10.0	 0.0	 0.0	 1	    1.10000	   -0.00000	 230.0	 1	    1.10000	    0.90000;
	2	 2	 15.0	 5.0	 0.0	 0.0	 1	    1.06319	   -6.04207	 230.0	 1	    1.10000	    0.90000;
	3	 1	 27.5	 11.0	 0.0	 0.0	 1	    1.05965	   -5.81220	 230.0	 1	    1.10000	    0.90000;
	4	 1	 0.0	 0.0	 0.0	 0.0	 1	    1.08466	   -4.16443	 230.0	 1	    1.10000	    0.90000;
	5	 1	 15.0	 9.0	 0.0	 0.0	 1	    1.05541	   -5.71989	 230.0	 1	    1.10000	    0.90000;
	6	 1	 25.0	 15.0	 0.0	 0.0	 1	    1.06571	   -4.52694	 230.0	 1	    1.10000	    0.90000;
];

%% generator data
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf
mpc.gen = [
	1	 109.555	 26.076	 100.0	 -100.0	 1.1	 100.0	 1	 949	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NUC
	2	 0.0	 25.419	 27.0	 -27.0	 1.06319	 100.0	 1	 53	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % COW
];

%% generator cost data
%	2	startup	shutdown	n	c(n-1)	...	c0
mpc.gencost = [
	2	 0.0	 0.0	 3	   0.000000	   0.211822	   0.000000; % NUC
	2	 0.0	 0.0	 3	   0.000000	   0.241315	   0.000000; % COW
];

%% branch data
%	fbus	tbus	r	x	b	rateA	rateB	rateC	ratio	angle	status	angmin	angmax
mpc.branch = [
	1	 4	 0.02	 0.185	 0.009	 170	 100.0	 100.0	 0.0	 0.0	 1	 -30.0	 30.0;
	1	 6	 0.031	 0.259	 0.01	 122	 100.0	 100.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 3	 0.006	 0.025	 0.0	 281	 100.0	 100.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 5	 0.071	 0.32	 0.015	 97	 97	 97	 0.0	 0.0	 1	 -30.0	 30.0;
	4	 6	 0.024	 0.204	 0.01	 154	 100.0	 100.0	 0.0	 0.0	 1	 -30.0	 30.0;
	3	 4	 0.075	 0.067	 0.0	 135	 100.0	 100.0	 0.0	 0.0	 1	 -30.0	 30.0;
	5	 6	 0.025	 0.15	 0.017	 208	 100.0	 100.0	 0.0	 0.0	 1	 -30.0	 30.0;
];

% INFO    : === Translation Options ===
% INFO    : Phase Angle Bound:           30.0 (deg.)
% INFO    : Line Capacity Model:         stat
% INFO    : Gen Active Capacity Model:   stat
% INFO    : Gen Reactive Capacity Model: am50ag
% INFO    : Gen Active Cost Model:       stat
% INFO    : AC OPF Solution File:        nesta_case6_c.dat.opf.sol
% WARNING : No active generation at the slack bus, assigning type - NUC
% INFO    : 
% INFO    : === Generator Classification Notes ===
% INFO    : NUC    1   -     0.00
% INFO    : COW    1   -   100.00
% INFO    : 
% INFO    : === Generator Active Capacity Stat Model Notes ===
% INFO    : Gen at bus 1 - NUC	: Pg=0.0, Pmax=100.0 -> Pmax=949   samples: 1
% INFO    : Gen at bus 2 - COW	: Pg=50.0, Pmax=100.0 -> Pmax=53   samples: 1
% INFO    : 
% INFO    : === Generator Reactive Capacity Atmost Max 50 Percent Active Model Notes ===
% INFO    : Gen at bus 2 - COW	: Pmax 53.0, Qmin -100.0, Qmax 100.0 -> Qmin -27.0, Qmax 27.0
% INFO    : 
% INFO    : === Generator Active Cost Stat Model Notes ===
% INFO    : Updated Generator Cost: NUC - 0.0 40.0 0.01 -> 0 0.211822043808 0
% INFO    : Updated Generator Cost: COW - 0.0 20.0 0.2 -> 0 0.241314691842 0
% INFO    : 
% INFO    : === Line Capacity Stat Model Notes ===
% WARNING : Updated Thermal Rating Stat Model was larger than UB Model: on 1-4 : 410 , 169
% INFO    : Updated Thermal Rating: on line 1-4 : Rate A , 999.0 -> 170
% WARNING : Updated Thermal Rating Stat Model was larger than UB Model: on 1-6 : 390 , 121
% INFO    : Updated Thermal Rating: on line 1-6 : Rate A , 999.0 -> 122
% INFO    : Updated Thermal Rating: on line 2-3 : Rate A , 999.0 -> 281
% WARNING : Updated Thermal Rating Stat Model was larger than UB Model: on 2-5 : 290 , 96
% INFO    : Updated Thermal Rating: on line 2-5 : Rate A, Rate B, Rate C , 999.0, 100.0, 100.0 -> 97
% WARNING : Updated Thermal Rating Stat Model was larger than UB Model: on 4-6 : 393 , 153
% INFO    : Updated Thermal Rating: on line 4-6 : Rate A , 999.0 -> 154
% INFO    : Updated Thermal Rating: on line 3-4 : Rate A , 999.0 -> 135
% WARNING : Updated Thermal Rating Stat Model was larger than UB Model: on 5-6 : 333 , 207
% INFO    : Updated Thermal Rating: on line 5-6 : Rate A , 999.0 -> 208
% INFO    : 
% INFO    : === Voltage Setpoint Replacement Notes ===
% INFO    : Bus 1	: V=1.0, theta=0.0 -> V=1.1, theta=-0.0
% INFO    : Bus 2	: V=1.0, theta=0.0 -> V=1.06319, theta=-6.04207
% INFO    : Bus 3	: V=1.0, theta=0.0 -> V=1.05965, theta=-5.8122
% INFO    : Bus 4	: V=1.0, theta=0.0 -> V=1.08466, theta=-4.16443
% INFO    : Bus 5	: V=1.0, theta=0.0 -> V=1.05541, theta=-5.71989
% INFO    : Bus 6	: V=1.0, theta=0.0 -> V=1.06571, theta=-4.52694
% INFO    : 
% INFO    : === Generator Setpoint Replacement Notes ===
% INFO    : Gen at bus 1	: Pg=0.0, Qg=0.0 -> Pg=109.555, Qg=26.076
% INFO    : Gen at bus 1	: Vg=1.05 -> Vg=1.1
% INFO    : Gen at bus 2	: Pg=50.0, Qg=0.0 -> Pg=0.0, Qg=25.419
% INFO    : Gen at bus 2	: Vg=1.05 -> Vg=1.06319
% INFO    : 
% INFO    : === Writing Matpower Case File Notes ===
