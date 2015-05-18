%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%                                                                  %%%%%
%%%%      NICTA Energy System Test Case Archive (NESTA) - v0.3.0      %%%%%
%%%%              Optimal Power Flow - Typical Operation              %%%%%
%%%%                        30 - April - 2015                         %%%%%
%%%%                                                                  %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Power flow data for 6 bus, 3 gen case from Wood & Wollenberg.
%
%   This is the 6 bus example from pp. 104, 112, 119, 123-124, 549 of
%   "Power Generation, Operation, and Control, 2nd Edition",
%   by Allen. J. Wood and Bruce F. Wollenberg, John Wiley & Sons, NY, Jan 1996.
%
function mpc = nesta_case6_ww
mpc.version = '2';
mpc.baseMVA = 100.0;

%% bus data
%	bus_i	type	Pd	Qd	Gs	Bs	area	Vm	Va	baseKV	zone	Vmax	Vmin
mpc.bus = [
	1	 3	 0.0	 0.0	 0.0	 0.0	 1	    1.05000	    0.00000	 230.0	 1	    1.05000	    1.05000;
	2	 2	 0.0	 0.0	 0.0	 0.0	 1	    1.05000	   -1.98484	 230.0	 1	    1.05000	    1.05000;
	3	 2	 0.0	 0.0	 0.0	 0.0	 1	    1.07000	   -2.23700	 230.0	 1	    1.07000	    1.07000;
	4	 1	 70.0	 70.0	 0.0	 0.0	 1	    0.98820	   -3.06893	 230.0	 1	    1.05000	    0.95000;
	5	 1	 70.0	 70.0	 0.0	 0.0	 1	    0.98507	   -3.91966	 230.0	 1	    1.05000	    0.95000;
	6	 1	 70.0	 70.0	 0.0	 0.0	 1	    1.00462	   -4.11825	 230.0	 1	    1.05000	    0.95000;
];

%% generator data
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf
mpc.gen = [
	1	 77.22	 25.715	 100.0	 -100.0	 1.05	 100.0	 1	 200.0	 50.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0;
	2	 69.268	 64.65	 100.0	 -100.0	 1.05	 100.0	 1	 150.0	 37.5	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0;
	3	 70.421	 86.644	 100.0	 -100.0	 1.07	 100.0	 1	 180.0	 45.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0;
];

%% generator cost data
%	2	startup	shutdown	n	c(n-1)	...	c0
mpc.gencost = [
	2	 0.0	 0.0	 3	   0.005330	  11.669000	 213.100000;
	2	 0.0	 0.0	 3	   0.008890	  10.333000	 200.000000;
	2	 0.0	 0.0	 3	   0.007410	  10.833000	 240.000000;
];

%% branch data
%	fbus	tbus	r	x	b	rateA	rateB	rateC	ratio	angle	status	angmin	angmax
mpc.branch = [
	1	 2	 0.1	 0.2	 0.04	 40.0	 40.0	 40.0	 0.0	 0.0	 1	 -30.0	 30.0;
	1	 4	 0.05	 0.2	 0.04	 60.0	 60.0	 60.0	 0.0	 0.0	 1	 -30.0	 30.0;
	1	 5	 0.08	 0.3	 0.06	 40.0	 40.0	 40.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 3	 0.05	 0.25	 0.06	 40.0	 40.0	 40.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 4	 0.05	 0.1	 0.02	 60.0	 60.0	 60.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 5	 0.1	 0.3	 0.04	 30.0	 30.0	 30.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 6	 0.07	 0.2	 0.05	 90.0	 90.0	 90.0	 0.0	 0.0	 1	 -30.0	 30.0;
	3	 5	 0.12	 0.26	 0.05	 70.0	 70.0	 70.0	 0.0	 0.0	 1	 -30.0	 30.0;
	3	 6	 0.02	 0.1	 0.02	 80.0	 80.0	 80.0	 0.0	 0.0	 1	 -30.0	 30.0;
	4	 5	 0.2	 0.4	 0.08	 20.0	 20.0	 20.0	 0.0	 0.0	 1	 -30.0	 30.0;
	5	 6	 0.1	 0.3	 0.06	 40.0	 40.0	 40.0	 0.0	 0.0	 1	 -30.0	 30.0;
];

% INFO    : === Translation Options ===
% INFO    : Phase Angle Bound:           30.0 (deg.)
% INFO    : AC OPF Solution File:        nesta_case6_ww.dat.opf.sol
% INFO    : 
% INFO    : === Voltage Setpoint Replacement Notes ===
% INFO    : Bus 1	: V=1.05, theta=0.0 -> V=1.05, theta=0.0
% INFO    : Bus 2	: V=1.05, theta=0.0 -> V=1.05, theta=-1.98484
% INFO    : Bus 3	: V=1.07, theta=0.0 -> V=1.07, theta=-2.237
% INFO    : Bus 4	: V=1.0, theta=0.0 -> V=0.9882, theta=-3.06893
% INFO    : Bus 5	: V=1.0, theta=0.0 -> V=0.98507, theta=-3.91966
% INFO    : Bus 6	: V=1.0, theta=0.0 -> V=1.00462, theta=-4.11825
% INFO    : 
% INFO    : === Generator Setpoint Replacement Notes ===
% INFO    : Gen at bus 1	: Pg=0.0, Qg=0.0 -> Pg=77.22, Qg=25.715
% INFO    : Gen at bus 1	: Vg=1.05 -> Vg=1.05
% INFO    : Gen at bus 2	: Pg=50.0, Qg=0.0 -> Pg=69.268, Qg=64.65
% INFO    : Gen at bus 2	: Vg=1.05 -> Vg=1.05
% INFO    : Gen at bus 3	: Pg=60.0, Qg=0.0 -> Pg=70.421, Qg=86.644
% INFO    : Gen at bus 3	: Vg=1.07 -> Vg=1.07
% INFO    : 
% INFO    : === Writing Matpower Case File Notes ===
