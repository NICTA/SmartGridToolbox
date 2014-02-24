function mpc = case14
%CASE14    Power flow data for IEEE 14 bus test case.
%   Please see CASEFORMAT for details on the case file format.
%   This data was converted from IEEE Common Data Format
%   (ieee14cdf.txt) on 20-Sep-2004 by cdf2matp, rev. 1.11
%   See end of file for warnings generated during conversion.
%
%   Converted from IEEE CDF file from:
%       http://www.ee.washington.edu/research/pstca/
% 
%  08/19/93 UW ARCHIVE           100.0  1962 W IEEE 14 Bus Test Case

%   MATPOWER
%   $Id: case14.m,v 1.11 2010/03/10 18:08:15 ray Exp $

%% MATPOWER Case Format : Version 2
mpc.version = '2';

%%-----  Power Flow Data  -----%%
%% system MVA base
mpc.baseMVA = 100;

%% bus data
%	bus_i	type	Pd	Qd	Gs	Bs	area	Vm	Va	baseKV	zone	Vmax	Vmin
mpc.bus = [
	1	3	0	0	0	0	1	1.06	0	0	1	1.06	0.94;
	4	1	47.8	-3.9	0	0	1	1.019	-10.33	0	1	1.06	0.94;
	5	1	7.6	1.6	0	0	1	1.02	-8.78	0	1	1.06	0.94;
	7	1	0	0	0	0	1	1.062	-13.37	0	1	1.06	0.94;
	9	1	29.5	16.6	0	19	1	1.056	-14.94	0	1	1.06	0.94;
];

%% generator data
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf
mpc.gen = [
	1	232.4	-16.9	10	0	1.06	100	1	332.4	0	0	0	0	0	0	0	0	0	0	0	0;
];

%% branch data
%	fbus	tbus	r	x	b	rateA	rateB	rateC	ratio	angle	status	angmin	angmax
mpc.branch = [
	1	5	0.05403	0.22304	0	9900	0	0	0	0	1	-360	360;
	4	5	0.01335	0.04211	0	9900	0	0	0	0	1	-360	360;
	4	7	0	0.20912	0	9900	0	0	0	0	1	-360	360;
	4	9	0	0.55618	0	9900	0	0	0	0	1	-360	360;
	7	9	0	0.11001	0	9900	0	0	0	0	1	-360	360;
];

%%-----  OPF Data  -----%%
%% generator cost data
%	1	startup	shutdown	n	x1	y1	...	xn	yn
%	2	startup	shutdown	n	c(n-1)	...	c0
mpc.gencost = [
	2	0	0	3	0.0430293	20	0;
	2	0	0	3	0.25	20	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
];

% Warnings from cdf2matp conversion:
%
% ***** Qmax = Qmin at generator at bus    1 (Qmax set to Qmin + 10)
% ***** area data conversion not yet implemented (creating dummy area data)
% ***** MVA limit of branch 1 - 2 not given, set to 9900
% ***** MVA limit of branch 1 - 5 not given, set to 9900
% ***** MVA limit of branch 2 - 3 not given, set to 9900
% ***** MVA limit of branch 2 - 4 not given, set to 9900
% ***** MVA limit of branch 2 - 5 not given, set to 9900
% ***** MVA limit of branch 3 - 4 not given, set to 9900
% ***** MVA limit of branch 4 - 5 not given, set to 9900
% ***** MVA limit of branch 4 - 7 not given, set to 9900
% ***** MVA limit of branch 4 - 9 not given, set to 9900
% ***** MVA limit of branch 5 - 6 not given, set to 9900
% ***** MVA limit of branch 6 - 11 not given, set to 9900
% ***** MVA limit of branch 6 - 12 not given, set to 9900
% ***** MVA limit of branch 6 - 13 not given, set to 9900
% ***** MVA limit of branch 7 - 8 not given, set to 9900
% ***** MVA limit of branch 7 - 9 not given, set to 9900
% ***** MVA limit of branch 9 - 10 not given, set to 9900
% ***** MVA limit of branch 9 - 14 not given, set to 9900
% ***** MVA limit of branch 10 - 11 not given, set to 9900
% ***** MVA limit of branch 12 - 13 not given, set to 9900
% ***** MVA limit of branch 13 - 14 not given, set to 9900
