% File: FitG.m
%
% Copyright (C) Daphne Koller, Stanford Univerity, 2012

function [mu sigma] = FitG(X, W)

% X: (N x 1): N examples (1 dimensional)
% W: (N x 1): Weights over examples (W(i) is the weight for X(i))

% Fit N(mu, sigma^2) to the empirical distribution

mu = 0;
sigma = 1;

mu = W'*X/sum(W);
v = W'*(X.*X)/sum(W) - mu^2;
sigma = sqrt(v);
