/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2009-06-11

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsMacrosTestClasses_h
#define _mtsMacrosTestClasses_h

#include <cisstMultiTask/mtsGenericObject.h>

// Always include last
#include "mtsTestExport.h"

class CISST_EXPORT mtsMacrosTestClassA: public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, 5);
    CMN_DECLARE_MEMBER_AND_ACCESSORS(double, Value);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsMacrosTestClassA);


class CISST_EXPORT mtsMacrosTestClassB
{
public:
    static size_t DefaultConstructorCalls;
    static size_t DestructorCalls;
    static size_t CopyConstructorCalls;

    mtsMacrosTestClassB(void);

    mtsMacrosTestClassB(const mtsMacrosTestClassB & other);

    ~mtsMacrosTestClassB();

    static void ResetAllCounters(void);

    double Value;

    /*! To stream human readable output */
    virtual void ToStream(std::ostream & outputStream) const {
        outputStream << this->Value << std::endl;
    }

    /*! Print in machine processable format */
    void ToStreamRaw(std::ostream & outputStream, const char CMN_UNUSED(delimiter) = ' ',
                     bool headerOnly = false, const std::string & headerPrefix = "") const {
        if (headerOnly) {
            outputStream << headerPrefix << "value";
        } else {
            outputStream << Value;
        }
    }

    void SerializeRaw(std::ostream & outputStream) const
    {
        cmnSerializeRaw(outputStream, this->Value);
    }

    void DeSerializeRaw(std::istream & inputStream)
    {
        cmnDeSerializeRaw(inputStream, this->Value);
    }

};

MTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC(mtsMacrosTestClassB, mtsMacrosTestClassBProxy);


#endif // _mtsMacrosTestClasses_h
