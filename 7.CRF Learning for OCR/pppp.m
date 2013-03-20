function ft = pppp(feat,p1,p2,p3,p12,p23)

for i = 1:length(feat)
	if feat(i).var == 1
		feat(i).p = p1.val([feat(i).assignment]);
	end

	if feat(i).var == 2
		feat(i).p = p2.val([feat(i).assignment]);
	end

	if feat(i).var == 3
		feat(i).p = p3.val([feat(i).assignment]);
	end
	if feat(i).var == [1,2]
		feat(i).p = p12.val(AssignmentToIndex([feat(i).assignment],[26,26]));
	end
	if feat(i).var == [2,3]
		feat(i).p = p23.val(AssignmentToIndex([feat(i).assignment],[26,26]));
	end
end
ft = feat;
return;
end

