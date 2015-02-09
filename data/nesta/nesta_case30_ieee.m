%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%                                                                  %%%%%
%%%%      NICTA Energy System Test Case Archive (NESTA) - v0.1.5      %%%%%
%%%%              Optimal Power Flow - Typical Operation              %%%%%
%%%%                       30 - October - 2014                        %%%%%
%%%%                                                                  %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Power flow data for IEEE 30 bus test case.
%   This data was converted from IEEE Common Data Format
%   (ieee30cdf.txt) on 20-Sep-2004 by cdf2matp, rev. 1.11
%
%   Converted from IEEE CDF file from:
%       http://www.ee.washington.edu/research/pstca/
%
%   CDF Header:
%   08/20/93 UW ARCHIVE           100.0  1961 W IEEE 30 Bus Test Case
%
function mpc = nesta_case30_ieee
mpc.version = '2';
mpc.baseMVA = 100.0;

%% bus data
%	bus_i	type	Pd	Qd	Gs	Bs	area	Vm	Va	baseKV	zone	Vmax	Vmin
mpc.bus = [
	1	 3	 0.0	 0.0	 0.0	 0.0	 1	     1.0600	     0.0000	 132.0	 1	     1.0600	     0.9400;
	2	 2	 21.7	 12.7	 0.0	 0.0	 1	     1.0430	    -5.4800	 132.0	 1	     1.0600	     0.9400;
	3	 1	 2.4	 1.2	 0.0	 0.0	 1	     1.0210	    -7.9600	 132.0	 1	     1.0600	     0.9400;
	4	 1	 7.6	 1.6	 0.0	 0.0	 1	     1.0120	    -9.6200	 132.0	 1	     1.0600	     0.9400;
	5	 2	 94.2	 19.0	 0.0	 0.0	 1	     1.0100	   -14.3700	 132.0	 1	     1.0600	     0.9400;
	6	 1	 0.0	 0.0	 0.0	 0.0	 1	     1.0100	   -11.3400	 132.0	 1	     1.0600	     0.9400;
	7	 1	 22.8	 10.9	 0.0	 0.0	 1	     1.0020	   -13.1200	 132.0	 1	     1.0600	     0.9400;
	8	 2	 30.0	 30.0	 0.0	 0.0	 1	     1.0100	   -12.1000	 132.0	 1	     1.0600	     0.9400;
	9	 1	 0.0	 0.0	 0.0	 0.0	 1	     1.0510	   -14.3800	 1.0	 1	     1.0600	     0.9400;
	10	 1	 5.8	 2.0	 0.0	 19.0	 1	     1.0450	   -15.9700	 33.0	 1	     1.0600	     0.9400;
	11	 2	 0.0	 0.0	 0.0	 0.0	 1	     1.0820	   -14.3900	 11.0	 1	     1.0600	     0.9400;
	12	 1	 11.2	 7.5	 0.0	 0.0	 1	     1.0570	   -15.2400	 33.0	 1	     1.0600	     0.9400;
	13	 2	 0.0	 0.0	 0.0	 0.0	 1	     1.0710	   -15.2400	 11.0	 1	     1.0600	     0.9400;
	14	 1	 6.2	 1.6	 0.0	 0.0	 1	     1.0420	   -16.1300	 33.0	 1	     1.0600	     0.9400;
	15	 1	 8.2	 2.5	 0.0	 0.0	 1	     1.0380	   -16.2200	 33.0	 1	     1.0600	     0.9400;
	16	 1	 3.5	 1.8	 0.0	 0.0	 1	     1.0450	   -15.8300	 33.0	 1	     1.0600	     0.9400;
	17	 1	 9.0	 5.8	 0.0	 0.0	 1	     1.0400	   -16.1400	 33.0	 1	     1.0600	     0.9400;
	18	 1	 3.2	 0.9	 0.0	 0.0	 1	     1.0280	   -16.8200	 33.0	 1	     1.0600	     0.9400;
	19	 1	 9.5	 3.4	 0.0	 0.0	 1	     1.0260	   -17.0000	 33.0	 1	     1.0600	     0.9400;
	20	 1	 2.2	 0.7	 0.0	 0.0	 1	     1.0300	   -16.8000	 33.0	 1	     1.0600	     0.9400;
	21	 1	 17.5	 11.2	 0.0	 0.0	 1	     1.0330	   -16.4200	 33.0	 1	     1.0600	     0.9400;
	22	 1	 0.0	 0.0	 0.0	 0.0	 1	     1.0330	   -16.4100	 33.0	 1	     1.0600	     0.9400;
	23	 1	 3.2	 1.6	 0.0	 0.0	 1	     1.0270	   -16.6100	 33.0	 1	     1.0600	     0.9400;
	24	 1	 8.7	 6.7	 0.0	 4.3	 1	     1.0210	   -16.7800	 33.0	 1	     1.0600	     0.9400;
	25	 1	 0.0	 0.0	 0.0	 0.0	 1	     1.0170	   -16.3500	 33.0	 1	     1.0600	     0.9400;
	26	 1	 3.5	 2.3	 0.0	 0.0	 1	     1.0000	   -16.7700	 33.0	 1	     1.0600	     0.9400;
	27	 1	 0.0	 0.0	 0.0	 0.0	 1	     1.0230	   -15.8200	 33.0	 1	     1.0600	     0.9400;
	28	 1	 0.0	 0.0	 0.0	 0.0	 1	     1.0070	   -11.9700	 132.0	 1	     1.0600	     0.9400;
	29	 1	 2.4	 0.9	 0.0	 0.0	 1	     1.0030	   -17.0600	 33.0	 1	     1.0600	     0.9400;
	30	 1	 10.6	 1.9	 0.0	 0.0	 1	     0.9920	   -17.9400	 33.0	 1	     1.0600	     0.9400;
];

