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


class CISST_EXPORT mtsMacrosTestClass: public mtsGenericObject
{
protected:
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, 5);
    CMN_DECLARE_MEMBER_AND_ACCESSORS(double, Value);

public:
    static size_t DefaultConstructorCalls;
    static size_t DestructorCalls;
    static size_t CopyConstructorCalls;

    mtsMacrosTestClass();
    mtsMacrosTestClass(const mtsMacrosTestClass & other);

    ~mtsMacrosTestClass();

    static void ResetAllCounters(void);

    /*! To stream human readable output */
    virtual void ToStream(std::ostream & outputStream) const override {
        outputStream << Value() << std::endl;
    }

    /*! Print in machine processable format */
    void ToStreamRaw(std::ostream & outputStream, const char CMN_UNUSED(delimiter) = ' ',
                     bool headerOnly = false, const std::string & headerPrefix = "") const override
    {
        if (headerOnly) {
            outputStream << headerPrefix << "value";
        } else {
            outputStream << Value();
        }
    }

    void SerializeRaw(std::ostream & outputStream) const override
    {
        cmnSerializeRaw(outputStream, Value());
    }

    void DeSerializeRaw(std::istream & inputStream) override
    {
        cmnDeSerializeRaw(inputStream, Value());
    }

};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsMacrosTestClass);

#endif // _mtsMacrosTestClasses_h
