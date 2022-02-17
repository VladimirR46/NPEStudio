/* Copyright 2020 The MathWorks, Inc. */

#ifndef MATLABTYPESINTERFACE_HPP
#define MATLABTYPESINTERFACE_HPP

#include "MatlabDataArray.hpp"

#ifdef ENGINE_APP

#include "MatlabEngine.hpp"
typedef matlab::engine::MATLABEngine MATLABControllerType;

// Start a MATLAB Engine
std::shared_ptr<matlab::engine::MATLABEngine> setupMatlab(const char *pathName)
{
    std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr(matlab::engine::startMATLAB());

    matlab::data::ArrayFactory arrayFactory;
    std::vector<matlab::data::Array> args = { arrayFactory.createCharArray(pathName) };
    matlabPtr->feval(u"addpath", args);

    return matlabPtr;
}

#else

#include "MatlabCppSharedLib.hpp"
typedef matlab::cpplib::MATLABLibrary MATLABControllerType;

std::shared_ptr<matlab::cpplib::MATLABApplication> setupCompiledApplication()
{
    auto mode = matlab::cpplib::MATLABApplicationMode::IN_PROCESS;
    std::vector<std::u16string> OPTIONS = {u"-nojvm"};
    return matlab::cpplib::initMATLABApplication(mode, OPTIONS);
}

std::shared_ptr<MATLABControllerType> setupMatlab(std::shared_ptr<matlab::cpplib::MATLABApplication>& app, const std::string &ctfName)
{
    auto libPtr = matlab::cpplib::initMATLABLibrary(app, std::u16string(ctfName.cbegin(), ctfName.cend()));
    return std::shared_ptr<MATLABControllerType>(std::move(libPtr));
}
#endif

template<typename ControllerType>
class MATLABObject {
public:
    operator matlab::data::Array() {
        return m_object;
    }

    MATLABObject() {}

    MATLABObject(std::shared_ptr<ControllerType> matlabPtr, std::u16string clsName) : m_matlabPtr(matlabPtr) {
        m_object = MATLABCallDefaultConstructor(clsName);
    }

    MATLABObject(std::shared_ptr<ControllerType> matlabPtr, matlab::data::Array obj) : m_matlabPtr(matlabPtr), m_object(obj) {}

protected:
    std::shared_ptr<ControllerType> m_matlabPtr = nullptr;
    matlab::data::Array m_object = matlab::data::Array();

    matlab::data::Array MATLABGetObject() { return m_object; }

    template<typename T>
    inline T MATLABGetProperty(std::u16string propName);

    template<typename T>
    inline void MATLABSetProperty(std::u16string propName, T value);

    template<typename T>
    inline std::vector<T> MATLABGetObjetArrayProperty(std::u16string propName)
    {
        matlab::data::Array objArray = MATLABGetArrayProperty(propName);
        size_t numel = objArray.getNumberOfElements();
        std::vector<T> objVec(numel);
        matlab::data::ArrayFactory arrayFactory;
        matlab::data::StructArray subsArray = arrayFactory.createStructArray({ 1, 1 }, { "type", "subs" });
        subsArray[0]["type"] = arrayFactory.createCharArray("()");
        subsArray[0]["subs"] = arrayFactory.createScalar<double>(0);
        for (size_t k = 0; k < numel; k++) {
            matlab::data::Reference<matlab::data::TypedArray<double>> subs = subsArray[0]["subs"];
            subs[0] = (double)k;
            matlab::data::Array objScalar = m_matlabPtr->feval(u"subsref", { objArray, subsArray });
            objVec[k] = T(m_matlabPtr, objScalar);
        }
    }

    template<typename T>
    inline void MATLABSetObjectArrayProperty(std::u16string propName, std::vector<T> value)
    {
        std::vector<matlab::data::Array> arrayList(value.size());
        for (size_t k = 0; k < value.size(); k++) {
            arrayList[k] = value[k];
        }
        matlab::data::Array objArray = m_matlabPtr->feval(u"vertcat", arrayList);
        MATLABSetArrayProperty(propName, objArray);
    }

    // Helper functions for calling methods
    inline void MATLABCallNoOutputMethod(std::u16string fcn, std::vector<matlab::data::Array> args)
    {
        m_matlabPtr->feval(fcn, 0, args);
    }

    inline matlab::data::Array MATLABCallOneOutputMethod(std::u16string fcn, std::vector<matlab::data::Array> args)
    {
        return m_matlabPtr->feval(fcn, args);
    }

