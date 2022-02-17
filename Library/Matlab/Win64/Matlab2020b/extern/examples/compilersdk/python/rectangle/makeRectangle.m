function ret = makeRectangle(CornerA, CornerB)
    %MAKERECTANGLE Construct a Rectangle object from diagonally opposite corners

%   Copyright 2019 The MathWorks, Inc.
    
    % This optional block validates that each of the arguments
    % is a scalar (1x1) Point object. By default, each corner
    % is set to the point at (0,0).
    arguments
        CornerA(1,1) Point = Point(0,0)
        CornerB(1,1) Point = Point(0,0)
    end
    ret = Rectangle(CornerA, CornerB);
end

