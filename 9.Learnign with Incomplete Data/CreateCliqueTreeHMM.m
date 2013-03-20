% CreateCliqueTreeHMM Takes in a list of factors F and returns a clique
% tree. Should only be called when F meets the following conditions:
%
% 1) Factors are over 1 or 2 variables
% 2) All 2-variable factors are over variables (i,i+1)
% 3) All variables have the same cardinality
%
% Roughly, these conditions mean that the factors of F define a standard
% chain model such as an HMM.
%
% Function returns a clique tree that has the following fields:
% - .edges: Contains indices of the nodes that have edges between them.
% - .factorList: Contains the list of factors used to build the Clique
%                tree.

%
% Copyright (C) Daphne Koller, Stanford Univerity, 2012

function P = CreateCliqueTreeHMM (F)

maxVar = max([F.var]);
numNodes = maxVar - 1;
card = F(1).card(1);

P.cliqueList = repmat(struct('var', [], 'card', [], 'val', []), numNodes, 1);
P.edges = zeros(numNodes);

for i = 1:numNodes
    P.cliqueList(i).var = [i i+1];
    P.cliqueList(i).card = [card card];
    P.cliqueList(i).val = ones(1, card * card);
    
    if (i > 1)
        P.edges(i, i-1) = 1;
        P.edges(i-1, i) = 1;
    end
    if (i < numNodes)
        P.edges(i, i+1) = 1;
        P.edges(i+1, i) = 1;
    end        
end

for i = 1:length(F)
    f = F(i);
    if (length(f.var) == 1)
        if (f.var > 1)
            cliqueIdx = f.var - 1;
        else
            cliqueIdx = 1;
        end
        
        for assignment = 1:card
           if (f.var == cliqueIdx)
               updateIdxs = AssignmentToIndex([(1:card)' assignment*ones(card,1)], [card card]);
           else
               updateIdxs = AssignmentToIndex([assignment*ones(card,1) (1:card)'], [card card]);
           end
           P.cliqueList(cliqueIdx).val(updateIdxs) = P.cliqueList(cliqueIdx).val(updateIdxs) + f.val;
        end        
    else
        assert(length(f.var) == 2);
        cliqueIdx = min(f.var);
        if (f.var(1) > f.var(2))
            % sort the factor val so it's in increasing var order
            [dummy, order] = sort(f.var); %#ok
            oldAssignments = IndexToAssignment(1:length(f.val), f.card);
            newAssignments = oldAssignments(:, order);
            f.card = f.card(order);
            f.var = f.var(order);
            f.val = f.val(AssignmentToIndex(newAssignments, f.card));
        end
        P.cliqueList(cliqueIdx).val = P.cliqueList(cliqueIdx).val + f.val;        
    end
end

end

