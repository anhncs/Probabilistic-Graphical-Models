function MonitorAssignment( F, names, valNames, E )

% MonitorAssignment( F, names, valNames, E ) - Pretty prints all the 
%   marginals for an assignment
%
%   F contains the struct array of factors
%   names contains the variable names
%   valNames contains the assignment names for each variable, 
%    as seen in SAMIAM
%   E is an N-by-2 cell array, each row being a variable/value pair. 
%     Variables are in the first column and values are in the second column.
%     If there is no evidence, pass in the empty matrix [] for E.
%
%   Example of use:
%    [F, names, valNames] = ConvertNetwork('Credit_net.net');
%    MonitorAssignment( F, names, valNames, {'Assets', 'Low'; 'Age', 'Over65'} )

Enum = zeros(size(E));
for i=1:size(E,1)
    Enum(i,1) = find(strcmp(E{i,1}, names),1,'first');
    Enum(i,2) = find(strcmp(E{i,2}, valNames{Enum(i,1)}),1,'first');
end
maxNameLength = 0;
for i=1:numel(valNames) 
    for j=1:numel(valNames{i})
        maxNameLength = max(maxNameLength, length(valNames{i}{j}));
    end
end
for i=1:numel(F)
    M = ComputeMarginal([i], F, Enum);
    fprintf('%s\n', names{i})
    for j=1:numel(M.val)
        xtra_space = char(' '*ones(maxNameLength-length(valNames{i}{j})+3,1));
        fprintf('\t%s:%s%.02f%%\n', valNames{i}{j}, xtra_space, M.val(j)*100);
    end
end

