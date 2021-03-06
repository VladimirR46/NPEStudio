/*==============================================================
 *
 * MATRIX_MDA.CPP
 * Sample driver code that uses the MATLAB Data API interface 
 * (introduced in R2018a) and MATLAB Data API to call a C++ 
 * shared library created using the MATLAB Compiler SDK.
 * Demonstrates passing matrices via the MATLAB Data API.
 * Refer to the MATLAB Compiler SDK documentation for more 
 * information.
 *
 * Copyright 2017-2019 The MathWorks, Inc.
 *
 *============================================================*/

// Include the header file required to use the MATLAB Data API
// interface for the C++ shared library generated by the
// MATLAB Compiler SDK.
#include "MatlabCppSharedLib.hpp"
#include <iostream>
#include <numeric> // for iota

namespace mc = matlab::cpplib;
namespace md = matlab::data;

std::u16string convertAsciiToUtf16(const std::string & asciiStr);

template <typename T>
void writeMatrix(std::ostream & ostr, const md::TypedArray<T> & matrix, 
    md::MemoryLayout layoutOfArray = md::MemoryLayout::ROW_MAJOR);

int mainFunc(std::shared_ptr<mc::MATLABApplication> app,
    const int argc, const char * argv[]);

// The main routine. On the Mac, the main thread runs the system code, and
// user code must be processed by a secondary thread. On other platforms, 
// the main thread runs both the system code and the user code.
int main(const int argc, const char * argv[])
{
    int ret = 0;
    try {
        auto mode = mc::MATLABApplicationMode::IN_PROCESS;
        const std::string STR_OPTIONS = "-nojvm";
        const std::u16string U16STR_OPTIONS = convertAsciiToUtf16(STR_OPTIONS);
        std::vector<std::u16string> options = {U16STR_OPTIONS};
        auto matlabApplication = mc::initMATLABApplication(mode, options);
        ret = mc::runMain(mainFunc, std::move(matlabApplication),  argc, argv);
        // Calling reset() on matlabApplication allows the user to control
        // when it is destroyed, which automatically cleans up its resources.
        // Here, the object would go out of scope and be destroyed at the end 
        // of the block anyway, even if reset() were not called.
        // Whether the matlabApplication object is explicitly or implicitly
        // destroyed, initMATLABApplication() cannot be called again within
        // the same process.
        matlabApplication.reset();
    } catch(const std::exception & exc) {
        std::cerr << exc.what() << std::endl;
        return -1;
    }
    return ret;
}

