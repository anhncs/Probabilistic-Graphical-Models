% CLUSTERGRAPHCALIBRATE Loopy belief propagation for cluster graph calibration.
%   P = CLUSTERGRAPHCALIBRATE(P, useSmart) calibrates a given cluster graph, G,
%   and set of of factors, F. The function returns the final potentials for
%   each cluster. 
%   The cluster graph data structure has the following fields:
%   - .clusterList: a list of the cluster beliefs in this graph. These entries
%                   have the following subfields:
%     - .var:  indices of variables in the specified cluster
%     - .card: cardinality of variables in the specified cluster
%     - .val:  the cluster's beliefs about these variables
%   - .edges: A cluster adjacency matrix where edges(i,j)=1 implies clusters i
%             and j share an edge.
%  
%   UseSmart is an indicator variable that tells us whether to use the Naive or Smart
%   implementation of GetNextClusters for our message ordering
%
%   See also FACTORPRODUCT, FACTORMARGINALIZATION
%
% Copyright (C) Daphne Koller, Stanford University, 2012

function [P MESSAGES resid] = ClusterGraphCalibrate(P,useSmartMP)

if(~exist('useSmartMP','var'))
  useSmartMP = 0;
end

N = length(P.clusterList);

MESSAGES = repmat(struct('var', [], 'card', [], 'val', []), N, N);
[edgeFromIndx, edgeToIndx] = find(P.edges ~= 0);

resid = [];

vars = unique([P.clusterList.var]);
m = length(vars);
cardinality = cell();

for i = 1:N
	var = P.clusterList(i).var;
	card = P.clusterList(i).card;
	m = length(var);
	for j = 1:m
		cardinality{var(j)} = card(j);
	end
end

for m = 1:length(edgeFromIndx),
    i = edgeFromIndx(m);
    j = edgeToIndx(m);

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % YOUR CODE HERE
    %
    %
    %
    % Set the initial message values
    % MESSAGES(i,j) should be set to the initial value for the
    % message from cluster i to cluster j
    %
    % The matlab/octave functions 'intersect' and 'find' may
    % be useful here (for making your code faster)
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    MESSAGES(i,j).var = intersect(P.clusterList(i).var,P.clusterList(j).var);
    MESSAGES(i,j).card = ones(size(MESSAGES(i,j).var));
    for kk = 1:length(MESSAGES(i,j).card)
	    MESSAGES(i,j).card(kk) = cardinality{MESSAGES(i,j).var(kk)};
    end
    MESSAGES(i,j).val = ones(1,prod(MESSAGES(i,j).card));    
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
end;



% perform loopy belief propagation
tic;
iteration = 0;

lastMESSAGES = MESSAGES;

while (1),
    iteration = iteration + 1;
    [i, j] = GetNextClusters(P, MESSAGES,lastMESSAGES, iteration, useSmartMP); 
    prevMessage = MESSAGES(i,j);

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % YOUR CODE HERE
    % We have already selected a message to pass, \delta_ij.
    % Compute the message from clique i to clique j and put it
    % in MESSAGES(i,j)
    % Finally, normalize the message to prevent overflow
    %
    % The function 'setdiff' may be useful to help you
    % obtain some speedup in this function
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    others = P.edges(:,i);others(j) = 0;
    MESSAGES(i,j) = P.clusterList(i);
    for k = 1:length(others)
	    if others(k)==1
		    MESSAGES(i,j) = FactorProduct(MESSAGES(i,j),MESSAGES(k,i));
	    end
    end

    summedout = setdiff(P.clusterList(i).var,P.clusterList(j).var);
    MESSAGES(i,j) = FactorMarginalization(MESSAGES(i,j),summedout); 
    MESSAGES(i,j).val = MESSAGES(i,j).val/(sum(MESSAGES(i,j).val));
    %m193 = MessageDelta(MESSAGES(19,3),lastMESSAGES(19,3));
    %m1540 = MessageDelta(MESSAGES(15,40),lastMESSAGES(15,40));
    %m172 = MessageDelta(MESSAGES(17,2),lastMESSAGES(17,2));
    %resid = [resid;m193 m1540 m172]; 
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    if(useSmartMP==1)
      lastMESSAGES(i,j)=prevMessage;
    end
    
    % Check for convergence every m iterations
    if mod(iteration, length(edgeFromIndx)) == 0
        if (CheckConvergence(MESSAGES, lastMESSAGES))
            break;
        end
        disp(['LBP Messages Passed: ', int2str(iteration), '...']);
        if(useSmartMP~=1)
          lastMESSAGES=MESSAGES;
        end
    end
    if iteration>1000000
	break;
    end
    
end;
toc;
disp(['Total number of messages passed: ', num2str(iteration)]);


% Compute final potentials and place them in P
for m = 1:length(edgeFromIndx),
    j = edgeFromIndx(m);
    i = edgeToIndx(m);
    P.clusterList(i) = FactorProduct(P.clusterList(i), MESSAGES(j, i));
end


% Get the max difference between the marginal entries of 2 messages -------
function delta = MessageDelta(Mes1, Mes2)
delta = max(abs(Mes1.val - Mes2.val));
return;


