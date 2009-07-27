/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: devSensableHDMasterSlave.h 556 2009-07-17 20:19:24Z gsevinc1 $

  Author(s): Gorkem Sevinc, Anton Deguet
  Created on: 2008-07-17

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
#include <cisstCommon.h>
#include <cisstDevices/devSensableHD.h>

// Always include last
#include <cisstDevices/devExport.h>

class CISST_EXPORT devSensableHDMasterSlave: public devSensableHD {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 10);

public:
    devSensableHDMasterSlave(const std::string & taskName,
                             const std::string & firstDeviceName,
                             const std::string & secondDeviceName);

    ~devSensableHDMasterSlave(void) {};
    void UserControl(void);
    
    void GetPositions(void);
    void GetPositions(int DeviceNo);
    void SetScaleFactor(double Scale);
    void SetForceLimit(double FLimit);
    void SetOffsetMultiplier(double OffMult);
    void IncrementScaleFactor(void);
    void DecrementScaleFactor(void);
    void IncrementForceLimit(void);
    void DecrementForceLimit(void);
    
protected:
    vctFixedSizeVector<double, 6> ForceMaster;
    vctFixedSizeVector<double, 6> ForceSlave;
    vct3 WorkspaceOffset;
    vct3 ClutchOffset;
    vct3 RatchetOffset;
    vct3 Error;
    vct3 p2RGoal;
    vct3 p1Goal;
    vct3 p2Goal;
    vct3 p1Error;
    vct3 p2Error;
   
    prmForceCartesianSet    firstDeviceForce;
    prmForceCartesianSet    secondDeviceForce;
    prmPositionCartesianGet p1;
    prmPositionCartesianGet p2;
    prmPositionCartesianGet p1Clutched;
    prmPositionCartesianGet p2Clutched;
    prmPositionCartesianGet p2R;
    vct3 firstDeviceLastForce;
    vct3 secondDeviceLastForce;

    bool firstIteration;
    bool ratchetMaster;
    bool ratchetSlave;
    int clutch;
    double ScaleFactor;
    double FMax;
    double ForceFeedNormMaster;
    double ForceFeedNormSlave;
    double OffsetMultiplier;

};


CMN_DECLARE_SERVICES_INSTANTIATION(devSensableHDMasterSlave);

#endif // _devSensableHDMasterSlave_h

