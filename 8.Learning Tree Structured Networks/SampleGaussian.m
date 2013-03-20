function sample = SampleGaussian(mu, sigma)

% sample from the Gaussian distribution specifed by mean value mu and standard deviation sigma
%
% Copyright (C) Daphne Koller, Stanford Univerity, 2012

 sample = mu + sigma*randn(1,1);
