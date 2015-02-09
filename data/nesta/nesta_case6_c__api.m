%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%                                                                  %%%%%
%%%%      NICTA Energy System Test Case Archive (NESTA) - v0.1.5      %%%%%
%%%%            Optimal Power Flow - Active Power Increase            %%%%%
%%%%                       30 - October - 2014                        %%%%%
%%%%                                                                  %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function mpc = nesta_case6_c__api
mpc.version = '2';
mpc.baseMVA = 100.0;

%% bus data
%	bus_i	type	Pd	Qd	Gs	Bs	area	Vm	Va	baseKV	zone	Vmax	Vmin
mpc.bus = [
	1	 3	 177.88	 10.00	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.1000	     0.9000;
	2	 2	 106.73	 5.00	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.1000	     0.9000;
	3	 1	 195.67	 11.00	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.1000	     0.9000;
	4	 1	 0.00	 0.00	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.1000	     0.9000;
	5	 1	 106.73	 9.00	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.1000	     0.9000;
	6	 1	 177.88	 15.00	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.1000	     0.9000;
];

%% generator data
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf
mpc.gen = [
	1	 359.0	 53.0	 183.0	 -183.0	 1.05	 100.0	 1	 366	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % COW
	2	 422.0	 87.0	 427.0	 -427.0	 1.05	 100.0	 1	 854	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
];

%% generator cost data
%	2	startup	shutdown	n	c(n-1)	...	c0
mpc.gencost = [
	2	 0.0	 0.0	 3	   0.000000	   0.724820	   0.000000; % COW
	2	 0.0	 0.0	 3	   0.000000	   1.300286	   0.000000; % NG
];

%% branch data
%	fbus	tbus	r	x	b	rateA	rateB	rateC	ratio	angle	status	angmin	angmax
mpc.branch = [
	1	 4	 0.02	 0.185	 0.009	 169.0	 100.0	 100.0	 0.0	 0.0	 1	 -30.0	 30.0;
	1	 6	 0.031	 0.259	 0.01	 121.0	 100.0	 100.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 3	 0.006	 0.025	 0.0	 280.0	 100.0	 100.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 5	 0.071	 0.32	 0.015	 96.0	 96.0	 96.0	 0.0	 0.0	 1	 -30.0	 30.0;
	4	 6	 0.024	 0.204	 0.01	 153.0	 100.0	 100.0	 0.0	 0.0	 1	 -30.0	 30.0;
	3	 4	 0.075	 0.067	 0.0	 134.0	 100.0	 100.0	 0.0	 0.0	 1	 -30.0	 30.0;
	5	 6	 0.025	 0.15	 0.017	 207.0	 100.0	 100.0	 0.0	 0.0	 1	 -30.0	 30.0;
];

% INFO    : === Translation Options ===
% INFO    : Load Model:                  from file ./nesta_case6_c.dat.sol
% INFO    : Gen Active Capacity Model:   stat
% INFO    : Gen Reactive Capacity Model: al50ag
% INFO    : Gen Active Cost Model:       stat
% INFO    : 
% INFO    : === Load Replacement Notes ===
% INFO    : Bus 1	: Pd=25.0, Qd=10.0 -> Pd=177.88, Qd=10.00
% INFO    : Bus 2	: Pd=15.0, Qd=5.0 -> Pd=106.73, Qd=5.00
% INFO    : Bus 3	: Pd=27.5, Qd=11.0 -> Pd=195.67, Qd=11.00
% INFO    : Bus 4	: Pd=0.0, Qd=0.0 -> Pd=0.00, Qd=0.00
% INFO    : Bus 5	: Pd=15.0, Qd=9.0 -> Pd=106.73, Qd=9.00
% INFO    : Bus 6	: Pd=25.0, Qd=15.0 -> Pd=177.88, Qd=15.00
% INFO    : 
% INFO    : === Generator Setpoint Replacement Notes ===
% INFO    : Gen at bus 1	: Pg=0.0, Qg=0.0 -> Pg=359.0, Qg=53.0
% INFO    : Gen at bus 2	: Pg=50.0, Qg=0.0 -> Pg=422.0, Qg=87.0
% INFO    : 
% INFO    : === Generator Reactive Capacity Atleast Setpoint Value Notes ===
% INFO    : Gen at bus 2	: Qg 87.0, Qmin -27.0, Qmax 27.0 -> Qmin -104.4, Qmax 104.4
% INFO    : 
% INFO    : === Generator Classification Notes ===
% INFO    : COW    1   -    45.97
% INFO    : NG     1   -    54.03
% INFO    : 
% INFO    : === Generator Active Capacity Stat Model Notes ===
% INFO    : Gen at bus 1 - COW	: Pg=359.0, Pmax=949.0 -> Pmax=366   samples: 3
% INFO    : Gen at bus 2 - NG	: Pg=422.0, Pmax=53.0 -> Pmax=854   samples: 46
% INFO    : 
% INFO    : === Generator Active Capacity LB Model Notes ===
% INFO    : 
% INFO    : === Generator Reactive Capacity Atleast Max 50 Percent Active Model Notes ===
% INFO    : Gen at bus 1 - COW	: Pmax 366.0, Qmin -100.0, Qmax 100.0 -> Qmin -183.0, Qmax 183.0
% INFO    : Gen at bus 2 - NG	: Pmax 854.0, Qmin -104.4, Qmax 104.4 -> Qmin -427.0, Qmax 427.0
% INFO    : 
% INFO    : === Generator Active Cost Stat Model Notes ===
% INFO    : Updated Generator Cost: COW - 0.0 0.211822 0.0 -> 0 0.724819795938 0
% INFO    : Updated Generator Cost: NG - 0.0 0.241315 0.0 -> 0 1.30028584921 0
% INFO    : 
% INFO    : === Writing Matpower Case File Notes ===
