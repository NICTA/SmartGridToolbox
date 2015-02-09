%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%                                                                  %%%%%
%%%%      NICTA Energy System Test Case Archive (NESTA) - v0.1.5      %%%%%
%%%%              Optimal Power Flow - Typical Operation              %%%%%
%%%%                       30 - October - 2014                        %%%%%
%%%%                                                                  %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%    
%  Power flow data for IEEE 14 bus test case.
%  This data was converted from IEEE Common Data Format
%  (ieee14cdf.txt) on 20-Sep-2004 by cdf2matp, rev. 1.11
%
%  Converted from IEEE CDF file from:
%       http://www.ee.washington.edu/research/pstca/
%  
%  CDF Header:
%  08/19/93 UW ARCHIVE           100.0  1962 W IEEE 14 Bus Test Case
%
function mpc = nesta_case14_ieee
mpc.version = '2';
mpc.baseMVA = 100.0;

%% bus data
%	bus_i	type	Pd	Qd	Gs	Bs	area	Vm	Va	baseKV	zone	Vmax	Vmin
mpc.bus = [
	1	 3	 0.0	 0.0	 0.0	 0.0	 1	     1.0600	     0.0000	 0.0	 1	     1.0600	     0.9400;
	2	 2	 21.7	 12.7	 0.0	 0.0	 1	     1.0450	    -4.9800	 0.0	 1	     1.0600	     0.9400;
	3	 2	 94.2	 19.0	 0.0	 0.0	 1	     1.0100	   -12.7200	 0.0	 1	     1.0600	     0.9400;
	4	 1	 47.8	 -3.9	 0.0	 0.0	 1	     1.0190	   -10.3300	 0.0	 1	     1.0600	     0.9400;
	5	 1	 7.6	 1.6	 0.0	 0.0	 1	     1.0200	    -8.7800	 0.0	 1	     1.0600	     0.9400;
	6	 2	 11.2	 7.5	 0.0	 0.0	 1	     1.0700	   -14.2200	 0.0	 1	     1.0600	     0.9400;
	7	 1	 0.0	 0.0	 0.0	 0.0	 1	     1.0620	   -13.3700	 0.0	 1	     1.0600	     0.9400;
	8	 2	 0.0	 0.0	 0.0	 0.0	 1	     1.0900	   -13.3600	 0.0	 1	     1.0600	     0.9400;
	9	 1	 29.5	 16.6	 0.0	 19.0	 1	     1.0560	   -14.9400	 0.0	 1	     1.0600	     0.9400;
	10	 1	 9.0	 5.8	 0.0	 0.0	 1	     1.0510	   -15.1000	 0.0	 1	     1.0600	     0.9400;
	11	 1	 3.5	 1.8	 0.0	 0.0	 1	     1.0570	   -14.7900	 0.0	 1	     1.0600	     0.9400;
	12	 1	 6.1	 1.6	 0.0	 0.0	 1	     1.0550	   -15.0700	 0.0	 1	     1.0600	     0.9400;
	13	 1	 13.5	 5.8	 0.0	 0.0	 1	     1.0500	   -15.1600	 0.0	 1	     1.0600	     0.9400;
	14	 1	 14.9	 5.0	 0.0	 0.0	 1	     1.0360	   -16.0400	 0.0	 1	     1.0600	     0.9400;
];

%% generator data
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf
mpc.gen = [
	1	 232.4	 -16.9	 10.0	 0.0	 1.06	 100.0	 1	 362	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	2	 40.0	 42.4	 32.0	 -32.0	 1.045	 100.0	 1	 63	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	3	 0.0	 23.4	 40.0	 0.0	 1.01	 100.0	 1	 0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % SYNC
	6	 0.0	 12.2	 24.0	 -6.0	 1.07	 100.0	 1	 0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % SYNC
	8	 0.0	 17.4	 24.0	 -6.0	 1.09	 100.0	 1	 0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % SYNC
];

%% generator cost data
%	2	startup	shutdown	n	c(n-1)	...	c0
mpc.gencost = [
	2	 0.0	 0.0	 3	   0.000000	   1.026133	   0.000000; % NG
	2	 0.0	 0.0	 3	   0.000000	   0.480811	   0.000000; % NG
	2	 0.0	 0.0	 3	   0.000000	   0.000000	   0.000000; % SYNC
	2	 0.0	 0.0	 3	   0.000000	   0.000000	   0.000000; % SYNC
	2	 0.0	 0.0	 3	   0.000000	   0.000000	   0.000000; % SYNC
];

