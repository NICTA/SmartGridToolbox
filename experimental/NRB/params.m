% TYPE : 0 = slack, 1 = PQ, 2 = PV
%	NBUS	TYPE	P	Q	M	t
busdata = ...
[
	1	1	-0.03	0.04	NA	NA
	2	2	0.035	NA	1.01	NA
	3	0	NA	NA	1	0
];


%	i	j	g	b
branchdata = ...
[
	1	2	0.3	0.02
	1	3	0.04	0.07
	2	3	0.2	0
];
