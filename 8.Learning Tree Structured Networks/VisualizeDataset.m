% Copyright (C) Daphne Koller, Stanford Univerity, 2012
%
% Author: Huayan Wang

function VisualizeDataset(Dataset)

f = figure;
for i=1:size(Dataset,1)
    img = ShowPose(reshape(Dataset(i,:,:), [10 3]));
    imshow(img);
    pause(0.3)
    if (~ishandle(f)) break; end; % quit loop when user closes the figure
end
