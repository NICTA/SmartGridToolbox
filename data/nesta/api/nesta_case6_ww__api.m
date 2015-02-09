%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%                                                                  %%%%%
%%%%      NICTA Energy System Test Case Archive (NESTA) - v0.1.5      %%%%%
%%%%            Optimal Power Flow - Active Power Increase            %%%%%
%%%%                       30 - October - 2014                        %%%%%
%%%%                                                                  %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function mpc = nesta_case6_ww__api
mpc.version = '2';
mpc.baseMVA = 100.0;

%% bus data
%	bus_i	type	Pd	Qd	Gs	Bs	area	Vm	Va	baseKV	zone	Vmax	Vmin
mpc.bus = [
	1	 3	 0.00	 0.00	 0.0	 0.0	 1	     1.0500	     0.0000	 230.0	 1	     1.0500	     1.0500;
	2	 2	 0.00	 0.00	 0.0	 0.0	 1	     1.0500	     0.0000	 230.0	 1	     1.0500	     1.0500;
	3	 2	 0.00	 0.00	 0.0	 0.0	 1	     1.0700	     0.0000	 230.0	 1	     1.0700	     1.0700;
	4	 1	 78.24	 70.00	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.0500	     0.9500;
	5	 1	 78.24	 70.00	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.0500	     0.9500;
	6	 1	 78.24	 70.00	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.0500	     0.9500;
];

%% generator data
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf
mpc.gen = [
	1	 116.0	 17.0	 100.0	 -100.0	 1.05	 100.0	 1	 200	 25.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	2	 56.0	 77.0	 100.0	 -100.0	 1.05	 100.0	 1	 106	 18.75	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % COW
	3	 73.0	 90.0	 100.0	 -100.0	 1.07	 100.0	 1	 93	 22.5	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
];

%% generator cost data
%	2	startup	shutdown	n	c(n-1)	...	c0
mpc.gencost = [
	2	 0.0	 0.0	 3	   0.000000	   1.276311	   0.000000; % NG
	2	 0.0	 0.0	 3	   0.000000	   0.586272	   0.000000; % COW
	2	 0.0	 0.0	 3	   0.000000	   1.291110	   0.000000; % NG
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
% INFO    : Load Model:                  from file ./nesta_case6_ww.dat.sol
% INFO    : Gen Active Capacity Model:   stat
% INFO    : Gen Reactive Capacity Model: al50ag
% INFO    : Gen Active Cost Model:       stat
% INFO    : 
% INFO    : === Load Replacement Notes ===
% INFO    : Bus 1	: Pd=0.0, Qd=0.0 -> Pd=0.00, Qd=0.00
% INFO    : Bus 2	: Pd=0.0, Qd=0.0 -> Pd=0.00, Qd=0.00
% INFO    : Bus 3	: Pd=0.0, Qd=0.0 -> Pd=0.00, Qd=0.00
% INFO    : Bus 4	: Pd=70.0, Qd=70.0 -> Pd=78.24, Qd=70.00
% INFO    : Bus 5	: Pd=70.0, Qd=70.0 -> Pd=78.24, Qd=70.00
% INFO    : Bus 6	: Pd=70.0, Qd=70.0 -> Pd=78.24, Qd=70.00
% INFO    : 
% INFO    : === Generator Setpoint Replacement Notes ===
% INFO    : Gen at bus 1	: Pg=0.0, Qg=0.0 -> Pg=116.0, Qg=17.0
% INFO    : Gen at bus 2	: Pg=50.0, Qg=0.0 -> Pg=56.0, Qg=77.0
% INFO    : Gen at bus 3	: Pg=60.0, Qg=0.0 -> Pg=73.0, Qg=90.0
% INFO    : 
% INFO    : === Generator Reactive Capacity Atleast Setpoint Value Notes ===
% INFO    : 
% INFO    : === Generator Classification Notes ===
% INFO    : COW    1   -    22.86
% INFO    : NG     2   -    77.14
% INFO    : 
% INFO    : === Generator Active Capacity Stat Model Notes ===
% INFO    : Gen at bus 1 - NG	: Pg=116.0, Pmax=200.0 -> Pmax=200   samples: 2
% INFO    : Gen at bus 2 - COW	: Pg=56.0, Pmax=150.0 -> Pmax=106   samples: 2
% INFO    : Gen at bus 3 - NG	: Pg=73.0, Pmax=180.0 -> Pmax=93   samples: 1
% INFO    : 
% INFO    : === Generator Active Capacity LB Model Notes ===
% INFO    : Gen at bus 1	: Pmin=50.0 -> Pmin=25.0 
% INFO    : Gen at bus 2	: Pmin=37.5 -> Pmin=18.75 
% INFO    : Gen at bus 3	: Pmin=45.0 -> Pmin=22.5 
% INFO    : 
% INFO    : === Generator Reactive Capacity Atleast Max 50 Percent Active Model Notes ===
% INFO    : 
% INFO    : === Generator Active Cost Stat Model Notes ===
% INFO    : Updated Generator Cost: NG - 213.1 11.669 0.00533 -> 0 1.27631075531 0
% INFO    : Updated Generator Cost: COW - 200.0 10.333 0.00889 -> 0 0.586271724466 0
% INFO    : Updated Generator Cost: NG - 240.0 10.833 0.00741 -> 0 1.29111018615 0
% INFO    : 
% INFO    : === Writing Matpower Case File Notes ===
