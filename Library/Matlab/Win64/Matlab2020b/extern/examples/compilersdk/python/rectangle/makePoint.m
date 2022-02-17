function ret = makePoint(X, Y)
    %MAKEPOINT Construct a Point object

%   Copyright 2019 The MathWorks, Inc.
    
    % This optional block validates that each of the arguments
    % is a scalar (1x1) real finite double.
    arguments
        X(1,1) double {mustBeReal, mustBeFinite}
        Y(1,1) double {mustBeReal, mustBeFinite}
    end
    ret = Point(X, Y);
end

