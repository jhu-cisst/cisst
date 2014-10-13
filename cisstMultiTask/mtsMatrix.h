/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:   2009-04-29

  (C) Copyright 2009-2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _mtsMatrix_h
#define _mtsMatrix_h

#include <cisstCommon/cmnClassRegister.h>
#include <cisstVector/vctDynamicMatrix.h>
#include <cisstVector/vctDataFunctionsDynamicMatrix.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

template <class _elementType>
class mtsMatrix: public mtsGenericObject,
                 public vctDynamicMatrix<_elementType> {
    // declare services, requires dynamic creation
    CMN_DECLARE_SERVICES_EXPORT(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
public:
    /* defines useful types */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    enum {DIMENSION = 2};
    VCT_NARRAY_TRAITS_TYPEDEFS(DIMENSION);

    /*! This type. */
    typedef mtsMatrix<value_type> ThisType;

    /*! Type of matrix used to store the elements. */
    typedef vctDynamicMatrix<value_type> MatrixType;

    /*! Default constructor. */
    inline mtsMatrix(void):
        mtsGenericObject(),
        MatrixType(0, 0)
    {}

    /*! Constructor with memory allocation for a given size. */
    inline mtsMatrix(size_type rows, size_type cols):
        mtsGenericObject(),
        MatrixType(rows, cols)
    {
        MatrixType::Zeros();
    }

    inline mtsMatrix(const nsize_type & size):
        mtsGenericObject(),
        MatrixType(size)
    {
        MatrixType::Zeros();
    }

    /*! Assignment from vector base class.  This operator assign the
      data from one vector to another, it doesn't replace the object
      itself, i.e. it doesn't release and allocate any new memory. */
    inline ThisType & operator = (const MatrixType & data) {
        MatrixType::SetSize(data.rows(), data.cols(), data.StorageOrder());
        MatrixType::Assign(data);
        return *this;
    }

    /*! Copy constructor. */
    inline mtsMatrix(const ThisType & otherMatrix):
        mtsGenericObject(otherMatrix),
        MatrixType(otherMatrix)
    {}

    /*! Pseudo copy constructor from matrix type. */
    inline mtsMatrix(const MatrixType & otherMatrix):
        mtsGenericObject(),
        MatrixType(otherMatrix)
    {}

    /*! Default destructor, will call the destructor of the contained
      vector and free the memory. */
    inline ~mtsMatrix() {}

     /*! To stream human readable output */
    virtual  std::string ToString(void) const {
        std::stringstream outputStream;
        this->ToStream(outputStream);
        return outputStream.str();
    }

    /*! To stream human readable output */
    virtual void ToStream(std::ostream & outputStream) const {
        mtsGenericObject::ToStream(outputStream);
        outputStream << std::endl;
        MatrixType::ToStream(outputStream);
    }

    /*! To stream raw data. */
    inline virtual void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                                    bool headerOnly = false, const std::string & headerPrefix = "") const {
        mtsGenericObject::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
        outputStream << delimiter;
        MatrixType::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
    }

    /*! Binary serialization */
    void SerializeRaw(std::ostream & outputStream) const
    {
        mtsGenericObject::SerializeRaw(outputStream);
        MatrixType::SerializeRaw(outputStream);
    }

    /*! Binary deserialization */
    void DeSerializeRaw(std::istream & inputStream)
    {
        mtsGenericObject::DeSerializeRaw(inputStream);
        MatrixType::DeSerializeRaw(inputStream);
    }

};


// define some basic vectors
typedef mtsMatrix<double> mtsDoubleMat;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsDoubleMat);

typedef mtsMatrix<float> mtsFloatMat;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsFloatMat);

typedef mtsMatrix<long> mtsLongMat;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsLongMat);

typedef mtsMatrix<unsigned long> mtsULongMat;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsULongMat);

typedef mtsMatrix<int> mtsIntMat;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsIntMat);

typedef mtsMatrix<unsigned int> mtsUIntMat;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsUIntMat);

typedef mtsMatrix<short> mtsShortMat;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsShortMat);

typedef mtsMatrix<unsigned short> mtsUShortMat;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsUShortMat);

typedef mtsMatrix<char> mtsCharMat;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsCharMat);

typedef mtsMatrix<unsigned char> mtsUCharMat;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsUCharMat);

typedef mtsMatrix<bool> mtsBoolMat;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsBoolMat);

#endif // _mtsMatrix_h
