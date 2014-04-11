/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2010-01-21

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsPeriodicTaskTest_h
#define _mtsPeriodicTaskTest_h

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsVector.h>

// code required for the DLL generation
#ifdef cisstMultiTaskPythonTest_EXPORTS
#define CISST_THIS_LIBRARY_AS_DLL
#endif
#include <cisstCommon/cmnExportMacros.h>
#undef CISST_THIS_LIBRARY_AS_DLL
// end of code for the DLL generation

class CISST_EXPORT mtsPeriodicTaskTest: public mtsTaskPeriodic {
    // used to control the log level, "Run Error" by default
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
 protected:
    // double
    mtsDouble Double;
    // vector
    mtsDoubleVec Vector;

    void AddDouble(const mtsDouble & addend); // method used for write command
    void ZeroAll(void); // method used for void command
	
 public:
    // provide a frequency (time interval between calls to the
    // periodic Run).  Also used to populate the interface(s)
    mtsPeriodicTaskTest(double periodInSeconds);
    inline ~mtsPeriodicTaskTest() {};
    // all four methods are pure virtual in mtsTask
    inline void Configure(const std::string & CMN_UNUSED(filename)) {};
    inline void Startup(void) {};
    void Run(void);
    inline void Cleanup(void) {};
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsPeriodicTaskTest);

#endif // _mtsPeriodicTaskTest_h
