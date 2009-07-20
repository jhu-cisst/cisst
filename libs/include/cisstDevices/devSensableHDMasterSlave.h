/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: devSensableHD.h 556 2009-07-17 20:19:24Z gsevinc1 $

  Author(s): Gorkem Sevinc, Anton Deguet
  Created on: 2008-04-04

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _devSensableHDMasterSlave_h
#define _devSensableHDMasterSlave_h

#include <cisstParameterTypes.h>
#include <cisstDevices/devSensableHD.h>

// Always include last
#include <cisstDevices/devExport.h>

class CISST_EXPORT devSensableHDMasterSlave: public devSensableHD {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 10);

public:
    devSensableHDMasterSlave(const std::string & taskName,
                             const std::string & firstDeviceName,
                             const std::string & secondDeviceName);

    ~devSensableHDMasterSlave() {};
    void UserControl();

protected:
    bool firstIteration;
    vctFixedSizeVector<double, 6> ForceFeed;
    double ScaleFactor;
    double FMax;
    double ForceFeedNorm;
    vct3 posDiff;
    
    prmForceCartesianSet    firstDeviceForce;
    prmForceCartesianSet    secondDeviceForce;
    prmPositionCartesianGet firstDevicePos;
    prmPositionCartesianGet secondDevicePos;

    vct3 Offset;
};


CMN_DECLARE_SERVICES_INSTANTIATION(devSensableHDMasterSlave);

#endif // _devSensableHDMasterSlave_h