    inline std::vector<matlab::data::Array> MATLABCallMultiOutputMethod(std::u16string fcn, size_t nOut, std::vector<matlab::data::Array> args)
    {
        return m_matlabPtr->feval(fcn, nOut, args);
    }

private:
    inline matlab::data::Array MATLABCallDefaultConstructor(std::u16string clsName)
    {
        std::vector<matlab::data::Array> args;
        matlab::data::Array obj = m_matlabPtr->feval(clsName, args);
        return obj;
    }

    inline matlab::data::Array MATLABGetArrayProperty(std::u16string propName)
    {
        matlab::data::ArrayFactory arrayFactory;

        matlab::data::StructArray subsArray = arrayFactory.createStructArray({ 1, 1 }, { "type", "subs" });
        subsArray[0]["type"] = arrayFactory.createCharArray(".");
        subsArray[0]["subs"] = arrayFactory.createCharArray(propName);
        return m_matlabPtr->feval(u"subsref", { m_object, subsArray });
    }

    inline void MATLABSetArrayProperty(std::u16string propName, matlab::data::Array value)
    {
        matlab::data::ArrayFactory arrayFactory;

        matlab::data::StructArray subsArray = arrayFactory.createStructArray({ 1, 1 }, { "type", "subs" });
        subsArray[0]["type"] = arrayFactory.createCharArray(".");
        subsArray[0]["subs"] = arrayFactory.createCharArray(propName);
        m_object = m_matlabPtr->feval(u"subsasgn", { m_object, subsArray, value });
    }

};

    // 1x1 property types
    template<>
    template<>
    inline double MATLABObject<MATLABControllerType>::MATLABGetProperty(std::u16string propName)
    {
        matlab::data::TypedArray<double> value = MATLABObject<MATLABControllerType>::MATLABGetArrayProperty(propName);
        return value[0];
    }

    template<>
    template<>
    inline void MATLABObject<MATLABControllerType>::MATLABSetProperty(std::u16string propName, double value)
    {
        matlab::data::ArrayFactory arrayFactory;
        MATLABObject<MATLABControllerType>::MATLABSetArrayProperty(propName, arrayFactory.createScalar<double>(value));
    }
    
    template<>
    template<>
    inline std::complex<double> MATLABObject<MATLABControllerType>::MATLABGetProperty(std::u16string propName)
    {
        matlab::data::TypedArray<std::complex<double>> value = MATLABObject<MATLABControllerType>::MATLABGetArrayProperty(propName);
        return value[0];
    }

    template<>
    template<>
    inline void MATLABObject<MATLABControllerType>::MATLABSetProperty(std::u16string propName, std::complex<double> value)
    {
        matlab::data::ArrayFactory arrayFactory;
        MATLABObject<MATLABControllerType>::MATLABSetArrayProperty(propName, arrayFactory.createScalar<std::complex<double>>(value));
    }

    template<>
    template<>
    inline float MATLABObject<MATLABControllerType>::MATLABGetProperty(std::u16string propName)
    {
        matlab::data::TypedArray<float> value = MATLABObject<MATLABControllerType>::MATLABGetArrayProperty(propName);
        return value[0];
    }

    template<>
    template<>
    inline void MATLABObject<MATLABControllerType>::MATLABSetProperty(std::u16string propName, float value)
    {
        matlab::data::ArrayFactory arrayFactory;
        MATLABObject<MATLABControllerType>::MATLABSetArrayProperty(propName, arrayFactory.createScalar<float>(value));
    }

    template<>
    template<>
    inline std::u16string MATLABObject<MATLABControllerType>::MATLABGetProperty(std::u16string propName)
    {
        matlab::data::Array stringResult = MATLABObject<MATLABControllerType>::MATLABGetArrayProperty(propName);
        if (stringResult.getType() == matlab::data::ArrayType::MATLAB_STRING) {
            matlab::data::StringArray strArray = std::move(stringResult);
            matlab::data::MATLABString str = strArray[0];
            if (str.has_value()) {
                return *str;
            }
            else {
                return u"";
            }
        }
        else if (stringResult.getType() == matlab::data::ArrayType::CHAR) {
            matlab::data::CharArray charArray = std::move(stringResult);
            return charArray.toUTF16();
        }
        return u"";
    }

    template<>
    template<>
    inline void MATLABObject<MATLABControllerType>::MATLABSetProperty(std::u16string propName, std::u16string value)
    {
        matlab::data::ArrayFactory arrayFactory;
        MATLABObject<MATLABControllerType>::MATLABSetArrayProperty(propName, arrayFactory.createCharArray(value));
    }

    template<>
    template<>
    inline matlab::data::Array MATLABObject<MATLABControllerType>::MATLABGetProperty(std::u16string propName)
    {
        return MATLABObject<MATLABControllerType>::MATLABGetArrayProperty(propName);
    }

    template<>
    template<>
    inline void MATLABObject<MATLABControllerType>::MATLABSetProperty(std::u16string propName, matlab::data::Array value)
    {
        MATLABObject<MATLABControllerType>::MATLABSetArrayProperty(propName, value);
    }

    // Nx1 or 1xN property types
    template<>
    template<>
    inline std::vector<double> MATLABObject<MATLABControllerType>::MATLABGetProperty(std::u16string propName)
    {
        matlab::data::TypedArray<double> arr = MATLABObject<MATLABControllerType>::MATLABGetArrayProperty(propName);
        std::vector<double> value(arr.cbegin(), arr.cend());
        return value;
    }

    template<>
    template<>
    inline void MATLABObject<MATLABControllerType>::MATLABSetProperty(std::u16string propName, std::vector<double> const value)
    {
        matlab::data::ArrayFactory arrayFactory;
        MATLABObject<MATLABControllerType>::MATLABSetArrayProperty(propName, arrayFactory.createArray<std::vector<double>::const_iterator, double>({ value.size(), size_t(1) }, value.cbegin(), value.cend()));
    }
    
    template<>
    template<>
    inline std::vector<std::complex<double>> MATLABObject<MATLABControllerType>::MATLABGetProperty(std::u16string propName)
    {
        matlab::data::TypedArray<std::complex<double>> arr = MATLABObject<MATLABControllerType>::MATLABGetArrayProperty(propName);
        std::vector<std::complex<double>> value(arr.cbegin(), arr.cend());
        return value;
    }

    template<>
    template<>
    inline void MATLABObject<MATLABControllerType>::MATLABSetProperty(std::u16string propName, std::vector<std::complex<double>> const value)
    {
        matlab::data::ArrayFactory arrayFactory;
        MATLABObject<MATLABControllerType>::MATLABSetArrayProperty(propName, arrayFactory.createArray<std::vector<std::complex<double>>::const_iterator, std::complex<double>>({ value.size(), size_t(1) }, value.cbegin(), value.cend()));
    }

    /*template<>
    template<>
    inline std::vector<float> MATLABObject<MATLABControllerType>::MATLABGetProperty(std::u16string propName)
    {
        matlab::data::TypedArray<float> arr = MATLABObject<MATLABControllerType>::MATLABGetArrayProperty(propName);
        std::vector<float> value(arr.cbegin(), arr.cend());
        return value;
    }

    template<>
    template<>
    inline void MATLABObject<MATLABControllerType>::MATLABSetProperty(std::u16string propName, std::vector<float> const value)
    {
        matlab::data::ArrayFactory arrayFactory;
        MATLABObject<MATLABControllerType>::MATLABSetArrayProperty(propName, arrayFactory.createArray<std::vector<double>::const_iterator, double>({ value.size(), size_t(1) }, value.cbegin(), value.cend()));
    }*/

    // Numeric MxN matrix properties
    /*
    template<>
    template<>
    inline matlab::data::TypedArray<double> MATLABObject<MATLABControllerType>::MATLABGetProperty(std::u16string propName)
    {
        return MATLABObject<MATLABControllerType>::MATLABGetArrayProperty(propName);
    }

    template<>
    template<>
    inline void MATLABObject<MATLABControllerType>::MATLABSetProperty(std::u16string propName, matlab::data::TypedArray<double> value)
    {
        MATLABObject<MATLABControllerType>::MATLABSetArrayProperty(propName, value);
    }

    template<>
    template<>
    inline matlab::data::TypedArray<float> MATLABObject<MATLABControllerType>::MATLABGetProperty(std::u16string propName)
    {
        return MATLABObject<MATLABControllerType>::MATLABGetArrayProperty(propName);
    }

    template<>
    template<>
    inline void MATLABObject<MATLABControllerType>::MATLABSetProperty(std::u16string propName, matlab::data::TypedArray<float> value)
    {
        MATLABObject<MATLABControllerType>::MATLABSetArrayProperty(propName, value);
    }*/

#endif  //MATLABTYPESINTERFACE_HPP