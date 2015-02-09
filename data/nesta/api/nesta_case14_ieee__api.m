%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%                                                                  %%%%%
%%%%      NICTA Energy System Test Case Archive (NESTA) - v0.1.5      %%%%%
%%%%            Optimal Power Flow - Active Power Increase            %%%%%
%%%%                       30 - October - 2014                        %%%%%
%%%%                                                                  %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function mpc = nesta_case14_ieee__api
mpc.version = '2';
mpc.baseMVA = 100.0;

%% bus data
%	bus_i	type	Pd	Qd	Gs	Bs	area	Vm	Va	baseKV	zone	Vmax	Vmin
mpc.bus = [
	1	 3	 0.00	 0.00	 0.0	 0.0	 1	     1.0600	     0.0000	 0.0	 1	     1.0600	     0.9400;
	2	 2	 42.39	 12.70	 0.0	 0.0	 1	     1.0450	    -4.9800	 0.0	 1	     1.0600	     0.9400;
	3	 2	 184.01	 19.00	 0.0	 0.0	 1	     1.0100	   -12.7200	 0.0	 1	     1.0600	     0.9400;
	4	 1	 47.80	 -3.90	 0.0	 0.0	 1	     1.0190	   -10.3300	 0.0	 1	     1.0600	     0.9400;
	5	 1	 14.85	 1.60	 0.0	 0.0	 1	     1.0200	    -8.7800	 0.0	 1	     1.0600	     0.9400;
	6	 2	 21.88	 7.50	 0.0	 0.0	 1	     1.0700	   -14.2200	 0.0	 1	     1.0600	     0.9400;
	7	 1	 0.00	 0.00	 0.0	 0.0	 1	     1.0620	   -13.3700	 0.0	 1	     1.0600	     0.9400;
	8	 2	 0.00	 0.00	 0.0	 0.0	 1	     1.0900	   -13.3600	 0.0	 1	     1.0600	     0.9400;
	9	 1	 57.62	 16.60	 0.0	 19.0	 1	     1.0560	   -14.9400	 0.0	 1	     1.0600	     0.9400;
	10	 1	 17.58	 5.80	 0.0	 0.0	 1	     1.0510	   -15.1000	 0.0	 1	     1.0600	     0.9400;
	11	 1	 6.84	 1.80	 0.0	 0.0	 1	     1.0570	   -14.7900	 0.0	 1	     1.0600	     0.9400;
	12	 1	 11.92	 1.60	 0.0	 0.0	 1	     1.0550	   -15.0700	 0.0	 1	     1.0600	     0.9400;
	13	 1	 26.37	 5.80	 0.0	 0.0	 1	     1.0500	   -15.1600	 0.0	 1	     1.0600	     0.9400;
	14	 1	 29.11	 5.00	 0.0	 0.0	 1	     1.0360	   -16.0400	 0.0	 1	     1.0600	     0.9400;
];

%% generator data
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf
mpc.gen = [
	1	 351.0	 33.0	 271.0	 -271.0	 1.06	 100.0	 1	 542	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % COW
	2	 151.0	 44.0	 95.0	 -95.0	 1.045	 100.0	 1	 190	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	3	 0.0	 59.0	 70.8	 -70.8	 1.01	 100.0	 1	 0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % SYNC
	6	 0.0	 46.0	 55.2	 -55.2	 1.07	 100.0	 1	 0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % SYNC
	8	 0.0	 25.0	 30.0	 -30.0	 1.09	 100.0	 1	 0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % SYNC
];

%% generator cost data
%	2	startup	shutdown	n	c(n-1)	...	c0
mpc.gencost = [
	2	 0.0	 0.0	 3	   0.000000	   0.664260	   0.000000; % COW
	2	 0.0	 0.0	 3	   0.000000	   0.606188	   0.000000; % NG
	2	 0.0	 0.0	 3	   0.000000	   0.000000	   0.000000; % SYNC
	2	 0.0	 0.0	 3	   0.000000	   0.000000	   0.000000; % SYNC
	2	 0.0	 0.0	 3	   0.000000	   0.000000	   0.000000; % SYNC
];

