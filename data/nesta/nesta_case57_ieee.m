%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%                                                                  %%%%%
%%%%      NICTA Energy System Test Case Archive (NESTA) - v0.1.5      %%%%%
%%%%              Optimal Power Flow - Typical Operation              %%%%%
%%%%                       30 - October - 2014                        %%%%%
%%%%                                                                  %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Power flow data for IEEE 57 bus test case.
%   This data was converted from IEEE Common Data Format
%   (ieee57cdf.txt) on 20-Sep-2004 by cdf2matp, rev. 1.11
%
%   Converted from IEEE CDF file from:
%       http://www.ee.washington.edu/research/pstca/
%
%   Manually modified Qmax, Qmin on generator 1 to 200, -140, respectively.
%   
%   CDF Header:
%   08/25/93 UW ARCHIVE           100.0  1961 W IEEE 57 Bus Test Case
%   
function mpc = nesta_case57_ieee
mpc.version = '2';
mpc.baseMVA = 100.0;

%% bus data
%	bus_i	type	Pd	Qd	Gs	Bs	area	Vm	Va	baseKV	zone	Vmax	Vmin
mpc.bus = [
	1	 3	 55.0	 17.0	 0.0	 0.0	 1	     1.0400	     0.0000	 0.0	 1	     1.0600	     0.9400;
	2	 2	 3.0	 88.0	 0.0	 0.0	 1	     1.0100	    -1.1800	 0.0	 1	     1.0600	     0.9400;
	3	 2	 41.0	 21.0	 0.0	 0.0	 1	     0.9850	    -5.9700	 0.0	 1	     1.0600	     0.9400;
	4	 1	 0.0	 0.0	 0.0	 0.0	 1	     0.9810	    -7.3200	 0.0	 1	     1.0600	     0.9400;
	5	 1	 13.0	 4.0	 0.0	 0.0	 1	     0.9760	    -8.5200	 0.0	 1	     1.0600	     0.9400;
	6	 2	 75.0	 2.0	 0.0	 0.0	 1	     0.9800	    -8.6500	 0.0	 1	     1.0600	     0.9400;
	7	 1	 0.0	 0.0	 0.0	 0.0	 1	     0.9840	    -7.5800	 0.0	 1	     1.0600	     0.9400;
	8	 2	 150.0	 22.0	 0.0	 0.0	 1	     1.0050	    -4.4500	 0.0	 1	     1.0600	     0.9400;
	9	 2	 121.0	 26.0	 0.0	 0.0	 1	     0.9800	    -9.5600	 0.0	 1	     1.0600	     0.9400;
	10	 1	 5.0	 2.0	 0.0	 0.0	 1	     0.9860	   -11.4300	 0.0	 1	     1.0600	     0.9400;
	11	 1	 0.0	 0.0	 0.0	 0.0	 1	     0.9740	   -10.1700	 0.0	 1	     1.0600	     0.9400;
	12	 2	 377.0	 24.0	 0.0	 0.0	 1	     1.0150	   -10.4600	 0.0	 1	     1.0600	     0.9400;
	13	 1	 18.0	 2.3	 0.0	 0.0	 1	     0.9790	    -9.7900	 0.0	 1	     1.0600	     0.9400;
	14	 1	 10.5	 5.3	 0.0	 0.0	 1	     0.9700	    -9.3300	 0.0	 1	     1.0600	     0.9400;
	15	 1	 22.0	 5.0	 0.0	 0.0	 1	     0.9880	    -7.1800	 0.0	 1	     1.0600	     0.9400;
	16	 1	 43.0	 3.0	 0.0	 0.0	 1	     1.0130	    -8.8500	 0.0	 1	     1.0600	     0.9400;
	17	 1	 42.0	 8.0	 0.0	 0.0	 1	     1.0170	    -5.3900	 0.0	 1	     1.0600	     0.9400;
	18	 1	 27.2	 9.8	 0.0	 10.0	 1	     1.0010	   -11.7100	 0.0	 1	     1.0600	     0.9400;
	19	 1	 3.3	 0.6	 0.0	 0.0	 1	     0.9700	   -13.2000	 0.0	 1	     1.0600	     0.9400;
	20	 1	 2.3	 1.0	 0.0	 0.0	 1	     0.9640	   -13.4100	 0.0	 1	     1.0600	     0.9400;
	21	 1	 0.0	 0.0	 0.0	 0.0	 1	     1.0080	   -12.8900	 0.0	 1	     1.0600	     0.9400;
	22	 1	 0.0	 0.0	 0.0	 0.0	 1	     1.0100	   -12.8400	 0.0	 1	     1.0600	     0.9400;
	23	 1	 6.3	 2.1	 0.0	 0.0	 1	     1.0080	   -12.9100	 0.0	 1	     1.0600	     0.9400;
	24	 1	 0.0	 0.0	 0.0	 0.0	 1	     0.9990	   -13.2500	 0.0	 1	     1.0600	     0.9400;
	25	 1	 6.3	 3.2	 0.0	 5.9	 1	     0.9820	   -18.1300	 0.0	 1	     1.0600	     0.9400;
	26	 1	 0.0	 0.0	 0.0	 0.0	 1	     0.9590	   -12.9500	 0.0	 1	     1.0600	     0.9400;
	27	 1	 9.3	 0.5	 0.0	 0.0	 1	     0.9820	   -11.4800	 0.0	 1	     1.0600	     0.9400;
	28	 1	 4.6	 2.3	 0.0	 0.0	 1	     0.9970	   -10.4500	 0.0	 1	     1.0600	     0.9400;
	29	 1	 17.0	 2.6	 0.0	 0.0	 1	     1.0100	    -9.7500	 0.0	 1	     1.0600	     0.9400;
	30	 1	 3.6	 1.8	 0.0	 0.0	 1	     0.9620	   -18.6800	 0.0	 1	     1.0600	     0.9400;
	31	 1	 5.8	 2.9	 0.0	 0.0	 1	     0.9360	   -19.3400	 0.0	 1	     1.0600	     0.9400;
	32	 1	 1.6	 0.8	 0.0	 0.0	 1	     0.9490	   -18.4600	 0.0	 1	     1.0600	     0.9400;
	33	 1	 3.8	 1.9	 0.0	 0.0	 1	     0.9470	   -18.5000	 0.0	 1	     1.0600	     0.9400;
	34	 1	 0.0	 0.0	 0.0	 0.0	 1	     0.9590	   -14.1000	 0.0	 1	     1.0600	     0.9400;
	35	 1	 6.0	 3.0	 0.0	 0.0	 1	     0.9660	   -13.8600	 0.0	 1	     1.0600	     0.9400;
	36	 1	 0.0	 0.0	 0.0	 0.0	 1	     0.9760	   -13.5900	 0.0	 1	     1.0600	     0.9400;
	37	 1	 0.0	 0.0	 0.0	 0.0	 1	     0.9850	   -13.4100	 0.0	 1	     1.0600	     0.9400;
	38	 1	 14.0	 7.0	 0.0	 0.0	 1	     1.0130	   -12.7100	 0.0	 1	     1.0600	     0.9400;
	39	 1	 0.0	 0.0	 0.0	 0.0	 1	     0.9830	   -13.4600	 0.0	 1	     1.0600	     0.9400;
	40	 1	 0.0	 0.0	 0.0	 0.0	 1	     0.9730	   -13.6200	 0.0	 1	     1.0600	     0.9400;
	41	 1	 6.3	 3.0	 0.0	 0.0	 1	     0.9960	   -14.0500	 0.0	 1	     1.0600	     0.9400;
	42	 1	 7.1	 4.4	 0.0	 0.0	 1	     0.9660	   -15.5000	 0.0	 1	     1.0600	     0.9400;
	43	 1	 2.0	 1.0	 0.0	 0.0	 1	     1.0100	   -11.3300	 0.0	 1	     1.0600	     0.9400;
	44	 1	 12.0	 1.8	 0.0	 0.0	 1	     1.0170	   -11.8600	 0.0	 1	     1.0600	     0.9400;
	45	 1	 0.0	 0.0	 0.0	 0.0	 1	     1.0360	    -9.2500	 0.0	 1	     1.0600	     0.9400;
	46	 1	 0.0	 0.0	 0.0	 0.0	 1	     1.0500	   -11.8900	 0.0	 1	     1.0600	     0.9400;
	47	 1	 29.7	 11.6	 0.0	 0.0	 1	     1.0330	   -12.4900	 0.0	 1	     1.0600	     0.9400;
	48	 1	 0.0	 0.0	 0.0	 0.0	 1	     1.0270	   -12.5900	 0.0	 1	     1.0600	     0.9400;
	49	 1	 18.0	 8.5	 0.0	 0.0	 1	     1.0360	   -12.9200	 0.0	 1	     1.0600	     0.9400;
	50	 1	 21.0	 10.5	 0.0	 0.0	 1	     1.0230	   -13.3900	 0.0	 1	     1.0600	     0.9400;
	51	 1	 18.0	 5.3	 0.0	 0.0	 1	     1.0520	   -12.5200	 0.0	 1	     1.0600	     0.9400;
	52	 1	 4.9	 2.2	 0.0	 0.0	 1	     0.9800	   -11.4700	 0.0	 1	     1.0600	     0.9400;
	53	 1	 20.0	 10.0	 0.0	 6.3	 1	     0.9710	   -12.2300	 0.0	 1	     1.0600	     0.9400;
	54	 1	 4.1	 1.4	 0.0	 0.0	 1	     0.9960	   -11.6900	 0.0	 1	     1.0600	     0.9400;
	55	 1	 6.8	 3.4	 0.0	 0.0	 1	     1.0310	   -10.7800	 0.0	 1	     1.0600	     0.9400;
	56	 1	 7.6	 2.2	 0.0	 0.0	 1	     0.9680	   -16.0400	 0.0	 1	     1.0600	     0.9400;
	57	 1	 6.7	 2.0	 0.0	 0.0	 1	     0.9650	   -16.5600	 0.0	 1	     1.0600	     0.9400;
];

