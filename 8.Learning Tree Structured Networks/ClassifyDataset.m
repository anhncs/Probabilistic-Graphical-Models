function accuracy = ClassifyDataset(dataset, labels, P, G)
% returns the accuracy of the model P and graph G on the dataset 
%
% Inputs:
% dataset: N x 10 x 3, N test instances represented by 10 parts
% labels:  N x 2 true class labels for the instances.
%          labels(i,j)=1 if the ith instance belongs to class j 
% P: struct array model parameters (explained in PA description)
% G: graph structure and parameterization (explained in PA description) 
%
% Outputs:
% accuracy: fraction of correctly classified instances (scalar)
%
% Copyright (C) Daphne Koller, Stanford Univerity, 2012

N = size(dataset, 1);
accuracy = 0.0;
calclabels = zeros(size(labels));
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% YOUR CODE HERE
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
	calclabels(i,:) = [log1,log2];
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

[temp,temp1] = max(calclabels,[],2);
[temp,temp2] = max(labels,[],2);
accuracy = sum(temp1==temp2)/N;
fprintf('Accuracy: %.2f\n', accuracy);
