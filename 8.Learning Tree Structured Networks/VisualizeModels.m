% Copyright (C) Daphne Koller, Stanford Univerity, 2012
%
% Author: Huayan Wang

function VisualizeModels(P, G)
K = length(P.c);

f = figure;
while(1)
    for k=1:K
        subplot(1,K,k);
        if size(G,3) == 1  % same graph structure for all classes
            
            pose = SamplePose(P,G,k);
            
        else  % different graph structure for each class
            
            pose = SamplePose(P,G(:,:,k),k);
            
        end
        
        img = ShowPose(pose);
        imshow(img);
        pause(0.3)
        if (~ishandle(f)) return; end; % quit loop when user closes the figure
    end
end
