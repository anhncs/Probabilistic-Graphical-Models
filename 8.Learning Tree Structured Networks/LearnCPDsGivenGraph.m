function [P loglikelihood] = LearnCPDsGivenGraph(dataset, G, labels)
%
% Inputs:
% dataset: N x 10 x 3, N poses represented by 10 parts in (y, x, alpha)
% G: graph parameterization as explained in PA description
% labels: N x 2 true class labels for the examples. labels(i,j)=1 if the 
%         the ith example belongs to class j and 0 elsewhere        
%
% Outputs:
% P: struct array parameters (explained in PA description)
% loglikelihood: log-likelihood of the data (scalar)
%
% Copyright (C) Daphne Koller, Stanford Univerity, 2012

N = size(dataset, 1);
K = size(labels,2);

loglikelihood = 0;
P.c = zeros(1,K);

% estimate parameters
% fill in P.c, MLE for class probabilities
% fill in P.clg for each body part and each class
% choose the right parameterization based on G(i,1)
% compute the likelihood - you may want to use ComputeLogLikelihood.m
% you just implemented.
%%%%%%%%%%%%%%%%%%%%%%%%%
% YOUR CODE HERE
if size(size(G),2) == 2
	G1 = G; G2 = G;
else
	G1 = reshape(G(:,:,1),10,2);
	G2 = reshape(G(:,:,2),10,2);
end
human = dataset([labels(:,1)==1],:,:);
alien = dataset([labels(:,2)==1],:,:);
P.c = [size(human,1),size(alien,1)];
P.c /= N;
for i = 1:10
	if G1(i,1) == 0
		[P.clg(i).mu_y(1),P.clg(i).sigma_y(1)] = FitGaussianParameters(human(:,i,1));
		[P.clg(i).mu_x(1),P.clg(i).sigma_x(1)] = FitGaussianParameters(human(:,i,2));
		[P.clg(i).mu_angle(1),P.clg(i).sigma_angle(1)] = FitGaussianParameters(human(:,i,3));
	else
		% calc beta and sigma
		[Betay,P.clg(i).sigma_y(1)] = FitLinearGaussianParameters(human(:,i,1),reshape(human(:,G1(i,2),:),size(human,1),3));
		[Betax,P.clg(i).sigma_x(1)] = FitLinearGaussianParameters(human(:,i,2),reshape(human(:,G1(i,2),:),size(human,1),3));
		[Betaangle,P.clg(i).sigma_angle(1)] = FitLinearGaussianParameters(human(:,i,3),reshape(human(:,G1(i,2),:),size(human,1),3));
		P.clg(i).theta(1,:) = [Betay(4),Betay(1:3)',Betax(4),Betax(1:3)',Betaangle(4),Betaangle(1:3)'];

	end
	if G2(i,1) == 0
		[P.clg(i).mu_y(2),P.clg(i).sigma_y(2)] = FitGaussianParameters(alien(:,i,1));
		[P.clg(i).mu_x(2),P.clg(i).sigma_x(2)] = FitGaussianParameters(alien(:,i,2));
		[P.clg(i).mu_angle(2),P.clg(i).sigma_angle(2)] = FitGaussianParameters(alien(:,i,3));
	else
		%calc beta and sigma
		[Betay,P.clg(i).sigma_y(2)] = FitLinearGaussianParameters(alien(:,i,1),reshape(alien(:,G2(i,2),:),size(alien,1),3));
		[Betax,P.clg(i).sigma_x(2)] = FitLinearGaussianParameters(alien(:,i,2),reshape(alien(:,G2(i,2),:),size(alien,1),3));
		[Betaangle,P.clg(i).sigma_angle(2)] = FitLinearGaussianParameters(alien(:,i,3),reshape(alien(:,G2(i,2),:),size(alien,1),3));
		P.clg(i).theta(2,:) = [Betay(4),Betay(1:3)',Betax(4),Betax(1:3)',Betaangle(4),Betaangle(1:3)'];
	end
end
loglikelihood = ComputeLogLikelihood(P,G,dataset);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
fprintf('log likelihood: %f\n', loglikelihood);

