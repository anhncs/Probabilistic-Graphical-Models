%CLIQUETREECALIBRATE Performs sum-product or max-product algorithm for 
%clique tree calibration.

%   P = CLIQUETREECALIBRATE(P, isMax) calibrates a given clique tree, P 
%   according to the value of isMax flag. If isMax is 1, it uses max-sum
%   message passing, otherwise uses sum-product. This function 
%   returns the clique tree where the .val for each clique in .cliqueList
%   is set to the final calibrated potentials.
%
% Copyright (C) Daphne Koller, Stanford University, 2012

function P = CliqueTreeCalibrate(P, isMax)


% Number of cliques in the tree.
N = length(P.cliqueList);

if isMax == 1
	for i = 1:N
		P.cliqueList(i).val = log(P.cliqueList(i).val);
	end
end


cardinality = zeros(1,N);

for i = 1:N
	var = P.cliqueList(i).var;
	card = P.cliqueList(i).card;
	m = length(var);
	for j = 1:m
		cardinality(var(j)) = card(j);
	end
end
% Setting up the messages that will be passed.
% MESSAGES(i,j) represents the message going from clique i to clique j. 
MESSAGES = repmat(struct('var', [], 'card', [], 'val', []), N, N);
messages = MESSAGES;


for i = 1:N
	for j = 1:N
		if P.edges(i,j) == 1
			messages(i,j).var = intersect(P.cliqueList(i).var,P.cliqueList(j).var);
			messages(i,j).card = cardinality(messages(i,j).var);
			messages(i,j).val = ones(1,prod(messages(i,j).card));
			if isMax == 1
				messages(i,j).val = zeros(1,prod(messages(i,j).card));
			end
		else
			continue
		end
	end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% We have split the coding part for this function in two chunks with
% specific comments. This will make implementation much easier.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% YOUR CODE HERE
% While there are ready cliques to pass messages between, keep passing
% messages. Use GetNextCliques to find cliques to pass messages between.
% Once you have clique i that is ready to send message to clique
% j, compute the message and put it in MESSAGES(i,j).
% Remember that you only need an upward pass and a downward pass.
%
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
[i,j] = GetNextCliques(P,MESSAGES);
while i != 0
	others = P.edges(:,i);others(j) = 0;
	if isMax==0
		messages(i,j) = FactorProduct(P.cliqueList(i),messages(i,j));
	else
		messages(i,j) = FactorSum(P.cliqueList(i),messages(i,j));
	end
	for x = 1:N
		if others(x)==1
			if isMax == 0
				messages(i,j) = FactorProduct(messages(i,j),messages(x,i));
			else
				messages(i,j) = FactorSum(messages(i,j),messages(x,i));
			end
		end
	end
	sepset = intersect(P.cliqueList(i).var,P.cliqueList(j).var);
	summedout = setdiff(P.cliqueList(i).var,P.cliqueList(j).var);
	%MESSAGES(i,j) = ComputeMarginal(sepset,messages(i,j),[]);
	if isMax == 0
		messages(i,j) = FactorMarginalization(messages(i,j),summedout);
		su = sum(messages(i,j).val);
		messages(i,j).val = messages(i,j).val/su;
	else
		messages(i,j) = FactorMaxMarginalization(messages(i,j),summedout);
	end
	MESSAGES(i,j) = messages(i,j);
	[i,j] = GetNextCliques(P,MESSAGES);
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% YOUR CODE HERE
%
% Now the clique tree has been calibrated. 
% Compute the final potentials for the cliques and place them in P.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%P = MESSAGES;

for i=1:N
	edges = P.edges(i,:);
	for j = 1:N
		if edges(j)==1
			if isMax == 0
				P.cliqueList(i) = FactorProduct(P.cliqueList(i),MESSAGES(j,i));
			else
				P.cliqueList(i) = FactorSum(P.cliqueList(i),MESSAGES(j,i));
			end
		end
	end
end

return
