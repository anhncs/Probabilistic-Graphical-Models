function Img = func_DrawLine(Img, X0, Y0, X1, Y1, nG)
% Connect two pixels in an image with the desired graylevel
%
% Command line
% ------------
% result = func_DrawLine(Img, X1, Y1, X2, Y2)
% input:    Img : the original image.
%           (X1, Y1), (X2, Y2) : points to connect.
%           nG : the gray level of the line.
% output:   result
%
% Note
% ----
%   Img can be anything
%   (X1, Y1), (X2, Y2) should be NOT be OUT of the Img
%
%   The computation cost of this program is around half as Cubas's [1]
%   [1] As for Cubas's code, please refer  
%   http://www.mathworks.com/matlabcentral/fileexchange/loadFile.do?objectId=4177  
%
% Example
% -------
% result = func_DrawLine(zeros(5, 10), 2, 1, 5, 10, 1)
% result =
%      0     0     0     0     0     0     0     0     0     0
%      1     1     1     0     0     0     0     0     0     0
%      0     0     0     1     1     1     0     0     0     0
%      0     0     0     0     0     0     1     1     1     0
%      0     0     0     0     0     0     0     0     0     1
%
%
% Jing Tian Oct. 31 2000
% scuteejtian@hotmail.com
% This program is written in Oct.2000 during my postgraduate in 
% GuangZhou, P. R. China.
% Version 1.0

W = size(Img,1);
H = size(Img,2);

if(X0 > 0 && X0 <= W && Y0 >0 && Y0 <= H)
    Img(X0, Y0, :) = nG;
end
if(X1 > 0 && X1 <= W && Y1 >0 && Y1 <= H)
    Img(X1, Y1, :) = nG;
end
if abs(X1 - X0) <= abs(Y1 - Y0)
   if Y1 < Y0
      k = X1; X1 = X0; X0 = k;
      k = Y1; Y1 = Y0; Y0 = k;
   end
   if (X1 >= X0) && (Y1 >= Y0)
      dy = Y1-Y0; dx = X1-X0;
      p = 2*dx; n = 2*dy - 2*dx; tn = dy;
      while (Y0 < Y1)
         if tn >= 0
            tn = tn - p;
         else
            tn = tn + n; X0 = X0 + 1;
         end
         Y0 = Y0 + 1; 
         if(X0 >0 && X0 <= W && Y0 >0 && Y0 <= H)
            Img(X0, Y0, :) = nG;
         end
      end
   else
      dy = Y1 - Y0; dx = X1 - X0;
      p = -2*dx; n = 2*dy + 2*dx; tn = dy;
      while (Y0 <= Y1)
         if tn >= 0
            tn = tn - p;
         else
            tn = tn + n; X0 = X0 - 1;
         end
         Y0 = Y0 + 1; 
         if(X0 > 0 && X0 <= W && Y0 >0 && Y0 <= H)
            Img(X0, Y0, :) = nG;   
         end
      end
   end
else if X1 < X0
      k = X1; X1 = X0; X0 = k;
      k = Y1; Y1 = Y0; Y0 = k;
   end
   if (X1 >= X0) && (Y1 >= Y0)
      dy = Y1 - Y0; dx = X1 - X0;
      p = 2*dy; n = 2*dx-2*dy; tn = dx;
      while (X0 < X1)
         if tn >= 0
            tn = tn - p;
         else
            tn = tn + n; Y0 = Y0 + 1;
         end
         X0 = X0 + 1;   
         if(X0 > 0 && X0 <= W && Y0 >0 && Y0 <= H)
            Img(X0, Y0, :) = nG;
         end
      end
   else
      dy = Y1 - Y0; dx = X1 - X0;
      p = -2*dy; n = 2*dy + 2*dx; tn = dx;
      while (X0 < X1)
         if tn >= 0
            tn = tn - p;
         else
            tn = tn + n; Y0 = Y0 - 1;
         end
         X0 = X0 + 1; 
         if(X0 > 0 && X0 <= W && Y0 >0 && Y0 <= H)
            Img(X0, Y0, :) = nG;
         end
      end
   end
end