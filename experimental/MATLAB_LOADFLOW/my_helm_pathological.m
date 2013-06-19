% Bus Data
%	ID	TYPE	P	Q	M	theta	gs	bs	IcR	IcI
busdata = [ ...
	1	1	0.2	0	NaN	NaN	0	0	0	0
	2	2	0.2	NaN	1	NaN	0	0	0	0
	3	2	0.2	NaN	1	NaN	0	0	0	0
	4	3	NaN	NaN	1	0	0	0	0	0
];

% Branch Data
%	FROM	TO	g	b
branchdata = [ ...
	1	2	18	-1
	2	3	18	-1
	3	4	18	-1
];
