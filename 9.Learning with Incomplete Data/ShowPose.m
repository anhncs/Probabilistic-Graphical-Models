% File: ShowPose.m
%
% Copyright (C) Daphne Koller, Stanford Univerity, 2012

% visualize a configuration the body parts

function img = ShowPose( pose )

% pose 10 x 3. 10 : body parts,
%               3 : y, x, alpha

pose(:,1) = pose(:,1) + 100;
pose(:,2) = pose(:,2) + 150;



pose = reshape(pose, [10 3]);
part_length = [60, 20, 32, 33, 32, 33, 46, 49, 46, 49];
part_width = [18, 10, 7, 5, 7, 5, 10, 7, 10, 7]; 
img = zeros(300, 300);
for part = 1:10
    
    startpt = round(pose(part, 1:2));
    axis = [sin(pose(part,3) - pi/2) cos(pose(part,3) - pi/2)];
    xaxis = [cos(pose(part,3) - pi/2) -sin(pose(part,3) - pi/2)];
    endpt = round(startpt + part_length(part) * axis);
    
    corner1 = round(startpt + xaxis * part_width(part));
    corner2 = round(startpt - xaxis * part_width(part));
    corner3 = round(endpt + xaxis * part_width(part));
    corner4 = round(endpt - xaxis * part_width(part));
    
   img = func_DrawLine(img, corner1(1), corner1(2), corner2(1), corner2(2),1);
   img = func_DrawLine(img, corner1(1), corner1(2), corner3(1), corner3(2),1);
   img = func_DrawLine(img, corner4(1), corner4(2), corner2(1), corner2(2),1);
   img = func_DrawLine(img, corner4(1), corner4(2), corner3(1), corner3(2),1);
   
   if startpt(1) > 3 && startpt(1) < 298 && startpt(2) > 3 && startpt(2) < 298
       img(startpt(1)-3 : startpt(1) + 3, startpt(2) - 3 : startpt(2)+3) = ones(7,7);
   end
end