%% generator data
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf
mpc.gen = [
	1	 260.2	 -16.1	 10.0	 0.0	 1.06	 100.0	 1	 784	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % COW
	2	 40.0	 50.0	 50.0	 -40.0	 1.045	 100.0	 1	 100	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	5	 0.0	 37.0	 40.0	 -40.0	 1.01	 100.0	 1	 0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % SYNC
	8	 0.0	 37.3	 40.0	 -10.0	 1.01	 100.0	 1	 0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % SYNC
	11	 0.0	 16.2	 24.0	 -6.0	 1.082	 100.0	 1	 0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % SYNC
	13	 0.0	 10.6	 24.0	 -6.0	 1.071	 100.0	 1	 0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % SYNC
];

%% generator cost data
%	2	startup	shutdown	n	c(n-1)	...	c0
mpc.gencost = [
	2	 0.0	 0.0	 3	   0.000000	   0.521378	   0.000000; % COW
	2	 0.0	 0.0	 3	   0.000000	   1.135166	   0.000000; % NG
	2	 0.0	 0.0	 3	   0.000000	   0.000000	   0.000000; % SYNC
	2	 0.0	 0.0	 3	   0.000000	   0.000000	   0.000000; % SYNC
	2	 0.0	 0.0	 3	   0.000000	   0.000000	   0.000000; % SYNC
	2	 0.0	 0.0	 3	   0.000000	   0.000000	   0.000000; % SYNC
];

