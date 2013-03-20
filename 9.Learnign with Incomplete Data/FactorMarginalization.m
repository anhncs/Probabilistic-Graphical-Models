%   FactorMarginalization Sums given variables out of a factor in log space.
%   B = FactorMarginalization(A,V) computes the factor with the variables
%   in V summed out. The factor data structure has the following fields:
%       .var    Vector of variables in the factor, e.g. [1 2 3]
%       .card   Vector of cardinalities corresponding to .var, e.g. [2 2 2]
%       .val    Value table of size prod(.card)
%
%   The resultant factor should have at least one variable remaining or this
%   function will throw an error.
%
% Copyright (C) Daphne Koller, Stanford Univerity, 2012

function B = FactorMarginalization(A,V)
B.var = A.var(2);
B.card = A.card(1);
Val = reshape(A.val,B.card,B.card);

if(V==A.var(2))
    Val = Val';
    B.var = A.var(1);
end

B.val = log(sum(exp(bsxfun(@minus, Val, max(Val)))))+max(Val);
