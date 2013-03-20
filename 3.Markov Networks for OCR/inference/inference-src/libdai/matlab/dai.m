% [logZ,q,md,qv,qf,qmap] = dai(psi,method,opts)
%
% INPUT:  psi        = linear cell array containing the factors
%                      (psi{i} should be a structure with a Member field
%                      and a P field).
%         method     = name of the method.
%         opts       = string of options.
% 
% OUTPUT: logZ       = approximation of the logarithm of the partition sum.
%         q          = linear cell array containing all final beliefs.
%         md         = maxdiff (final linf-dist between new and old single node beliefs).
%         qv         = linear cell array containing all variable beliefs.
%         qf         = linear cell array containing all factor beliefs.
%         qmap       = linear array containing the MAP state (only for BP,JTree).
