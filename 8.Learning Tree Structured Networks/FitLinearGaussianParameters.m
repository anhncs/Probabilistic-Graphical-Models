function [Beta sigma] = FitLinearGaussianParameters(X, U)

% Estimate parameters of the linear Gaussian model:
% X|U ~ N(Beta(1)*U(1) + ... + Beta(n)*U(n) + Beta(n+1), sigma^2);

% Note that Matlab/Octave index from 1, we can't write Beta(0).
% So Beta(n+1) is essentially Beta(0) in the text book.

% X: (M x 1), the child variable, M examples
% U: (M x N), N parent variables, M examples
%
% Copyright (C) Daphne Koller, Stanford Univerity, 2012

M = size(U,1);
N = size(U,2);

Beta = zeros(N+1,1);
sigma = 1;

% collect expectations and solve the linear system
% A = [ E[U(1)],      E[U(2)],      ... , E[U(n)],      1     ; 
%       E[U(1)*U(1)], E[U(2)*U(1)], ... , E[U(n)*U(1)], E[U(1)];
%       ...         , ...         , ... , ...         , ...   ;
%       E[U(1)*U(n)], E[U(2)*U(n)], ... , E[U(n)*U(n)], E[U(n)] ]

% construct A
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% YOUR CODE HERE
A = [U,ones(M,1)];
A = repmat(A,1,N+1);
mult = ones(M,N+1);
for i = 1:N
	mult = [mult,repmat(U(:,i),1,4)];
end
A = A.*mult;
A = mean(A);
A = reshape(A,N+1,N+1)';
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


% B = [ E[X]; E[X*U(1)]; ... ; E[X*U(n)] ]

% construct B
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% YOUR CODE HERE
B = repmat(X,1,N+1);
B = B.*[ones(M,1),U];
B = mean(B)';
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% solve A*Beta = B
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% YOUR CODE HERE
Beta = A\B;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% then compute sigma according to eq. (11) in PA description
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% YOUR CODE HERE
sigma = cov(X,1);
temp = 0;
for i = 1:N
	for j = 1:N
		temp += Beta(i)*Beta(j)*cov(U(:,i),U(:,j),1);
	end
end
sigma -= temp;
sigma = sqrt(sigma);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
