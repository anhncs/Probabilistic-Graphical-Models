% This file is part of libDAI - http://www.libdai.org/
%
% Copyright (c) 2006-2011, The libDAI authors. All rights reserved.
%
% Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.


% This example program illustrates how to construct a factorgraph
% by means of the sprinkler network example discussed at
% http://www.cs.ubc.ca/~murphyk/Bayes/bnintro.html
% using the SWIG octave wrapper of libDAI

dai

C = dai.Var(0, 2);  % Define binary variable Cloudy (with label 0)
S = dai.Var(1, 2);  % Define binary variable Sprinkler (with label 1)
R = dai.Var(2, 2);  % Define binary variable Rain (with label 2)
W = dai.Var(3, 2);  % Define binary variable Wetgrass (with label 3)

% Define probability distribution for C
P_C = dai.Factor(C);
P_C(0) = 0.5;            % C = 0
P_C(1) = 0.5;            % C = 1

% Define conditional probability of S given C
P_S_given_C = dai.Factor(dai.VarSet(S,C));
P_S_given_C(0) = 0.5;    % C = 0, S = 0
P_S_given_C(1) = 0.9;    % C = 1, S = 0
P_S_given_C(2) = 0.5;    % C = 0, S = 1
P_S_given_C(3) = 0.1;    % C = 1, S = 1

% Define conditional probability of R given C
P_R_given_C = dai.Factor(dai.VarSet(R,C));
P_R_given_C(0) = 0.8;    % C = 0, R = 0
P_R_given_C(1) = 0.2;    % C = 1, R = 0
P_R_given_C(2) = 0.2;    % C = 0, R = 1
P_R_given_C(3) = 0.8;    % C = 1, R = 1

% Define conditional probability of W given S and R
SRW = dai.VarSet(S,R);
SRW.append(W);
P_W_given_S_R = dai.Factor(SRW);
P_W_given_S_R(0) = 1.0;  % S = 0, R = 0, W = 0
P_W_given_S_R(1) = 0.1;  % S = 1, R = 0, W = 0
P_W_given_S_R(2) = 0.1;  % S = 0, R = 1, W = 0
P_W_given_S_R(3) = 0.01; % S = 1, R = 1, W = 0
P_W_given_S_R(4) = 0.0;  % S = 0, R = 0, W = 1
P_W_given_S_R(5) = 0.9;  % S = 1, R = 0, W = 1
P_W_given_S_R(6) = 0.9;  % S = 0, R = 1, W = 1
P_W_given_S_R(7) = 0.99; % S = 1, R = 1, W = 1

% Build factor graph consisting of those four factors
SprinklerFactors = dai.VecFactor();
SprinklerFactors.append(P_C);
SprinklerFactors.append(P_R_given_C);
SprinklerFactors.append(P_S_given_C);
SprinklerFactors.append(P_W_given_S_R);
SprinklerNetwork = dai.FactorGraph(SprinklerFactors);

% Write factorgraph to a file
SprinklerNetwork.WriteToFile('sprinkler.fg');
fprintf('Sprinkler network written to sprinkler.fg\n');

% Output some information about the factorgraph
fprintf('%d variables\n', SprinklerNetwork.nrVars());
fprintf('%d factors\n', SprinklerNetwork.nrFactors());

% Calculate joint probability of all four variables
P = dai.Factor();
for I = 0:(SprinklerNetwork.nrFactors()-1)
    P *= SprinklerNetwork.factor(I);
end
P.normalize(); % Not necessary: a Bayesian network is already normalized by definition

% Calculate some probabilities
denom = P.marginal(dai.VarSet(W))(1);
fprintf('P(W=1) = %f\n', denom);
fprintf('P(S=1 | W=1) = %f\n', P.marginal(dai.VarSet(S,W))(3) / denom);
fprintf('P(R=1 | W=1) = %f\n', P.marginal(dai.VarSet(R,W))(3) / denom);
