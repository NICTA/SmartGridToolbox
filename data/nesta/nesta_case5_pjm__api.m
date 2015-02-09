%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%                                                                  %%%%%
%%%%      NICTA Energy System Test Case Archive (NESTA) - v0.1.5      %%%%%
%%%%            Optimal Power Flow - Active Power Increase            %%%%%
%%%%                       30 - October - 2014                        %%%%%
%%%%                                                                  %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function mpc = nesta_case5_pjm__api
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
	1	 2	 0.00	 0.00	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.1000	     0.9000;
	2	 1	 805.57	 98.61	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.1000	     0.9000;
	3	 2	 805.57	 98.61	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.1000	     0.9000;
	4	 3	 1074.10	 131.47	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.1000	     0.9000;
	5	 2	 0.00	 0.00	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.1000	     0.9000;
];

%% generator data
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf
mpc.gen = [
	1	 229.0	 182.0	 218.4	 -218.4	 1.0	 100.0	 1	 244	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	1	 229.0	 182.0	 218.4	 -218.4	 1.0	 100.0	 1	 256	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	3	 1111.0	 246.0	 628.0	 -628.0	 1.0	 100.0	 1	 1255	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	4	 978.0	 -71.0	 552.0	 -552.0	 1.0	 100.0	 1	 1104	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	5	 147.0	 -151.0	 450.0	 -450.0	 1.0	 100.0	 1	 264	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % COW
];

%% generator cost data
%	2	startup	shutdown	n	c(n-1)	...	c0
mpc.gencost = [
	2	 0.0	 0.0	 3	   0.000000	   0.732186	   0.000000; % NG
	2	 0.0	 0.0	 3	   0.000000	   1.171778	   0.000000; % NG
	2	 0.0	 0.0	 3	   0.000000	   1.057515	   0.000000; % NG
	2	 0.0	 0.0	 3	   0.000000	   1.267058	   0.000000; % NG
	2	 0.0	 0.0	 3	   0.000000	   1.221496	   0.000000; % COW
];

%% branch data
%	fbus	tbus	r	x	b	rateA	rateB	rateC	ratio	angle	status	angmin	angmax
mpc.branch = [
	1	 2	 0.00281	 0.0281	 0.00712	 400.0	 400.0	 400.0	 0.0	 0.0	 1	 -30.0	 30.0;
	1	 4	 0.00304	 0.0304	 0.00658	 425.0	 425.0	 425.0	 0.0	 0.0	 1	 -30.0	 30.0;
	1	 5	 0.00064	 0.0064	 0.03126	 425.0	 425.0	 425.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 3	 0.00108	 0.0108	 0.01852	 425.0	 425.0	 425.0	 0.0	 0.0	 1	 -30.0	 30.0;
	3	 4	 0.00297	 0.0297	 0.00674	 425.0	 425.0	 425.0	 0.0	 0.0	 1	 -30.0	 30.0;
	4	 5	 0.00297	 0.0297	 0.00674	 240.0	 240.0	 240.0	 0.0	 0.0	 1	 -30.0	 30.0;
];

% INFO    : === Translation Options ===
% INFO    : Load Model:                  from file ./nesta_case5_pjm.dat.sol
% INFO    : Gen Active Capacity Model:   stat
% INFO    : Gen Reactive Capacity Model: al50ag
% INFO    : Gen Active Cost Model:       stat
% INFO    : 
% INFO    : === Load Replacement Notes ===
% INFO    : Bus 1	: Pd=0.0, Qd=0.0 -> Pd=0.00, Qd=0.00
% INFO    : Bus 2	: Pd=300.0, Qd=98.61 -> Pd=805.57, Qd=98.61
% INFO    : Bus 3	: Pd=300.0, Qd=98.61 -> Pd=805.57, Qd=98.61
% INFO    : Bus 4	: Pd=400.0, Qd=131.47 -> Pd=1074.10, Qd=131.47
% INFO    : Bus 5	: Pd=0.0, Qd=0.0 -> Pd=0.00, Qd=0.00
% INFO    : 
% INFO    : === Generator Setpoint Replacement Notes ===
% INFO    : Gen at bus 1	: Pg=40.0, Qg=0.0 -> Pg=229.0, Qg=182.0
% INFO    : Gen at bus 1	: Pg=170.0, Qg=0.0 -> Pg=229.0, Qg=182.0
% INFO    : Gen at bus 3	: Pg=323.49, Qg=0.0 -> Pg=1111.0, Qg=246.0
% INFO    : Gen at bus 4	: Pg=0.0, Qg=0.0 -> Pg=978.0, Qg=-71.0
% INFO    : Gen at bus 5	: Pg=466.51, Qg=0.0 -> Pg=147.0, Qg=-151.0
% INFO    : 
% INFO    : === Generator Reactive Capacity Atleast Setpoint Value Notes ===
% INFO    : Gen at bus 1	: Qg 182.0, Qmin -30.0, Qmax 30.0 -> Qmin -218.4, Qmax 218.4
% INFO    : Gen at bus 1	: Qg 182.0, Qmin -127.5, Qmax 127.5 -> Qmin -218.4, Qmax 218.4
% INFO    : 
% INFO    : === Generator Classification Notes ===
% INFO    : COW    1   -     5.46
% INFO    : NG     4   -    94.54
% INFO    : 
% INFO    : === Generator Active Capacity Stat Model Notes ===
% INFO    : Gen at bus 1 - NG	: Pg=229.0, Pmax=40.0 -> Pmax=244   samples: 10
% INFO    : Gen at bus 1 - NG	: Pg=229.0, Pmax=170.0 -> Pmax=256   samples: 1
% WARNING : Failed to find a generator capacity within (1111.0-5555.0) after 100 samples, using percent increase model
% INFO    : Gen at bus 3 - NG	: Pg=1111.0, Pmax=520.0 -> Pmax=1255   samples: 100
% WARNING : Failed to find a generator capacity within (978.0-4890.0) after 100 samples, using percent increase model
% INFO    : Gen at bus 4 - NG	: Pg=978.0, Pmax=200.0 -> Pmax=1104   samples: 100
% INFO    : Gen at bus 5 - COW	: Pg=147.0, Pmax=600.0 -> Pmax=264   samples: 1
% INFO    : 
% INFO    : === Generator Active Capacity LB Model Notes ===
% INFO    : 
% INFO    : === Generator Reactive Capacity Atleast Max 50 Percent Active Model Notes ===
% INFO    : Gen at bus 3 - NG	: Pmax 1255.0, Qmin -390.0, Qmax 390.0 -> Qmin -628.0, Qmax 628.0
% INFO    : Gen at bus 4 - NG	: Pmax 1104.0, Qmin -150.0, Qmax 150.0 -> Qmin -552.0, Qmax 552.0
% INFO    : 
% INFO    : === Generator Active Cost Stat Model Notes ===
% INFO    : Updated Generator Cost: NG - 0.0 14.0 0.0 -> 0 0.732185642141 0
% INFO    : Updated Generator Cost: NG - 0.0 15.0 0.0 -> 0 1.17177806036 0
% INFO    : Updated Generator Cost: NG - 0.0 30.0 0.0 -> 0 1.05751499583 0
% INFO    : Updated Generator Cost: NG - 0.0 40.0 0.0 -> 0 1.26705818044 0
% INFO    : Updated Generator Cost: COW - 0.0 10.0 0.0 -> 0 1.22149645228 0
% INFO    : 
% INFO    : === Writing Matpower Case File Notes ===
