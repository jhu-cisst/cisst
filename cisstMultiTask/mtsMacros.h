/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*!
  Author(s):  Anton Deguet
  Created on: 2008-06-26

  (C) Copyright 2008-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#pragma once
#ifndef _mtsMacros_h
#define _mtsMacros_h

#include <cisstCommon/cmnAccessorMacros.h>
// the following macro is now part of cisstCommon, cmnAccessorMacros.h
#define MTS_DECLARE_MEMBER_AND_ACCESSORS CMN_DECLARE_MEMBER_AND_ACCESSORS

// deprecated macro
#define MTS_PROXY_CLASS_DECLARATION_FROM(className, newName) You_should_use__MTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC__instead

/*! macro to create multiple inheritance with mtsGenericObject.  This
  macro should not be used with classes having a virtual table,
  including any class derived from cmnGenericObject. */
#define MTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC(className, newName)        \
class newName: public mtsGenericObject, public className                     \
{                                                                            \
    CMN_DECLARE_SERVICES_EXPORT(CMN_DYNAMIC_CREATION, CMN_LOG_DEFAULT_LOD);  \
public:                                                                      \
    newName(void): mtsGenericObject(), className() {}                        \
    newName(const className & other):                                        \
        mtsGenericObject(),                                                  \
        className(other) {}                                                  \
    void ToStream(std::ostream & outputStream) const {                       \
        mtsGenericObject::ToStream(outputStream);                            \
        outputStream << std::endl;                                           \
        className::ToStream(outputStream);                                   \
    }                                                                        \
    void SerializeRaw(std::ostream & outputStream) const {                   \
        mtsGenericObject::SerializeRaw(outputStream);                        \
        className::SerializeRaw(outputStream);                               \
    }                                                                        \
    void DeSerializeRaw(std::istream & inputStream) {                        \
        mtsGenericObject::DeSerializeRaw(inputStream);                       \
        className::DeSerializeRaw(inputStream);                              \
    }                                                                        \
    void ToStreamRaw(std::ostream & outputStream,                            \
                     const char delimiter = ' ',                             \
                     bool headerOnly = false,                                \
                     const std::string & headerPrefix = "") const {          \
        mtsGenericObject::ToStreamRaw(outputStream, delimiter,               \
                                      headerOnly, headerPrefix);             \
        outputStream << delimiter;                                           \
        className::ToStreamRaw(outputStream, delimiter,                      \
                               headerOnly, headerPrefix);                    \
    }                                                                        \
    className & Data(void) {                                                 \
        return *this;                                                        \
    }                                                                        \
    const className & Data(void) const {                                     \
        return *this;                                                        \
    }                                                                        \
};                                                                           \
inline std::ostream & operator << (std::ostream & output,                    \
                                   const newName & object) {                 \
    object.ToStream(output);                                                 \
    return output;                                                           \
}                                                                            \
CMN_DECLARE_SERVICES_INSTANTIATION(newName)


#endif // _mtsMacros_h

