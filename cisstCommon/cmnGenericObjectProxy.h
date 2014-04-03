/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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

#pragma once

#ifndef _cmnGenericObjectProxy_h
#define _cmnGenericObjectProxy_h

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassRegister.h>
#include <cisstCommon/cmnSerializer.h>
#include <cisstCommon/cmnDeSerializer.h>
#include <cisstCommon/cmnExport.h>

/*!  Proxy class used to create a simple cmnGenericObject, i.e. data
  object with a registered type, dynamic creation, serialization and
  de-serialization.  The proxy object contains one data member of the
  actual type provided as a template parameter.

  For example, to create a cmnGenericObject from an existing type, one
  would have to specialize cmnGenericObjectProxy as follows in the
  header file:
  \code
  class UserType {
      ....
  };

  typedef cmnGenericObjectProxy<UserType> UserTypeProxy;
  CMN_DECLARE_SERVICES_INSTANTIATION(UserTypeProxy);
  \endcode

  It is also required to instantiate the associated services in the
  compiled code (.cpp) file with:
  \code
  CMN_IMPLEMENT_SERVICES_TEMPLATED(UserTypeProxy);
  \endcode

  \note It is really important to note that this simple way of
  creating cmnGenericObject derived classes works as is for classes
  which can be serialized as a memory block, i.e. native types,
  compact structs, ...  For classes which require a more complex way
  to serialize and deserialize, the user will also have to overload
  the functions cmnSerializeRaw and cmnDeSerializeRaw.

  \note As it is, this class can not be used in libraries other than
  cisstCommon if a DLL is to be created.   The issue is that the class
  will be declared as imported in the header file because it is part of
  cisstCommon.

  \sa cmnGenericObject, cmnClassServices, cmnSerializer, cmnDeSerializer

  \param _elementType The actual type for which we want to provide a
  proxy object.
*/
template <class _elementType>
class cmnGenericObjectProxy: public cmnGenericObject
{
    CMN_DECLARE_SERVICES_EXPORT(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    typedef cmnGenericObjectProxy<_elementType> ThisType;
    typedef _elementType value_type;
    value_type Data;

    /*! Default constructor.  The data member is initialized using its
        default constructor. */
    inline cmnGenericObjectProxy(void)
    {}

    /*! Conversion constructor.  This allows to construct the proxy
        object using an object of the actual type. */
    inline cmnGenericObjectProxy(const value_type & data):
        Data(data)
    {}

    inline ~cmnGenericObjectProxy(void) {}

    /*! Conversion assignment.  This allows to assign from an object
      of the actual type without explicitly referencing the public
      data member "Data". */
    inline ThisType & operator=(value_type data) {
        Data = data;
        return *this;
    }

    /*! Cast operator.  This allows to assign to an object of the
      actual type without explicitly referencing the public data
      member "Data". */
    //@{
    inline operator value_type & (void) {
        return Data;
    }
    inline operator const value_type & (void) const {
        return this->Data;
    }
    //@}

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
        outputStream << Data;
    }

    /*! To stream raw data. */
    inline virtual void ToStreamRaw(std::ostream & outputStream, const char CMN_UNUSED(delimiter) = ' ',
                                    bool headerOnly = false, const std::string & headerPrefix = "") const {
        if (headerOnly) {
            outputStream << headerPrefix << "-data";
        } else {
            outputStream << this->Data;
        }
    }

    /*! From stream raw data. */
    inline virtual bool FromStreamRaw(std::istream & inputStream, const char CMN_UNUSED(delimiter) = ' ') {
        inputStream >> this->Data;  // assumes that operator >> is defined for _elementType
        bool valid = inputStream.good();
        if (!valid) inputStream.clear();
        return valid;
    }

};

/* Some basic types defined here for now, could move somewhere
   else. */
typedef cmnGenericObjectProxy<double> cmnDouble;
CMN_DECLARE_SERVICES_INSTANTIATION(cmnDouble);

typedef cmnGenericObjectProxy<long> cmnLong;
CMN_DECLARE_SERVICES_INSTANTIATION(cmnLong);

typedef cmnGenericObjectProxy<unsigned long> cmnULong;
CMN_DECLARE_SERVICES_INSTANTIATION(cmnULong);

typedef cmnGenericObjectProxy<long long> cmnLongLong;
CMN_DECLARE_SERVICES_INSTANTIATION(cmnLongLong);

typedef cmnGenericObjectProxy<unsigned long long> cmnULongLong;
CMN_DECLARE_SERVICES_INSTANTIATION(cmnULongLong);

typedef cmnGenericObjectProxy<int> cmnInt;
CMN_DECLARE_SERVICES_INSTANTIATION(cmnInt);

typedef cmnGenericObjectProxy<unsigned int> cmnUInt;
CMN_DECLARE_SERVICES_INSTANTIATION(cmnUInt);

typedef cmnGenericObjectProxy<short> cmnShort;
CMN_DECLARE_SERVICES_INSTANTIATION(cmnShort);

typedef cmnGenericObjectProxy<unsigned short> cmnUShort;
CMN_DECLARE_SERVICES_INSTANTIATION(cmnUShort);

typedef cmnGenericObjectProxy<char> cmnChar;
CMN_DECLARE_SERVICES_INSTANTIATION(cmnChar);

typedef cmnGenericObjectProxy<unsigned char> cmnUChar;
CMN_DECLARE_SERVICES_INSTANTIATION(cmnUChar);

typedef cmnGenericObjectProxy<bool> cmnBool;
CMN_DECLARE_SERVICES_INSTANTIATION(cmnBool);


#endif

