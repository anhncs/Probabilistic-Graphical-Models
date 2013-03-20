%COMPUTEEXACTMARGINALSHMM Runs exact inference and returns the marginals
%over all the variables and the calibrated clique tree.

%   M = COMPUTEEXACTMARGINALSHMM(F) Takes a list of factors F,
%   and runs exact inference and returns the calibrated clique tree (unnormalized) and
%   final marginals (normalized) for the variables in the network.
%   It returns an array of size equal to the number of variables in the 
%   network where M(i) represents the ith variable and M(i).val represents 
%   the marginals of the ith variable. 


%
% Copyright (C) Daphne Koller, Stanford Univerity, 2012

function [M, PCalibrated] = ComputeExactMarginalsHMM(F)

% M = repmat(struct('var', 0, 'card', 0, 'val', []), length(N), 1);
%
% where N is the number of variables in the network, which can be determined
% from the factors F.

% Create a clique tree, compute the initial potentails, calibrate the
% clique tree, and find the belief for each varaible at a clique that has
% that variable in its scope

compressedCliqueTree = CreateCliqueTreeHMM(F);
PCalibrated = CliqueTreeCalibrate(compressedCliqueTree);
varsList = sort(unique([F(:).var]));
M = repmat(struct('var', 0, 'card', 0, 'val', []), length(varsList), 1);
for i = 1:length(varsList)
    assert (varsList(i) == i);
    if (i == 1)
        clique = PCalibrated.cliqueList(i);
        M(i) = FactorMarginalization(clique, 2);
    else
        clique = PCalibrated.cliqueList(i-1);
        M(i) = FactorMarginalization(clique, i-1);
    end
    
    if any(M(i).val ~= 0)
        % Normalize
        M(i).val = M(i).val - logsumexp(M(i).val);
    end

end

end
