function FOut = ReOrderVariables( FIn, NewVarOrder )

    %Check that new variable order is OK:
    [isMem MAP] = ismember( NewVarOrder, FIn.var );
    assert( length(FIn.var) == length(NewVarOrder), 'Different # variables!' );
    assert( all(isMem), 'Some of new variables not in factor!' );
    assert( length(NewVarOrder) == length(unique(NewVarOrder)), 'Some variables repeated!');

    %Set up variables and cardinality; preallocate
    FOut.var = NewVarOrder;
    FOut.card = FIn.card( MAP );
    FOut.val = ones(1,prod(FIn.card));

    for i=1:prod(FIn.card)
        %Loop through every value in FIn, and copy to FOut as approprate
        OldAssignment = IndexToAssignment(i, FIn.card);
        NewAssignment = OldAssignment( MAP );

        newIdx = AssignmentToIndex( NewAssignment, FOut.card );
        FOut.val( newIdx ) = FIn.val( i );
    end
end

