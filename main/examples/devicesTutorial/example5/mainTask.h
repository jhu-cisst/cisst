/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: displayTask.h 537 2009-07-14 00:32:25Z adeguet1 $ */

#ifndef _mainTask_h
#define _mainTask_h

#include <cisstCommon.h>
#include <cisstVector.h>
#include <cisstMultiTask.h>
#include <cisstParameterTypes.h>
#include <cisstRobot/robCollaborativeControlForce.h>

class mainTask: public mtsTaskPeriodic {
    // set log level to 5
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
    
 protected:
    
    // local copy of data used in commands
    prmPositionCartesianGet CartesianPosition;
    prmPositionCartesianGet CartesianPositionLocal;
    prmForceCartesianSet CartesianForce;
    prmForceCartesianSet CartesianForceLocal;
    mtsFunctionRead GetCartesianPosition;
    mtsFunctionRead GetCartesianPositionLocal;
    mtsFunctionWrite SetForceCartesian;
    mtsFunctionWrite SetForceCartesianLocal;

    // event handlers
    void Button1MasterEventHandler(const prmEventButton & buttonEvent);
    void Button1SlaveEventHandler(const prmEventButton & buttonEvent);

    // teleoperation data
    prmPositionCartesianGet       p1;
    prmPositionCartesianGet       p2;
    bool                          MasterClutch;
    bool                          SlaveClutch;
    prmForceCartesianSet          firstDeviceForce;
    prmForceCartesianSet          secondDeviceForce;
    vctFixedSizeVector<double, 6> ForceMaster;
    vctFixedSizeVector<double, 6> ForceSlave;

    // teleoperation class object
    robCollaborativeControlForce * Robot;

 public:
    // see sineTask.h documentation
    mainTask(const std::string & taskName, double period);
    ~mainTask() {};
    void Configure(const std::string & CMN_UNUSED(filename) = "") {};
    void Startup(void);
    void Run(void);
    void Cleanup(void) {};
};

CMN_DECLARE_SERVICES_INSTANTIATION(mainTask);

#endif // _mainTask_h

/*
  Author(s):  Gorkem Sevinc, Anton Deguet
  Created on: 2009-08-21

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/
