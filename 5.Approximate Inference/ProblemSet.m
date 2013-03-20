load('exampleIOPA5.mat');
[G,F] = ConstructToyNetwork(1.0,0.1);
%G = exampleINPUT.t8a1{1};
%F = exampleINPUT.t8a2{1};
E = exampleINPUT.t8a3{1};
TransName = exampleINPUT.t8a4{1};
mix_time = exampleINPUT.t8a5{1};
num_samples = exampleINPUT.t8a6{1};
sampling_interval= exampleINPUT.t8a7{1};
A0 = exampleINPUT.t8a8{1};
ExactMarginals = ComputeExactMarginalsBP(F,[],0);
A1 = ones(1,16);
A2 = A1*2;
[M,all_samples1] = MCMCInference(G,F,E,TransName,mix_time,num_samples,sampling_interval,A1);
[M,all_samples2] = MCMCInference(G,F,E,TransName,mix_time,num_samples,sampling_interval,A2);
samples_list{1} = all_samples1;
samples_list{2} = all_samples2;

%VisualizeMCMCMarginals(samples_list,1,G.card,F,100,ExactMarginals,"woohoo");






