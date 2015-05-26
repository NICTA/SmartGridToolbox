function mpc = simulation
mpc.version = '2';

%%-----  Power Flow Data  -----%%
%% system MVA base
mpc.baseMVA = 1;

%% bus data
%	bus_i	type	Pd	Qd	Gs	Bs	area	Vm	Va	baseKV	zone	Vmax	Vmin
mpc.bus = [
	1	2	0	0	0	0	1	1	0	11	1	1.1	0.9;
	2	1	0.3	0.01	0	0	1	1	0	11	1	1.1	0.9;
	3	2	0.3	0.01	0	0	1	1	0	11	1	1.1	0.9;
	4	3	0.4	0.013	0	0	1	1	0	11	1	1.1	0.9;
	5	2	0	0	0	0	1	1	0	11	1	1.1	0.9;
];

%% generator data
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf
mpc.gen = [
	1	0.04	0	0.03	-0.03	1	1	1	0.04	0	0	0	0	0	0	0	0	0	0	0	0;
	1	0.17	0	0.13	-0.13	1	1	1	0.17	0	0	0	0	0	0	0	0	0	0	0	0;
	3	0.3	0	0.4	-0.4	1	1	1	0.5	0	0	0	0	0	0	0	0	0	0	0	0;
	4	0	0	0.15	-0.15	1	1	1	0.2	0	0	0	0	0	0	0	0	0	0	0	0;
	5	0.5	0	0.45	-0.45	1	1	1	0.6	0	0	0	0	0	0	0	0	0	0	0	0;
];

%% branch data
%	fbus	tbus	r	x	b	rateA	rateB	rateC	ratio	angle	status	angmin	angmax
mpc.branch = [
	1	2	0.0281	0.0281	0	0.4	0.24	0.4	0	0	1	-360	360;
	1	4	0.0304	0.0304	0	0	0	0	0	0	1	-360	360;
	1	5	0.0064	0.0064	0	0	0	0	0	0	1	-360	360;
	2	3	0.0108	0.0108	0	0	0	0	0	0	1	-360	360;
	3	4	0.0297	0.0297	0	0	0	0	0	0	1	-360	360;
	4	5	0.0297	0.0297	0	0.24	0.24	0.24	0	0	1	-360	360;
];

%%-----  OPF Data  -----%%
%% generator cost data
%	1	startup	shutdown	n	x1	y1	...	xn	yn
%	2	startup	shutdown	n	c(n-1)	...	c0
mpc.gencost = [
	2	0	0	2	14	0;
	2	0	0	2	15	0;
	2	0	0	2	30	0;
	2	0	0	2	40	0;
	2	0	0	2	10	0;
];