%% branch data
%	fbus	tbus	r	x	b	rateA	rateB	rateC	ratio	angle	status	angmin	angmax
mpc.branch = [
	1	 2	 0.01938	 0.05917	 0.0528	 471.0	 471.0	 471.0	 0.0	 0.0	 1	 -30.0	 30.0;
	1	 5	 0.05403	 0.22304	 0.0492	 127.0	 127.0	 127.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 3	 0.04699	 0.19797	 0.0438	 144.0	 144.0	 144.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 4	 0.05811	 0.17632	 0.034	 157.0	 157.0	 157.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 5	 0.05695	 0.17388	 0.0346	 160.0	 160.0	 160.0	 0.0	 0.0	 1	 -30.0	 30.0;
	3	 4	 0.06701	 0.17103	 0.0128	 159.0	 159.0	 159.0	 0.0	 0.0	 1	 -30.0	 30.0;
	4	 5	 0.01335	 0.04211	 0.0	 663.0	 663.0	 663.0	 0.0	 0.0	 1	 -30.0	 30.0;
	4	 7	 0.0	 0.20912	 0.0	 140.0	 140.0	 140.0	 0.978	 0.0	 1	 -30.0	 30.0;
	4	 9	 0.0	 0.55618	 0.0	 52.0	 52.0	 52.0	 0.969	 0.0	 1	 -30.0	 30.0;
	5	 6	 0.0	 0.25202	 0.0	 116.0	 116.0	 116.0	 0.932	 0.0	 1	 -30.0	 30.0;
	6	 11	 0.09498	 0.1989	 0.0	 133.0	 133.0	 133.0	 0.0	 0.0	 1	 -30.0	 30.0;
	6	 12	 0.12291	 0.25581	 0.0	 103.0	 103.0	 103.0	 0.0	 0.0	 1	 -30.0	 30.0;
	6	 13	 0.06615	 0.13027	 0.0	 200.0	 200.0	 200.0	 0.0	 0.0	 1	 -30.0	 30.0;
	7	 8	 0.0	 0.17615	 0.0	 166.0	 166.0	 166.0	 0.0	 0.0	 1	 -30.0	 30.0;
	7	 9	 0.0	 0.11001	 0.0	 266.0	 266.0	 266.0	 0.0	 0.0	 1	 -30.0	 30.0;
	9	 10	 0.03181	 0.0845	 0.0	 324.0	 324.0	 324.0	 0.0	 0.0	 1	 -30.0	 30.0;
	9	 14	 0.12711	 0.27038	 0.0	 98.0	 98.0	 98.0	 0.0	 0.0	 1	 -30.0	 30.0;
	10	 11	 0.08205	 0.19207	 0.0	 140.0	 140.0	 140.0	 0.0	 0.0	 1	 -30.0	 30.0;
	12	 13	 0.22092	 0.19988	 0.0	 98.0	 98.0	 98.0	 0.0	 0.0	 1	 -30.0	 30.0;
	13	 14	 0.17093	 0.34802	 0.0	 75.0	 75.0	 75.0	 0.0	 0.0	 1	 -30.0	 30.0;
];

