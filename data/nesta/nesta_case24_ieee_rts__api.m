%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%                                                                  %%%%%
%%%%      NICTA Energy System Test Case Archive (NESTA) - v0.1.5      %%%%%
%%%%            Optimal Power Flow - Active Power Increase            %%%%%
%%%%                       30 - October - 2014                        %%%%%
%%%%                                                                  %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function mpc = nesta_case24_ieee_rts__api
mpc.version = '2';
mpc.baseMVA = 100.0;

%% area data
%	area	refbus
mpc.areas = [
	1	 1;
	2	 3;
	3	 8;
	4	 6;
];

%% bus data
%	bus_i	type	Pd	Qd	Gs	Bs	area	Vm	Va	baseKV	zone	Vmax	Vmin
mpc.bus = [
	1	 2	 207.42	 22.00	 0.0	 0.0	 1	     1.0000	     0.0000	 138.0	 1	     1.0500	     0.9500;
	2	 2	 186.30	 20.00	 0.0	 0.0	 1	     1.0000	     0.0000	 138.0	 1	     1.0500	     0.9500;
	3	 1	 345.70	 37.00	 0.0	 0.0	 1	     1.0000	     0.0000	 138.0	 1	     1.0500	     0.9500;
	4	 1	 142.12	 15.00	 0.0	 0.0	 1	     1.0000	     0.0000	 138.0	 1	     1.0500	     0.9500;
	5	 1	 136.36	 14.00	 0.0	 0.0	 1	     1.0000	     0.0000	 138.0	 1	     1.0500	     0.9500;
	6	 1	 261.20	 28.00	 0.0	 -100.0	 2	     1.0000	     0.0000	 138.0	 1	     1.0500	     0.9500;
	7	 2	 240.07	 25.00	 0.0	 0.0	 2	     1.0000	     0.0000	 138.0	 1	     1.0500	     0.9500;
	8	 1	 328.42	 35.00	 0.0	 0.0	 2	     1.0000	     0.0000	 138.0	 1	     1.0500	     0.9500;
	9	 1	 336.10	 36.00	 0.0	 0.0	 1	     1.0000	     0.0000	 138.0	 1	     1.0500	     0.9500;
	10	 1	 374.51	 40.00	 0.0	 0.0	 2	     1.0000	     0.0000	 138.0	 1	     1.0500	     0.9500;
	11	 1	 0.00	 0.00	 0.0	 0.0	 3	     1.0000	     0.0000	 230.0	 1	     1.0500	     0.9500;
	12	 1	 0.00	 0.00	 0.0	 0.0	 3	     1.0000	     0.0000	 230.0	 1	     1.0500	     0.9500;
	13	 3	 508.95	 54.00	 0.0	 0.0	 3	     1.0000	     0.0000	 230.0	 1	     1.0500	     0.9500;
	14	 2	 372.59	 39.00	 0.0	 0.0	 3	     1.0000	     0.0000	 230.0	 1	     1.0500	     0.9500;
	15	 2	 608.82	 64.00	 0.0	 0.0	 4	     1.0000	     0.0000	 230.0	 1	     1.0500	     0.9500;
	16	 2	 192.06	 20.00	 0.0	 0.0	 4	     1.0000	     0.0000	 230.0	 1	     1.0500	     0.9500;
	17	 1	 0.00	 0.00	 0.0	 0.0	 4	     1.0000	     0.0000	 230.0	 1	     1.0500	     0.9500;
	18	 2	 639.55	 68.00	 0.0	 0.0	 4	     1.0000	     0.0000	 230.0	 1	     1.0500	     0.9500;
	19	 1	 347.62	 37.00	 0.0	 0.0	 3	     1.0000	     0.0000	 230.0	 1	     1.0500	     0.9500;
	20	 1	 245.83	 26.00	 0.0	 0.0	 3	     1.0000	     0.0000	 230.0	 1	     1.0500	     0.9500;
	21	 2	 0.00	 0.00	 0.0	 0.0	 4	     1.0000	     0.0000	 230.0	 1	     1.0500	     0.9500;
	22	 2	 0.00	 0.00	 0.0	 0.0	 4	     1.0000	     0.0000	 230.0	 1	     1.0500	     0.9500;
	23	 2	 0.00	 0.00	 0.0	 0.0	 3	     1.0000	     0.0000	 230.0	 1	     1.0500	     0.9500;
	24	 1	 0.00	 0.00	 0.0	 0.0	 4	     1.0000	     0.0000	 230.0	 1	     1.0500	     0.9500;
];