%% branch data
%	fbus	tbus	r	x	b	rateA	rateB	rateC	ratio	angle	status	angmin	angmax
mpc.branch = [
	1	 2	 0.01938	 0.05917	 0.0528	 471	 471	 471	 0.0	 0.0	 1	 -30.0	 30.0;
	1	 5	 0.05403	 0.22304	 0.0492	 127	 127	 127	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 3	 0.04699	 0.19797	 0.0438	 144	 144	 144	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 4	 0.05811	 0.17632	 0.034	 157	 157	 157	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 5	 0.05695	 0.17388	 0.0346	 160	 160	 160	 0.0	 0.0	 1	 -30.0	 30.0;
	3	 4	 0.06701	 0.17103	 0.0128	 159	 159	 159	 0.0	 0.0	 1	 -30.0	 30.0;
	4	 5	 0.01335	 0.04211	 0.0	 663	 663	 663	 0.0	 0.0	 1	 -30.0	 30.0;
	4	 7	 0.0	 0.20912	 0.0	 140	 140	 140	 0.978	 0.0	 1	 -30.0	 30.0;
	4	 9	 0.0	 0.55618	 0.0	 52	 52	 52	 0.969	 0.0	 1	 -30.0	 30.0;
	5	 6	 0.0	 0.25202	 0.0	 116	 116	 116	 0.932	 0.0	 1	 -30.0	 30.0;
	6	 11	 0.09498	 0.1989	 0.0	 133	 133	 133	 0.0	 0.0	 1	 -30.0	 30.0;
	6	 12	 0.12291	 0.25581	 0.0	 103	 103	 103	 0.0	 0.0	 1	 -30.0	 30.0;
	6	 13	 0.06615	 0.13027	 0.0	 200	 200	 200	 0.0	 0.0	 1	 -30.0	 30.0;
	7	 8	 0.0	 0.17615	 0.0	 166	 166	 166	 0.0	 0.0	 1	 -30.0	 30.0;
	7	 9	 0.0	 0.11001	 0.0	 266	 266	 266	 0.0	 0.0	 1	 -30.0	 30.0;
	9	 10	 0.03181	 0.0845	 0.0	 324	 324	 324	 0.0	 0.0	 1	 -30.0	 30.0;
	9	 14	 0.12711	 0.27038	 0.0	 98	 98	 98	 0.0	 0.0	 1	 -30.0	 30.0;
	10	 11	 0.08205	 0.19207	 0.0	 140	 140	 140	 0.0	 0.0	 1	 -30.0	 30.0;
	12	 13	 0.22092	 0.19988	 0.0	 98	 98	 98	 0.0	 0.0	 1	 -30.0	 30.0;
	13	 14	 0.17093	 0.34802	 0.0	 75	 75	 75	 0.0	 0.0	 1	 -30.0	 30.0;
];

