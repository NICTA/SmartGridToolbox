function [cost] = combineCost(maxcliques,maxcliquesidx)
%COMBINECOST Calculate the cost of combining two maximal cliques.
%   [COST] = COMBINECOST(MAXCLIQUES,MAXCLIQUEIDX)
%
%   Calculate the cost of combining two maximal cliques in terms of the 
%   number of scalar variables and linking constraints that will be 
%   required after combining the maximal cliques specified in
%   maxcliquesidx. This is the clique combination heuristic described in
%   [1]. Negative costs indicate that the heuristic predicts
%   decreased computational costs after combining the specified maximal
%   cliques.
%
%   Inputs:
%       MAXCLIQUES : Cell array containing the buses contained in each
%           maximal clique.
%       MAXCLIQUESIDX : Vector of length two with elements corresponding to
%           the candidate maximal cliques.
%
%   Outputs:
%       COST : Scalar indicating the cost, as defined by the heuristic in
%       [1] of combining the specified maximal cliques.
%
%   [1] D.K. Molzahn, J.T. Holzer, B.C. Lesieutre, and C.L. DeMarco,
%       "Implementation of a Large-Scale Optimal Power Flow Solver Based on
%       Semidefinite Programming," IEEE Transactions on Power Systems,
%       vol. 28, no. 4, pp. 3987-3998, November 2013.

%   MATPOWER
%   $Id: combineCost.m 2280 2014-01-17 23:28:37Z ray $
%   by Daniel Molzahn, PSERC U of Wisc, Madison
%   Copyright (c) 2013-2014 by Power System Engineering Research Center (PSERC)
%
%   This file is part of MATPOWER.
%   See http://www.pserc.cornell.edu/matpower/ for more info.
%
%   MATPOWER is free software: you can redistribute it and/or modify
%   it under the terms of the GNU General Public License as published
%   by the Free Software Foundation, either version 3 of the License,
%   or (at your option) any later version.
%
%   MATPOWER is distributed in the hope that it will be useful,
%   but WITHOUT ANY WARRANTY; without even the implied warranty of
%   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
%   GNU General Public License for more details.
%
%   You should have received a copy of the GNU General Public License
%   along with MATPOWER. If not, see <http://www.gnu.org/licenses/>.
%
%   Additional permission under GNU GPL version 3 section 7
%
%   If you modify MATPOWER, or any covered work, to interface with
%   other modules (such as MATLAB code and MEX-files) available in a
%   MATLAB(R) or comparable environment containing parts covered
%   under other licensing terms, the licensors of MATPOWER grant
%   you additional permission to convey the resulting work.

maxcliques1 = maxcliques{maxcliquesidx(1)};
maxcliques2 = maxcliques{maxcliquesidx(2)};
nintersect = sum(ismembc(maxcliques1, maxcliques2));

elimmaxcliques(1) = length(maxcliques1);
elimmaxcliques(2) = length(maxcliques2);
lnewmaxcliques = sum(elimmaxcliques) - nintersect;

nvarafter = (lnewmaxcliques)*(2*lnewmaxcliques+1) - sum((elimmaxcliques).*(2*elimmaxcliques+1));

ocostbefore = (nintersect)*(2*nintersect+1);

cost = nvarafter - ocostbefore;