%% generator data
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf
mpc.gen = [
	1	 73.0	 1.0	 37.0	 -37.0	 1.035	 100.0	 1	 73	 8.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % PEL
	1	 73.0	 1.0	 61.0	 -61.0	 1.035	 100.0	 1	 122	 8.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % PEL
	1	 72.0	 1.0	 67.0	 -67.0	 1.035	 100.0	 1	 133	 7.6	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % PEL
	1	 72.0	 1.0	 59.0	 -59.0	 1.035	 100.0	 1	 117	 7.6	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % PEL
	2	 157.0	 31.0	 233.0	 -233.0	 1.035	 100.0	 1	 466	 8.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	2	 157.0	 31.0	 86.0	 -86.0	 1.035	 100.0	 1	 171	 8.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % PEL
	2	 156.0	 31.0	 293.0	 -293.0	 1.035	 100.0	 1	 585	 7.6	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % COW
	2	 156.0	 31.0	 91.0	 -91.0	 1.035	 100.0	 1	 181	 7.6	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % PEL
	7	 125.0	 42.0	 109.0	 -109.0	 1.025	 100.0	 1	 218	 12.5	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % COW
	7	 125.0	 42.0	 205.0	 -205.0	 1.025	 100.0	 1	 410	 12.5	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % COW
	7	 125.0	 42.0	 66.0	 -66.0	 1.025	 100.0	 1	 131	 12.5	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % COW
	13	 409.0	 106.0	 288.0	 -288.0	 1.02	 100.0	 1	 575	 34.5	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % COW
	13	 409.0	 106.0	 563.0	 -563.0	 1.02	 100.0	 1	 1126	 34.5	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % COW
	13	 409.0	 106.0	 272.0	 -272.0	 1.02	 100.0	 1	 543	 34.5	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	14	 0.0	 225.0	 270.0	 -270.0	 0.98	 100.0	 1	 0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % SYNC
	15	 134.0	 85.0	 102.0	 -102.0	 1.014	 100.0	 1	 171	 1.2	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % PEL
	15	 134.0	 85.0	 102.0	 -102.0	 1.014	 100.0	 1	 143	 1.2	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	15	 134.0	 85.0	 158.0	 -158.0	 1.014	 100.0	 1	 315	 1.2	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	15	 134.0	 85.0	 102.0	 -102.0	 1.014	 100.0	 1	 172	 1.2	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % PEL
	15	 134.0	 85.0	 168.0	 -168.0	 1.014	 100.0	 1	 335	 1.2	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	15	 186.0	 85.0	 228.0	 -228.0	 1.014	 100.0	 1	 456	 27.15	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % COW
	16	 419.0	 -48.0	 287.0	 -287.0	 1.017	 100.0	 1	 574	 27.15	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	18	 277.0	 44.0	 200.0	 -158.0	 1.05	 100.0	 1	 316	 50.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	21	 217.0	 -132.0	 200.0	 -158.4	 1.05	 100.0	 1	 269	 50.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	22	 81.0	 -7.0	 58.0	 -58.0	 1.05	 100.0	 1	 115	 5.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	22	 81.0	 -7.0	 43.0	 -43.0	 1.05	 100.0	 1	 85	 5.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	22	 81.0	 -7.0	 68.0	 -68.0	 1.05	 100.0	 1	 135	 5.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	22	 81.0	 -7.0	 55.0	 -55.0	 1.05	 100.0	 1	 109	 5.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % PEL
	22	 81.0	 -7.0	 112.0	 -112.0	 1.05	 100.0	 1	 224	 5.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	22	 81.0	 -7.0	 83.0	 -83.0	 1.05	 100.0	 1	 166	 5.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	23	 250.0	 16.0	 185.0	 -185.0	 1.05	 100.0	 1	 370	 27.15	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	23	 250.0	 16.0	 135.0	 -135.0	 1.05	 100.0	 1	 269	 27.15	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	23	 336.0	 16.0	 177.0	 -177.0	 1.05	 100.0	 1	 353	 70.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
];

%% generator cost data
%	2	startup	shutdown	n	c(n-1)	...	c0
mpc.gencost = [
	2	 1500.0	 0.0	 3	   0.000000	   6.647751	   0.000000; % PEL
	2	 1500.0	 0.0	 3	   0.000000	   6.219813	   0.000000; % PEL
	2	 1500.0	 0.0	 3	   0.000000	   7.358006	   0.000000; % PEL
	2	 1500.0	 0.0	 3	   0.000000	   7.349334	   0.000000; % PEL
	2	 1500.0	 0.0	 3	   0.000000	   1.105532	   0.000000; % NG
	2	 1500.0	 0.0	 3	   0.000000	   6.610238	   0.000000; % PEL
	2	 1500.0	 0.0	 3	   0.000000	   0.973207	   0.000000; % COW
	2	 1500.0	 0.0	 3	   0.000000	   7.119281	   0.000000; % PEL
	2	 1500.0	 0.0	 3	   0.000000	   0.876700	   0.000000; % COW
	2	 1500.0	 0.0	 3	   0.000000	   0.718776	   0.000000; % COW
	2	 1500.0	 0.0	 3	   0.000000	   0.719416	   0.000000; % COW
	2	 1500.0	 0.0	 3	   0.000000	   0.656781	   0.000000; % COW
	2	 1500.0	 0.0	 3	   0.000000	   0.622836	   0.000000; % COW
	2	 1500.0	 0.0	 3	   0.000000	   1.104077	   0.000000; % NG
	2	 1500.0	 0.0	 3	   0.000000	   0.000000	   0.000000; % SYNC
	2	 1500.0	 0.0	 3	   0.000000	   6.543459	   0.000000; % PEL
	2	 1500.0	 0.0	 3	   0.000000	   1.048022	   0.000000; % NG
	2	 1500.0	 0.0	 3	   0.000000	   0.734774	   0.000000; % NG
	2	 1500.0	 0.0	 3	   0.000000	   7.209866	   0.000000; % PEL
	2	 1500.0	 0.0	 3	   0.000000	   0.989379	   0.000000; % NG
	2	 1500.0	 0.0	 3	   0.000000	   1.020578	   0.000000; % COW
	2	 1500.0	 0.0	 3	   0.000000	   0.993749	   0.000000; % NG
	2	 1500.0	 0.0	 3	   0.000000	   1.055880	   0.000000; % NG
	2	 1500.0	 0.0	 3	   0.000000	   1.304062	   0.000000; % NG
	2	 1500.0	 0.0	 3	   0.000000	   1.455688	   0.000000; % NG
	2	 1500.0	 0.0	 3	   0.000000	   0.950415	   0.000000; % NG
	2	 1500.0	 0.0	 3	   0.000000	   1.185803	   0.000000; % NG
	2	 1500.0	 0.0	 3	   0.000000	   7.449273	   0.000000; % PEL
	2	 1500.0	 0.0	 3	   0.000000	   1.054805	   0.000000; % NG
	2	 1500.0	 0.0	 3	   0.000000	   1.140347	   0.000000; % NG
	2	 1500.0	 0.0	 3	   0.000000	   1.064903	   0.000000; % NG
	2	 1500.0	 0.0	 3	   0.000000	   0.769840	   0.000000; % NG
	2	 1500.0	 0.0	 3	   0.000000	   1.138411	   0.000000; % NG
];