%% generator data
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf
mpc.gen = [
	1	 128.9	 -16.1	 141.0	 -140.0	 1.04	 100.0	 1	 282	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % COW
	2	 0.0	 -0.8	 50.0	 -17.0	 1.01	 100.0	 1	 0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % SYNC
	3	 40.0	 -1.0	 31.0	 -10.0	 0.985	 100.0	 1	 61	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
	6	 0.0	 0.8	 25.0	 -8.0	 0.98	 100.0	 1	 0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % SYNC
	8	 450.0	 62.1	 200.0	 -140.0	 1.005	 100.0	 1	 510	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % COW
	9	 0.0	 2.2	 9.0	 -3.0	 0.98	 100.0	 1	 0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % SYNC
	12	 310.0	 128.5	 155.0	 -150.0	 1.015	 100.0	 1	 524	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0; % NG
];

%% generator cost data
%	2	startup	shutdown	n	c(n-1)	...	c0
mpc.gencost = [
	2	 0.0	 0.0	 3	   0.000000	   0.750694	   0.000000; % COW
	2	 0.0	 0.0	 3	   0.000000	   0.000000	   0.000000; % SYNC
	2	 0.0	 0.0	 3	   0.000000	   0.836961	   0.000000; % NG
	2	 0.0	 0.0	 3	   0.000000	   0.000000	   0.000000; % SYNC
	2	 0.0	 0.0	 3	   0.000000	   1.172038	   0.000000; % COW
	2	 0.0	 0.0	 3	   0.000000	   0.000000	   0.000000; % SYNC
	2	 0.0	 0.0	 3	   0.000000	   0.787329	   0.000000; % NG
];

