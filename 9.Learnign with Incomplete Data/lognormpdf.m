% File: lognormpdf.m
%
% Copyright (C) Daphne Koller, Stanford Univerity, 2012

function [log_prob] = lognormpdf(x,mu,sigma)

% LOGNORMPDF Natural logarithm of the normal probability density function (pdf)
% Y = lognormpdf(X,MU,SIGMA) returns the log of the pdf of the normal
% distribution parameterized by mean MU and standard deviation SIGMA evaluated
% at each value in the vector X. Thus, the size of the return
% vector Y is the size of X. 
% 
% MU and X should have the same dimensions.

log_prob = -log(sigma*sqrt(2*pi))-(x-mu).^2 ./ (2*sigma.^2);