%% branch data
%	fbus	tbus	r	x	b	rateA	rateB	rateC	ratio	angle	status	angmin	angmax
mpc.branch = [
	1	 2	 0.0026	 0.0139	 0.4611	 175.0	 250.0	 200.0	 0.0	 0.0	 1	 -30.0	 30.0;
	1	 3	 0.0546	 0.2112	 0.0572	 175.0	 208.0	 220.0	 0.0	 0.0	 1	 -30.0	 30.0;
	1	 5	 0.0218	 0.0845	 0.0229	 175.0	 208.0	 220.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 4	 0.0328	 0.1267	 0.0343	 175.0	 208.0	 220.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 6	 0.0497	 0.192	 0.052	 175.0	 208.0	 220.0	 0.0	 0.0	 1	 -30.0	 30.0;
	3	 9	 0.0308	 0.119	 0.0322	 175.0	 208.0	 220.0	 0.0	 0.0	 1	 -30.0	 30.0;
	3	 24	 0.0023	 0.0839	 0.0	 400.0	 510.0	 600.0	 1.03	 0.0	 1	 -30.0	 30.0;
	4	 9	 0.0268	 0.1037	 0.0281	 175.0	 208.0	 220.0	 0.0	 0.0	 1	 -30.0	 30.0;
	5	 10	 0.0228	 0.0883	 0.0239	 175.0	 208.0	 220.0	 0.0	 0.0	 1	 -30.0	 30.0;
	6	 10	 0.0139	 0.0605	 2.459	 175.0	 193.0	 200.0	 0.0	 0.0	 1	 -30.0	 30.0;
	7	 8	 0.0159	 0.0614	 0.0166	 175.0	 208.0	 220.0	 0.0	 0.0	 1	 -30.0	 30.0;
	8	 9	 0.0427	 0.1651	 0.0447	 175.0	 208.0	 220.0	 0.0	 0.0	 1	 -30.0	 30.0;
	8	 10	 0.0427	 0.1651	 0.0447	 175.0	 208.0	 220.0	 0.0	 0.0	 1	 -30.0	 30.0;
	9	 11	 0.0023	 0.0839	 0.0	 400.0	 510.0	 600.0	 1.03	 0.0	 1	 -30.0	 30.0;
	9	 12	 0.0023	 0.0839	 0.0	 400.0	 510.0	 600.0	 1.03	 0.0	 1	 -30.0	 30.0;
	10	 11	 0.0023	 0.0839	 0.0	 400.0	 510.0	 600.0	 1.02	 0.0	 1	 -30.0	 30.0;
	10	 12	 0.0023	 0.0839	 0.0	 400.0	 510.0	 600.0	 1.02	 0.0	 1	 -30.0	 30.0;
	11	 13	 0.0061	 0.0476	 0.0999	 500.0	 600.0	 625.0	 0.0	 0.0	 1	 -30.0	 30.0;
	11	 14	 0.0054	 0.0418	 0.0879	 500.0	 625.0	 625.0	 0.0	 0.0	 1	 -30.0	 30.0;
	12	 13	 0.0061	 0.0476	 0.0999	 500.0	 625.0	 625.0	 0.0	 0.0	 1	 -30.0	 30.0;
	12	 23	 0.0124	 0.0966	 0.203	 500.0	 625.0	 625.0	 0.0	 0.0	 1	 -30.0	 30.0;
	13	 23	 0.0111	 0.0865	 0.1818	 500.0	 625.0	 625.0	 0.0	 0.0	 1	 -30.0	 30.0;
	14	 16	 0.005	 0.0389	 0.0818	 500.0	 625.0	 625.0	 0.0	 0.0	 1	 -30.0	 30.0;
	15	 16	 0.0022	 0.0173	 0.0364	 500.0	 600.0	 625.0	 0.0	 0.0	 1	 -30.0	 30.0;
	15	 21	 0.0063	 0.049	 0.103	 500.0	 600.0	 625.0	 0.0	 0.0	 1	 -30.0	 30.0;
	15	 21	 0.0063	 0.049	 0.103	 500.0	 600.0	 625.0	 0.0	 0.0	 1	 -30.0	 30.0;
	15	 24	 0.0067	 0.0519	 0.1091	 500.0	 600.0	 625.0	 0.0	 0.0	 1	 -30.0	 30.0;
	16	 17	 0.0033	 0.0259	 0.0545	 500.0	 600.0	 625.0	 0.0	 0.0	 1	 -30.0	 30.0;
	16	 19	 0.003	 0.0231	 0.0485	 500.0	 600.0	 625.0	 0.0	 0.0	 1	 -30.0	 30.0;
	17	 18	 0.0018	 0.0144	 0.0303	 500.0	 600.0	 625.0	 0.0	 0.0	 1	 -30.0	 30.0;
	17	 22	 0.0135	 0.1053	 0.2212	 500.0	 600.0	 625.0	 0.0	 0.0	 1	 -30.0	 30.0;
	18	 21	 0.0033	 0.0259	 0.0545	 500.0	 600.0	 625.0	 0.0	 0.0	 1	 -30.0	 30.0;
	18	 21	 0.0033	 0.0259	 0.0545	 500.0	 600.0	 625.0	 0.0	 0.0	 1	 -30.0	 30.0;
	19	 20	 0.0051	 0.0396	 0.0833	 500.0	 600.0	 625.0	 0.0	 0.0	 1	 -30.0	 30.0;
	19	 20	 0.0051	 0.0396	 0.0833	 500.0	 600.0	 625.0	 0.0	 0.0	 1	 -30.0	 30.0;
	20	 23	 0.0028	 0.0216	 0.0455	 500.0	 600.0	 625.0	 0.0	 0.0	 1	 -30.0	 30.0;
	20	 23	 0.0028	 0.0216	 0.0455	 500.0	 600.0	 625.0	 0.0	 0.0	 1	 -30.0	 30.0;
	21	 22	 0.0087	 0.0678	 0.1424	 500.0	 600.0	 625.0	 0.0	 0.0	 1	 -30.0	 30.0;
];

