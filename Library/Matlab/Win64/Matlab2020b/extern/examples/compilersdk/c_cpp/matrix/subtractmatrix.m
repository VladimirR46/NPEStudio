function [afinal, astruct, acell] = subtractmatrix(a1, a2)
%SUBTRACTMATRIX Subtract two matrices and return result with other information
%    This function takes two matrices as input and subtracts the second from 
%    the first. It returns the result, along with the original matrices and the
%    number of elements. The function is used to demonstrate the functionality 
%    of MATLAB Compiler SDK. Refer to the documentation for MATLAB Compiler SDK 
%    C/C++ Shared Library for more information.

%   Copyright 2020 The MathWorks, Inc.

afinal = a1 - a2;
astruct.elem1 = a1;
astruct.elem2 = a2;
% acell is a cell array containing a char vector and a number.
acell = {'Elements in astruct.elem1', numel(astruct.elem1)};
disp('Original matrices stored as the elements of a struct:');
disp('astruct.elem1');
disp(astruct.elem1);
disp('astruct.elem2');
disp(astruct.elem2);
disp('Final matrix:');
disp(afinal);
disp('Cell array:');
disp(acell);
