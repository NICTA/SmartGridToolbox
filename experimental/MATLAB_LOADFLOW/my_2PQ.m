% Bus Data
%	ID	TYPE	P	Q	M	theta	gs	bs	IcR	IcI
busdata = [ ...
	1   	1   	0.0240	0.0120	NaN	NaN	0.0000	0.0000	0.0000	0.0000
	2   	1   	0.0840	-0.0520	NaN	NaN	0.0000	0.0000	0.0000	0.0000
	3   	3   	NaN	NaN	1.0000	0.0000	0.0000	0.0000	0.0000	0.0000
];

% Branch Data
%	FROM	TO	g	b
branchdata = [ ...
	1   	2   	5.0000	-15.0000
	1   	3   	3.0000	-9.0000
];
