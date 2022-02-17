function corners = getRectangleCorners(rect)
    %GETRECTANGLECORNERS Get corners of a rectangle as a struct

%   Copyright 2019 The MathWorks, Inc.

    arguments
        % This optional validation block indicates that the function
        % must take a single (scalar) rectangle.
        rect(1,1) Rectangle
    end

    corners.upperLeft = rect.upperLeftHandCorner();
    corners.lowerLeft = rect.lowerLeftHandCorner();
    corners.upperRight = rect.upperRightHandCorner();
    corners.lowerRight = rect.lowerRightHandCorner();
end


