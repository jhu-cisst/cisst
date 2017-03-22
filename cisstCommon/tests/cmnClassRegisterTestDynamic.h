/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2006-10-27
  
  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _cmnClassRegisterTestDynamic_h
#define _cmnClassRegisterTestDynamic_h

#include <cisstCommon/cmnClassRegister.h>
#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnPortability.h>


// check if this module is build as a DLL, CMake defines the _EXPORTS
#ifdef CISST_COMPILER_IS_MSVC 
#undef CISST_EXPORT
#pragma warning (disable: 4275)
#ifdef cmnClassRegisterTestDynamic_EXPORTS
  #define CISST_EXPORT _declspec(dllexport)
#else
  #define CISST_EXPORT _declspec(dllimport)
#endif
#endif
#include <cisstCommon/cmnClassRegisterMacros.h>


class dynamicAllInline: public cmnGenericObject {
    CMN_DECLARE_SERVICES_EXPORT(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    inline dynamicAllInline(void) {}
    
    inline std::string Name(void) const {
        return this->Services()->GetName();
    }
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(dynamicAllInline);

class CISST_EXPORT dynamicNoInline: public cmnGenericObject {
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    dynamicNoInline(void);

    std::string Name(void) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(dynamicNoInline);


#endif // _cmnClassRegisterTestDynamic_h

