/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _clockComponent_h
#define _clockComponent_h

#include <cisstOSAbstraction/osaStopwatch.h>

#include <cisstMultiTask/mtsComponent.h>

// check if this module is built as a DLL
#ifdef mtsExPeriodicTaskComponents_EXPORTS
#define CISST_THIS_LIBRARY_AS_DLL
#endif
#include <cisstCommon/cmnExportMacros.h>

// avoid impact on other modules
#undef CISST_THIS_LIBRARY_AS_DLL

class CISST_EXPORT clockComponent: public mtsComponent {
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION_ONEARG, CMN_LOG_LOD_RUN_ERROR);

 protected:
    osaStopwatch Timer;  // this is the actual component (wrapped)
    void GetTime(mtsDouble & time) const;  // used by the command "GetTime"

    // internal method to configure this component
    void SetupInterfaces(void);

 public:
    clockComponent(const std::string & componentName);
    ~clockComponent() {};
    void Configure(const std::string & CMN_UNUSED(filename) = "") {};
};

CMN_DECLARE_SERVICES_INSTANTIATION(clockComponent);

#endif // _clockComponent_h
