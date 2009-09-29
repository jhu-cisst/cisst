/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
// $Id: clockDevice.h 456 2009-06-13 03:11:44Z adeguet1 $

#ifndef _clockDevice_h
#define _clockDevice_h

#include <cisstOSAbstraction.h>
#include <cisstMultiTask.h>

class clockDevice: public mtsDevice {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
    
 protected:
    osaStopwatch Timer;  // this is the actual device (wrapped)
    void GetTime(mtsDouble & time) const;  // used by the command "GetTime"
	
 public:
    // constructor doesn't need a period!
    clockDevice(const std::string & deviceName);
    ~clockDevice() {};
    void Configure(const std::string & CMN_UNUSED(filename) = "") {};
    // no Startup, Run, Cleanup required
};

CMN_DECLARE_SERVICES_INSTANTIATION(clockDevice);

#endif // _clockDevice_h

/*
  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/
