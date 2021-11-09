/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:   2008-02-05

  (C) Copyright 2008-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _prmMaskedVector_h
#define _prmMaskedVector_h

#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstParameterTypes/prmExport.h>

//#include <cisstMultiTask/mtsForwardDeclarations.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

template <class _elementType>
class prmMaskedVector: public mtsGenericObject
{
    // declare services, requires dynamic creation
    CMN_DECLARE_SERVICES_EXPORT_ALWAYS(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    typedef size_t size_type;
    typedef prmMaskedVector<_elementType> ThisType;
    typedef mtsGenericObject BaseType;
    typedef vctDynamicVector<_elementType> DataType;

protected:
    /*! Vector of values for this container */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(DataType, Data);
    //@}

    /*! Masks for the cooresponding vector */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctBoolVec, Mask);
    //@}

public:
    /*! Default constructor. */
    inline prmMaskedVector(void):
        DataMember(0),
        MaskMember(0)
    {}

    /*! Set vector same size for each element, this is required if the above constructor is not used
      Sets all elements to 0
    */
    inline void SetSize(const size_type size) {
        DataMember.SetSize(size);
        MaskMember.SetSize(size);
        DataMember.Zeros();
        MaskMember.Zeros();
    }


    /*! Constructor with memory allocation for a given size. */
    inline prmMaskedVector(const size_type size) {
        SetSize(size);
    }

    inline prmMaskedVector(const DataType & data,
                           const vctBoolVec & mask):
        DataMember(data),
        MaskMember(mask)
    {
        CMN_ASSERT(data.size() == mask.size());
    }

    /*! Copy constructor. */
    inline prmMaskedVector(const ThisType & otherVector):
        mtsGenericObject(otherVector),
        DataMember(otherVector.Data()),
        MaskMember(otherVector.Mask())
    {}

    /*! Default destructor, will call the destructor of the contained
      vector and free the memory. */
    inline ~prmMaskedVector() {}

    /*! To stream human readable output */
    virtual  std::string ToString(void) const {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }

    /*! To stream human readable output */
    virtual void ToStream(std::ostream & outputStream) const {
        mtsGenericObject::ToStream(outputStream);
        outputStream << ", " <<DataMember << ", " << MaskMember;
    }

    /*! Binary serialization */
    void SerializeRaw(std::ostream & outputStream) const{
        BaseType::SerializeRaw(outputStream);
        MaskMember.SerializeRaw(outputStream);
        DataMember.SerializeRaw(outputStream);
    }

    /*! Binary deserialization */
    void DeSerializeRaw(std::istream & inputStream){
        BaseType::DeSerializeRaw(inputStream);
        MaskMember.DeSerializeRaw(inputStream);
        DataMember.DeSerializeRaw(inputStream);
    }
};


// define some basic vectors
typedef prmMaskedVector<double> prmMaskedDoubleVec;
CMN_DECLARE_SERVICES_INSTANTIATION(prmMaskedDoubleVec);

typedef prmMaskedVector<float> prmMaskedFloatVec;
CMN_DECLARE_SERVICES_INSTANTIATION(prmMaskedFloatVec);

typedef prmMaskedVector<long> prmMaskedLongVec;
CMN_DECLARE_SERVICES_INSTANTIATION(prmMaskedLongVec);

typedef prmMaskedVector<unsigned long> prmMaskedULongVec;
CMN_DECLARE_SERVICES_INSTANTIATION(prmMaskedULongVec);

typedef prmMaskedVector<int> prmMaskedIntVec;
CMN_DECLARE_SERVICES_INSTANTIATION(prmMaskedIntVec);

typedef prmMaskedVector<unsigned int> prmMaskedUIntVec;
CMN_DECLARE_SERVICES_INSTANTIATION(prmMaskedUIntVec);

typedef prmMaskedVector<short> prmMaskedShortVec;
CMN_DECLARE_SERVICES_INSTANTIATION(prmMaskedShortVec);

typedef prmMaskedVector<unsigned short> prmMaskedUShortVec;
CMN_DECLARE_SERVICES_INSTANTIATION(prmMaskedUShortVec);

typedef prmMaskedVector<char> prmMaskedCharVec;
CMN_DECLARE_SERVICES_INSTANTIATION(prmMaskedCharVec);

typedef prmMaskedVector<unsigned char> prmMaskedUCharVec;
CMN_DECLARE_SERVICES_INSTANTIATION(prmMaskedUCharVec);

typedef prmMaskedVector<bool> prmMaskedBoolVec;
CMN_DECLARE_SERVICES_INSTANTIATION(prmMaskedBoolVec);

#endif // _prmMaskedVector_h
