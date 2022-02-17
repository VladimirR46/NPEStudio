classdef Rectangle
    %RECTANGLE Representation of a rectangle
    %    Coordinates increase in value from left to right and from top to
    %    bottom.

%   Copyright 2019 The MathWorks, Inc.
    
    % This is a class definition file, and cannot be compiled directly the way
    % that a function file can. Instead, it must be present on the MATLAB
    % search path when a function that uses it is compiled.

    properties
        % CornerA and CornerB are any two diagonally opposite corners of
        % the rectangle.
        % Here we not only define the properties but validate that:
        % (1) there is only one instance of CornerA and one instance
        % of CornerB per rectangle
        % (2) both CornerA and CornerB are scalar (1x1) Point objects
        % (3) by default, CornerA and CornerB are set to the Point (0,0)
        % If we did not want to perform validation, we could simply
        % write this:
        %   CornerA
        %   CornerB
        CornerA(1,1) Point = Point(0,0)
        CornerB(1,1) Point = Point(0,0)
    end

    methods
        function obj = Rectangle(CornerA,CornerB)
            %RECTANGLE Construct an instance of the Rectangle class

            % The arguments block is optional, but allows us to
            % validate that each of the arguments is a scalar Point object.
            arguments
                CornerA(1,1) Point
                CornerB(1,1) Point
            end
            obj.CornerA = CornerA;
            obj.CornerB = CornerB;
        end

        function ret = height(obj)
            %HEIGHT Get height of the rectangle
            ret = abs(obj.CornerB.Y - obj.CornerA.Y);
        end

        function ret = width(obj)
            %WIDTH Get height of the rectangle
            ret = abs(obj.CornerB.X - obj.CornerA.X);
        end

        function ret = area(obj)
            %AREA Obtain area of the rectangle
            ret = obj.width * obj.height;
        end

        function ret = upperLeftHandCorner(rect)
            %UPPERLEFTHANDCORNER Get upper left-hand corner of rectangle.
            ret = Point(min(rect.CornerA.X, rect.CornerB.X), ...
                min(rect.CornerA.Y, rect.CornerB.Y));
        end

        function ret = lowerLeftHandCorner(rect)
            %LOWERLEFTHANDCORNER Get lower left-hand corner of rectangle.
            ret = Point(min(rect.CornerA.X, rect.CornerB.X), ...
                max(rect.CornerA.Y, rect.CornerB.Y));
        end

        function ret = upperRightHandCorner(rect)
            %UPPERRIGHTHANDCORNER Get upper right-hand corner of rectangle.
            ret = Point(max(rect.CornerA.X, rect.CornerB.X), ...
                min(rect.CornerA.Y, rect.CornerB.Y));
        end

        function ret = lowerRightHandCorner(rect)
            %LOWERRIGHTHANDCORNER Get lower right-hand corner of rectangle.
            ret = Point(max(rect.CornerA.X, rect.CornerB.X), ...
                max(rect.CornerA.Y, rect.CornerB.Y));
        end
    end
end

