function ret = getRectangleHeight(rect)
%GETRECTANGLEHEIGHT Get area of rectangle

%   Copyright 2019 The MathWorks, Inc.

    arguments
        % This optional validation block indicates that the function
        % must take a single (scalar) rectangle.
        rect(1,1) Rectangle
    end

ret = rect.height();
end

