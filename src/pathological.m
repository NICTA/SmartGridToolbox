function mpc = pathological
%CASE2737SOP    Power flow data for Polish system - summer 2004 off-peak.
%   Please see CASEFORMAT for details on the case file format.
%
%   This case represents the Polish 400, 220 and 110 kV networks during
%   summer 2004 off-peak conditions. Multiple centrally dispatchable
%   generators at a bus have not been aggregated. Generators that are
%   not centrally dispatchable in the Polish energy market are given a
%   cost of zero.
%
%   This data was graciously provided by, and is distributed with the
%   permission of, Roman Korab <roman.korab@polsl.pl>.

%   MATPOWER
%   $Id: case2737sop.m,v 1.5 2010/03/10 18:08:13 ray Exp $

%% MATPOWER Case Format : Version 2
mpc.version = '2';

%%-----  Power Flow Data  -----%%
%% system MVA base
mpc.baseMVA = 100;

%% bus data
%	bus_i	type	Pd	Qd	Gs	Bs	area	Vm	Va	baseKV	zone	Vmax	Vmin	lam_P	lam_Q	mu_Vmax	mu_Vmin
mpc.bus = [
	26	2	48.178	65.087	0	0	1	1.1082562	3.5097454	220	1	1.11	0.95	89.3018	0.0000	0.0000	0.0000;
	28	3	0	0	0	0	1	1.0449384	1.77808	400	1	1.05	0.95	90.2156	-0.0851	0.0000	0.0000;
	29	2	44.698	72	0	0	1	1.0449301	1.7863243	400	1	1.05	0.95	90.2126	-0.0850	0.0000	0.0000;
];

%% generator data
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf	mu_Pmax	mu_Pmin	mu_Qmax	mu_Qmin
mpc.gen = [
	26	370	2.61068	190	-19	1.08527	415.9	1	370	290	0	0	0	0	0	0	0	0	0	0	0	13.2018	0.0000	0.0000	0.0000;
	28	370	-19	190	-19	1.027	415.9	1	370	290	0	0	0	0	0	0	0	0	0	0	0	14.1156	0.0000	0.0000	0.0851;
	28	0	0	190	-19	1.027	415.9	0	370	290	0	0	0	0	0	0	0	0	0	0	0	0.0000	0.0000	0.0000	0.0000;
];

%% branch data
%	fbus	tbus	r	x	b	rateA	rateB	rateC	ratio	angle	status	angmin	angmax	Pf	Qf	Pt	Qt	mu_Sf	mu_St	mu_angmin	mu_angmax
mpc.branch = [
	26	28	0.00036	0.02519	0	500	500	500	1.0911	0	1	-360	360	125.6517	-117.4898	-125.5484	124.7145	0.0000	0.0000	0.0000	0.0000;
	29	28	6e-06	6.3e-05	0	1593	1593	1593	0	0	1	-360	360	247.5049	-38.7257	-247.5013	38.7617	0.0000	0.0000	0.0000	0.0000;
];

%% generator cost data
%	1	startup	shutdown	n	x1	y1	...	xn	yn
%	2	startup	shutdown	n	c(n-1)	...	c0
mpc.gencost = [
	2	0	0	3	0	112.78	0;
	2	0	0	3	0	112.78	0;
	2	0	0	3	0	102.05	0;
];
