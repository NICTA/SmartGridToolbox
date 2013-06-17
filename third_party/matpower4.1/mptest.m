function mpc = mptest
mpc.version = '2';
mpc.baseMVA = 100;
% bus data
% bus_i		Bus number, > 0
% bus_type	Bus type, 1 = PQ, 2 = PV, 3 = swing, 4 = isolated
% Pd		Real power demand
% Qd		Reactive power demand
% Gs		Shunt conductance
% Bs		Shunt susceptance
% area		Area number (?)
% Vm		Voltage magnitude
% Va		Voltage angle (deg)
% baseKV	Base voltage 
%	bus_i	type	Pd	Qd	Gs	Bs	area	Vm	Va	baseKV	zone	Vmax	Vmin
mpc.bus = [
	1	3	0	0	0	0	1	1	0	240	1	100	100;
	2	1	1	0	0	0	1	1	0	240	1	100	100;	
];

% generator data
% bus		Bus number
% Pg		Real power output
% Qg		Reactive power output
% Qmax		Max reactive power
% Qmin		Min reactive power
% Vg		Voltage magnitude setpoint
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf
mpc.gen = [
	1	0	0	100	-100	1	100	1	1000	0	0	0	0	0	0	0	0	0;
];

% branch data
% fbus		From bus
% tbus		To bus
% r		Resistance
% x		Reactance	
% b		Reactance	
%	fbus	tbus	r	x	b	rateA	rateB	rateC	ratio	angle	status	angmin	angmax
mpc.branch = [
	1	2	0.01008	0	0.0	250	0	0	0	0	1	-360	360;
];
