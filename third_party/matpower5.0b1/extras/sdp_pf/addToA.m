function [A] = addToA(sdpmat, Wref_dd, Wref_qq, Wref_dq, matidx_dd, matidx_qq, matidx_dq, A, svar, maxclique)
%ADDTOA Adds a term to the decomposed A matrices
%   [A] = ADDTOA(SDPMAT, WREF_DD, WREF_QQ, WREF_DQ, MATIDX_DD, MATIDX_QQ, MATIDX_DQ, A, SDPVAR, MAXCLIQUE)
%
%   Adds a matrix multiplied by a sdp variable to the A matrices.
%
%   Inputs:
%       SDPMAT : A 2*nbus by 2*nbus matrix.
%       WREF_DD : Matrix with three columns. The first column is a 
%           numbering 1:size(Wref_dd,1). The second and third columns 
%           indicate the row and column indices of the elements of the 
%           matrix sdpmat, with the row of Wref_dd corresponding to the 
%           index of matidx_dd. That is, the element of sdpmat located in 
%           row Wref_dd(i,1), column Wref_dd(i,2) corresponds to 
%           matidx_dd(i).
%       WREF_QQ : Similar to Wref_dd, except for the qq entries of sdpmat.
%       WREF_DQ : Similar to Wref_dd, except for the dq entries of sdpmat.
%       MATIDX_DD : Matrix with three columns. Row i of matidx_dd indicates
%           the location of sdpmat(Wref_dd(i,1), Wref_dd(i,2)). The first
%           column indicates the index of the corresponding matrix. The
%           second and third columns indicate the row and column,
%           respectively, of the corresponding matrix. 
%       MATIDX_QQ : Similar to matidx_dd, except corresponding to the qq 
%           entries of sdpmat.
%       MATIDX_DQ : Similar to matidx_dd, except corresponding to the dq 
%           entries of sdpmat.
%       A : Cell array of decomposed A matrices corresponding to maxclique.
%       SVAR : SDP variable (sdpvar) that will be multiplied by sdpmat and
%           added to the decomposed A matrices.
%       MAXCLIQUE : Cell array of the buses in each maximal clique.
%
%   Outputs:
%       A : Cell array of decomposed A matrices with addition of
%       svar*sdpmat.

%   MATPOWER
%   $Id: addToA.m 2272 2014-01-17 14:15:47Z ray $
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

[Ykvec] = mat2vec(sdpmat, Wref_dd, Wref_qq, Wref_dq, matidx_dd, matidx_qq, matidx_dq);

Amats = unique(Ykvec(:,1));
for m=1:length(Amats)
    Ykvec_rows = find(Ykvec(:,1) == Amats(m));
    A{Amats(m)} = A{Amats(m)} + svar * sparse(Ykvec(Ykvec_rows,2),Ykvec(Ykvec_rows,3),Ykvec(Ykvec_rows,4),2*length(maxclique{Amats(m)}),2*length(maxclique{Amats(m)}));
end