% INFO    : === Translation Options ===
% INFO    : Phase Angle Bound:           30.0 (deg.)
% INFO    : Line Capacity Model:         stat
% INFO    : Gen Active Capacity Model:   stat
% INFO    : Gen Reactive Capacity Model: am50ag
% INFO    : Gen Active Cost Model:       stat
% INFO    : 
% INFO    : === Generator Classification Notes ===
% INFO    : SYNC   3   -     0.00
% INFO    : NG     2   -   100.00
% INFO    : 
% INFO    : === Generator Active Capacity Stat Model Notes ===
% INFO    : Gen at bus 1 - NG	: Pg=232.4, Pmax=332.4 -> Pmax=362   samples: 15
% INFO    : Gen at bus 2 - NG	: Pg=40.0, Pmax=140.0 -> Pmax=63   samples: 1
% INFO    : Gen at bus 3 - SYNC	: Pg=0.0, Pmax=100.0 -> Pmax=0   samples: 0
% INFO    : Gen at bus 6 - SYNC	: Pg=0.0, Pmax=100.0 -> Pmax=0   samples: 0
% INFO    : Gen at bus 8 - SYNC	: Pg=0.0, Pmax=100.0 -> Pmax=0   samples: 0
% INFO    : 
% INFO    : === Generator Reactive Capacity Atmost Max 50 Percent Active Model Notes ===
% INFO    : Gen at bus 2 - NG	: Pmax 63.0, Qmin -40.0, Qmax 50.0 -> Qmin -32.0, Qmax 32.0
% INFO    : 
% INFO    : === Generator Active Cost Stat Model Notes ===
% INFO    : Updated Generator Cost: NG - 0.0 20.0 0.0430293 -> 0 1.02613295282 0
% INFO    : Updated Generator Cost: NG - 0.0 20.0 0.25 -> 0 0.480811431055 0
% INFO    : Updated Generator Cost: SYNC - 0.0 40.0 0.01 -> 0 0.0 0
% INFO    : Updated Generator Cost: SYNC - 0.0 40.0 0.01 -> 0 0.0 0
% INFO    : Updated Generator Cost: SYNC - 0.0 40.0 0.01 -> 0 0.0 0
% INFO    : 
% INFO    : === Line Capacity Stat Model Notes ===
% WARNING : Missing data for branch flow stat model on line 1-2 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.01938 x=0.05917
% INFO    : Updated Thermal Rating: on line 1-2 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 471
% WARNING : Missing data for branch flow stat model on line 1-5 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.05403 x=0.22304
% INFO    : Updated Thermal Rating: on line 1-5 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 127
% WARNING : Missing data for branch flow stat model on line 2-3 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.04699 x=0.19797
% INFO    : Updated Thermal Rating: on line 2-3 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 144
% WARNING : Missing data for branch flow stat model on line 2-4 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.05811 x=0.17632
% INFO    : Updated Thermal Rating: on line 2-4 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 157
% WARNING : Missing data for branch flow stat model on line 2-5 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.05695 x=0.17388
% INFO    : Updated Thermal Rating: on line 2-5 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 160
% WARNING : Missing data for branch flow stat model on line 3-4 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.06701 x=0.17103
% INFO    : Updated Thermal Rating: on line 3-4 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 159
% WARNING : Missing data for branch flow stat model on line 4-5 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.01335 x=0.04211
% INFO    : Updated Thermal Rating: on line 4-5 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 663
% WARNING : Missing data for branch flow stat model on line 4-7 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0 x=0.20912
% INFO    : Updated Thermal Rating: on transformer 4-7 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 140
% WARNING : Missing data for branch flow stat model on line 4-9 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0 x=0.55618
% INFO    : Updated Thermal Rating: on transformer 4-9 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 52
% WARNING : Missing data for branch flow stat model on line 5-6 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0 x=0.25202
% INFO    : Updated Thermal Rating: on transformer 5-6 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 116
% WARNING : Missing data for branch flow stat model on line 6-11 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.09498 x=0.1989
% INFO    : Updated Thermal Rating: on line 6-11 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 133
% WARNING : Missing data for branch flow stat model on line 6-12 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.12291 x=0.25581
% INFO    : Updated Thermal Rating: on line 6-12 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 103
% WARNING : Missing data for branch flow stat model on line 6-13 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.06615 x=0.13027
% INFO    : Updated Thermal Rating: on line 6-13 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 200
% WARNING : Missing data for branch flow stat model on line 7-8 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0 x=0.17615
% INFO    : Updated Thermal Rating: on line 7-8 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 166
% WARNING : Missing data for branch flow stat model on line 7-9 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0 x=0.11001
% INFO    : Updated Thermal Rating: on line 7-9 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 266
% WARNING : Missing data for branch flow stat model on line 9-10 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.03181 x=0.0845
% INFO    : Updated Thermal Rating: on line 9-10 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 324
% WARNING : Missing data for branch flow stat model on line 9-14 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.12711 x=0.27038
% INFO    : Updated Thermal Rating: on line 9-14 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 98
% WARNING : Missing data for branch flow stat model on line 10-11 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.08205 x=0.19207
% INFO    : Updated Thermal Rating: on line 10-11 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 140
% WARNING : Missing data for branch flow stat model on line 12-13 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.22092 x=0.19988
% INFO    : Updated Thermal Rating: on line 12-13 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 98
% WARNING : Missing data for branch flow stat model on line 13-14 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.17093 x=0.34802
% INFO    : Updated Thermal Rating: on line 13-14 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 75
% INFO    : 
% INFO    : === Writing Matpower Case File Notes ===
