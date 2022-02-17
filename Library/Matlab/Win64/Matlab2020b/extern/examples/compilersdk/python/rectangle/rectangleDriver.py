# Copyright 2019 The MathWorks, Inc.
import rectangleLib

def initializeLibrary():
    """ Initialize MATLAB rectangleLibrary library and retrieve a handle to it """
    handle = rectangleLib.initialize()
    return handle

class PyPoint:
    """Python class that uses a MATLAB class compiled with MATLAB Compiler SDK"""
    libHandle = 0

    @staticmethod
    def initializeHandle(handle):
        PyPoint.libHandle = handle

    # Initialize the point either from an existing Point (not PyPoint) object,
    # or from an X and Y pair.
    def __init__(self, pt=None, X=None, Y=None):
        if pt:
            if X or Y:
                raise ValueError('If pt is specified, neither X nor Y can be specified')
            else:
                self.data = pt
        else:
            if not X or not Y:
                raise ValueError('If pt is not specified, X and Y must be specified')
            else:
                self.data = PyPoint.libHandle.makePoint(X, Y)
    
    def getCoordinates(self):
        coordinates = PyPoint.libHandle.getPointCoordinates(self.data)
        return (coordinates['X'], coordinates['Y'])
        
class PyRectangle:
    """Python class that uses a MATLAB class compiled with MATLAB Compiler SDK"""

    # Class member that must be initialized before any PyRectangle object is instantiated
    libHandle = 0
    
    @staticmethod
    def initializeHandle(handle):
        PyRectangle.libHandle = handle
    
    # Initialize the point either from an existing Rectangle (not PyRectangle) object,
    # or from the coordinates of any two diagonally opposite corners.
    def __init__(self, rect=None, cornerAX=None, cornerAY=None, cornerBX=None, cornerBY=None):
        if rect:
            if cornerAX or cornerAY or cornerBX or cornerBY:
                raise ValueError(
                    'If rect is specified, no other keyword argument can be specified')
            else:
                self.data = rect
        else:
            if not cornerAX or not cornerAY or not cornerBX or not cornerBY:
                raise ValueError(
                    'If rect is not specified, the other keyword arguments must be specified')
            else:
                self.data = PyRectangle.libHandle.makeRectangle(
                    PyRectangle.libHandle.makePoint(cornerAX, cornerAY),
                    PyRectangle.libHandle.makePoint(cornerBX, cornerBY))

    def area(self):
        return PyRectangle.libHandle.getRectangleArea(self.data)
    
    def height(self):
        return PyRectangle.libHandle.getRectangleHeight(self.data)
        
    def width(self):
        return PyRectangle.libHandle.getRectangleWidth(self.data)
        
    def getCorners(self):
        return PyRectangle.libHandle.getRectangleCorners(self.data)
        
if __name__ == "__main__":

    libHandle = initializeLibrary()
    
    PyPoint.initializeHandle(libHandle)
    PyRectangle.initializeHandle(libHandle)
   
    print('Initializing module and creating a rectangle with corners (-1, 2) and (5, 10)...')
    rectA = PyRectangle(cornerAX=-1, cornerAY=2, cornerBX=5, cornerBY=10)
    print('Area: {0:.1f}'.format(rectA.area()))
    print('Height: {0:.1f}'.format(rectA.height()))
    print('Width: {0:.1f}'.format(rectA.width()))

    print('Corners:')
    corners = rectA.getCorners()
    for label in ('upperLeft', 'lowerLeft', 'upperRight', 'lowerRight'):
        pypoint = PyPoint(pt=corners[label])
        print('    {0}: {1}'.format(label, pypoint.getCoordinates()))

    print('')
    print('Executing rectangleDemo...')
    # nargout represents the number of output arguments. Its default value
    # is 1. rectangleDemo returns no output, so we need to explicitly set 
    # nargout to 0.
    libHandle.rectangleDemo(nargout=0)
