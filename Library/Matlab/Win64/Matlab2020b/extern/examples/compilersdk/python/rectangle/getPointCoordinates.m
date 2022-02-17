function coordinates = getPointCoordinates(pt)
    % GETPOINTCOORDINATES retrieves the coordinates of a 2D point as a struct

%   Copyright 2019 The MathWorks, Inc.

    % This optional arguments block validates that the input
    % argument must be a scalar Point object.
    arguments 
        pt(1,1) Point
    end

    coordinates.X = pt.X;
    coordinates.Y = pt.Y;
end