%% branch data
%	fbus	tbus	r	x	b	rateA	rateB	rateC	ratio	angle	status	angmin	angmax
mpc.branch = [
	1	 2	 0.0083	 0.028	 0.129	 1004	 1004	 1004	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 3	 0.0298	 0.085	 0.0818	 325	 325	 325	 0.0	 0.0	 1	 -30.0	 30.0;
	3	 4	 0.0112	 0.0366	 0.038	 766	 766	 766	 0.0	 0.0	 1	 -30.0	 30.0;
	4	 5	 0.0625	 0.132	 0.0258	 200	 200	 200	 0.0	 0.0	 1	 -30.0	 30.0;
	4	 6	 0.043	 0.148	 0.0348	 190	 190	 190	 0.0	 0.0	 1	 -30.0	 30.0;
	6	 7	 0.02	 0.102	 0.0276	 282	 282	 282	 0.0	 0.0	 1	 -30.0	 30.0;
	6	 8	 0.0339	 0.173	 0.047	 166	 166	 166	 0.0	 0.0	 1	 -30.0	 30.0;
	8	 9	 0.0099	 0.0505	 0.0548	 569	 569	 569	 0.0	 0.0	 1	 -30.0	 30.0;
	9	 10	 0.0369	 0.1679	 0.044	 170	 170	 170	 0.0	 0.0	 1	 -30.0	 30.0;
	9	 11	 0.0258	 0.0848	 0.0218	 330	 330	 330	 0.0	 0.0	 1	 -30.0	 30.0;
	9	 12	 0.0648	 0.295	 0.0772	 97	 97	 97	 0.0	 0.0	 1	 -30.0	 30.0;
	9	 13	 0.0481	 0.158	 0.0406	 177	 177	 177	 0.0	 0.0	 1	 -30.0	 30.0;
	13	 14	 0.0132	 0.0434	 0.011	 646	 646	 646	 0.0	 0.0	 1	 -30.0	 30.0;
	13	 15	 0.0269	 0.0869	 0.023	 322	 322	 322	 0.0	 0.0	 1	 -30.0	 30.0;
	1	 15	 0.0178	 0.091	 0.0988	 316	 316	 316	 0.0	 0.0	 1	 -30.0	 30.0;
	1	 16	 0.0454	 0.206	 0.0546	 139	 139	 139	 0.0	 0.0	 1	 -30.0	 30.0;
	1	 17	 0.0238	 0.108	 0.0286	 265	 265	 265	 0.0	 0.0	 1	 -30.0	 30.0;
	3	 15	 0.0162	 0.053	 0.0544	 529	 529	 529	 0.0	 0.0	 1	 -30.0	 30.0;
	4	 18	 0.0	 0.555	 0.0	 52	 52	 52	 0.97	 0.0	 1	 -30.0	 30.0;
	4	 18	 0.0	 0.43	 0.0	 68	 68	 68	 0.978	 0.0	 1	 -30.0	 30.0;
	5	 6	 0.0302	 0.0641	 0.0124	 413	 413	 413	 0.0	 0.0	 1	 -30.0	 30.0;
	7	 8	 0.0139	 0.0712	 0.0194	 404	 404	 404	 0.0	 0.0	 1	 -30.0	 30.0;
	10	 12	 0.0277	 0.1262	 0.0328	 227	 227	 227	 0.0	 0.0	 1	 -30.0	 30.0;
	11	 13	 0.0223	 0.0732	 0.0188	 383	 383	 383	 0.0	 0.0	 1	 -30.0	 30.0;
	12	 13	 0.0178	 0.058	 0.0604	 483	 483	 483	 0.0	 0.0	 1	 -30.0	 30.0;
	12	 16	 0.018	 0.0813	 0.0216	 352	 352	 352	 0.0	 0.0	 1	 -30.0	 30.0;
	12	 17	 0.0397	 0.179	 0.0476	 159	 159	 159	 0.0	 0.0	 1	 -30.0	 30.0;
	14	 15	 0.0171	 0.0547	 0.0148	 511	 511	 511	 0.0	 0.0	 1	 -30.0	 30.0;
	18	 19	 0.461	 0.685	 0.0	 35	 35	 35	 0.0	 0.0	 1	 -30.0	 30.0;
	19	 20	 0.283	 0.434	 0.0	 56	 56	 56	 0.0	 0.0	 1	 -30.0	 30.0;
	21	 20	 0.0	 0.7767	 0.0	 37	 37	 37	 1.043	 0.0	 1	 -30.0	 30.0;
	21	 22	 0.0736	 0.117	 0.0	 212	 212	 212	 0.0	 0.0	 1	 -30.0	 30.0;
	22	 23	 0.0099	 0.0152	 0.0	 1616	 1616	 1616	 0.0	 0.0	 1	 -30.0	 30.0;
	23	 24	 0.166	 0.256	 0.0084	 96	 96	 96	 0.0	 0.0	 1	 -30.0	 30.0;
	24	 25	 0.0	 1.182	 0.0	 24	 24	 24	 1.0	 0.0	 1	 -30.0	 30.0;
	24	 25	 0.0	 1.23	 0.0	 23	 23	 23	 1.0	 0.0	 1	 -30.0	 30.0;
	24	 26	 0.0	 0.0473	 0.0	 620	 620	 620	 1.043	 0.0	 1	 -30.0	 30.0;
	26	 27	 0.165	 0.254	 0.0	 96	 96	 96	 0.0	 0.0	 1	 -30.0	 30.0;
	27	 28	 0.0618	 0.0954	 0.0	 258	 258	 258	 0.0	 0.0	 1	 -30.0	 30.0;
	28	 29	 0.0418	 0.0587	 0.0	 407	 407	 407	 0.0	 0.0	 1	 -30.0	 30.0;
	7	 29	 0.0	 0.0648	 0.0	 452	 452	 452	 0.967	 0.0	 1	 -30.0	 30.0;
	25	 30	 0.135	 0.202	 0.0	 120	 120	 120	 0.0	 0.0	 1	 -30.0	 30.0;
	30	 31	 0.326	 0.497	 0.0	 49	 49	 49	 0.0	 0.0	 1	 -30.0	 30.0;
	31	 32	 0.507	 0.755	 0.0	 32	 32	 32	 0.0	 0.0	 1	 -30.0	 30.0;
	32	 33	 0.0392	 0.036	 0.0	 551	 551	 551	 0.0	 0.0	 1	 -30.0	 30.0;
	34	 32	 0.0	 0.953	 0.0	 30	 30	 30	 0.975	 0.0	 1	 -30.0	 30.0;
	34	 35	 0.052	 0.078	 0.0032	 312	 312	 312	 0.0	 0.0	 1	 -30.0	 30.0;
	35	 36	 0.043	 0.0537	 0.0016	 426	 426	 426	 0.0	 0.0	 1	 -30.0	 30.0;
	36	 37	 0.029	 0.0366	 0.0	 628	 628	 628	 0.0	 0.0	 1	 -30.0	 30.0;
	37	 38	 0.0651	 0.1009	 0.002	 244	 244	 244	 0.0	 0.0	 1	 -30.0	 30.0;
	37	 39	 0.0239	 0.0379	 0.0	 654	 654	 654	 0.0	 0.0	 1	 -30.0	 30.0;
	36	 40	 0.03	 0.0466	 0.0	 529	 529	 529	 0.0	 0.0	 1	 -30.0	 30.0;
	22	 38	 0.0192	 0.0295	 0.0	 833	 833	 833	 0.0	 0.0	 1	 -30.0	 30.0;
	11	 41	 0.0	 0.749	 0.0	 39	 39	 39	 0.955	 0.0	 1	 -30.0	 30.0;
	41	 42	 0.207	 0.352	 0.0	 71	 71	 71	 0.0	 0.0	 1	 -30.0	 30.0;
	41	 43	 0.0	 0.412	 0.0	 71	 71	 71	 0.0	 0.0	 1	 -30.0	 30.0;
	38	 44	 0.0289	 0.0585	 0.002	 449	 449	 449	 0.0	 0.0	 1	 -30.0	 30.0;
	15	 45	 0.0	 0.1042	 0.0	 281	 281	 281	 0.955	 0.0	 1	 -30.0	 30.0;
	14	 46	 0.0	 0.0735	 0.0	 399	 399	 399	 0.9	 0.0	 1	 -30.0	 30.0;
	46	 47	 0.023	 0.068	 0.0032	 408	 408	 408	 0.0	 0.0	 1	 -30.0	 30.0;
	47	 48	 0.0182	 0.0233	 0.0	 992	 992	 992	 0.0	 0.0	 1	 -30.0	 30.0;
	48	 49	 0.0834	 0.129	 0.0048	 190	 190	 190	 0.0	 0.0	 1	 -30.0	 30.0;
	49	 50	 0.0801	 0.128	 0.0	 194	 194	 194	 0.0	 0.0	 1	 -30.0	 30.0;
	50	 51	 0.1386	 0.22	 0.0	 112	 112	 112	 0.0	 0.0	 1	 -30.0	 30.0;
	10	 51	 0.0	 0.0712	 0.0	 411	 411	 411	 0.93	 0.0	 1	 -30.0	 30.0;
	13	 49	 0.0	 0.191	 0.0	 153	 153	 153	 0.895	 0.0	 1	 -30.0	 30.0;
	29	 52	 0.1442	 0.187	 0.0	 124	 124	 124	 0.0	 0.0	 1	 -30.0	 30.0;
	52	 53	 0.0762	 0.0984	 0.0	 235	 235	 235	 0.0	 0.0	 1	 -30.0	 30.0;
	53	 54	 0.1878	 0.232	 0.0	 98	 98	 98	 0.0	 0.0	 1	 -30.0	 30.0;
	54	 55	 0.1732	 0.2265	 0.0	 102	 102	 102	 0.0	 0.0	 1	 -30.0	 30.0;
	11	 43	 0.0	 0.153	 0.0	 191	 191	 191	 0.958	 0.0	 1	 -30.0	 30.0;
	44	 45	 0.0624	 0.1242	 0.004	 211	 211	 211	 0.0	 0.0	 1	 -30.0	 30.0;
	40	 56	 0.0	 1.195	 0.0	 24	 24	 24	 0.958	 0.0	 1	 -30.0	 30.0;
	56	 41	 0.553	 0.549	 0.0	 37	 37	 37	 0.0	 0.0	 1	 -30.0	 30.0;
	56	 42	 0.2125	 0.354	 0.0	 71	 71	 71	 0.0	 0.0	 1	 -30.0	 30.0;
	39	 57	 0.0	 1.355	 0.0	 21	 21	 21	 0.98	 0.0	 1	 -30.0	 30.0;
	57	 56	 0.174	 0.26	 0.0	 93	 93	 93	 0.0	 0.0	 1	 -30.0	 30.0;
	38	 49	 0.115	 0.177	 0.003	 138	 138	 138	 0.0	 0.0	 1	 -30.0	 30.0;
	38	 48	 0.0312	 0.0482	 0.0	 510	 510	 510	 0.0	 0.0	 1	 -30.0	 30.0;
	9	 55	 0.0	 0.1205	 0.0	 243	 243	 243	 0.94	 0.0	 1	 -30.0	 30.0;
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
% INFO    : COW    2   -    62.32
% INFO    : NG     2   -    37.68
% INFO    : 
% INFO    : === Generator Active Capacity Stat Model Notes ===
% INFO    : Gen at bus 1 - COW	: Pg=128.9, Pmax=575.88 -> Pmax=282   samples: 1
% INFO    : Gen at bus 2 - SYNC	: Pg=0.0, Pmax=100.0 -> Pmax=0   samples: 0
% INFO    : Gen at bus 3 - NG	: Pg=40.0, Pmax=140.0 -> Pmax=61   samples: 2
% INFO    : Gen at bus 6 - SYNC	: Pg=0.0, Pmax=100.0 -> Pmax=0   samples: 0
% INFO    : Gen at bus 8 - COW	: Pg=450.0, Pmax=550.0 -> Pmax=510   samples: 5
% INFO    : Gen at bus 9 - SYNC	: Pg=0.0, Pmax=100.0 -> Pmax=0   samples: 0
% INFO    : Gen at bus 12 - NG	: Pg=310.0, Pmax=410.0 -> Pmax=524   samples: 8
% INFO    : 
% INFO    : === Generator Reactive Capacity Atmost Max 50 Percent Active Model Notes ===
% INFO    : Gen at bus 1 - COW	: Pmax 282.0, Qmin -140.0, Qmax 200.0 -> Qmin -140.0, Qmax 141.0
% INFO    : Gen at bus 3 - NG	: Pmax 61.0, Qmin -10.0, Qmax 60.0 -> Qmin -10.0, Qmax 31.0
% INFO    : 
% INFO    : === Generator Active Cost Stat Model Notes ===
% INFO    : Updated Generator Cost: COW - 0.0 20.0 0.0775795 -> 0 0.75069401612 0
% INFO    : Updated Generator Cost: SYNC - 0.0 40.0 0.01 -> 0 0.0 0
% INFO    : Updated Generator Cost: NG - 0.0 20.0 0.25 -> 0 0.836960666499 0
% INFO    : Updated Generator Cost: SYNC - 0.0 40.0 0.01 -> 0 0.0 0
% INFO    : Updated Generator Cost: COW - 0.0 20.0 0.0222222 -> 0 1.17203790149 0
% INFO    : Updated Generator Cost: SYNC - 0.0 40.0 0.01 -> 0 0.0 0
% INFO    : Updated Generator Cost: NG - 0.0 20.0 0.0322581 -> 0 0.787329396343 0
% INFO    : 
% INFO    : === Line Capacity Stat Model Notes ===
% WARNING : Missing data for branch flow stat model on line 1-2 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0083 x=0.028
% INFO    : Updated Thermal Rating: on line 1-2 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 1004
% WARNING : Missing data for branch flow stat model on line 2-3 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0298 x=0.085
% INFO    : Updated Thermal Rating: on line 2-3 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 325
% WARNING : Missing data for branch flow stat model on line 3-4 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0112 x=0.0366
% INFO    : Updated Thermal Rating: on line 3-4 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 766
% WARNING : Missing data for branch flow stat model on line 4-5 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0625 x=0.132
% INFO    : Updated Thermal Rating: on line 4-5 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 200
% WARNING : Missing data for branch flow stat model on line 4-6 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.043 x=0.148
% INFO    : Updated Thermal Rating: on line 4-6 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 190
% WARNING : Missing data for branch flow stat model on line 6-7 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.02 x=0.102
% INFO    : Updated Thermal Rating: on line 6-7 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 282
% WARNING : Missing data for branch flow stat model on line 6-8 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0339 x=0.173
% INFO    : Updated Thermal Rating: on line 6-8 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 166
% WARNING : Missing data for branch flow stat model on line 8-9 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0099 x=0.0505
% INFO    : Updated Thermal Rating: on line 8-9 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 569
% WARNING : Missing data for branch flow stat model on line 9-10 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0369 x=0.1679
% INFO    : Updated Thermal Rating: on line 9-10 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 170
% WARNING : Missing data for branch flow stat model on line 9-11 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0258 x=0.0848
% INFO    : Updated Thermal Rating: on line 9-11 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 330
% WARNING : Missing data for branch flow stat model on line 9-12 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0648 x=0.295
% INFO    : Updated Thermal Rating: on line 9-12 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 97
% WARNING : Missing data for branch flow stat model on line 9-13 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0481 x=0.158
% INFO    : Updated Thermal Rating: on line 9-13 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 177
% WARNING : Missing data for branch flow stat model on line 13-14 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0132 x=0.0434
% INFO    : Updated Thermal Rating: on line 13-14 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 646
% WARNING : Missing data for branch flow stat model on line 13-15 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0269 x=0.0869
% INFO    : Updated Thermal Rating: on line 13-15 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 322
% WARNING : Missing data for branch flow stat model on line 1-15 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0178 x=0.091
% INFO    : Updated Thermal Rating: on line 1-15 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 316
% WARNING : Missing data for branch flow stat model on line 1-16 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0454 x=0.206
% INFO    : Updated Thermal Rating: on line 1-16 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 139
% WARNING : Missing data for branch flow stat model on line 1-17 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0238 x=0.108
% INFO    : Updated Thermal Rating: on line 1-17 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 265
% WARNING : Missing data for branch flow stat model on line 3-15 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0162 x=0.053
% INFO    : Updated Thermal Rating: on line 3-15 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 529
% WARNING : Missing data for branch flow stat model on line 4-18 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0 x=0.555
% INFO    : Updated Thermal Rating: on transformer 4-18 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 52
% WARNING : Missing data for branch flow stat model on line 4-18 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0 x=0.43
% INFO    : Updated Thermal Rating: on transformer 4-18 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 68
% WARNING : Missing data for branch flow stat model on line 5-6 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0302 x=0.0641
% INFO    : Updated Thermal Rating: on line 5-6 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 413
% WARNING : Missing data for branch flow stat model on line 7-8 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0139 x=0.0712
% INFO    : Updated Thermal Rating: on line 7-8 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 404
% WARNING : Missing data for branch flow stat model on line 10-12 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0277 x=0.1262
% INFO    : Updated Thermal Rating: on line 10-12 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 227
% WARNING : Missing data for branch flow stat model on line 11-13 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0223 x=0.0732
% INFO    : Updated Thermal Rating: on line 11-13 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 383
% WARNING : Missing data for branch flow stat model on line 12-13 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0178 x=0.058
% INFO    : Updated Thermal Rating: on line 12-13 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 483
% WARNING : Missing data for branch flow stat model on line 12-16 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.018 x=0.0813
% INFO    : Updated Thermal Rating: on line 12-16 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 352
% WARNING : Missing data for branch flow stat model on line 12-17 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0397 x=0.179
% INFO    : Updated Thermal Rating: on line 12-17 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 159
% WARNING : Missing data for branch flow stat model on line 14-15 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0171 x=0.0547
% INFO    : Updated Thermal Rating: on line 14-15 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 511
% WARNING : Missing data for branch flow stat model on line 18-19 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.461 x=0.685
% INFO    : Updated Thermal Rating: on line 18-19 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 35
% WARNING : Missing data for branch flow stat model on line 19-20 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.283 x=0.434
% INFO    : Updated Thermal Rating: on line 19-20 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 56
% WARNING : Missing data for branch flow stat model on line 21-20 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0 x=0.7767
% INFO    : Updated Thermal Rating: on transformer 21-20 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 37
% WARNING : Missing data for branch flow stat model on line 21-22 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0736 x=0.117
% INFO    : Updated Thermal Rating: on line 21-22 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 212
% WARNING : Missing data for branch flow stat model on line 22-23 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0099 x=0.0152
% INFO    : Updated Thermal Rating: on line 22-23 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 1616
% WARNING : Missing data for branch flow stat model on line 23-24 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.166 x=0.256
% INFO    : Updated Thermal Rating: on line 23-24 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 96
% WARNING : Missing data for branch flow stat model on line 24-25 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0 x=1.182
% INFO    : Updated Thermal Rating: on line 24-25 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 24
% WARNING : Missing data for branch flow stat model on line 24-25 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0 x=1.23
% INFO    : Updated Thermal Rating: on line 24-25 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 23
% WARNING : Missing data for branch flow stat model on line 24-26 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0 x=0.0473
% INFO    : Updated Thermal Rating: on transformer 24-26 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 620
% WARNING : Missing data for branch flow stat model on line 26-27 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.165 x=0.254
% INFO    : Updated Thermal Rating: on line 26-27 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 96
% WARNING : Missing data for branch flow stat model on line 27-28 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0618 x=0.0954
% INFO    : Updated Thermal Rating: on line 27-28 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 258
% WARNING : Missing data for branch flow stat model on line 28-29 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0418 x=0.0587
% INFO    : Updated Thermal Rating: on line 28-29 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 407
% WARNING : Missing data for branch flow stat model on line 7-29 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0 x=0.0648
% INFO    : Updated Thermal Rating: on transformer 7-29 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 452
% WARNING : Missing data for branch flow stat model on line 25-30 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.135 x=0.202
% INFO    : Updated Thermal Rating: on line 25-30 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 120
% WARNING : Missing data for branch flow stat model on line 30-31 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.326 x=0.497
% INFO    : Updated Thermal Rating: on line 30-31 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 49
% WARNING : Missing data for branch flow stat model on line 31-32 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.507 x=0.755
% INFO    : Updated Thermal Rating: on line 31-32 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 32
% WARNING : Missing data for branch flow stat model on line 32-33 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0392 x=0.036
% INFO    : Updated Thermal Rating: on line 32-33 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 551
% WARNING : Missing data for branch flow stat model on line 34-32 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0 x=0.953
% INFO    : Updated Thermal Rating: on transformer 34-32 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 30
% WARNING : Missing data for branch flow stat model on line 34-35 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.052 x=0.078
% INFO    : Updated Thermal Rating: on line 34-35 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 312
% WARNING : Missing data for branch flow stat model on line 35-36 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.043 x=0.0537
% INFO    : Updated Thermal Rating: on line 35-36 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 426
% WARNING : Missing data for branch flow stat model on line 36-37 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.029 x=0.0366
% INFO    : Updated Thermal Rating: on line 36-37 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 628
% WARNING : Missing data for branch flow stat model on line 37-38 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0651 x=0.1009
% INFO    : Updated Thermal Rating: on line 37-38 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 244
% WARNING : Missing data for branch flow stat model on line 37-39 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0239 x=0.0379
% INFO    : Updated Thermal Rating: on line 37-39 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 654
% WARNING : Missing data for branch flow stat model on line 36-40 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.03 x=0.0466
% INFO    : Updated Thermal Rating: on line 36-40 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 529
% WARNING : Missing data for branch flow stat model on line 22-38 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0192 x=0.0295
% INFO    : Updated Thermal Rating: on line 22-38 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 833
% WARNING : Missing data for branch flow stat model on line 11-41 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0 x=0.749
% INFO    : Updated Thermal Rating: on transformer 11-41 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 39
% WARNING : Missing data for branch flow stat model on line 41-42 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.207 x=0.352
% INFO    : Updated Thermal Rating: on line 41-42 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 71
% WARNING : Missing data for branch flow stat model on line 41-43 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0 x=0.412
% INFO    : Updated Thermal Rating: on line 41-43 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 71
% WARNING : Missing data for branch flow stat model on line 38-44 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0289 x=0.0585
% INFO    : Updated Thermal Rating: on line 38-44 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 449
% WARNING : Missing data for branch flow stat model on line 15-45 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0 x=0.1042
% INFO    : Updated Thermal Rating: on transformer 15-45 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 281
% WARNING : Missing data for branch flow stat model on line 14-46 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0 x=0.0735
% INFO    : Updated Thermal Rating: on transformer 14-46 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 399
% WARNING : Missing data for branch flow stat model on line 46-47 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.023 x=0.068
% INFO    : Updated Thermal Rating: on line 46-47 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 408
% WARNING : Missing data for branch flow stat model on line 47-48 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0182 x=0.0233
% INFO    : Updated Thermal Rating: on line 47-48 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 992
% WARNING : Missing data for branch flow stat model on line 48-49 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0834 x=0.129
% INFO    : Updated Thermal Rating: on line 48-49 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 190
% WARNING : Missing data for branch flow stat model on line 49-50 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0801 x=0.128
% INFO    : Updated Thermal Rating: on line 49-50 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 194
% WARNING : Missing data for branch flow stat model on line 50-51 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.1386 x=0.22
% INFO    : Updated Thermal Rating: on line 50-51 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 112
% WARNING : Missing data for branch flow stat model on line 10-51 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0 x=0.0712
% INFO    : Updated Thermal Rating: on transformer 10-51 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 411
% WARNING : Missing data for branch flow stat model on line 13-49 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0 x=0.191
% INFO    : Updated Thermal Rating: on transformer 13-49 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 153
% WARNING : Missing data for branch flow stat model on line 29-52 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.1442 x=0.187
% INFO    : Updated Thermal Rating: on line 29-52 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 124
% WARNING : Missing data for branch flow stat model on line 52-53 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0762 x=0.0984
% INFO    : Updated Thermal Rating: on line 52-53 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 235
% WARNING : Missing data for branch flow stat model on line 53-54 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.1878 x=0.232
% INFO    : Updated Thermal Rating: on line 53-54 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 98
% WARNING : Missing data for branch flow stat model on line 54-55 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.1732 x=0.2265
% INFO    : Updated Thermal Rating: on line 54-55 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 102
% WARNING : Missing data for branch flow stat model on line 11-43 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0 x=0.153
% INFO    : Updated Thermal Rating: on transformer 11-43 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 191
% WARNING : Missing data for branch flow stat model on line 44-45 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0624 x=0.1242
% INFO    : Updated Thermal Rating: on line 44-45 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 211
% WARNING : Missing data for branch flow stat model on line 40-56 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0 x=1.195
% INFO    : Updated Thermal Rating: on transformer 40-56 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 24
% WARNING : Missing data for branch flow stat model on line 56-41 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.553 x=0.549
% INFO    : Updated Thermal Rating: on line 56-41 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 37
% WARNING : Missing data for branch flow stat model on line 56-42 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.2125 x=0.354
% INFO    : Updated Thermal Rating: on line 56-42 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 71
% WARNING : Missing data for branch flow stat model on line 39-57 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0 x=1.355
% INFO    : Updated Thermal Rating: on transformer 39-57 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 21
% WARNING : Missing data for branch flow stat model on line 57-56 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.174 x=0.26
% INFO    : Updated Thermal Rating: on line 57-56 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 93
% WARNING : Missing data for branch flow stat model on line 38-49 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.115 x=0.177
% INFO    : Updated Thermal Rating: on line 38-49 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 138
% WARNING : Missing data for branch flow stat model on line 38-48 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0312 x=0.0482
% INFO    : Updated Thermal Rating: on line 38-48 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 510
% WARNING : Missing data for branch flow stat model on line 9-55 using max current model : from_basekv=0.0 to_basekv=0.0 r=0.0 x=0.1205
% INFO    : Updated Thermal Rating: on transformer 9-55 : Rate A, Rate B, Rate C , 9900.0, 0.0, 0.0 -> 243
% INFO    : 
% INFO    : === Writing Matpower Case File Notes ===