% INFO    : === Translation Options ===
% INFO    : Load Model:                  from file ./nesta_case14_ieee.dat.sol
% INFO    : Gen Active Capacity Model:   stat
% INFO    : Gen Reactive Capacity Model: al50ag
% INFO    : Gen Active Cost Model:       stat
% INFO    : 
% INFO    : === Load Replacement Notes ===
% INFO    : Bus 1	: Pd=0.0, Qd=0.0 -> Pd=0.00, Qd=0.00
% INFO    : Bus 2	: Pd=21.7, Qd=12.7 -> Pd=42.39, Qd=12.70
% INFO    : Bus 3	: Pd=94.2, Qd=19.0 -> Pd=184.01, Qd=19.00
% INFO    : Bus 4	: Pd=47.8, Qd=-3.9 -> Pd=47.80, Qd=-3.90
% INFO    : Bus 5	: Pd=7.6, Qd=1.6 -> Pd=14.85, Qd=1.60
% INFO    : Bus 6	: Pd=11.2, Qd=7.5 -> Pd=21.88, Qd=7.50
% INFO    : Bus 7	: Pd=0.0, Qd=0.0 -> Pd=0.00, Qd=0.00
% INFO    : Bus 8	: Pd=0.0, Qd=0.0 -> Pd=0.00, Qd=0.00
% INFO    : Bus 9	: Pd=29.5, Qd=16.6 -> Pd=57.62, Qd=16.60
% INFO    : Bus 10	: Pd=9.0, Qd=5.8 -> Pd=17.58, Qd=5.80
% INFO    : Bus 11	: Pd=3.5, Qd=1.8 -> Pd=6.84, Qd=1.80
% INFO    : Bus 12	: Pd=6.1, Qd=1.6 -> Pd=11.92, Qd=1.60
% INFO    : Bus 13	: Pd=13.5, Qd=5.8 -> Pd=26.37, Qd=5.80
% INFO    : Bus 14	: Pd=14.9, Qd=5.0 -> Pd=29.11, Qd=5.00
% INFO    : 
% INFO    : === Generator Setpoint Replacement Notes ===
% INFO    : Gen at bus 1	: Pg=232.4, Qg=-16.9 -> Pg=351.0, Qg=33.0
% INFO    : Gen at bus 2	: Pg=40.0, Qg=42.4 -> Pg=151.0, Qg=44.0
% INFO    : Gen at bus 3	: Pg=0.0, Qg=23.4 -> Pg=0.0, Qg=59.0
% INFO    : Gen at bus 6	: Pg=0.0, Qg=12.2 -> Pg=0.0, Qg=46.0
% INFO    : Gen at bus 8	: Pg=0.0, Qg=17.4 -> Pg=0.0, Qg=25.0
% INFO    : 
% INFO    : === Generator Reactive Capacity Atleast Setpoint Value Notes ===
% INFO    : Gen at bus 1	: Qg 33.0, Qmin 0.0, Qmax 10.0 -> Qmin -39.6, Qmax 39.6
% INFO    : Gen at bus 2	: Qg 44.0, Qmin -32.0, Qmax 32.0 -> Qmin -52.8, Qmax 52.8
% INFO    : Gen at bus 3	: Qg 59.0, Qmin 0.0, Qmax 40.0 -> Qmin -70.8, Qmax 70.8
% INFO    : Gen at bus 6	: Qg 46.0, Qmin -6.0, Qmax 24.0 -> Qmin -55.2, Qmax 55.2
% INFO    : Gen at bus 8	: Qg 25.0, Qmin -6.0, Qmax 24.0 -> Qmin -30.0, Qmax 30.0
% INFO    : 
% INFO    : === Generator Classification Notes ===
% INFO    : SYNC   3   -     0.00
% INFO    : COW    1   -    69.92
% INFO    : NG     1   -    30.08
% INFO    : 
% INFO    : === Generator Active Capacity Stat Model Notes ===
% INFO    : Gen at bus 1 - COW	: Pg=351.0, Pmax=362.0 -> Pmax=542   samples: 4
% INFO    : Gen at bus 2 - NG	: Pg=151.0, Pmax=63.0 -> Pmax=190   samples: 5
% INFO    : Gen at bus 3 - SYNC	: Pg=0.0, Pmax=0.0 -> Pmax=0   samples: 0
% INFO    : Gen at bus 6 - SYNC	: Pg=0.0, Pmax=0.0 -> Pmax=0   samples: 0
% INFO    : Gen at bus 8 - SYNC	: Pg=0.0, Pmax=0.0 -> Pmax=0   samples: 0
% INFO    : 
% INFO    : === Generator Active Capacity LB Model Notes ===
% INFO    : 
% INFO    : === Generator Reactive Capacity Atleast Max 50 Percent Active Model Notes ===
% INFO    : Gen at bus 1 - COW	: Pmax 542.0, Qmin -39.6, Qmax 39.6 -> Qmin -271.0, Qmax 271.0
% INFO    : Gen at bus 2 - NG	: Pmax 190.0, Qmin -52.8, Qmax 52.8 -> Qmin -95.0, Qmax 95.0
% INFO    : 
% INFO    : === Generator Active Cost Stat Model Notes ===
% INFO    : Updated Generator Cost: COW - 0.0 1.026133 0.0 -> 0 0.664259701438 0
% INFO    : Updated Generator Cost: NG - 0.0 0.480811 0.0 -> 0 0.606187717211 0
% INFO    : Updated Generator Cost: SYNC - 0.0 0.0 0.0 -> 0 0.0 0
% INFO    : Updated Generator Cost: SYNC - 0.0 0.0 0.0 -> 0 0.0 0
% INFO    : Updated Generator Cost: SYNC - 0.0 0.0 0.0 -> 0 0.0 0
% INFO    : 
% INFO    : === Writing Matpower Case File Notes ===
