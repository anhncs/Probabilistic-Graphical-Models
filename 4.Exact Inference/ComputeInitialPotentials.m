%COMPUTEINITIALPOTENTIALS Sets up the cliques in the clique tree that is
%passed in as a parameter.
%
%   P = COMPUTEINITIALPOTENTIALS(C) Takes the clique tree skeleton C which is a
%   struct with three fields:
%   - nodes: cell array representing the cliques in the tree.
%       C.nodes represet the var in each clique
%   - edges: represents the adjacency matrix of the tree.
%   - factorList: represents the list of factors that were used to build
%   the tree. 
%   
%   It returns the standard form of a clique tree P that we will use through 
%   the rest of the assigment. P is struct with two fields:
%   - cliqueList: represents an array of cliques with appropriate factors 
%   from factorList assigned to each clique. Where the .val of each clique
%   is initialized to the initial potential of that clique.
%   - edges: represents the adjacency matrix of the tree. 
%
% Copyright (C) Daphne Koller, Stanford University, 2012


function P = ComputeInitialPotentials(C)

% number of cliques
N = length(C.nodes);
card = cell();
for i = 1:length(C.factorList)
	va = C.factorList(i).var;
	ca = C.factorList(i).card;
	for j = 1:length(va)
		card{va(j)} = ca(j);
	end
end
cardi = ones(1,length(card));
for i = 1:length(card)
	cardi(i) = card{i};
end
% initialize cluster potentials 
P.cliqueList = repmat(struct('var', [], 'card', [], 'val', []), 1,N);
for i = 1:N
	P.cliqueList(i).var = C.nodes{i};
	P.cliqueList(i).card = ones(1,length(P.cliqueList(i).var));
	for j = 1:length(P.cliqueList(i).var)
		P.cliqueList(i).card(j) = card{P.cliqueList(i).var(j)};
	end
	P.cliqueList(i).val = ones(1,prod(P.cliqueList(i).card));
end
P.edges = C.edges; 
clique = cell();
clique{N}=[];
for i = 1:length(C.factorList);
	n = length(C.factorList(i).var);
	for j = 1:N
		if length(setdiff(C.factorList(i).var,C.nodes{j}))==0
			clique{j} = [i,clique{j}];
			break
		end
	end
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% YOUR CODE HERE
%
% First, compute an assignment of factors from factorList to cliques. 
% Then use that assignment to initialize the cliques in cliqueList to 
% their initial potentials. 

% C.nodes is a list of cliques.
% So in your code, you should start with: P.cliqueList(i).var = C.nodes{i};
% Print out C to get a better understanding of its structure.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
for i = 1:N
	facts = clique{i};
	%P.cliqueList(i) = FactorProduct(P.C.factorList(facts(1));
	for j = 1:length(facts)
		P.cliqueList(i) = FactorProduct(P.cliqueList(i),C.factorList(facts(j))) ;
	end
end

end

