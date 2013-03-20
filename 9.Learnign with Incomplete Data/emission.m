function logEmissionProb = emission(poseData,G,P,N,K)
logEmissionProb = zeros(N,K);
  for i = 1:N
	  data = reshape(poseData(i,:,:),10,3);
	  for j = 1:10
		  if G(j,1) == 1
			  parent = data(G(j,2),:);
			  for k = 1:K
				  theta = P.clg(j).theta(k,:);
				  mu_y = sum(theta(1:4).*[1,parent]);
				  mu_x = sum(theta(5:8).*[1,parent]);
				  mu_a = sum(theta(9:12).*[1,parent]);
				  logEmissionProb(i,k) += lognormpdf(data(j,1),mu_y,P.clg(j).sigma_y(k));
				  logEmissionProb(i,k) += lognormpdf(data(j,2),mu_x,P.clg(j).sigma_x(k));
				  logEmissionProb(i,k) += lognormpdf(data(j,3),mu_a,P.clg(j).sigma_angle(k));
			  end
		  else
			  for k = 1:K
				  logEmissionProb(i,k) += lognormpdf(data(j,1),P.clg(j).mu_y(k),P.clg(j).sigma_y(k));
				  logEmissionProb(i,k) += lognormpdf(data(j,2),P.clg(j).mu_x(k),P.clg(j).sigma_x(k));
				  logEmissionProb(i,k) += lognormpdf(data(j,3),P.clg(j).mu_angle(k),P.clg(j).sigma_angle(k));
			  end
		  end
	  end
  end
  end
