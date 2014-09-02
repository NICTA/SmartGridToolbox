function rv = sdp_pf_ver(varargin)
%SPD_PF_VER  Prints or returns SDP_PF version info for current installation.
%   V = SDP_PF_VER returns the current SDP_PF version number.
%   V = SDP_PF_VER('all') returns a struct with the fields Name, Version,
%   Release and Date (all strings). Calling SDP_PF_VER without assigning the
%   return value prints the version and release date of the current
%   installation of SDP_PF.
%
%   See also MPVER.

%   MATPOWER
%   $Id: sdp_pf_ver.m 2280 2014-01-17 23:28:37Z ray $
%   by Ray Zimmerman, PSERC Cornell
%   Copyright (c) 2014 by Power System Engineering Research Center (PSERC)
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

v = struct( 'Name',     'SDP_PF', ... 
            'Version',  '1.0', ...
            'Release',  '', ...
            'Date',     '17-Jan-2014' );
if nargout > 0
    if nargin > 0
        rv = v;
    else
        rv = v.Version;
    end
else
    fprintf('%-22s Version %-9s  %11s\n', v.Name, v.Version, v.Date);
end
