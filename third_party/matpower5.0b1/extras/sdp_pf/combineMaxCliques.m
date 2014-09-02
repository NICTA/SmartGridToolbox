function [maxcliques,E]=combineMaxCliques(maxcliques,E,maxNumberOfCliques,ndisplay)
%COMBINEMAXCLIQUES Combine the maximal cliques.
%   [MAXCLIQUES,E] = COMBINEMAXCLIQUES(MAXCLIQUES,E,MAXNUMBEROFCLIQUES,NDISPLAY)
%
%   Combine the maximal cliques to reduce computation time. This function
%   uses the heuristic that an additional variable in a matrix is
%   equivalent to an additional linking constraint between overlapping 
%   maximal cliques. See [1] for more details.
%   
%   Inputs:
%       MAXCLIQUES : Cell array containing the buses contained in each
%           maximal clique.
%       E : Matrix with two columns representing the branches of the
%           maximum weight clique tree formed from the maximal cliques.
%           Values of E correspond to indices of maxcliques.
%       MAXNUMBEROFCLIQUES : The maximum number of cliques (stopping
%           criterion for this combineMaxCliques). If maxNumberOfCliques
%           is in (0,1), assume maxNumberOfCliques is a fraction of the 
%           original number of maxcliquess. If maxNumberOfCliques is 0, do 
%           not combine maxcliquess (no maximum). If maxNumberOfCliques is
%           greater than one, cap the number of maxcliques at the specified
%           value of maxNumberOfCliques.
%       NDISPLAY : (Optional) Frequency of displaying progress updates.
%
%   Outputs:
%       MAXCLIQUES : Cell array containing the buses contained in each
%           maximal clique after combining maximal cliques.
%       E : Matrix with two columns representing the branches of the
%           maximum weight clique tree formed from the maximal cliques
%           after combining maximal cliques. Values of E correspond to 
%           indices of maxcliques.
%
%   [1] D.K. Molzahn, J.T. Holzer, B.C. Lesieutre, and C.L. DeMarco,
%       "Implementation of a Large-Scale Optimal Power Flow Solver Based on
%       Semidefinite Programming," IEEE Transactions on Power Systems,
%       vol. 28, no. 4, pp. 3987-3998, November 2013.

%   MATPOWER
%   $Id: combineMaxCliques.m 2280 2014-01-17 23:28:37Z ray $
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

%% Setup

if nargin < 4
    ndisplay = inf;
end

% Intrepret maxNumberOfCliques
if maxNumberOfCliques == 0
    maxNumberOfCliques = inf;
elseif maxNumberOfCliques < 1
    maxNumberOfCliques = max(round(length(maxcliques)*maxNumberOfCliques),1);
elseif maxNumberOfCliques < 0
    error('combineMaxCliques: Invalid maxNumberOfCliques. Must be non-negative.');
else
    % don't change maxNumberOfCliques, the given value is a specified
    % number of maxcliques.
end
   

%% Combine maximal cliques
% Simultaneously update the clique tree.

if length(maxcliques) > maxNumberOfCliques
    
    % Make sure that E has two columns
    if size(E,2) < 2
        E = E(:).';
    end
    E = [E nan(length(E(:,1)),1)];
    
    % Calculate a cost in terms of number of variables and constraints for
    % combining the maximal cliques for each branch of the clique tree.
    for i=1:size(E,1)
        maxcliquesidx = [E(i,1) E(i,2)];
        E(i,3) = combineCost(maxcliques,maxcliquesidx);
    end
    
    while length(maxcliques) > maxNumberOfCliques

        % Find the best pair of maximal cliques to combine
        [junk,combine_idx] = min(E(:,3)); 
        maxcliquesidx = [E(combine_idx,1) E(combine_idx,2)];
        
        % Combine these maximal cliques
        maxcliques{E(combine_idx,1)} = unique([maxcliques{E(combine_idx,1)}(:); maxcliques{E(combine_idx,2)}(:)]);
        maxcliques = maxcliques(setdiff(1:length(maxcliques),E(combine_idx,2)));
        
        % Update references in E to removed maximal clique
        E12 = E(:,1:2);
        Eref_removed = find(E12 == E(combine_idx,2));
        E12(Eref_removed) = E(combine_idx,1);
        E(:,1:2) = E12;
        E = E(setdiff(1:size(E,1),combine_idx),:);
        
        % Any maximal cliques that have indices greater than the removed 
        % E(combine_idx,2) must have their references corrected.
        E12 = E(:,1:2);
        Eref_update = find(E12 > maxcliquesidx(2));
        E12(Eref_update) = E12(Eref_update) - 1;
        E(:,1:2) = E12;
        
        if maxcliquesidx(2) < maxcliquesidx(1)
            maxcliquesidx(1) = maxcliquesidx(1) - 1;
        end

        % Update E costs
        % We need to update the third col of E for any row of E that has a
        % reference to the combined maximal cliques
        for i=1:size(E,1)
            if E(i,1) == maxcliquesidx(1) || E(i,2) == maxcliquesidx(1)
                maxcliquesidx_recalc = [E(i,1) E(i,2)];
                E(i,3) = combineCost(maxcliques,maxcliquesidx_recalc);
            end
        end
        
        if mod(size(E,1),ndisplay) == 0
            fprintf('combineLoops: %i maximal cliques of maxNumberOfCliques = %i\n',size(E,1),maxNumberOfCliques);
        end

    end
else
    % No need to combine maximal cliques, the inital set is less than the
    % maximum number of maximal cliquess
end
