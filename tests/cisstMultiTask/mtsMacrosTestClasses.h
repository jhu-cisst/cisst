/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsMacrosTest.h 300 2009-04-30 03:04:51Z adeguet1 $
  
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

// check if this module is build as a DLL
#ifdef cisstMultiTasksTestsLib_EXPORTS
#define CISST_THIS_LIBRARY_AS_DLL
#endif
#include <cisstCommon/cmnExportMacros.h>

class CISST_EXPORT mtsMacrosTestClassA: public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, 5);
    MTS_DECLARE_MEMBER_AND_ACCESSORS(double, Value);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsMacrosTestClassA);


class mtsMacrosTestClassB
{
public:
    double Value;

    void SerializeRaw(std::ostream & outputStream) const 
    {
        cmnSerializeRaw(outputStream, this->Value);
    }

    void DeSerializeRaw(std::istream & inputStream)
    {
        cmnDeSerializeRaw(inputStream, this->Value);
    }

};

MTS_PROXY_CLASS_DECLARATION_FROM(mtsMacrosTestClassB, mtsMacrosTestClassBProxy);


#endif // _mtsMacrosTestClasses_h
