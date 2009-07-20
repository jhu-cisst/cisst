/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: devSensableHD.cpp 557 2009-07-17 20:39:06Z gsevinc1 $

  Author(s): Gorkem Sevinc, Anton Deguet
  Created on: 2008-04-04

  (C) Copyright 2008-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

// Sensable headers
#include <cisstDevices/devSensableHDMasterSlave.h>

CMN_IMPLEMENT_SERVICES(devSensableHDMasterSlave);

devSensableHDMasterSlave::devSensableHDMasterSlave(const std::string & taskName,
                                                   const std::string & firstDeviceName,
                                                   const std::string & secondDeviceName):
    devSensableHD(taskName, firstDeviceName, secondDeviceName, true, true)
{
    if (firstDeviceName == secondDeviceName) {
        CMN_LOG_CLASS_INIT_ERROR << "In constructor: name of devices provided are identical, \""
                                 << firstDeviceName << "\" and \""
                                 << secondDeviceName << "\"" << std::endl;
    }
    CMN_LOG_CLASS_INIT_DEBUG << "constructor called, looking for \"" << firstDeviceName
                             << "\" and \"" << secondDeviceName << "\"" << std::endl;

    ScaleFactor = 0.15;
    FMax = 40.0;
    firstIteration = true;
}


void devSensableHDMasterSlave::UserControl()
{   
    ForceFeed.SetAll(0.0);
    posDiff.SetAll(0.0);
 
    firstDevicePos = DevicesVector(0)->PositionCartesian;    
    secondDevicePos = DevicesVector(1)->PositionCartesian;

    if(firstIteration)
    {
        firstIteration = false;
        Offset.DifferenceOf(firstDevicePos.Position().Translation(), secondDevicePos.Position().Translation());
    }

    posDiff.DifferenceOf(secondDevicePos.Position().Translation(), firstDevicePos.Position().Translation());
    Offset.Multiply(0.999);    

    //posDiff.Add(Offset);
    int i = 0;
    for(i; i<3; i++) {
        ForceFeed(i) = posDiff(i) + Offset(i);
    }
    // cap the forces
    ForceFeedNorm = ForceFeed.Norm();
    if(ForceFeedNorm >= FMax) {
        ForceFeed.Divide(FMax / ForceFeedNorm);
        Offset.DifferenceOf(firstDevicePos.Position().Translation(), secondDevicePos.Position().Translation());
    }

    ForceFeed.Multiply(ScaleFactor);

    // set force to the prm type
    firstDeviceForce.SetForce(ForceFeed);
    ForceFeed.Multiply(-1);
    secondDeviceForce.SetForce(ForceFeed);

    
    DevicesVector(0)->ForceCartesian = firstDeviceForce;
    DevicesVector(1)->ForceCartesian = secondDeviceForce;
}


