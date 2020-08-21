/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:   2008-02-05

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _mtsVector_h
#define _mtsVector_h

#include <cisstCommon/cmnClassRegister.h>
#include <cisstVector/vctDynamicVector.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

template <class _elementType>
class mtsVector: public mtsGenericObject,
                 public vctDynamicVector<_elementType> {
    // declare services, requires dynamic creation
    CMN_DECLARE_SERVICES_EXPORT(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
public:
    /* defines useful types */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /*! This type. */
    typedef mtsVector<value_type> ThisType;

    /*! Type of vector used to store the elements. */
    typedef vctDynamicVector<value_type> VectorType;

    /*! Default constructor. */
    inline mtsVector(void):
        mtsGenericObject(),
        VectorType(0)
    {}

    /*! Constructor with memory allocation for a given size.  Assign
        zero to all elements. */
    inline mtsVector(size_type size):
        mtsGenericObject(),
        VectorType(size)
    {
        VectorType::Zeros();
    }

    /*! Assignment operator */
    inline ThisType & operator = (const ThisType & other) {
        mtsGenericObject::operator = (other);
        VectorType::operator = (other);
        return *this;
    }

    /*! Assignment from vector base class.  This operator assign the
      data from one vector to another, it doesn't replace the object
      itself, i.e. it doesn't release and allocate any new memory. */
    inline ThisType & operator = (const VectorType & data) {
        VectorType::SetSize(data.size());
        VectorType::Assign(data);
        return *this;
    }

    /*! Copy constructor. */
    inline mtsVector(const ThisType & otherVector):
        mtsGenericObject(otherVector),
        VectorType(otherVector)
    {}

    /*! Pseudo copy constructor from vector type. */
    inline mtsVector(const VectorType & otherVector):
        mtsGenericObject(),
        VectorType(otherVector)
    {}

    /*! Default destructor, will call the destructor of the contained
      vector and free the memory. */
    inline ~mtsVector() {}

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
        VectorType::ToStream(outputStream);
    }

    /*! To stream raw data. */
    inline virtual void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                                    bool headerOnly = false, const std::string & headerPrefix = "") const {
        mtsGenericObject::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
        outputStream << delimiter;
        VectorType::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
    }

    /*! Binary serialization */
    void SerializeRaw(std::ostream & outputStream) const
    {
        mtsGenericObject::SerializeRaw(outputStream);
        VectorType::SerializeRaw(outputStream);
    }

    /*! Binary deserialization */
    void DeSerializeRaw(std::istream & inputStream)
    {
        mtsGenericObject::DeSerializeRaw(inputStream);
        VectorType::DeSerializeRaw(inputStream);
    }
};


// define some basic vectors
typedef mtsVector<double> mtsDoubleVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsDoubleVec);

typedef mtsVector<float> mtsFloatVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsFloatVec);

typedef mtsVector<long> mtsLongVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsLongVec);

typedef mtsVector<unsigned long> mtsULongVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsULongVec);

typedef mtsVector<int> mtsIntVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsIntVec);

typedef mtsVector<unsigned int> mtsUIntVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsUIntVec);

typedef mtsVector<short> mtsShortVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsShortVec);

typedef mtsVector<unsigned short> mtsUShortVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsUShortVec);

typedef mtsVector<char> mtsCharVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsCharVec);

typedef mtsVector<unsigned char> mtsUCharVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsUCharVec);

typedef mtsVector<bool> mtsBoolVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsBoolVec);

typedef mtsVector<std::string> mtsStdStringVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsStdStringVec);

#endif // _mtsVector_h
