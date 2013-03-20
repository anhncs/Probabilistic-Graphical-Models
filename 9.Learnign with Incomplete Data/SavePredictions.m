function SavePredictions (yourPredictions)
% This function will save your test set predictions into a mat file. The
% submit script for will then read in that
% file to send your predictions to the grading server.
%
% The input `yourPredictions' should be a 90x1 vector where 
% yourPredictions(i) is the predicted class (1-3) for the i'th action.
%
% Copyright (C) Daphne Koller, Stanford Univerity, 2012

if (~isequal([90 1], size(yourPredictions)))
    error ('The input to SavePredictions is not the right size.');
end

save('Predictions.mat', 'yourPredictions');


end

