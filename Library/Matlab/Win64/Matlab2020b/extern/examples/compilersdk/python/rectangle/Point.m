classdef Point
    %POINT 2D representation of a point
    %    Coordinates increase in value from left to right and from top to
    %    bottom.

%   Copyright 2019 The MathWorks, Inc.

    % This is a class definition file, and cannot be compiled directly the way
    % that a function file can. Instead, it must be present on the MATLAB
    % search path when a function that uses it is compiled.

    % This optional block validates that each of the properties
    % is a scalar (1x1) real finite double, and defaults to 0.
    properties
        X(1,1) double {mustBeReal, mustBeFinite} = 0
        Y(1,1) double {mustBeReal, mustBeFinite} = 0
    end

    methods
        function obj = Point(inputX,inputY)
            %POINT Construct an instance of this class
            obj.X = inputX;
            obj.Y = inputY;
        end
    end
end

