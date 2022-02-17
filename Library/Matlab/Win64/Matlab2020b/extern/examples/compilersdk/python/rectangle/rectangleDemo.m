function rectangleDemo()
    % RECTANGLEDEMO Construct a rectangle and print information about it

%   Copyright 2019 The MathWorks, Inc.

    pointZeroZero = makePoint(0, 0);
    pointThreeFour = makePoint(3, 4);
    rectA = makeRectangle(pointZeroZero, pointThreeFour);
    corners = getRectangleCorners(rectA);
    showPointCoordinates(corners.upperLeft, 'Upper left-hand corner');
    showPointCoordinates(corners.lowerLeft, 'Lower left-hand corner');
    showPointCoordinates(corners.upperRight, 'Upper right-hand corner');
    showPointCoordinates(corners.lowerRight, 'Lower right-hand corner');
    fprintf('Area: %.1f\n', area(rectA));
    fprintf('Height: %.1f\n', height(rectA));
    fprintf('Width: %.1f\n', width(rectA));
end

% This is an auxiliary function. It cannot be called outside rectangleDemo().
function showPointCoordinates(pt, desc)
    coordinates = getPointCoordinates(pt);
    fprintf('%s: (%.1f, %.1f)\n', desc, coordinates.X, coordinates.Y);
end
