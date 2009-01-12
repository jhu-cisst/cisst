/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsVector.h,v 1.8 2008/12/03 22:17:56 mbalicki Exp $

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

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnGenericObjectProxy.h>
#include <cisstCommon/cmnClassRegister.h>
#include <cisstVector/vctDynamicVector.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

template <class _elementType>
class mtsVector: public cmnGenericObject,
                 public vctDynamicVector<_elementType> {
    // declare services, requires dynamic creation
    CMN_DECLARE_SERVICES_EXPORT(CMN_DYNAMIC_CREATION, 5);
public:
    /*! Type of elements. */
    typedef _elementType value_type;

    /*! This type. */
    typedef mtsVector<value_type> ThisType;

    /*! Type of vector used to store the elements. */
    typedef vctDynamicVector<value_type> VectorType;

    /*! Type used to define the size of the vector. */
    typedef typename VectorType::size_type size_type;


    /*! Default constructor. */
    inline mtsVector(void):
        VectorType(0)
    {}

    /*! Constructor with memory allocation for a given size. */
    inline mtsVector(size_type size):
        VectorType(size)
    {}

    /*! Assignment from vector base class.  This operator assign the
      data from one vector to another, it doesn't replace the object
      itself, i.e. it doesn't release and allocate any new memory. */
    inline ThisType & operator=(const VectorType & data) {
        VectorType::Assign(data);
        return *this;
    }

    /*! Copy constructor. */
    inline mtsVector(const ThisType & otherVector):
        cmnGenericObject(otherVector),
        VectorType(otherVector)
    {}

    /*! Default destructor, will call the destructor of the contained
      vector and free the memory. */
    inline ~mtsVector() {}

     /*! To stream human readable output */
    virtual  std::string ToString(void) const {
        std::stringstream outputStream;
         VectorType::ToStream(outputStream);
        return outputStream.str();
    }

    /*! To stream human readable output */
    virtual void ToStream(std::ostream & outputStream) const {
        VectorType::ToStream(outputStream);
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

#ifdef CISST_GETVECTOR
// PK: the StateTable GetVector implementation will require an mtsVector
//     for every parameter type!!

// Following is for a vector of cmnDouble
typedef mtsVector<cmnDouble> mtscmnDoubleVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtscmnDoubleVec);

// Following is for a vector of cmnVector<double>
typedef mtsVector<mtsDoubleVec> mtsDoubleVecVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsDoubleVecVec);
#endif


#endif // _mtsVector_h
