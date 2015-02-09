%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%                                                                  %%%%%
%%%%      NICTA Energy System Test Case Archive (NESTA) - v0.1.5      %%%%%
%%%%            Optimal Power Flow - Active Power Increase            %%%%%
%%%%                       30 - October - 2014                        %%%%%
%%%%                                                                  %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function mpc = nesta_case4_gs__api
mpc.version = '2';
mpc.baseMVA = 100.0;

%% bus data
%	bus_i	type	Pd	Qd	Gs	Bs	area	Vm	Va	baseKV	zone	Vmax	Vmin
mpc.bus = [
	1	 3	 103.09	 30.99	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.1000	     0.9000;
	2	 1	 350.50	 105.35	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.1000	     0.9000;
	3	 1	 412.35	 123.94	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.1000	     0.9000;
	4	 2	 164.94	 49.58	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.1000	     0.9000;
];

%% generator data
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf
mpc.gen = [
	4	 586.0	 190.0	 633.0	 -633.0	 1.02	 100.0	 1	 1266	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % COW
	1	 459.0	 143.0	 545.0	 -545.0	 1.0	 100.0	 1	 1089	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % COW
];

%% generator cost data
%	2	startup	shutdown	n	c(n-1)	...	c0
mpc.gencost = [
	2	 0.0	 0.0	 3	   0.000000	   0.694244	   0.000000; % COW
	2	 0.0	 0.0	 3	   0.000000	   0.787873	   0.000000; % COW
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
% INFO    : Load Model:                  from file ./nesta_case4_gs.dat.sol
% INFO    : Gen Active Capacity Model:   stat
% INFO    : Gen Reactive Capacity Model: al50ag
% INFO    : Gen Active Cost Model:       stat
% INFO    : 
% INFO    : === Load Replacement Notes ===
% INFO    : Bus 1	: Pd=50.0, Qd=30.99 -> Pd=103.09, Qd=30.99
% INFO    : Bus 2	: Pd=170.0, Qd=105.35 -> Pd=350.50, Qd=105.35
% INFO    : Bus 3	: Pd=200.0, Qd=123.94 -> Pd=412.35, Qd=123.94
% INFO    : Bus 4	: Pd=80.0, Qd=49.58 -> Pd=164.94, Qd=49.58
% INFO    : 
% INFO    : === Generator Setpoint Replacement Notes ===
% INFO    : Gen at bus 4	: Pg=318.0, Qg=0.0 -> Pg=586.0, Qg=190.0
% INFO    : Gen at bus 1	: Pg=0.0, Qg=0.0 -> Pg=459.0, Qg=143.0
% INFO    : 
% INFO    : === Generator Reactive Capacity Atleast Setpoint Value Notes ===
% INFO    : Gen at bus 4	: Qg 190.0, Qmin -150.0, Qmax 150.0 -> Qmin -228.0, Qmax 228.0
% INFO    : 
% INFO    : === Generator Classification Notes ===
% INFO    : COW    2   -   100.00
% INFO    : 
% INFO    : === Generator Active Capacity Stat Model Notes ===
% INFO    : Gen at bus 4 - COW	: Pg=586.0, Pmax=419.0 -> Pmax=1266   samples: 4
% INFO    : Gen at bus 1 - COW	: Pg=459.0, Pmax=1220.0 -> Pmax=1089   samples: 3
% INFO    : 
% INFO    : === Generator Active Capacity LB Model Notes ===
% INFO    : 
% INFO    : === Generator Reactive Capacity Atleast Max 50 Percent Active Model Notes ===
% INFO    : Gen at bus 4 - COW	: Pmax 1266.0, Qmin -228.0, Qmax 228.0 -> Qmin -633.0, Qmax 633.0
% INFO    : Gen at bus 1 - COW	: Pmax 1089.0, Qmin -150.0, Qmax 150.0 -> Qmin -545.0, Qmax 545.0
% INFO    : 
% INFO    : === Generator Active Cost Stat Model Notes ===
% INFO    : Updated Generator Cost: COW - 0.0 1.074863 0.0 -> 0 0.694243983513 0
% INFO    : Updated Generator Cost: COW - 0.0 0.219975 0.0 -> 0 0.78787271406 0
% INFO    : 
% INFO    : === Writing Matpower Case File Notes ===