%% branch data
%	fbus	tbus	r	x	b	rateA	rateB	rateC	ratio	angle	status	angmin	angmax
mpc.branch = [
	1	 2	 0.0192	 0.0575	 0.0528	 137	 137	 137	 0.0	 0.0	 1	 -30.0	 30.0;
	1	 3	 0.0452	 0.1652	 0.0408	 151	 151	 151	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 4	 0.057	 0.1737	 0.0368	 138	 138	 138	 0.0	 0.0	 1	 -30.0	 30.0;
	3	 4	 0.0132	 0.0379	 0.0084	 134	 134	 134	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 5	 0.0472	 0.1983	 0.0418	 143	 143	 143	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 6	 0.0581	 0.1763	 0.0374	 138	 138	 138	 0.0	 0.0	 1	 -30.0	 30.0;
	4	 6	 0.0119	 0.0414	 0.009	 147	 147	 147	 0.0	 0.0	 1	 -30.0	 30.0;
	5	 7	 0.046	 0.116	 0.0204	 126	 126	 126	 0.0	 0.0	 1	 -30.0	 30.0;
	6	 7	 0.0267	 0.082	 0.017	 139	 139	 139	 0.0	 0.0	 1	 -30.0	 30.0;
	6	 8	 0.012	 0.042	 0.009	 147	 147	 147	 0.0	 0.0	 1	 -30.0	 30.0;
	6	 9	 0.0	 0.208	 0.0	 141	 141	 141	 0.978	 0.0	 1	 -30.0	 30.0;
	6	 10	 0.0	 0.556	 0.0	 52	 52	 52	 0.969	 0.0	 1	 -30.0	 30.0;
	9	 11	 0.0	 0.208	 0.0	 141	 141	 141	 0.0	 0.0	 1	 -30.0	 30.0;
	9	 10	 0.0	 0.11	 0.0	 266	 266	 266	 0.0	 0.0	 1	 -30.0	 30.0;
	4	 12	 0.0	 0.256	 0.0	 114	 114	 114	 0.932	 0.0	 1	 -30.0	 30.0;
	12	 13	 0.0	 0.14	 0.0	 209	 209	 209	 0.0	 0.0	 1	 -30.0	 30.0;
	12	 14	 0.1231	 0.2559	 0.0	 28	 28	 28	 0.0	 0.0	 1	 -30.0	 30.0;
	12	 15	 0.0662	 0.1304	 0.0	 28	 28	 28	 0.0	 0.0	 1	 -30.0	 30.0;
	12	 16	 0.0945	 0.1987	 0.0	 29	 29	 29	 0.0	 0.0	 1	 -30.0	 30.0;
	14	 15	 0.221	 0.1997	 0.0	 19	 19	 19	 0.0	 0.0	 1	 -30.0	 30.0;
	16	 17	 0.0524	 0.1923	 0.0	 37	 37	 37	 0.0	 0.0	 1	 -30.0	 30.0;
	15	 18	 0.1073	 0.2185	 0.0	 28	 28	 28	 0.0	 0.0	 1	 -30.0	 30.0;
	18	 19	 0.0639	 0.1292	 0.0	 28	 28	 28	 0.0	 0.0	 1	 -30.0	 30.0;
	19	 20	 0.034	 0.068	 0.0	 28	 28	 28	 0.0	 0.0	 1	 -30.0	 30.0;
	10	 20	 0.0936	 0.209	 0.0	 29	 29	 29	 0.0	 0.0	 1	 -30.0	 30.0;
	10	 17	 0.0324	 0.0845	 0.0	 32	 32	 32	 0.0	 0.0	 1	 -30.0	 30.0;
	10	 21	 0.0348	 0.0749	 0.0	 29	 29	 29	 0.0	 0.0	 1	 -30.0	 30.0;
	10	 22	 0.0727	 0.1499	 0.0	 28	 28	 28	 0.0	 0.0	 1	 -30.0	 30.0;
	21	 22	 0.0116	 0.0236	 0.0	 28	 28	 28	 0.0	 0.0	 1	 -30.0	 30.0;
	15	 23	 0.1	 0.202	 0.0	 28	 28	 28	 0.0	 0.0	 1	 -30.0	 30.0;
	22	 24	 0.115	 0.179	 0.0	 25	 25	 25	 0.0	 0.0	 1	 -30.0	 30.0;
	23	 24	 0.132	 0.27	 0.0	 28	 28	 28	 0.0	 0.0	 1	 -30.0	 30.0;
	24	 25	 0.1885	 0.3292	 0.0	 26	 26	 26	 0.0	 0.0	 1	 -30.0	 30.0;
	25	 26	 0.2544	 0.38	 0.0	 24	 24	 24	 0.0	 0.0	 1	 -30.0	 30.0;
	25	 27	 0.1093	 0.2087	 0.0	 27	 27	 27	 0.0	 0.0	 1	 -30.0	 30.0;
	28	 27	 0.0	 0.396	 0.0	 74	 74	 74	 0.968	 0.0	 1	 -30.0	 30.0;
	27	 29	 0.2198	 0.4153	 0.0	 27	 27	 27	 0.0	 0.0	 1	 -30.0	 30.0;
	27	 30	 0.3202	 0.6027	 0.0	 27	 27	 27	 0.0	 0.0	 1	 -30.0	 30.0;
	29	 30	 0.2399	 0.4533	 0.0	 27	 27	 27	 0.0	 0.0	 1	 -30.0	 30.0;
	8	 28	 0.0636	 0.2	 0.0428	 139	 139	 139	 0.0	 0.0	 1	 -30.0	 30.0;
	6	 28	 0.0169	 0.0599	 0.013	 148	 148	 148	 0.0	 0.0	 1	 -30.0	 30.0;
];

