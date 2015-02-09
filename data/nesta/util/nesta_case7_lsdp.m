%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%                                                                  %%%%%
%%%%      NICTA Energy System Test Case Archive (NESTA) - v0.1.5      %%%%%
%%%%              Optimal Power Flow - Typical Operation              %%%%%
%%%%                       30 - October - 2014                        %%%%%
%%%%                                                                  %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Power flow data for 7 bus, 1 generator case
%
%   Lesieutre, B.C. & Molzahn, D.K. & Borden, AR. & Demarco, C.L., 
%   "Examining the Limits of the Application of Semidefinite Programming to Power Flow Problems",
%   49th Annual Allerton Conference on Communication, Control, and Computing (Allerton),  
%   September, 2011, pp. 1492-1499
%
%   This was studied as a load flow case.  There are 4 solutions to the load flow.
%   This is the only one within the voltage bounds listed here.
%
%    Bus      Voltage          Generation             Load        
%     #   Mag(pu) Ang(deg)   P (MW)   Q (MVAr)   P (MW)   Q (MVAr)
%   ----- ------- --------  --------  --------  --------  --------
%       1  1.077    5.275       -         -      -90.00    -30.00 
%       2  0.965   -2.928       -         -       47.80      3.90
%       3  0.905   -8.424       -         -       94.20     19.00 
%       4  0.929   -5.737       -         -       13.50      5.80 
%       5  0.965   -2.441       -         -       18.30     12.70 
%       6  0.969   -2.586       -         -        7.60      1.60 
%       7  1.001    0.000*   103.44     46.19       -         -   
%                           --------  --------  --------  --------
%                  Total:    103.44     46.19     91.40     13.00 
%
%   Note that the network diagram in the original paper is incorrect.
%   The correct diagram can be found in this revision,
%   http://www-personal.umich.edu/~molzahn/pubs/lesieutre_molzahn_borden_demarco-allerton2011.pdf
%   Accessed on July, 2014
%

function mpc = nesta_case7_lsdp
mpc.version = '2';
mpc.baseMVA = 100.0;

%% bus data
%	bus_i	type	Pd	Qd	Gs	Bs	area	Vm	Va	baseKV	zone	Vmax	Vmin
mpc.bus = [
	1	 1	 -90.0	 -30.0	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     5.0000	     0.0000;
	2	 1	 47.8	 3.9	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     5.0000	     0.0000;
	3	 1	 94.2	 19.0	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     5.0000	     0.0000;
	4	 1	 13.5	 5.8	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     5.0000	     0.0000;
	5	 1	 18.3	 12.7	 0.0	 0.0	 1	     1.0600	     0.0000	 230.0	 1	     5.0000	     0.0000;
	6	 1	 7.6	 1.6	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     5.0000	     0.0000;
	7	 3	 0.0	 0.0	 0.0	 0.0	 1	     1.0000	     0.0000	 230.0	 1	     1.0010	     0.9990;
];

%% generator data
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf
mpc.gen = [
	7	 0.0	 0.0	 100.0	 -100.0	 1.0	 1.0	 1	 500.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0	 0.0;
];

%% generator cost data
%	2	startup	shutdown	n	c(n-1)	...	c0
mpc.gencost = [
	2	 0.0	 0.0	 3	   0.000000	   1.000000	   0.000000;
];

%% branch data
%	fbus	tbus	r	x	b	rateA	rateB	rateC	ratio	angle	status	angmin	angmax
mpc.branch = [
	1	 2	 0.082	 0.192	 0.0	 250.0	 250.0	 250.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 3	 0.067	 0.171	 0.0	 250.0	 250.0	 250.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 5	 0.058	 0.176	 0.0	 250.0	 250.0	 250.0	 0.0	 0.0	 1	 -30.0	 30.0;
	2	 6	 0.013	 0.042	 0.0	 250.0	 250.0	 250.0	 0.0	 0.0	 1	 -30.0	 30.0;
	3	 4	 0.024	 0.1	 0.0	 250.0	 250.0	 250.0	 0.0	 0.0	 1	 -30.0	 30.0;
	4	 5	 0.024	 0.1	 0.0	 250.0	 250.0	 250.0	 0.0	 0.0	 1	 -30.0	 30.0;
	5	 6	 0.057	 0.174	 0.0	 250.0	 250.0	 250.0	 0.0	 0.0	 1	 -30.0	 30.0;
	5	 7	 0.019	 0.059	 0.0	 250.0	 250.0	 250.0	 0.0	 0.0	 1	 -30.0	 30.0;
	6	 7	 0.054	 0.223	 0.0	 250.0	 250.0	 250.0	 0.0	 0.0	 1	 -30.0	 30.0;
];

% INFO    : === Translation Options ===
% INFO    : Phase Angle Bound:           30.0 (deg.)
% INFO    : 
% INFO    : === Writing Matpower Case File Notes ===
