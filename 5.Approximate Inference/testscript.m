switch 12
case 1
    disp('NaiveGetNextClusters')
    result = 1;
    for iter = 1:3
        disp(['iter ', num2str(iter),')'])
        [i, j] = NaiveGetNextClusters( ...
            exampleINPUT.t1a1, exampleINPUT.t1a2{iter});
        disp(['Output:   ', num2str([i, j])])
        disp(['Expected: ', num2str(exampleOUTPUT.t1{iter})])  
        disp(' ')
        result = result && isequal([i, j],exampleOUTPUT.t1{iter});
    end
case 2
    disp('CreateClusterGraph')
    P = CreateClusterGraph(exampleINPUT.t2a1, exampleINPUT.t2a2);
    result = isequal(P, exampleOUTPUT.t2);
case 3
    disp('CheckConvergence')
    result = 1;
    for iter = 1:2
        disp(['iter ', num2str(iter),')'])
        converged = CheckConvergence( ...
            exampleINPUT.t3a1{iter}, exampleINPUT.t3a2{iter});
        disp(['Output:   ', num2str(converged)])
        disp(['Expected: ', num2str(exampleOUTPUT.t3{iter})])  
        disp(' ')
        result = result && (converged == exampleOUTPUT.t3{iter});
    end    
case 4
    disp('ClusterGraphCalibrate')
    [P MESSAGES] = ClusterGraphCalibrate(exampleINPUT.t4a1, 0);
    result = isequal(P, exampleOUTPUT.t4o1) && ...
        isequal(MESSAGES, exampleOUTPUT.t4o2);
case 5
    disp('ComputeApproxMarginalsBP')
    M = ComputeApproxMarginalsBP( ...
        exampleINPUT.t5a1, exampleINPUT.t5a2);
    result = isequal(M, exampleOUTPUT.t5);        

case 6
    disp('BlockLogDistribution')
    LogBS = BlockLogDistribution(exampleINPUT.t6a1, ...
        exampleINPUT.t6a2, exampleINPUT.t6a3, exampleINPUT.t6a4);
    disp(['Output:   ', num2str(LogBS)])
    disp(['Expected: ', num2str(exampleOUTPUT.t6)]) 
    result = isequal(LogBS, exampleOUTPUT.t6);
case 7
    disp('GibbsTrans')
    randi('seed',1);
    result = 1;
    for iter = 1:10
        disp(['iter ', num2str(iter),')'])
        A = GibbsTrans(exampleINPUT.t7a1{iter}, ...
            exampleINPUT.t7a2{iter}, exampleINPUT.t7a3{iter});
        disp(['Output:   ', num2str(A)])
        disp(['Expected: ', num2str(exampleOUTPUT.t7{iter})])  
        disp(' ')
        result = result && isequal(A, exampleOUTPUT.t7{iter});
    end
case 8
    disp('MCMCInference')
    % second iteration works if exampleINPUT.t8a4{2} = MHGibbs
    result = 1;
    randi('seed',1);
    for iter = 1:2
        disp(['iter ', num2str(iter),')'])
        [M, all_samples] = MCMCInference(exampleINPUT.t8a1{iter},...
            exampleINPUT.t8a2{iter}, exampleINPUT.t8a3{iter}, ...
            exampleINPUT.t8a4{iter}, exampleINPUT.t8a5{iter}, ...
            exampleINPUT.t8a6{iter}, exampleINPUT.t8a7{iter}, ...
            exampleINPUT.t8a8{iter});
        result = isequal(M, exampleOUTPUT.t8o1{1,iter}) && ...
            isequal(all_samples, exampleOUTPUT.t8o2{iter});
    end
case 9
    disp('MHUniformTrans')
    randi('seed',1);
    result = 1;
    for iter = 1:10
        A = MHUniformTrans(exampleINPUT.t9a1{iter}, ...
            exampleINPUT.t9a2{iter}, exampleINPUT.t9a3{iter});
        if isequal(A, exampleOUTPUT.t9{iter})
            disp(['iter ', num2str(iter),') ok'])
        else
            disp(['iter ', num2str(iter),') x'])
            result = 0;
        end
    end
case 10
    disp('MHSWTrans (Variant 1)')
    randi('seed',1);
    result = 1;
    for iter = 1:10
        A = MHSWTrans(exampleINPUT.t10a1{iter}, ...
            exampleINPUT.t10a2{iter}, exampleINPUT.t10a3{iter}, ...
            exampleINPUT.t10a4{iter});
        if isequal(A, exampleOUTPUT.t10{iter})
            disp(['iter ', num2str(iter),') ok'])
        else
            disp(['iter ', num2str(iter),') x'])
            result = 0;
        end
    end
case 11
    disp('MHSWTrans (Variant 2)')
    randi('seed',1);
    result = 1;
    for iter = 1:20
        A = MHSWTrans(exampleINPUT.t11a1{iter}, ...
            exampleINPUT.t11a2{iter}, exampleINPUT.t11a3{iter}, ...
            exampleINPUT.t11a4{iter});
        if isequal(A, exampleOUTPUT.t11{iter})
            disp(['iter ', num2str(iter),') ok'])
        else
            disp(['iter ', num2str(iter),') x'])
            result = 0;
        end
    end
case 12
    disp('MCMCInference (part 2)')
    result = 1;
    randi('seed',1);
    for iter = 1:1 % check the comments for a working solution for 2nd iteration
        [M, all_samples] = MCMCInference(exampleINPUT.t12a1{iter},...
            exampleINPUT.t12a2{iter}, exampleINPUT.t12a3{iter}, ...
            exampleINPUT.t12a4{iter}, exampleINPUT.t12a5{iter}, ...
            exampleINPUT.t12a6{iter}, exampleINPUT.t12a7{iter}, ...
            exampleINPUT.t12a8{iter});
        if isequal(M, exampleOUTPUT.t12o1{iter}) && ...
            isequal(all_samples, exampleOUTPUT.t12o2{iter}),
            disp(['iter ', num2str(iter),') ok'])
        else
            disp(['iter ', num2str(iter),') x'])
            result = 0;
        end
    end

end

if result
    disp('Correct answer!')
else
    disp('Incorrect!')
end