% INFO    : === Translation Options ===
% INFO    : Phase Angle Bound:           30.0 (deg.)
% INFO    : Line Capacity Model:         stat
% INFO    : Gen Active Capacity Model:   stat
% INFO    : Gen Reactive Capacity Model: am50ag
% INFO    : Gen Active Cost Model:       stat
% INFO    : 
% INFO    : === Generator Classification Notes ===
% INFO    : SYNC   4   -     0.00
% INFO    : COW    1   -    86.68
% INFO    : NG     1   -    13.32
% INFO    : 
% INFO    : === Generator Active Capacity Stat Model Notes ===
% INFO    : Gen at bus 1 - COW	: Pg=260.2, Pmax=360.2 -> Pmax=784   samples: 13
% INFO    : Gen at bus 2 - NG	: Pg=40.0, Pmax=140.0 -> Pmax=100   samples: 1
% INFO    : Gen at bus 5 - SYNC	: Pg=0.0, Pmax=100.0 -> Pmax=0   samples: 0
% INFO    : Gen at bus 8 - SYNC	: Pg=0.0, Pmax=100.0 -> Pmax=0   samples: 0
% INFO    : Gen at bus 11 - SYNC	: Pg=0.0, Pmax=100.0 -> Pmax=0   samples: 0
% INFO    : Gen at bus 13 - SYNC	: Pg=0.0, Pmax=100.0 -> Pmax=0   samples: 0
% INFO    : 
% INFO    : === Generator Reactive Capacity Atmost Max 50 Percent Active Model Notes ===
% INFO    : 
% INFO    : === Generator Active Cost Stat Model Notes ===
% INFO    : Updated Generator Cost: COW - 0.0 20.0 0.038432 -> 0 0.521377501002 0
% INFO    : Updated Generator Cost: NG - 0.0 20.0 0.25 -> 0 1.13516647936 0
% INFO    : Updated Generator Cost: SYNC - 0.0 40.0 0.01 -> 0 0.0 0
% INFO    : Updated Generator Cost: SYNC - 0.0 40.0 0.01 -> 0 0.0 0
% INFO    : Updated Generator Cost: SYNC - 0.0 40.0 0.01 -> 0 0.0 0
% INFO    : Updated Generator Cost: SYNC - 0.0 40.0 0.01 -> 0 0.0 0
% INFO    : 
% INFO    : === Line Capacity Stat Model Notes ===
% INFO    : Updated Thermal Rating: on line 1-2 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 137
% INFO    : Updated Thermal Rating: on line 1-3 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 151
% INFO    : Updated Thermal Rating: on line 2-4 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 138
% INFO    : Updated Thermal Rating: on line 3-4 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 134
% WARNING : Updated Thermal Rating Stat Model was larger than UB Model: on 2-5 : 161 , 143
% INFO    : Updated Thermal Rating: on line 2-5 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 143
% INFO    : Updated Thermal Rating: on line 2-6 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 138
% INFO    : Updated Thermal Rating: on line 4-6 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 147
% INFO    : Updated Thermal Rating: on line 5-7 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 126
% INFO    : Updated Thermal Rating: on line 6-7 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 139
% INFO    : Updated Thermal Rating: on line 6-8 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 147
% WARNING : Missing data for branch flow stat model on line 6-9 using max current model : from_basekv=132.0 to_basekv=1.0 r=0.0 x=0.208
% INFO    : Updated Thermal Rating: on transformer 6-9 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 141
% WARNING : Missing data for branch flow stat model on line 6-10 using max current model : from_basekv=132.0 to_basekv=33.0 r=0.0 x=0.556
% INFO    : Updated Thermal Rating: on transformer 6-10 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 52
% WARNING : Missing data for branch flow stat model on line 9-11 using max current model : from_basekv=1.0 to_basekv=11.0 r=0.0 x=0.208
% INFO    : Updated Thermal Rating: on line 9-11 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 141
% WARNING : Missing data for branch flow stat model on line 9-10 using max current model : from_basekv=1.0 to_basekv=33.0 r=0.0 x=0.11
% INFO    : Updated Thermal Rating: on line 9-10 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 266
% WARNING : Missing data for branch flow stat model on line 4-12 using max current model : from_basekv=132.0 to_basekv=33.0 r=0.0 x=0.256
% INFO    : Updated Thermal Rating: on transformer 4-12 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 114
% WARNING : Missing data for branch flow stat model on line 12-13 using max current model : from_basekv=33.0 to_basekv=11.0 r=0.0 x=0.14
% INFO    : Updated Thermal Rating: on line 12-13 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 209
% INFO    : Updated Thermal Rating: on line 12-14 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 28
% INFO    : Updated Thermal Rating: on line 12-15 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 28
% INFO    : Updated Thermal Rating: on line 12-16 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 29
% INFO    : Updated Thermal Rating: on line 14-15 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 19
% INFO    : Updated Thermal Rating: on line 16-17 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 37
% INFO    : Updated Thermal Rating: on line 15-18 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 28
% INFO    : Updated Thermal Rating: on line 18-19 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 28
% INFO    : Updated Thermal Rating: on line 19-20 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 28
% INFO    : Updated Thermal Rating: on line 10-20 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 29
% INFO    : Updated Thermal Rating: on line 10-17 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 32
% INFO    : Updated Thermal Rating: on line 10-21 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 29
% INFO    : Updated Thermal Rating: on line 10-22 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 28
% INFO    : Updated Thermal Rating: on line 21-22 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 28
% INFO    : Updated Thermal Rating: on line 15-23 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 28
% INFO    : Updated Thermal Rating: on line 22-24 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 25
% INFO    : Updated Thermal Rating: on line 23-24 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 28
% INFO    : Updated Thermal Rating: on line 24-25 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 26
% INFO    : Updated Thermal Rating: on line 25-26 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 24
% INFO    : Updated Thermal Rating: on line 25-27 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 27
% WARNING : Missing data for branch flow stat model on line 28-27 using max current model : from_basekv=132.0 to_basekv=33.0 r=0.0 x=0.396
% INFO    : Updated Thermal Rating: on transformer 28-27 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 74
% INFO    : Updated Thermal Rating: on line 27-29 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 27
% INFO    : Updated Thermal Rating: on line 27-30 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 27
% INFO    : Updated Thermal Rating: on line 29-30 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 27
% WARNING : Updated Thermal Rating Stat Model was larger than UB Model: on 8-28 : 140 , 139
% INFO    : Updated Thermal Rating: on line 8-28 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 139
% INFO    : Updated Thermal Rating: on line 6-28 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 148
% INFO    : 
% INFO    : === Writing Matpower Case File Notes ===
