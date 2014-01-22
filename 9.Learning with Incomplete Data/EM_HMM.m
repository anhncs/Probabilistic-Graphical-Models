% File: EM_HMM.m
%
% Copyright (C) Daphne Koller, Stanford Univerity, 2012

function [P loglikelihood ClassProb PairProb] = EM_HMM(actionData, poseData, G, InitialClassProb, InitialPairProb, maxIter)

% INPUTS
% actionData: structure holding the actions as described in the PA
% poseData: N x 10 x 3 matrix, where N is number of poses in all actions
% G: graph parameterization as explained in PA description
% InitialClassProb: N x K matrix, initial allocation of the N poses to the K
%   states. InitialClassProb(i,j) is the probability that example i belongs
%   to state j.
%   This is described in more detail in the PA.
% InitialPairProb: V x K^2 matrix, where V is the total number of pose
%   transitions in all HMM action models, and K is the number of states.
%   This is described in more detail in the PA.
% maxIter: max number of iterations to run EM

% OUTPUTS
% P: structure holding the learned parameters as described in the PA
% loglikelihood: #(iterations run) x 1 vector of loglikelihoods stored for
%   each iteration
% ClassProb: N x K matrix of the conditional class probability of the N examples to the
%   K states in the final iteration. ClassProb(i,j) is the probability that
%   example i belongs to state j. This is described in more detail in the PA.
% PairProb: V x K^2 matrix, where V is the total number of pose transitions
%   in all HMM action models, and K is the number of states. This is
%   described in more detail in the PA.

% Initialize variables
N = size(poseData, 1);
K = size(InitialClassProb, 2);
L = size(actionData, 2); % number of actions
V = size(InitialPairProb, 1);

ClassProb = InitialClassProb;
PairProb = InitialPairProb;

loglikelihood = zeros(maxIter,1);

P.c = [];
P.clg.sigma_x = [];
P.clg.sigma_y = [];
P.clg.sigma_angle = [];

