function loglikelihood = ComputeLogLikelihood(P, G, dataset)
% returns the (natural) log-likelihood of data given the model and graph structure
%
% Inputs:
% P: struct array parameters (explained in PA description)
% G: graph structure and parameterization (explained in PA description)
%
%    NOTICE that G could be either 10x2 (same graph shared by all classes)
%    or 10x2x2 (each class has its own graph). your code should compute
%    the log-likelihood using the right graph.
%
% dataset: N x 10 x 3, N poses represented by 10 parts in (y, x, alpha)
% 
% Output:
% loglikelihood: log-likelihood of the data (scalar)
%
% Copyright (C) Daphne Koller, Stanford Univerity, 2012

N = size(dataset,1); % number of examples
K = length(P.c); % number of classes

loglikelihood = 0;
% You should compute the log likelihood of data as in eq. (12) and (13)
% in the PA description
% Hint: Use lognormpdf instead of log(normpdf) to prevent underflow.
%       You may use log(sum(exp(logProb))) to do addition in the original
%       space, sum(Prob).
% assuming data = [y x alpha]
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% YOUR CODE HERE
% function val = lognormpdf(x, mu, sigma);
if size(size(G),2) == 2
	G1 = G; G2 = G;
else
	G1 = reshape(G(:,:,1),10,2);
	G2 = reshape(G(:,:,2),10,2);
end

for i = 1:N
	data = reshape(dataset(i,:,:),10,3);
	log1 = log(P.c(1));
	log2 = log(P.c(2));
	for j = 1:10
		if G1(j,1) == 1   % construct g1 and g2 in the beginning from g
			%do shit
			theta1 = P.clg(j).theta(1,:);   theta2 = P.clg(j).theta(2,:);
			parent1 = data(G1(j,2),:);   parent2 = data(G2(j,2),:);
			mu_y1 = sum(theta1(1:4).*[1, parent1]);mu_y2 = sum(theta2(1:4).*[1, parent2]);
			mu_x1 = sum(theta1(5:8).*[1, parent1]);mu_x2 = sum(theta2(5:8).*[1, parent2]);
			mu_a1 = sum(theta1(9:12).*[1, parent1]);mu_a2 = sum(theta2(9:12).*[1, parent2]);
			log1 += lognormpdf(data(j,1),mu_y1,P.clg(j).sigma_y(1));
			log1 += lognormpdf(data(j,2),mu_x1,P.clg(j).sigma_x(1));
			log1 += lognormpdf(data(j,3),mu_a1,P.clg(j).sigma_angle(1));
			log2 += lognormpdf(data(j,1),mu_y2,P.clg(j).sigma_y(2));
			log2 += lognormpdf(data(j,2),mu_x2,P.clg(j).sigma_x(2));
			log2 += lognormpdf(data(j,3),mu_a2,P.clg(j).sigma_angle(2));

		else
			log1 += lognormpdf(data(j,1),P.clg(j).mu_y(1),P.clg(j).sigma_y(1));
			log1 += lognormpdf(data(j,2),P.clg(j).mu_x(1),P.clg(j).sigma_x(1));
			log1 += lognormpdf(data(j,3),P.clg(j).mu_angle(1),P.clg(j).sigma_angle(1));
			log2 += lognormpdf(data(j,1),P.clg(j).mu_y(2),P.clg(j).sigma_y(2));
			log2 += lognormpdf(data(j,2),P.clg(j).mu_x(2),P.clg(j).sigma_x(2));
			log2 += lognormpdf(data(j,3),P.clg(j).mu_angle(2),P.clg(j).sigma_angle(2));
		end
	end
	loglikelihood += log(sum(exp(log1)+exp(log2)));
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%












