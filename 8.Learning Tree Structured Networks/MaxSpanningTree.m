function adj = MaxSpanningTree (weights)
% MAXSPANNINGTREE    Maximum weight spanning tree
%
%   adj = MaxSpanningTree(weights) takes an n-by-n weight matrix, which
%   should be symmetric, and returns an adjacency list representation of
%   the maximum weight spanning tree. The adjacency list will be directed,
%   i.e. not symmetric.
%
%   Simply an implementation of Prim's MST algorithm
%
% Copyright (C) Daphne Koller, Stanford Univerity, 2012

n = size(weights, 1);
if (n ~= size(weights, 2))
    help(mfilename);
end
% The keys are the weights of the edges
keys = -inf*ones(n, 1);
predecessors = -ones(n, 1);
root = 1;
keys(root) = 0;
% Queue of vertices to put in the tree
Q = 1:n;

while (length(Q) ~= 0)
    [index, vertex] = MaxVertex(Q, keys);
    remover = [1:(index-1) (index+1):length(Q)];
    % Remove the index of the maximum key vertex from Q
    Q = Q(remover);
    % Loop over all vertices adjacent to vertex
    for v = 1:n
        % If the vertex is still in the queue
        ind = find(Q == v);
        if (length(ind) == 1 && weights(vertex, v) > keys(v))
            % If the v is still in the queue and the weight of the
            % edge is greater than its key, set the vertex to be v's
            % predecessor
            predecessors(v) = vertex;
            keys(v) = weights(vertex, v);
        end
    end
end

adj = AdjFromPreds(predecessors, n);


% ---- MaxVertex ---- %
function [index, vertex] = MaxVertex(Q, keys)
% Returns the maximum weight vertex (maximum weight across the cut set)
% given the keys. Returns the index into Q in index, return the actual
% vertex in vertex.

% We use keys(Q) to only consider the keys for vertices still in the queue
[val, index] = max(keys(Q));
vertex = Q(index);


% ---- AdjFromPreds ---- %
function adj = AdjFromPreds(predecessors, n)
% Constructs an adjacency matrix representation of the matrix from the list
% of predecessors found by the MST algorithm.
adj = zeros(n);
for i = 1:n
    if (predecessors(i) ~= -1)
        adj(predecessors(i), i) = 1;
    end
end
% Make it symmetric (undirected)
% adj = adj + adj';
