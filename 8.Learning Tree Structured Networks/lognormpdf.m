%
% Copyright (C) Daphne Koller, Stanford Univerity, 2012
function val = lognormpdf(x, mu, sigma)
val = - (x - mu).^2 / (2*sigma^2) - log (sqrt(2*pi) * sigma);