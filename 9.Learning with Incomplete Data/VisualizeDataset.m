% File: VisualizeDataset.m
%
% Copyright (C) Daphne Koller, Stanford Univerity, 2012

function VisualizeDataset(Dataset)

figure
for i=1:size(Dataset,1)
    img = ShowPose(reshape(Dataset(i,:,:), [10 3]));
    imshow(img);
    pause(0.3);
end
