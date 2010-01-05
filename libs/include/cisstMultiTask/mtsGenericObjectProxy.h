/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsGenericObjectProxy.h 75 2009-02-24 16:47:20Z adeguet1 $
  
  Author(s):	Ankur Kapoor, Anton Deguet
  Created on:	2006-05-05
  
  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsGenericObjectProxy_h
#define _mtsGenericObjectProxy_h

#include <cisstCommon/cmnClassRegister.h>
#include <cisstCommon/cmnSerializer.h>
#include <cisstCommon/cmnDeSerializer.h>

#include <cisstMultiTask/mtsGenericObject.h>
// Always include last!
#include <cisstMultiTask/mtsExport.h>

/*!  Proxy class used to create a simple mtsGenericObject, i.e. data
  object with a registered type, dynamic creation, serialization and
  de-serialization.  The proxy object contains one data member of the
  actual type provided as a template parameter.

  For example, to create a mtsGenericObject from an existing type, one
  would have to specialize mtsGenericObjectProxy as follows in the
  header file:
  \code
  class UserType {
      ....
  };

  typedef mtsGenericObjectProxy<UserType> UserTypeProxy;
  CMN_DECLARE_SERVICES_INSTANTIATION(UserTypeProxy);
  \endcode
  
  It is also required to instantiate the associated services in the
  compiled code (.cpp) file with:
  \code
  CMN_IMPLEMENT_SERVICES_TEMPLATED(UserTypeProxy);
  \endcode

  \note It is really important to note that this simple way of
  creating mtsGenericObject derived classes works as is for classes
  which can be serialized as a memory block, i.e. native types,
  compact structs, ...  For classes which require a more complex way
  to serialize and deserialize, the user will also have to overload
  the functions cmnSerializeRaw and cmnDeSerializeRaw.

  \note As it is, this class can not be used in libraries other than 
  cisstCommon if a DLL is to be created.   The issue is that the class
  will be declared as imported in the header file because it is part of
  cisstCommon.

  \sa mtsGenericObject, cmnClassServices, cmnSerializer, cmnDeSerializer
  
  \param _elementType The actual type for which we want to provide a
  proxy object.
*/ 
template <class _elementType>
class mtsGenericObjectProxy: public mtsGenericObject
{
    CMN_DECLARE_SERVICES_EXPORT(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    typedef mtsGenericObjectProxy<_elementType> ThisType;
    typedef mtsGenericObject BaseType;
    typedef _elementType value_type;
    value_type Data;

    /*! Default constructor.  The data member is initialized using its
        default constructor. */
    inline mtsGenericObjectProxy(void)
    {}

    /*! Copy constructor. */
    inline mtsGenericObjectProxy(const ThisType & other) : 
        BaseType(other), Data(other.Data)
    {}

    /*! Conversion constructor.  This allows to construct the proxy
        object using an object of the actual type. */
    inline mtsGenericObjectProxy(const value_type & data):
        Data(data)
    {}

    inline ~mtsGenericObjectProxy(void) {}

    /*! Conversion assignment.  This allows to assign from an object
      of the actual type without explicitly referencing the public
      data member "Data". */
    inline ThisType & operator=(value_type data) {
        this->Data = data;
        return *this;
    }

    /*! Cast operator.  This allows to assign to an object of the
      actual type without explicitly referencing the public data
      member "Data". */
    inline operator value_type & (void) {
        return this->Data;
    }

    /*! Serialization.  Relies on the specialization, if any, of
      cmnSerializeRaw. */
    inline void SerializeRaw(std::ostream & outputStream) const {
        cmnSerializeRaw(outputStream, this->Data);
    }

    /*! DeSerialization.  Relies on the specialization, if any, of
      cmnDeSerializeRaw. */
    inline void DeSerializeRaw(std::istream & inputStream) {
        cmnDeSerializeRaw(inputStream, this->Data);
    }

    /*! To stream method.  Uses the default << operator as defined for
        the actual type. */
    inline virtual void ToStream(std::ostream & outputStream) const {
        BaseType::ToStream(outputStream);
        outputStream << " Value: " << this->Data;
    }

    /*! To stream raw data. */
    inline virtual void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                                    bool headerOnly = false, const std::string & headerPrefix = "") const {
        if (headerOnly) {
            BaseType::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
            outputStream << delimiter << headerPrefix << "-data";
        } else {
            BaseType::ToStreamRaw(outputStream, delimiter);
            outputStream << delimiter << this->Data;
        }
    }
};

/* Some basic types defined here for now, could move somewhere
   else. */
typedef mtsGenericObjectProxy<double> mtsDouble;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsDouble);

typedef mtsGenericObjectProxy<long> mtsLong;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsLong);

typedef mtsGenericObjectProxy<unsigned long> mtsULong;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsULong);

typedef mtsGenericObjectProxy<int> mtsInt;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsInt);

typedef mtsGenericObjectProxy<unsigned int> mtsUInt;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsUInt);

typedef mtsGenericObjectProxy<short> mtsShort;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsShort);

typedef mtsGenericObjectProxy<unsigned short> mtsUShort;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsUShort);

typedef mtsGenericObjectProxy<char> mtsChar;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsChar);

typedef mtsGenericObjectProxy<unsigned char> mtsUChar;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsUChar);

typedef mtsGenericObjectProxy<bool> mtsBool;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsBool);

typedef mtsGenericObjectProxy<std::string> mtsStdString;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsStdString);

#endif
