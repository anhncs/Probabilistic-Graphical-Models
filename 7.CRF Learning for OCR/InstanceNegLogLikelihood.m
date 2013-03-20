% function [nll, grad] = InstanceNegLogLikelihood(X, y, theta, modelParams)
% returns the negative log-likelihood and its gradient, given a CRF with parameters theta,
% on data (X, y). 
%
% Inputs:
% X            Data.                           (numCharacters x numImageFeatures matrix)
%              X(:,1) is all ones, i.e., it encodes the intercept/bias term.
% y            Data labels.                    (numCharacters x 1 vector)
% theta        CRF weights/parameters.         (numParams x 1 vector)
%              These are shared among the various singleton / pairwise features.
% modelParams  Struct with three fields:
%   .numHiddenStates     in our case, set to 26 (26 possible characters)
%   .numObservedStates   in our case, set to 2  (each pixel is either on or off)
%   .lambda              the regularization parameter lambda
%
% Outputs:
% nll          Negative log-likelihood of the data.    (scalar)
% grad         Gradient of nll with respect to theta   (numParams x 1 vector)
%
% Copyright (C) Daphne Koller, Stanford Univerity, 2012

function [nll, grad] = InstanceNegLogLikelihood(X, y, theta, modelParams)

    % featureSet is a struct with two fields:
    %    .numParams - the number of parameters in the CRF (this is not numImageFeatures
    %                 nor numFeatures, because of parameter sharing)
    %    .features  - an array comprising the features in the CRF.
    %
    % Each feature is a binary indicator variable, represented by a struct 
    % with three fields:
    %    .var          - a vector containing the variables in the scope of this feature
    %    .assignment   - the assignment that this indicator variable corresponds to
    %    .paramIdx     - the index in theta that this feature corresponds to
    %
    % For example, if we have:
    %   
    %   feature = struct('var', [2 3], 'assignment', [5 6], 'paramIdx', 8);
    %
    % then feature is an indicator function over X_2 and X_3, which takes on a value of 1
    % if X_2 = 5 and X_3 = 6 (which would be 'e' and 'f'), and 0 otherwise. 
    % Its contribution to the log-likelihood would be theta(8) if it's 1, and 0 otherwise.
    %
    % If you're interested in the implementation details of CRFs, 
    % feel free to read through GenerateAllFeatures.m and the functions it calls!
    % For the purposes of this assignment, though, you don't
    % have to understand how this code works. (It's complicated.)
    
    featureSet = GenerateAllFeatures(X, modelParams);

    % Use the featureSet to calculate nll and grad.
    % This is the main part of the assignment, and it is very tricky - be careful!
    % You might want to code up your own numerical gradient checker to make sure
    % your answers are correct.
    %
    % Hint: you can use CliqueTreeCalibrate to calculate logZ effectively. 
    %       We have halfway-modified CliqueTreeCalibrate; complete our implementation 
    %       if you want to use it to compute logZ.
    
    nll = 0;
    grad = zeros(size(theta));
    %%%
    % Your code here:
    n = length(y);
    K = modelParams.numHiddenStates;
    Factors = repmat(struct('var',[],'card',[],'val',[]),1,2*n-1);
    for i = 1:n-1
	    Factors(i).var = [i];
	    Factors(i).card = [K];
	    Factors(i).val = zeros(1,prod(Factors(i).card));
	    Factors(n+i).var = [i,i+1];
	    Factors(n+i).card = [K,K];
	    Factors(n+i).val = zeros(1,prod(Factors(i+n).card));
    end
    Factors(n).var = [n];
    Factors(n).card = [K];
    Factors(n).val = zeros(1,prod(Factors(n).card));


    featureCounts = zeros(size(theta));
    modelFeatureCounts = zeros(size(theta));

    for i = 1:length(featureSet.features)
	  feature = featureSet.features(i);
	  if length(feature.var) == 1
		  Factors(feature.var(1)).val(feature.assignment) += theta(feature.paramIdx);
	  else
		  % assuming feature.var is always [i,i+1].. if this fails accomodate for [i+1,i] as well
	          indx = AssignmentToIndex(feature.assignment,[K,K]);
		  fact = n + feature.var(1);
		  Factors(fact).val(indx) = theta(feature.paramIdx);
	  end

	  if feature.assignment == y([feature.var])
		  featureCounts(feature.paramIdx) = 1;
	  end
    end

    weightedFeatureCounts = featureCounts.*theta;

    logFactors = Factors;
    
    for i = 1:length(Factors)
	    Factors(i).val = exp(Factors(i).val);
    end
    
    P = CreateCliqueTree(Factors);
    [P, logZ] = CliqueTreeCalibrate(P, 0);
    lambda = modelParams.lambda;
    regularizationCost = sum(theta.*theta)*lambda/2;
    regularizationGradient = theta*lambda;
   
    % change the code how we do inference over all the probablilities to in case of words with letters not equal to 3.. do it using code instead of this lame crap..
    
    p1 = FactorMarginalization(P.cliqueList(1),2);
    p2 = FactorMarginalization(P.cliqueList(1),1);
    p3 = FactorMarginalization(P.cliqueList(2),2);
    p12 = P.cliqueList(1);
    p23 = P.cliqueList(2);
    p1.val/=sum(p1.val);
    p2.val/=sum(p2.val);
    p3.val/=sum(p3.val);
    p23.val/=sum(p23.val);
    p12.val/=sum(p12.val);
    ft = pppp(featureSet.features,p1,p2,p3,p12,p23);
    for i = 1:length(ft)
	    modelFeatureCounts(ft(i).paramIdx) += ft(i).p;
    end	    


    nll = logZ - sum(weightedFeatureCounts) + regularizationCost;
    grad = modelFeatureCounts - featureCounts + regularizationGradient;
end
