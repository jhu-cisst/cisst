/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:   2010-02-27

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _mtsFixedSizeVector_h
#define _mtsFixedSizeVector_h

#include <cisstCommon/cmnClassRegister.h>
#include <cisstVector/vctFixedSizeVector.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

template <class _elementType, vct::size_type _size>
class mtsFixedSizeVector: public mtsGenericObject,
                          public vctFixedSizeVector<_elementType, _size> {
    // declare services, requires dynamic creation
    CMN_DECLARE_SERVICES_EXPORT(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
public:
    /* defines useful types */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /*! This type. */
    typedef mtsFixedSizeVector<value_type, _size> ThisType;

    /*! Type of vector used to store the elements. */
    typedef vctFixedSizeVector<value_type, _size> VectorType;

    /*! Default constructor. */
    inline mtsFixedSizeVector(void)
    {
        VectorType::Zeros();
    }

    /*! Assignment from vector base class.  This operator assign the
      data from one vector to another. */
    inline ThisType & operator = (const VectorType & data) {
        VectorType::Assign(data);
        return *this;
    }

    /*! Copy constructor. */
    inline mtsFixedSizeVector(const ThisType & otherVector):
        mtsGenericObject(otherVector),
        VectorType(otherVector)
    {}

    /*! Pseudo copy constructor from vector type. */
    inline mtsFixedSizeVector(const VectorType & otherVector):
        mtsGenericObject(),
        VectorType(otherVector)
    {}

    /*! Default destructor, will call the destructor of the contained
      vector and free the memory. */
    inline ~mtsFixedSizeVector() {}

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


#endif // _mtsFixedSizeVector_h
