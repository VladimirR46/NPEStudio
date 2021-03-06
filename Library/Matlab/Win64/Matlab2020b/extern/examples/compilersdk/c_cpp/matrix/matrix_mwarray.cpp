/*==============================================================
 *
 * MATRIX_MWARRAY.CPP
 * Sample driver code that calls a C++ mwArray shared library 
 * created using the MATLAB Compiler SDK. Refer to the MATLAB 
 * Compiler SDK documentation for more information.
 *
 * Copyright 1984-2019 The MathWorks, Inc.
 *
 *============================================================*/

// Include the library specific header file as generated by the 
// MATLAB Compiler SDK
#include "libmatrix.h"

int run_main(int argc, const char **argv)
{
    if( !libmatrixInitialize() )
    {
        std::cerr << "Could not initialize the library properly"
                   << std::endl;
        return -1;
    }
    else
    {
        try
        {
            // Create input data
            double data[] = {1,2,3,4,5,6,7,8,9};
            mwArray in1(3, 3, mxDOUBLE_CLASS, mxREAL);
            mwArray in2(3, 3, mxDOUBLE_CLASS, mxREAL);
            in1.SetData(data, 9);
            in2.SetData(data, 9);
            
            // Create output array
            mwArray out;
            
            // Call the library function
            addmatrix(1, out, in1, in2);
            
            // Display the return value of the library function
            std::cout << "The sum of the matrix with itself is:" << std::endl;
            std::cout << out << std::endl;
            
            multiplymatrix(1, out, in1, in2);
            std::cout << "The product of the matrix with itself is:" 
                      << std::endl;
            std::cout << out << std::endl;
            
            eigmatrix(1, out, in1);
            std::cout << "The eigenvalues of the original matrix are:"
                      << std::endl;
            std::cout << out << std::endl;
        }
        catch (const mwException& e)
        {
            std::cerr << e.what() << std::endl;
            return -2;
        }
        catch (...)
        {
            std::cerr << "Unexpected error thrown" << std::endl;
            return -3;
        }     
        // Call the application and library termination routine
        libmatrixTerminate();
    }
    // mclTerminateApplication shuts down the MATLAB Runtime.
    // You cannot restart it by calling mclInitializeApplication.
    // Call mclTerminateApplication once and only once in your application.
    mclTerminateApplication();
    return 0;
}

// The main routine. On the Mac, the main thread runs the system code, and
// user code must be processed by a secondary thread. On other platforms, 
// the main thread runs both the system code and the user code.
int main(int argc, const char **argv)
{
    // Call application and library initialization. Perform this 
    // initialization before calling any API functions or
    // Compiler SDK-generated libraries.
    if (!mclInitializeApplication(nullptr, 0)) 
    {
        std::cerr << "Could not initialize the application properly"
                  << std::endl;
        return -1;
    }

    return mclRunMain(static_cast<mclMainFcnType>(run_main), argc, argv);
}
