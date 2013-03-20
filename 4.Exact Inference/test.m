A = struct('var', [1 2 3], 'card' ,[2 3 4], 'val' ,[ 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24]);

B = FactorMaxMarginalization(A, [3])
% B.val: [19 20 21 22 23 24]

C = struct('var', [1 2 3], 'card' ,[2 3 4], 'val' ,-1*[ 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24]);

D = FactorMaxMarginalization(C, [3])
% D.val: [-1 -2 -3 -4 -5 -6]

E = FactorMaxMarginalization(A, [1 2])
% E.val: [6 12 18 24]

F = FactorMaxMarginalization(A, [])
% F.val: [1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24]

G = FactorMaxMarginalization(A, [2])
% G.val: [5 6 11 12 17 18 23 24]

H = struct('var', [1 2 3], 'card' ,[2 3 4], 'val' ,[ 1 2 3 4 5 -6 7 8 9 -10 11 12 13 14 -15 16 17 18 -19 20 21 22 23 24]);

I = FactorMaxMarginalization(H, [2])
% I.val: [5 4 11 12 17 18 23 24]

