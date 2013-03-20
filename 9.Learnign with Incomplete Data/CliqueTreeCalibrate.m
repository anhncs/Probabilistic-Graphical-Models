%CLIQUETREECALIBRATE Performs sum-product or max-product algorithm for 
%clique tree calibration.

%   P = CLIQUETREECALIBRATE(P, isMax) calibrates a given clique tree, P 
%   according to the value of isMax flag. If isMax is 1, it uses max-product
%   message passing, otherwise uses sum-product. This function 
%   returns the clique tree where the .val for each clique in .cliqueList
%   is set to the final calibrated potentials.

%
% Copyright (C) Daphne Koller, Stanford Univerity, 2012

function P = CliqueTreeCalibrate(P)


% number of cliques in the tree.
N = length(P.cliqueList);

% setting up the messages that will be passed.
MESSAGES = repmat(struct('var', [], 'card', [], 'val', []), N, N);

% These will make sure that the same message is not passed twice in a row
lastCliqueOne = 0;
lastCliqueTwo = 1;
change = 1;
N = length(P.cliqueList);

forwardInds = repmat(1:P.cliqueList(1).card(1),1,P.cliqueList(1).card(1));
backInds = zeros(size(forwardInds));
%prepare our matrices
for i=1:P.cliqueList(1).card(1)
    backInds(((i-1)*P.cliqueList(1).card(1)+1):(i*P.cliqueList(1).card(1)))=i;
end

varOffset=0;

while (1), 
    
    % While there are ready cliques to pass messages between, keep passing
    % messages. Use GetNextCliques to find cliques to pass messages between.
    % Once you have clique i that is ready to send message to clique
    % j, compute the message and put it in MESSAGES(i,j).
    % Remember for clique tree, you only need an upward pass and a downward
    % pass.
    
    % Find the clique that is ready, compute the message for that clique,
    % and add that message to the queue

     cliqueOne=lastCliqueOne+change;
     cliqueTwo=lastCliqueTwo+change;
     lastCliqueOne=cliqueOne;
     lastCliqueTwo=cliqueTwo;
     
     
     if (cliqueTwo == 0)
         % There are no ready cliques, so stop
         break
     end

     currentMessage = P.cliqueList(cliqueOne);
     if(cliqueOne>1&&cliqueOne<N)
        if(change>0)
           currentMessage.val = currentMessage.val+MESSAGES(...
               cliqueOne-change,cliqueOne).val(forwardInds);
        else
           currentMessage.val = currentMessage.val+MESSAGES(...
               cliqueOne-change,cliqueOne).val(backInds);
        end
     end

    varsToRemove=cliqueOne+varOffset;

    % Use FactorMaxMarginalization to get the marginals
     MESSAGES(cliqueOne,cliqueTwo) = FactorMarginalization(currentMessage,varsToRemove);
    %Reverse if necessary
    if(cliqueTwo>=N)
         change=-1;
         lastCliqueOne=lastCliqueTwo+1;
         varOffset=1;
    end
end

% Now the clique tree has been calibaarated. 
% Compute the final potentials for the cliques and place them in P.

% Iterate through the incoming messages, multiply them by the initial
% potential, and normalize
P.cliqueList(1).val = P.cliqueList(1).val+MESSAGES(2,1).val(backInds);
P.cliqueList(N).val = P.cliqueList(N).val+MESSAGES(N-1,N).val(forwardInds);


for i = 2:N-1
    P.cliqueList(i).val = P.cliqueList(i).val+MESSAGES(i+1,i).val(backInds);
    P.cliqueList(i).val = P.cliqueList(i).val+MESSAGES(i-1,i).val(forwardInds);
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


end



