% Copyright (C) Daphne Koller, Stanford University, 2012

function [CPD] = CPDFromFactor(F, Y)
  nvars = length(F.var);

  % Reorder the var, card and val fields of Fnew so that the last var is the 
  % child variable.
  Fnew = F;
  YIndexInF = find(F.var == Y);
  this.card = F.card( YIndexInF );
  
  % Parents is a dummy factor
  Parents.var = F.var(find(F.var ~= Y));
  Parents.card = F.card(find(F.var ~= Y));
  Parents.val = ones(prod(Parents.card),1);

  Fnew.var = [Parents.var Y];
  Fnew.card = [Parents.card this.card];
  for i=1:length(F.val)
    A = IndexToAssignment(i, F.card);
    y = A(YIndexInF);
    A( YIndexInF ) = [];
    A = [A y];
    j = AssignmentToIndex(A, Fnew.card);
    Fnew.val(j) = F.val(i);
  end
  
  % For each assignment of Parents...
  for i=1:length(Parents.val)
    
    A = IndexToAssignment(i, Parents.card);
    SumValuesForA = 0;
    for j=1:this.card
      A_augmented = [A j];
      idx = AssignmentToIndex(A_augmented, Fnew.card);
      SumValuesForA = SumValuesForA + Fnew.val( idx );
    end
    
    for j=1:this.card
      A_augmented = [A j];
      idx = AssignmentToIndex(A_augmented, Fnew.card);
      Fnew.val( idx ) = Fnew.val( idx )  / SumValuesForA;
    end
    
  end

  CPD = Fnew;