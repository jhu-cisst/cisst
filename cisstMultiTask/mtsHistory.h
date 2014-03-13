/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsHistory.h 2009-03-02 mjung5

  Author(s):  Min Yang Jung
  Created on: 2009-03-25

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

/*!
  \file
  \brief A data collection tool
*/

#error "This file has been deprecated, there is no replacement for the class mtsHistory.  adeguet1, 2013/06/24"

#ifndef _mtsHistory_h
#define _mtsHistory_h

#include <cisstVector/vctDynamicVector.h>
#include <cisstMultiTask/mtsVector.h>

#include <cisstMultiTask/mtsExport.h>

/*!
\ingroup cisstMultiTask
*/
class mtsHistoryBase : public mtsGenericObject
{
    friend class mtsHistoryBaseTest;

public:
    mtsHistoryBase() {}
    mtsHistoryBase(const mtsHistoryBase & other) : mtsGenericObject(other) {}
    virtual ~mtsHistoryBase() {}

    /*! In the derived class, vctDynamicVector::SetSize() should be called. */
    virtual void SetHistorySize(const int historySize) = 0;

    // Methods to be used by GetHistory() (maybe: [], ...)
};

template <class _elementType>
class mtsHistory : public mtsHistoryBase,
                   public vctDynamicVector<_elementType>
{
    CMN_DECLARE_SERVICES_EXPORT(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    /*! Type of elements. */
    typedef _elementType value_type;

    /*! This type. */
    typedef mtsHistory<value_type> ThisType;

    /*! Type of vector used to store the elements. */
    typedef vctDynamicVector<value_type> VectorType;

    /*! Type used to define the size of the vector. */
    typedef typename VectorType::size_type size_type;

    /*! Default constructor. */
    inline mtsHistory(void) : VectorType(0) {}

    /*! Constructor with memory allocation for a given size. */
    inline mtsHistory(size_type size) : VectorType(size) {}

    /*! Set the size of history buffer */
    inline void SetHistorySize(const int historySize) {
        this->SetSize(historySize);
    }

    /*! Assignment from vector base class.  This operator assign the
      data from one vector to another, it doesn't replace the object
      itself, i.e. it doesn't release and allocate any new memory. */
    inline ThisType & operator=(const VectorType & data) {
        VectorType::Assign(data);
        return *this;
    }

    /*! Copy constructor. */
    inline mtsHistory(const ThisType & otherVector):
        mtsHistoryBase(otherVector),
        VectorType(otherVector)
    {}

    /*! Default destructor, will call the destructor of the contained
      vector and free the memory. */
    inline ~mtsHistory() {}

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

    /*! To stream human readable output raw */
    virtual void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                             bool headerOnly = false, const std::string & headerPrefix = "") const {
        mtsGenericObject::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
        outputStream << delimiter;
        VectorType::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
    }
};

// PK: the StateTable GetHistory implementation will require an mtsVector
//     for every parameter type!!

// Following is for a vector of cmnDouble
typedef mtsHistory<mtsDouble> mtsDoubleHistory;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsDoubleHistory);

// Following is for a vector of cmnVector<double>
typedef mtsHistory<mtsDoubleVec> mtsDoubleVecHistory;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsDoubleVecHistory);

#endif // _mtsHistory_h