int mainFunc(std::shared_ptr<mc::MATLABApplication> app,
    const int argc, const char * argv[])
{
    try {
        // If using a compiler that supports the u"" prefix to indicate
        // a char16_t *, you could simply pass u"libmatrix.ctf" as
        // the second parameter to initMATLABLibrary(), and would
        // not need to perform an extra step to convert from a
        // narrow string. Visual C++ 2013 does not support the u""
        // prefix, but later versions of Visual C++ do, as do other
        // third-party compilers supported for use with MATLAB.
        const std::string STR_CTF_NAME = "libmatrix.ctf";
        const std::u16string U16STR_CTF_NAME = convertAsciiToUtf16(STR_CTF_NAME);
        
        // The path to the CTF (library archive file) passed to 
        // initMATLABLibrary or initMATLABLibraryAsync may be either absolute
        // or relative. If it is relative, the following will be prepended
        // to it, in turn, in order to find the CTF:
        // - the directory named by the environment variable 
        // CPPSHARED_BASE_CTF_PATH, if defined
        // - the working directory
        // - the directory where the executable is located
        // - on Mac, the directory three levels above the directory
        // where the executable is located
        
        // If the CTF is not in one of these locations, do one of the following:
        // - copy the CTF
        // - move the CTF
        // - change the working directory ("cd") to the location of the CTF
        // - set the environment variable to the location of the CTF
        // - edit the code to change the path
        auto lib = mc::initMATLABLibrary(app, U16STR_CTF_NAME);
        md::ArrayFactory factory;
        const size_t NUM_ROWS = 3;
        const size_t NUM_COLS = 3;
        md::TypedArray<double> doubles = factory.createArray<double>({NUM_ROWS, NUM_COLS}, 
            {1.0, 2.0, 3.0,
             4.0, 5.0, 6.0,
             7.0, 8.0, 9.0}); 
             
        // Note that the matrix is interpreted as being in column-major order 
        // (the MATLAB convention) rather than row-major order (the C++ 
        // convention). Thus, the output from the next two lines of code will 
        // look like this:
        //     The original matrix is:
        //     1 4 7
        //     2 5 8
        //     3 6 9
        // If you want to work with a matrix that looks like this:
        //   1 2 3
        //   4 5 6
        //   7 8 9
        // you can either store the data as follows:
        //   md::TypedArray<double> doubles = 
        //     factory.createArray<double>({NUM_ROWS, NUM_COLS},
        //       {1.0, 4.0, 7.0,
        //        2.0, 5.0, 8.0,
        //        3.0, 6.0, 9.0}); 
        // or apply the MATLAB transpose function to the original matrix.
        std::cout << "The original matrix is: " << std::endl;
        writeMatrix<double>(std::cout, doubles);
        
        std::vector<md::Array> matrices{doubles, doubles};
        std::cout << "The sum of the matrix with itself is: " << std::endl;
        auto sum = lib->feval("addmatrix", 1, matrices);
        // The feval call returns a vector (of length 1) of md::Array objects.
        writeMatrix<double>(std::cout, sum[0]);

        std::cout << "The product of the matrix with itself is: " << std::endl;
        auto product = lib->feval("multiplymatrix", 1, matrices);
        writeMatrix<double>(std::cout, product[0]);

        std::cout << "The eigenvalues of the original matrix are: " << std::endl;
        std::vector<md::Array>single_matrix{doubles};
        auto eigenvalues = lib->feval("eigmatrix", 1, single_matrix);
        writeMatrix<double>(std::cout, eigenvalues[0]);

        // This part of the code shows how createBuffer and createArrayFromBuffer
        // can be used to convert from row-major to column-major order.
        auto colMajorMatrixBuffer = factory.createBuffer<int>(6);
        // The following call writes the values 100, 101, 102, 103, 104, 105
        // into colMajorMatrixBuffer.
        std::iota(colMajorMatrixBuffer.get(), colMajorMatrixBuffer.get() + 6, 100);
        auto colMajorMatrixArray = factory.createArrayFromBuffer({2, 3},
            std::move(colMajorMatrixBuffer), md::MemoryLayout::COLUMN_MAJOR);
        // OUTPUT:
        // The original contents of the column-major matrix are:
        //      100 102 104
        //      101 103 105
        std::cout << "The original contents of the column-major matrix are: " << std::endl;
        writeMatrix<int>(std::cout, colMajorMatrixArray);
        std::vector<md::Array> colMajorMatrixArrays{colMajorMatrixArray,
            colMajorMatrixArray};
        
        // OUTPUT:
        // The sum of the column-major matrix with itself is:
        // 200 204 208
        // 202 206 210
        std::cout << "The sum of the column-major matrix with itself is: " << std::endl;
        auto sumOfColMajorMatrixArrays = lib->feval("addmatrix", 1, colMajorMatrixArrays);
        // The feval call returns a vector (of length 1) of md::Array objects.
        writeMatrix<int>(std::cout, sumOfColMajorMatrixArrays[0]);
        
        auto rowMajorMatrixBuffer = factory.createBuffer<int>(6);
        std::iota(rowMajorMatrixBuffer.get(), rowMajorMatrixBuffer.get() + 6, 100);
        auto rowMajorMatrixArray = factory.createArrayFromBuffer({3, 2},
            std::move(rowMajorMatrixBuffer), md::MemoryLayout::ROW_MAJOR);
        // OUTPUT:
        // The original contents of the row-major matrix are:
        // 100 101
        // 102 103
        // 104 105
        std::cout << "The original contents of the row-major matrix are: " << std::endl;
        writeMatrix<int>(std::cout, rowMajorMatrixArray);
        std::vector<md::Array> rowMajorMatrixArrays{rowMajorMatrixArray, rowMajorMatrixArray};
        
        // OUTPUT:
        // The sum of the row-major matrix with itself is:
        // 200 202
        // 204 206
        // 208 210
        std::cout << "The sum of the row-major matrix with itself is: " << std::endl;
        auto sumOfRowMajorMatrixArrays = lib->feval("addmatrix", 1, rowMajorMatrixArrays);
        // The feval call returns a vector (of length 1) of md::Array objects.
        writeMatrix<int>(std::cout, sumOfRowMajorMatrixArrays[0]);
    } catch(const std::exception & exc) {
        std::cerr << exc.what() << std::endl;
        return -1;
    }
    return 0;
}

std::u16string convertAsciiToUtf16(const std::string & asciiStr)
{
    return std::u16string(asciiStr.cbegin(), asciiStr.cend());
}

template <typename T>
void writeMatrix(std::ostream & ostr, const md::TypedArray<T> & matrix, 
    md::MemoryLayout layoutOfArray /*= md::MemoryLayout::ROW_MAJOR*/)
{
    md::ArrayDimensions dims = matrix.getDimensions();
    if (dims.size() != 2)
    {
        std::ostringstream ostrstrm;
        ostrstrm << "Number of dimensions must be 2; actual number: " << dims.size();
        throw std::runtime_error(ostrstrm.str());
    }

    switch(layoutOfArray)
    {
        case md::MemoryLayout::ROW_MAJOR:
            for (size_t row = 0; row < dims[0]; ++row)
            {
                for (size_t col = 0; col < dims[1]; ++col)
                {
                    std::cout << matrix[row][col] << " ";
                }
                std::cout << std::endl;
            }
            break;

        case md::MemoryLayout::COLUMN_MAJOR:
            for (size_t col = 0; col < dims[1]; ++col)
            {
                for (size_t row = 0; row < dims[0]; ++row)
                {
                    std::cout << matrix[row][col] << " ";
                }
                std::cout << std::endl;
            }
            break;

        default:
            std::cout << "WARNING: invalid layout passed to writeMatrix." << std::endl;
            break;
    }
    std::cout << std::endl;
}

