% File: RecognizeActions.m
%
% Copyright (C) Daphne Koller, Stanford Univerity, 2012

function [accuracy, predicted_labels] = RecognizeActions(datasetTrain, datasetTest, G, maxIter)

% INPUTS
% datasetTrain: dataset for training models, see PA for details
% datasetTest: dataset for testing models, see PA for details
% G: graph parameterization as explained in PA decription
% maxIter: max number of iterations to run for EM

% OUTPUTS
% accuracy: recognition accuracy, defined as (#correctly classified examples / #total examples)
% predicted_labels: N x 1 vector with the predicted labels for each of the instances in datasetTest, with N being the number of unknown test instances


% Train a model for each action
% Note that all actions share the same graph parameterization and number of max iterations
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% YOUR CODE HERE
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
[P1 logli1 ClassProb1 PairProb1] = EM_HMM(datasetTrain(1).actionData, datasetTrain(1).poseData, G, datasetTrain(1).InitialClassProb, datasetTrain(1).InitialPairProb, maxIter);


[P2 logli2 ClassProb2 PairProb2] = EM_HMM(datasetTrain(2).actionData, datasetTrain(2).poseData, G, datasetTrain(2).InitialClassProb, datasetTrain(2).InitialPairProb, maxIter);


[P3 logli3 ClassProb3 PairProb3] = EM_HMM(datasetTrain(3).actionData, datasetTrain(3).poseData, G, datasetTrain(3).InitialClassProb, datasetTrain(3).InitialPairProb, maxIter);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


% Classify each of the instances in datasetTest
% Compute and return the predicted labels and accuracy
% Accuracy is defined as (#correctly classified examples / #total examples)
% Note that all actions share the same graph parameterization

accuracy = 0;
predicted_labels = [];
P = [P1,P2,P3];
N = size(datasetTest.poseData,1);
I = size(datasetTest.labels,1);
K = 3;
loglikelihood = zeros(I,3);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% YOUR CODE HERE
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
for iclass = 1:3
  logEmissionProb = zeros(N,K);
  
  for i = 1:N
	  data = reshape(datasetTest.poseData(i,:,:),10,3);
	  for j = 1:10
		  if G(j,1) == 1
			  parent = data(G(j,2),:);
			  for k = 1:K
				  theta = P(iclass).clg(j).theta(k,:);
				  mu_y = sum(theta(1:4).*[1,parent]);
				  mu_x = sum(theta(5:8).*[1,parent]);
				  mu_a = sum(theta(9:12).*[1,parent]);
				  logEmissionProb(i,k) += lognormpdf(data(j,1),mu_y,P(iclass).clg(j).sigma_y(k));
				  logEmissionProb(i,k) += lognormpdf(data(j,2),mu_x,P(iclass).clg(j).sigma_x(k));
				  logEmissionProb(i,k) += lognormpdf(data(j,3),mu_a,P(iclass).clg(j).sigma_angle(k));
			  end
		  else
			  for k = 1:K
				  logEmissionProb(i,k) += lognormpdf(data(j,1),P(iclass).clg(j).mu_y(k),P(iclass).clg(j).sigma_y(k));
				  logEmissionProb(i,k) += lognormpdf(data(j,2),P(iclass).clg(j).mu_x(k),P(iclass).clg(j).sigma_x(k));
				  logEmissionProb(i,k) += lognormpdf(data(j,3),P(iclass).clg(j).mu_angle(k),P(iclass).clg(j).sigma_angle(k));
			  end
		  end
	  end
  end

  for i = 1:I
	  % construct all the three types of factors for each action and do inference to fill ClassProb and PairProb
	  m = length(datasetTest.actionData(i).marg_ind); % 1 to m represents S variables
	  F = repmat(struct('var',[],'card',[],'var',[]),1,2*m); % 1 is P(S1), 2 to m is P(Si/Si-1) and m+1 to 2m P(S/O)
	  F(1).var = 1; F(1).card = [K];F(1).val = log(P(iclass).c);
	  temp = log(reshape(P(iclass).transMatrix',1,9));
	  for j = 2:m
		  F(j).var = [j j-1]; F(j).card = [K K]; F(j).val = temp;
	  end
	  for j = 1:m
		  F(j+m).var = [j];F(j+m).card = [K];
		  F(j+m).val = logEmissionProb(datasetTest.actionData(i).marg_ind(j),:);
	  end
	  [M, PCalibrated] = ComputeExactMarginalsHMM(F);
	  loglikelihood(i,iclass) += logsumexp(PCalibrated.cliqueList(1).val);
  end
end
[temp, predicted_labels] = max(loglikelihood,[],2);
accuracy = sum(predicted_labels==datasetTest.labels)/I;
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