% INFO    : === Translation Options ===
% INFO    : Load Model:                  from file ./nesta_case24_ieee_rts.dat.sol
% INFO    : Gen Active Capacity Model:   stat
% INFO    : Gen Reactive Capacity Model: al50ag
% INFO    : Gen Active Cost Model:       stat
% INFO    : 
% INFO    : === Load Replacement Notes ===
% INFO    : Bus 1	: Pd=108.0, Qd=22.0 -> Pd=207.42, Qd=22.00
% INFO    : Bus 2	: Pd=97.0, Qd=20.0 -> Pd=186.30, Qd=20.00
% INFO    : Bus 3	: Pd=180.0, Qd=37.0 -> Pd=345.70, Qd=37.00
% INFO    : Bus 4	: Pd=74.0, Qd=15.0 -> Pd=142.12, Qd=15.00
% INFO    : Bus 5	: Pd=71.0, Qd=14.0 -> Pd=136.36, Qd=14.00
% INFO    : Bus 6	: Pd=136.0, Qd=28.0 -> Pd=261.20, Qd=28.00
% INFO    : Bus 7	: Pd=125.0, Qd=25.0 -> Pd=240.07, Qd=25.00
% INFO    : Bus 8	: Pd=171.0, Qd=35.0 -> Pd=328.42, Qd=35.00
% INFO    : Bus 9	: Pd=175.0, Qd=36.0 -> Pd=336.10, Qd=36.00
% INFO    : Bus 10	: Pd=195.0, Qd=40.0 -> Pd=374.51, Qd=40.00
% INFO    : Bus 11	: Pd=0.0, Qd=0.0 -> Pd=0.00, Qd=0.00
% INFO    : Bus 12	: Pd=0.0, Qd=0.0 -> Pd=0.00, Qd=0.00
% INFO    : Bus 13	: Pd=265.0, Qd=54.0 -> Pd=508.95, Qd=54.00
% INFO    : Bus 14	: Pd=194.0, Qd=39.0 -> Pd=372.59, Qd=39.00
% INFO    : Bus 15	: Pd=317.0, Qd=64.0 -> Pd=608.82, Qd=64.00
% INFO    : Bus 16	: Pd=100.0, Qd=20.0 -> Pd=192.06, Qd=20.00
% INFO    : Bus 17	: Pd=0.0, Qd=0.0 -> Pd=0.00, Qd=0.00
% INFO    : Bus 18	: Pd=333.0, Qd=68.0 -> Pd=639.55, Qd=68.00
% INFO    : Bus 19	: Pd=181.0, Qd=37.0 -> Pd=347.62, Qd=37.00
% INFO    : Bus 20	: Pd=128.0, Qd=26.0 -> Pd=245.83, Qd=26.00
% INFO    : Bus 21	: Pd=0.0, Qd=0.0 -> Pd=0.00, Qd=0.00
% INFO    : Bus 22	: Pd=0.0, Qd=0.0 -> Pd=0.00, Qd=0.00
% INFO    : Bus 23	: Pd=0.0, Qd=0.0 -> Pd=0.00, Qd=0.00
% INFO    : Bus 24	: Pd=0.0, Qd=0.0 -> Pd=0.00, Qd=0.00
% INFO    : 
% INFO    : === Generator Setpoint Replacement Notes ===
% INFO    : Gen at bus 1	: Pg=10.0, Qg=0.0 -> Pg=73.0, Qg=1.0
% INFO    : Gen at bus 1	: Pg=10.0, Qg=0.0 -> Pg=73.0, Qg=1.0
% INFO    : Gen at bus 1	: Pg=76.0, Qg=0.0 -> Pg=72.0, Qg=1.0
% INFO    : Gen at bus 1	: Pg=76.0, Qg=0.0 -> Pg=72.0, Qg=1.0
% INFO    : Gen at bus 2	: Pg=10.0, Qg=0.0 -> Pg=157.0, Qg=31.0
% INFO    : Gen at bus 2	: Pg=10.0, Qg=0.0 -> Pg=157.0, Qg=31.0
% INFO    : Gen at bus 2	: Pg=76.0, Qg=0.0 -> Pg=156.0, Qg=31.0
% INFO    : Gen at bus 2	: Pg=76.0, Qg=0.0 -> Pg=156.0, Qg=31.0
% INFO    : Gen at bus 7	: Pg=80.0, Qg=0.0 -> Pg=125.0, Qg=42.0
% INFO    : Gen at bus 7	: Pg=80.0, Qg=0.0 -> Pg=125.0, Qg=42.0
% INFO    : Gen at bus 7	: Pg=80.0, Qg=0.0 -> Pg=125.0, Qg=42.0
% INFO    : Gen at bus 13	: Pg=95.1, Qg=0.0 -> Pg=409.0, Qg=106.0
% INFO    : Gen at bus 13	: Pg=95.1, Qg=0.0 -> Pg=409.0, Qg=106.0
% INFO    : Gen at bus 13	: Pg=95.1, Qg=0.0 -> Pg=409.0, Qg=106.0
% INFO    : Gen at bus 14	: Pg=0.0, Qg=35.3 -> Pg=0.0, Qg=225.0
% INFO    : Gen at bus 15	: Pg=12.0, Qg=0.0 -> Pg=134.0, Qg=85.0
% INFO    : Gen at bus 15	: Pg=12.0, Qg=0.0 -> Pg=134.0, Qg=85.0
% INFO    : Gen at bus 15	: Pg=12.0, Qg=0.0 -> Pg=134.0, Qg=85.0
% INFO    : Gen at bus 15	: Pg=12.0, Qg=0.0 -> Pg=134.0, Qg=85.0
% INFO    : Gen at bus 15	: Pg=12.0, Qg=0.0 -> Pg=134.0, Qg=85.0
% INFO    : Gen at bus 15	: Pg=155.0, Qg=0.0 -> Pg=186.0, Qg=85.0
% INFO    : Gen at bus 16	: Pg=155.0, Qg=0.0 -> Pg=419.0, Qg=-48.0
% INFO    : Gen at bus 18	: Pg=400.0, Qg=0.0 -> Pg=277.0, Qg=44.0
% INFO    : Gen at bus 21	: Pg=400.0, Qg=0.0 -> Pg=217.0, Qg=-132.0
% INFO    : Gen at bus 22	: Pg=50.0, Qg=0.0 -> Pg=81.0, Qg=-7.0
% INFO    : Gen at bus 22	: Pg=50.0, Qg=0.0 -> Pg=81.0, Qg=-7.0
% INFO    : Gen at bus 22	: Pg=50.0, Qg=0.0 -> Pg=81.0, Qg=-7.0
% INFO    : Gen at bus 22	: Pg=50.0, Qg=0.0 -> Pg=81.0, Qg=-7.0
% INFO    : Gen at bus 22	: Pg=50.0, Qg=0.0 -> Pg=81.0, Qg=-7.0
% INFO    : Gen at bus 22	: Pg=50.0, Qg=0.0 -> Pg=81.0, Qg=-7.0
% INFO    : Gen at bus 23	: Pg=155.0, Qg=0.0 -> Pg=250.0, Qg=16.0
% INFO    : Gen at bus 23	: Pg=155.0, Qg=0.0 -> Pg=250.0, Qg=16.0
% INFO    : Gen at bus 23	: Pg=350.0, Qg=0.0 -> Pg=336.0, Qg=16.0
% INFO    : 
% INFO    : === Generator Reactive Capacity Atleast Setpoint Value Notes ===
% INFO    : Gen at bus 1	: Qg 1.0, Qmin 0.0, Qmax 10.0 -> Qmin -1.2, Qmax 10.0
% INFO    : Gen at bus 1	: Qg 1.0, Qmin 0.0, Qmax 10.0 -> Qmin -1.2, Qmax 10.0
% INFO    : Gen at bus 2	: Qg 31.0, Qmin 0.0, Qmax 10.0 -> Qmin -37.2, Qmax 37.2
% INFO    : Gen at bus 2	: Qg 31.0, Qmin 0.0, Qmax 10.0 -> Qmin -37.2, Qmax 37.2
% INFO    : Gen at bus 2	: Qg 31.0, Qmin -25.0, Qmax 30.0 -> Qmin -37.2, Qmax 37.2
% INFO    : Gen at bus 2	: Qg 31.0, Qmin -25.0, Qmax 30.0 -> Qmin -37.2, Qmax 37.2
% INFO    : Gen at bus 7	: Qg 42.0, Qmin 0.0, Qmax 60.0 -> Qmin -50.4, Qmax 60.0
% INFO    : Gen at bus 7	: Qg 42.0, Qmin 0.0, Qmax 60.0 -> Qmin -50.4, Qmax 60.0
% INFO    : Gen at bus 7	: Qg 42.0, Qmin 0.0, Qmax 60.0 -> Qmin -50.4, Qmax 60.0
% INFO    : Gen at bus 13	: Qg 106.0, Qmin 0.0, Qmax 80.0 -> Qmin -127.2, Qmax 127.2
% INFO    : Gen at bus 13	: Qg 106.0, Qmin 0.0, Qmax 80.0 -> Qmin -127.2, Qmax 127.2
% INFO    : Gen at bus 13	: Qg 106.0, Qmin 0.0, Qmax 80.0 -> Qmin -127.2, Qmax 127.2
% INFO    : Gen at bus 14	: Qg 225.0, Qmin -50.0, Qmax 200.0 -> Qmin -270.0, Qmax 270.0
% INFO    : Gen at bus 15	: Qg 85.0, Qmin 0.0, Qmax 6.0 -> Qmin -102.0, Qmax 102.0
% INFO    : Gen at bus 15	: Qg 85.0, Qmin 0.0, Qmax 6.0 -> Qmin -102.0, Qmax 102.0
% INFO    : Gen at bus 15	: Qg 85.0, Qmin 0.0, Qmax 6.0 -> Qmin -102.0, Qmax 102.0
% INFO    : Gen at bus 15	: Qg 85.0, Qmin 0.0, Qmax 6.0 -> Qmin -102.0, Qmax 102.0
% INFO    : Gen at bus 15	: Qg 85.0, Qmin 0.0, Qmax 6.0 -> Qmin -102.0, Qmax 102.0
% INFO    : Gen at bus 15	: Qg 85.0, Qmin -50.0, Qmax 80.0 -> Qmin -102.0, Qmax 102.0
% INFO    : Gen at bus 21	: Qg -132.0, Qmin -50.0, Qmax 200.0 -> Qmin -158.4, Qmax 200.0
% INFO    : 
% INFO    : === Generator Classification Notes ===
% INFO    : PEL    9   -    16.97
% INFO    : SYNC   1   -     0.00
% INFO    : COW    7   -    27.37
% INFO    : NG     16  -    55.66
% INFO    : 
% INFO    : === Generator Active Capacity Stat Model Notes ===
% INFO    : Gen at bus 1 - PEL	: Pg=73.0, Pmax=20.0 -> Pmax=73   samples: 5
% INFO    : Gen at bus 1 - PEL	: Pg=73.0, Pmax=20.0 -> Pmax=122   samples: 5
% INFO    : Gen at bus 1 - PEL	: Pg=72.0, Pmax=76.0 -> Pmax=133   samples: 11
% INFO    : Gen at bus 1 - PEL	: Pg=72.0, Pmax=76.0 -> Pmax=117   samples: 6
% INFO    : Gen at bus 2 - NG	: Pg=157.0, Pmax=20.0 -> Pmax=466   samples: 2
% INFO    : Gen at bus 2 - PEL	: Pg=157.0, Pmax=20.0 -> Pmax=171   samples: 54
% INFO    : Gen at bus 2 - COW	: Pg=156.0, Pmax=76.0 -> Pmax=585   samples: 2
% WARNING : Failed to find a generator capacity within (156.0-780.0) after 100 samples, using percent increase model
% INFO    : Gen at bus 2 - PEL	: Pg=156.0, Pmax=76.0 -> Pmax=181   samples: 100
% INFO    : Gen at bus 7 - COW	: Pg=125.0, Pmax=100.0 -> Pmax=218   samples: 1
% INFO    : Gen at bus 7 - COW	: Pg=125.0, Pmax=100.0 -> Pmax=410   samples: 2
% INFO    : Gen at bus 7 - COW	: Pg=125.0, Pmax=100.0 -> Pmax=131   samples: 2
% INFO    : Gen at bus 13 - COW	: Pg=409.0, Pmax=197.0 -> Pmax=575   samples: 5
% INFO    : Gen at bus 13 - COW	: Pg=409.0, Pmax=197.0 -> Pmax=1126   samples: 2
% INFO    : Gen at bus 13 - NG	: Pg=409.0, Pmax=197.0 -> Pmax=543   samples: 4
% INFO    : Gen at bus 14 - SYNC	: Pg=0.0, Pmax=0.0 -> Pmax=0   samples: 0
% INFO    : Gen at bus 15 - PEL	: Pg=134.0, Pmax=12.0 -> Pmax=171   samples: 35
% INFO    : Gen at bus 15 - NG	: Pg=134.0, Pmax=12.0 -> Pmax=143   samples: 5
% INFO    : Gen at bus 15 - NG	: Pg=134.0, Pmax=12.0 -> Pmax=315   samples: 6
% INFO    : Gen at bus 15 - PEL	: Pg=134.0, Pmax=12.0 -> Pmax=172   samples: 47
% INFO    : Gen at bus 15 - NG	: Pg=134.0, Pmax=12.0 -> Pmax=335   samples: 7
% INFO    : Gen at bus 15 - COW	: Pg=186.0, Pmax=155.0 -> Pmax=456   samples: 1
% INFO    : Gen at bus 16 - NG	: Pg=419.0, Pmax=155.0 -> Pmax=574   samples: 49
% INFO    : Gen at bus 18 - NG	: Pg=277.0, Pmax=400.0 -> Pmax=316   samples: 6
% INFO    : Gen at bus 21 - NG	: Pg=217.0, Pmax=400.0 -> Pmax=269   samples: 5
% INFO    : Gen at bus 22 - NG	: Pg=81.0, Pmax=50.0 -> Pmax=115   samples: 2
% INFO    : Gen at bus 22 - NG	: Pg=81.0, Pmax=50.0 -> Pmax=85   samples: 3
% INFO    : Gen at bus 22 - NG	: Pg=81.0, Pmax=50.0 -> Pmax=135   samples: 1
% INFO    : Gen at bus 22 - PEL	: Pg=81.0, Pmax=50.0 -> Pmax=109   samples: 2
% INFO    : Gen at bus 22 - NG	: Pg=81.0, Pmax=50.0 -> Pmax=224   samples: 6
% INFO    : Gen at bus 22 - NG	: Pg=81.0, Pmax=50.0 -> Pmax=166   samples: 2
% INFO    : Gen at bus 23 - NG	: Pg=250.0, Pmax=155.0 -> Pmax=370   samples: 16
% INFO    : Gen at bus 23 - NG	: Pg=250.0, Pmax=155.0 -> Pmax=269   samples: 9
% INFO    : Gen at bus 23 - NG	: Pg=336.0, Pmax=350.0 -> Pmax=353   samples: 14
% INFO    : 
% INFO    : === Generator Active Capacity LB Model Notes ===
% INFO    : Gen at bus 1	: Pmin=16.0 -> Pmin=8.0 
% INFO    : Gen at bus 1	: Pmin=16.0 -> Pmin=8.0 
% INFO    : Gen at bus 1	: Pmin=15.2 -> Pmin=7.6 
% INFO    : Gen at bus 1	: Pmin=15.2 -> Pmin=7.6 
% INFO    : Gen at bus 2	: Pmin=16.0 -> Pmin=8.0 
% INFO    : Gen at bus 2	: Pmin=16.0 -> Pmin=8.0 
% INFO    : Gen at bus 2	: Pmin=15.2 -> Pmin=7.6 
% INFO    : Gen at bus 2	: Pmin=15.2 -> Pmin=7.6 
% INFO    : Gen at bus 7	: Pmin=25.0 -> Pmin=12.5 
% INFO    : Gen at bus 7	: Pmin=25.0 -> Pmin=12.5 
% INFO    : Gen at bus 7	: Pmin=25.0 -> Pmin=12.5 
% INFO    : Gen at bus 13	: Pmin=69.0 -> Pmin=34.5 
% INFO    : Gen at bus 13	: Pmin=69.0 -> Pmin=34.5 
% INFO    : Gen at bus 13	: Pmin=69.0 -> Pmin=34.5 
% INFO    : Gen at bus 15	: Pmin=2.4 -> Pmin=1.2 
% INFO    : Gen at bus 15	: Pmin=2.4 -> Pmin=1.2 
% INFO    : Gen at bus 15	: Pmin=2.4 -> Pmin=1.2 
% INFO    : Gen at bus 15	: Pmin=2.4 -> Pmin=1.2 
% INFO    : Gen at bus 15	: Pmin=2.4 -> Pmin=1.2 
% INFO    : Gen at bus 15	: Pmin=54.3 -> Pmin=27.15 
% INFO    : Gen at bus 16	: Pmin=54.3 -> Pmin=27.15 
% INFO    : Gen at bus 18	: Pmin=100.0 -> Pmin=50.0 
% INFO    : Gen at bus 21	: Pmin=100.0 -> Pmin=50.0 
% INFO    : Gen at bus 22	: Pmin=10.0 -> Pmin=5.0 
% INFO    : Gen at bus 22	: Pmin=10.0 -> Pmin=5.0 
% INFO    : Gen at bus 22	: Pmin=10.0 -> Pmin=5.0 
% INFO    : Gen at bus 22	: Pmin=10.0 -> Pmin=5.0 
% INFO    : Gen at bus 22	: Pmin=10.0 -> Pmin=5.0 
% INFO    : Gen at bus 22	: Pmin=10.0 -> Pmin=5.0 
% INFO    : Gen at bus 23	: Pmin=54.3 -> Pmin=27.15 
% INFO    : Gen at bus 23	: Pmin=54.3 -> Pmin=27.15 
% INFO    : Gen at bus 23	: Pmin=140.0 -> Pmin=70.0 
% INFO    : 
% INFO    : === Generator Reactive Capacity Atleast Max 50 Percent Active Model Notes ===
% INFO    : Gen at bus 1 - PEL	: Pmax 73.0, Qmin -1.2, Qmax 10.0 -> Qmin -37.0, Qmax 37.0
% INFO    : Gen at bus 1 - PEL	: Pmax 122.0, Qmin -1.2, Qmax 10.0 -> Qmin -61.0, Qmax 61.0
% INFO    : Gen at bus 1 - PEL	: Pmax 133.0, Qmin -25.0, Qmax 30.0 -> Qmin -67.0, Qmax 67.0
% INFO    : Gen at bus 1 - PEL	: Pmax 117.0, Qmin -25.0, Qmax 30.0 -> Qmin -59.0, Qmax 59.0
% INFO    : Gen at bus 2 - NG	: Pmax 466.0, Qmin -37.2, Qmax 37.2 -> Qmin -233.0, Qmax 233.0
% INFO    : Gen at bus 2 - PEL	: Pmax 171.0, Qmin -37.2, Qmax 37.2 -> Qmin -86.0, Qmax 86.0
% INFO    : Gen at bus 2 - COW	: Pmax 585.0, Qmin -37.2, Qmax 37.2 -> Qmin -293.0, Qmax 293.0
% INFO    : Gen at bus 2 - PEL	: Pmax 181.0, Qmin -37.2, Qmax 37.2 -> Qmin -91.0, Qmax 91.0
% INFO    : Gen at bus 7 - COW	: Pmax 218.0, Qmin -50.4, Qmax 60.0 -> Qmin -109.0, Qmax 109.0
% INFO    : Gen at bus 7 - COW	: Pmax 410.0, Qmin -50.4, Qmax 60.0 -> Qmin -205.0, Qmax 205.0
% INFO    : Gen at bus 7 - COW	: Pmax 131.0, Qmin -50.4, Qmax 60.0 -> Qmin -66.0, Qmax 66.0
% INFO    : Gen at bus 13 - COW	: Pmax 575.0, Qmin -127.2, Qmax 127.2 -> Qmin -288.0, Qmax 288.0
% INFO    : Gen at bus 13 - COW	: Pmax 1126.0, Qmin -127.2, Qmax 127.2 -> Qmin -563.0, Qmax 563.0
% INFO    : Gen at bus 13 - NG	: Pmax 543.0, Qmin -127.2, Qmax 127.2 -> Qmin -272.0, Qmax 272.0
% INFO    : Gen at bus 15 - NG	: Pmax 315.0, Qmin -102.0, Qmax 102.0 -> Qmin -158.0, Qmax 158.0
% INFO    : Gen at bus 15 - NG	: Pmax 335.0, Qmin -102.0, Qmax 102.0 -> Qmin -168.0, Qmax 168.0
% INFO    : Gen at bus 15 - COW	: Pmax 456.0, Qmin -102.0, Qmax 102.0 -> Qmin -228.0, Qmax 228.0
% INFO    : Gen at bus 16 - NG	: Pmax 574.0, Qmin -50.0, Qmax 80.0 -> Qmin -287.0, Qmax 287.0
% INFO    : Gen at bus 18 - NG	: Pmax 316.0, Qmin -50.0, Qmax 200.0 -> Qmin -158.0, Qmax 200.0
% INFO    : Gen at bus 22 - NG	: Pmax 115.0, Qmin -10.0, Qmax 16.0 -> Qmin -58.0, Qmax 58.0
% INFO    : Gen at bus 22 - NG	: Pmax 85.0, Qmin -10.0, Qmax 16.0 -> Qmin -43.0, Qmax 43.0
% INFO    : Gen at bus 22 - NG	: Pmax 135.0, Qmin -10.0, Qmax 16.0 -> Qmin -68.0, Qmax 68.0
% INFO    : Gen at bus 22 - PEL	: Pmax 109.0, Qmin -10.0, Qmax 16.0 -> Qmin -55.0, Qmax 55.0
% INFO    : Gen at bus 22 - NG	: Pmax 224.0, Qmin -10.0, Qmax 16.0 -> Qmin -112.0, Qmax 112.0
% INFO    : Gen at bus 22 - NG	: Pmax 166.0, Qmin -10.0, Qmax 16.0 -> Qmin -83.0, Qmax 83.0
% INFO    : Gen at bus 23 - NG	: Pmax 370.0, Qmin -50.0, Qmax 80.0 -> Qmin -185.0, Qmax 185.0
% INFO    : Gen at bus 23 - NG	: Pmax 269.0, Qmin -50.0, Qmax 80.0 -> Qmin -135.0, Qmax 135.0
% INFO    : Gen at bus 23 - NG	: Pmax 353.0, Qmin -25.0, Qmax 150.0 -> Qmin -177.0, Qmax 177.0
% INFO    : 
% INFO    : === Generator Active Cost Stat Model Notes ===
% INFO    : Updated Generator Cost: PEL - 400.6849 130.0 0.0 -> 0 6.64775067853 0
% INFO    : Updated Generator Cost: PEL - 400.6849 130.0 0.0 -> 0 6.2198125368 0
% INFO    : Updated Generator Cost: PEL - 212.3076 16.0811 0.014142 -> 0 7.3580061635 0
% INFO    : Updated Generator Cost: PEL - 212.3076 16.0811 0.014142 -> 0 7.34933403902 0
% INFO    : Updated Generator Cost: NG - 400.6849 130.0 0.0 -> 0 1.10553208554 0
% INFO    : Updated Generator Cost: PEL - 400.6849 130.0 0.0 -> 0 6.61023801304 0
% INFO    : Updated Generator Cost: COW - 212.3076 16.0811 0.014142 -> 0 0.973206958281 0
% INFO    : Updated Generator Cost: PEL - 212.3076 16.0811 0.014142 -> 0 7.11928051959 0
% INFO    : Updated Generator Cost: COW - 781.521 43.6615 0.052672 -> 0 0.876699859385 0
% INFO    : Updated Generator Cost: COW - 781.521 43.6615 0.052672 -> 0 0.718776232894 0
% INFO    : Updated Generator Cost: COW - 781.521 43.6615 0.052672 -> 0 0.719415792127 0
% INFO    : Updated Generator Cost: COW - 832.7575 48.5804 0.00717 -> 0 0.656781292032 0
% INFO    : Updated Generator Cost: COW - 832.7575 48.5804 0.00717 -> 0 0.622835741096 0
% INFO    : Updated Generator Cost: NG - 832.7575 48.5804 0.00717 -> 0 1.10407651675 0
% INFO    : Updated Generator Cost: SYNC - 0.0 0.0 0.0 -> 0 0.0 0
% INFO    : Updated Generator Cost: PEL - 86.3852 56.564 0.328412 -> 0 6.54345853408 0
% INFO    : Updated Generator Cost: NG - 86.3852 56.564 0.328412 -> 0 1.0480222293 0
% INFO    : Updated Generator Cost: NG - 86.3852 56.564 0.328412 -> 0 0.734774483894 0
% INFO    : Updated Generator Cost: PEL - 86.3852 56.564 0.328412 -> 0 7.20986620641 0
% INFO    : Updated Generator Cost: NG - 86.3852 56.564 0.328412 -> 0 0.989379046736 0
% INFO    : Updated Generator Cost: COW - 382.2391 12.3883 0.008342 -> 0 1.02057824917 0
% INFO    : Updated Generator Cost: NG - 382.2391 12.3883 0.008342 -> 0 0.993748547288 0
% INFO    : Updated Generator Cost: NG - 395.3749 4.4231 0.000213 -> 0 1.055880214 0
% INFO    : Updated Generator Cost: NG - 395.3749 4.4231 0.000213 -> 0 1.30406229176 0
% INFO    : Updated Generator Cost: NG - 0.001 0.001 0.0 -> 0 1.45568800583 0
% INFO    : Updated Generator Cost: NG - 0.001 0.001 0.0 -> 0 0.950415456282 0
% INFO    : Updated Generator Cost: NG - 0.001 0.001 0.0 -> 0 1.18580287278 0
% INFO    : Updated Generator Cost: PEL - 0.001 0.001 0.0 -> 0 7.44927314346 0
% INFO    : Updated Generator Cost: NG - 0.001 0.001 0.0 -> 0 1.05480453244 0
% INFO    : Updated Generator Cost: NG - 0.001 0.001 0.0 -> 0 1.14034653261 0
% INFO    : Updated Generator Cost: NG - 382.2391 12.3883 0.008342 -> 0 1.0649034894 0
% INFO    : Updated Generator Cost: NG - 382.2391 12.3883 0.008342 -> 0 0.769840407205 0
% INFO    : Updated Generator Cost: NG - 665.1094 11.8495 0.004895 -> 0 1.1384113881 0
% INFO    : 
% INFO    : === Writing Matpower Case File Notes ===