% EM algorithm
for iter=1:maxIter
  
  % M-STEP to estimate parameters for Gaussians
  % Fill in P.c, the initial state prior probability (NOT the class probability as in PA8 and EM_cluster.m)
  % Fill in P.clg for each body part and each class
  % Make sure to choose the right parameterization based on G(i,1)
  % Hint: This part should be similar to your work from PA8 and EM_cluster.m
  
  P.c = zeros(1,K);
  
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  % YOUR CODE HERE
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  
  for i = 1:L
	 P.c += ClassProb(actionData(i).marg_ind(1),:); 
  end
  P.c/=L;

  for i = 1:10
	  if G(i,1) == 1
		  % do shit
		  for j = 1:K
			  [Betay,P.clg(i).sigma_y(j)] = FitLG(poseData(:,i,1),reshape(poseData(:,G(i,2),:),N,3),ClassProb(:,j));
			  [Betax,P.clg(i).sigma_x(j)] = FitLG(poseData(:,i,2),reshape(poseData(:,G(i,2),:),N,3),ClassProb(:,j));
			  [Betaangle,P.clg(i).sigma_angle(j)] = FitLG(poseData(:,i,3),reshape(poseData(:,G(i,2),:),N,3),ClassProb(:,j));
			  P.clg(i).theta(j,:) = [Betay(4),Betay(1:3)',Betax(4),Betax(1:3)',Betaangle(4),Betaangle(1:3)'];

		  end
	  else
		  % do shit
		  for j = 1:K
			  [P.clg(i).mu_y(j),P.clg(i).sigma_y(j)] = FitG(poseData(:,i,1),ClassProb(:,j));
			  [P.clg(i).mu_x(j),P.clg(i).sigma_x(j)] = FitG(poseData(:,i,2),ClassProb(:,j));
			  [P.clg(i).mu_angle(j),P.clg(i).sigma_angle(j)] = FitG(poseData(:,i,3),ClassProb(:,j));
		  end
	  end
  end
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
  % M-STEP to estimate parameters for transition matrix
  % Fill in P.transMatrix, the transition matrix for states
  % P.transMatrix(i,j) is the probability of transitioning from state i to state j
  P.transMatrix = zeros(K,K);
  
  % Add Dirichlet prior based on size of poseData to avoid 0 probabilities
  P.transMatrix = P.transMatrix + size(PairProb,1) * .05;
  
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  % YOUR CODE HERE
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  P.transMatrix += reshape(sum(PairProb),3,3);
  noo = repmat(sum(P.transMatrix,2),1,3);
  P.transMatrix = P.transMatrix./noo;
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
    
  % E-STEP preparation: compute the emission model factors (emission probabilities) in log space for each 
  % of the poses in all actions = log( P(Pose | State) )
  % Hint: This part should be similar to (but NOT the same as) your code in EM_cluster.m
  
  logEmissionProb = zeros(N,K);
  
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  % YOUR CODE HERE
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  for i = 1:N
	  data = reshape(poseData(i,:,:),10,3);
	  for j = 1:10
		  if G(j,1) == 1
			  parent = data(G(j,2),:);
			  for k = 1:K
				  theta = P.clg(j).theta(k,:);
				  mu_y = sum(theta(1:4).*[1,parent]);
				  mu_x = sum(theta(5:8).*[1,parent]);
				  mu_a = sum(theta(9:12).*[1,parent]);
				  logEmissionProb(i,k) += lognormpdf(data(j,1),mu_y,P.clg(j).sigma_y(k));
				  logEmissionProb(i,k) += lognormpdf(data(j,2),mu_x,P.clg(j).sigma_x(k));
				  logEmissionProb(i,k) += lognormpdf(data(j,3),mu_a,P.clg(j).sigma_angle(k));
			  end
		  else
			  for k = 1:K
				  logEmissionProb(i,k) += lognormpdf(data(j,1),P.clg(j).mu_y(k),P.clg(j).sigma_y(k));
				  logEmissionProb(i,k) += lognormpdf(data(j,2),P.clg(j).mu_x(k),P.clg(j).sigma_x(k));
				  logEmissionProb(i,k) += lognormpdf(data(j,3),P.clg(j).mu_angle(k),P.clg(j).sigma_angle(k));
			  end
		  end
	  end
  end
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
    
  % E-STEP to compute expected sufficient statistics
  % ClassProb contains the conditional class probabilities for each pose in all actions
  % PairProb contains the expected sufficient statistics for the transition CPDs (pairwise transition probabilities)
  % Also compute log likelihood of dataset for this iteration
  % You should do inference and compute everything in log space, only converting to probability space at the end
  % Hint: You should use the logsumexp() function here to do probability normalization in log space to avoid numerical issues
  
  ClassProb = zeros(N,K);
  PairProb = zeros(V,K^2);
  loglikelihood(iter) = 0;
  
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  % YOUR CODE HERE
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  for i = 1:L
	  % construct all the three types of factors for each action and do inference to fill ClassProb and PairProb
	  m = length(actionData(i).marg_ind); % 1 to m represents S variables
	  F = repmat(struct('var',[],'card',[],'var',[]),1,2*m); % 1 is P(S1), 2 to m is P(Si/Si-1) and m+1 to 2m P(S/O)
	  F(1).var = 1; F(1).card = [K];F(1).val = log(P.c);
	  temp = log(reshape(P.transMatrix',1,9));
	  for j = 2:m
		  F(j).var = [j j-1]; F(j).card = [K K]; F(j).val = temp;
	  end
	  for j = 1:m
		  F(j+m).var = [j];F(j+m).card = [K];
		  F(j+m).val = logEmissionProb(actionData(i).marg_ind(j),:);
	  end
	  [M, PCalibrated] = ComputeExactMarginalsHMM(F);
	  for j = 1:m
		  ClassProb(actionData(i).marg_ind(j),:) = M(j).val;
	  end
	  for j = 1:m-1
		  temp = PCalibrated.cliqueList(j).val;
		  PairProb(actionData(i).pair_ind(j),:) = temp-logsumexp(temp);
	  end
	  loglikelihood(iter) += logsumexp(PCalibrated.cliqueList(1).val);
  end
  ClassProb = exp(ClassProb); PairProb = exp(PairProb);
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
  % Print out loglikelihood
  disp(sprintf('EM iteration %d: log likelihood: %f', ...
    iter, loglikelihood(iter)));
  if exist('OCTAVE_VERSION')
    fflush(stdout);
  end
  
  % Check for overfitting by decreasing loglikelihood
  if iter > 1
    if loglikelihood(iter) < loglikelihood(iter-1)
      break;
    end
  end
  
end

% Remove iterations if we exited early
loglikelihood = loglikelihood(1:iter);
