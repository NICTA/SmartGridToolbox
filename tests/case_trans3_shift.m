function mpc = caseSLPQ
%CASE4GS  Power flow data for 4 bus, 2 gen case from Grainger & Stevenson.
%   Please see CASEFORMAT for details on the case file format.
%
%   This is the 4 bus example from pp. 337-338 of "Power System Analysis",
%   by John Grainger, Jr., William Stevenson, McGraw-Hill, 1994.

%   MATPOWER
%   $Id: case4gs.m,v 1.4 2010/03/10 18:08:14 ray Exp $

%% MATPOWER Case Format : Version 2
mpc.version = '2';

%%-----  Power Flow Data  -----%%
%% system MVA base
mpc.baseMVA = 100;

%% bus data
%	bus_i	type	Pd	Qd	Gs	Bs	area	Vm	Va	baseKV	zone	Vmax	Vmin
mpc.bus = [
	1	3	50	30.99	0	0	1	1	0	230	1	10	0;
	2	1	30	20	0	0	1	0.5	0	230	1	10	0;
];

%% generator data
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf
mpc.gen = [
	1	0	0	100	-100	1	100	1	0	0	0	0	0	0	0	0	0	0	0	0	0;
];

%% branch data
%	fbus	tbus	r	x	b	rateA	rateB	rateC	ratio	angle	status	angmin	angmax
mpc.branch = [
	1	2	0.01008	0.0504	0.1025	250	250	250	2.0	30	1	-360	360;
];
