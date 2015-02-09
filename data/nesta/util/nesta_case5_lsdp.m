%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%                                                                  %%%%%
%%%%      NICTA Energy System Test Case Archive (NESTA) - v0.1.5      %%%%%
%%%%              Optimal Power Flow - Typical Operation              %%%%%
%%%%                       30 - October - 2014                        %%%%%
%%%%                                                                  %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Power flow data for 5 bus, 2 generator case
%
%   Lesieutre, B.C. & Molzahn, D.K. & Borden, AR. & Demarco, C.L., 
%   "Examining the Limits of the Application of Semidefinite Programming to Power Flow Problems",
%   49th Annual Allerton Conference on Communication, Control, and Computing (Allerton),  
%   September, 2011, pp. 1492-1499
%
%   This was studied as a load flow case.  There are 10 solutions to the load flow.
%   This is the only one within the voltage bounds listed here.
%
%    Bus      Voltage          Generation             Load        
%     #   Mag(pu) Ang(deg)   P (MW)   Q (MVAr)   P (MW)   Q (MVAr)
%   ----- ------- --------  --------  --------  --------  --------
%       1  1.000   -2.068     40.00    -41.05     20.00     10.00 
%       2  0.981   -4.536       -         -       45.00     15.00 
%       3  0.977   -4.853       -         -       40.00      5.00 
%       4  0.966   -5.693       -         -       60.00     10.00 
%       5  1.060    0.000*   131.27     99.85       -         -   
%                           --------  --------  --------  --------
%                  Total:    171.27     58.80    165.00     40.00
%
function mpc = nesta_case5_lsdp
mpc.version = '2';
mpc.baseMVA = 100.0;

%% bus data
%	bus_i	type	Pd	Qd	Gs	Bs	area	Vm	Va	baseKV	zone	Vmax	Vmin
mpc.bus = [
	1	 2	 20.0	 10.0	 0.0	 0.0	 1	     1.0000	     0.0000	 240.0	 1	     1.1000	     0.9000;
	2	 1	 45.0	 15.0	 0.0	 0.0	 1	     1.0000	     0.0000	 240.0	 1	     1.1000	     0.9000;
	3	 1	 40.0	 5.0	 0.0	 0.0	 1	     1.0000	     0.0000	 240.0	 1	     1.1000	     0.9000;
	4	 1	 60.0	 10.0	 0.0	 0.0	 1	     1.0000	     0.0000	 240.0	 1	     1.1000	     0.9000;
	5	 3	 0.0	 0.0	 0.0	 0.0	 1	     1.0600	     0.0000	 240.0	 1	     1.1000	     0.9000;
];

%% generator data
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf
mpc.gen = [
	1	 40.0	 0.0	 1000.0	 -1000.0	 1.0	 100.0	 1	 1000.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0;
	5	 0.0	 0.0	 1000.0	 -1000.0	 1.06	 100.0	 1	 1000.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0;
];

%% generator cost data
%	2	startup	shutdown	n	c(n-1)	...	c0
mpc.gencost = [
	2	 0.0	 0.0	 3	   0.110000	   5.000000	   0.000000;
	2	 0.0	 0.0	 3	   0.110000	   5.000000	   0.000000;
];

%% branch data
%	fbus	tbus	r	x	b	rateA	rateB	rateC	ratio	angle	status	angmin	angmax
mpc.branch = [
	1	 2	 0.06	 0.18	 0.0	 9000.0	 0.0	 0.0	 0.0	 0.0	 1	 -30.0	 30.0;
	1	 3	 0.06	 0.18	 0.0	 9000.0	 0.0	 0.0	 0.0	 0.0	 1	 -30.0	 30.0;
	1	 4	 0.04	 0.12	 0.0	 9000.0	 0.0	 0.0	 0.0	 0.0	 1	 -30.0	 30.0;
	1	 5	 0.02	 0.06	 0.0	 9000.0	 0.0	 0.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 3	 0.01	 0.03	 0.0	 9000.0	 0.0	 0.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 5	 0.08	 0.24	 0.0	 9000.0	 0.0	 0.0	 0.0	 0.0	 1	 -30.0	 30.0;
	3	 4	 0.08	 0.24	 0.0	 9000.0	 0.0	 0.0	 0.0	 0.0	 1	 -30.0	 30.0;
];

% INFO    : === Translation Options ===
% INFO    : Phase Angle Bound:           30.0 (deg.)
% INFO    : 
% INFO    : === Writing Matpower Case File Notes ===
