% File: FitLG.m
%
% Copyright (C) Daphne Koller, Stanford Univerity, 2012

function [Beta sigma] = FitLG(X, U, W)

% Estimate parameters of the linear Gaussian model:
% X|U ~ N(Beta(1)*U(1) + ... + Beta(K)*U(K) + Beta(K+1), sigma^2);

% Note that Matlab index from 1, we can't write Beta(0). So Beta(K+1) is
% essentially Beta(0) in PA3 description (and the text book).

% X: (N x 1), the child variable, N examples
% U: (N x K), K parent variables, N examples
% W: (N x 1), weights over the examples.

N = size(U,1);
K = size(U,2);

Beta = zeros(K+1,1);
sigma = 1;

% collect expectations and solve the linear system
% A = [ E[U(1)],      E[U(2)],      ... , E[U(K)],      1     ; 
%       E[U(1)*U(1)], E[U(2)*U(1)], ... , E[U(K)*U(1)], E[U(1)];
%       ...         , ...         , ... , ...         , ...   ;
%       E[U(1)*U(K)], E[U(2)*U(K)], ... , E[U(K)*U(K)], E[U(K)] ]

A = zeros(K,K);
for j = 1:K
    row = 1:K;
    for x = 1:K
        mu = W'*(U(:,x).*U(:,j))/sum(W);
        row(x) = mu;
    end
	A(j,:) = row;
end
row1 = 1:K;
for x = 1:K
    mu = W'*U(:,x)/sum(W);
    row1(x) = mu;
end
col2 = [1;row1'];
A = [row1;A];
A = [A, col2];

% B = [ E[X]; E[X*U(1)]; ... ; E[X*U(K)] ]

B = 1:K;
for x = 1:K
    mu = W'*(X.*U(:,x))/sum(W);
    B(x) = mu;
end

mu = W'*X/sum(W);
B = [mu;B'];

% solve A*Beta = B
Beta = A\B;

% then compute sigma according to eq. (17) in PA description
CovU = A(2:end, 1:K)  - A(2:end, K+1) * A(1, 1:K);
[MuX, SigmaX] = FitG(X, W);
sigma = sqrt( SigmaX^2 - Beta(1:K)' * CovU * Beta(1:K) );

% catch in case sigma is badly conditioned
if sigma == 0 || ~isreal(sigma)
	sigma = .01;
else
	sigma = sigma + .01;
end
