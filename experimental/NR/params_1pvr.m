% TYPE : 0 = slack, 1 = PQ, 2 = PV
%       NBUS    TYPE    P       Q       M       t
bus = ...
[
        1       2       0.01    NA      1.01    NA
        2       0       NA      NA      1       0
];

%       i       j       g       b
branch = ...
[
        1       2       0.7     0
];
