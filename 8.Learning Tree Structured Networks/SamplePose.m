function sample = SamplePose(P, G, label)

% sample from the distribution specified by P and G,
% label == 0: class label unknown
% label == k: class label = k; k=1,2,3, ...
%
% Copyright (C) Daphne Koller, Stanford Univerity, 2012

sample = zeros(10,3);

if label == 0
    k = SampleMultinomial(P.c); % sample label using P.c
else
    k = label;
end

visited = zeros(10,1);

while sum(visited) < 10
    
    for i=1:10 % for each body part
        
        par = G(i,2); % parent body part, if exists
        
        if G(i,1) == 0 % parametrized by (2),(3),(4)
            
            muy = P.clg(i).mu_y(k);
            mux = P.clg(i).mu_x(k);
            muangle = P.clg(i).mu_angle(k);
            sample(i,1) = SampleGaussian(muy, P.clg(i).sigma_y(k));
            sample(i,2) = SampleGaussian(mux, P.clg(i).sigma_x(k));
            sample(i,3) = SampleGaussian(muangle, P.clg(i).sigma_angle(k));
            visited(i) = 1;
            
        elseif G(i,1) == 1 % parametrized by (5),(6),(7)
            
            if visited(par)
                
                muy = P.clg(i).theta(k,1) + ...
                    P.clg(i).theta(k,2) * sample(par,1) + ...
                    P.clg(i).theta(k,3) * sample(par,2) + ...
                    P.clg(i).theta(k,4) * sample(par,3);
                mux = P.clg(i).theta(k,5) + ...
                    P.clg(i).theta(k,6) * sample(par,1) + ...
                    P.clg(i).theta(k,7) * sample(par,2) + ...
                    P.clg(i).theta(k,8) * sample(par,3);
                muangle = P.clg(i).theta(k,9) + ...
                    P.clg(i).theta(k,10) * sample(par,1) + ...
                    P.clg(i).theta(k,11) * sample(par,2) + ...
                    P.clg(i).theta(k,12) * sample(par,3);
                sample(i,1) = SampleGaussian(muy, P.clg(i).sigma_y(k));
                sample(i,2) = SampleGaussian(mux, P.clg(i).sigma_x(k));
                sample(i,3) = SampleGaussian(muangle, P.clg(i).sigma_angle(k));
                visited(i) = 1;
            end
            
        elseif G(i,1) == 2 % parametrized by (8),(9),(10)
            
            if visited(par)
                
                muy = P.clg(i).gamma(k,1) + ...
                    P.clg(i).gamma(k,2) * sample(par,1) + ...
                    P.clg(i).gamma(k,3) * sample(par,2) + ...
                    P.clg(i).gamma(k,4) * sin(sample(par,3)) + ...
                    P.clg(i).gamma(k,5) * cos(sample(par,3));
                mux = P.clg(i).gamma(k,6) + ...
                    P.clg(i).gamma(k,7) * sample(par,1) + ...
                    P.clg(i).gamma(k,8) * sample(par,2) + ...
                    P.clg(i).gamma(k,9) * sin(sample(par,3)) + ...
                    P.clg(i).gamma(k,10) * cos(sample(par,3));
                muangle = P.clg(i).gamma(k,11) + ...
                    P.clg(i).gamma(k,12) * sample(par,1) + ...
                    P.clg(i).gamma(k,13) * sample(par,2) + ...
                    P.clg(i).gamma(k,14) * sample(par,3);
                sample(i,1) = SampleGaussian(muy, P.clg(i).sigma_y(k));
                sample(i,2) = SampleGaussian(mux, P.clg(i).sigma_x(k));
                sample(i,3) = SampleGaussian(muangle, P.clg(i).sigma_angle(k));
                visited(i) = 1;
            end
        end
        
    end
end

