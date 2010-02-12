/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: devSensableHDMasterSlave.h 556 2009-07-17 20:19:24Z gsevinc1 $

  Author(s): Gorkem Sevinc, Anton Deguet
  Created on: 2009-07-17

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
#include <cisstMultiTask/mtsTaskFromCallback.h>
#include <cisstDevices/devSensableHD.h>
#include <cisstRobot/robCollaborativeControlForce.h>

// Always include last
#include <cisstDevices/devExport.h>


class CISST_EXPORT devSensableHDMasterSlave: public devSensableHD {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:

    devSensableHDMasterSlave(const std::string & taskName,
                             const std::string & firstDeviceName,
                             const std::string & secondDeviceName,
                             const vctFrm3 & relativeTransformation = vctFrm3::Identity());

    devSensableHDMasterSlave(const std::string & taskName,
                             const std::string & firstDeviceName,
                             const std::string & secondDeviceName,
                             const std::string & thirdDeviceName,
                             const std::string & fourthDeviceName);

    ~devSensableHDMasterSlave(void) {};

    /*!
        SetupTeleoperationInterfaces function takes in two device names and
        creates the necessary interfaces for multitasking.
    */
    void SetupTeleoperationInterfaces(const std::string & firstDeviceName, 
                                      const std::string & secondDeviceName,
                                      int pair);

    /*!
        UserControl function overloads the base function from devSensableHD. 
        This function has been designed to carry out any user control task
        using the Phantom Omnis. In this case, this function is carrying out
        teleoperation using the robCollaborativeControlForce class.
    */
    void UserControl(void);
    /*!
        Mutators for GUI related parameters, such as scale factor and force limit.
    */
    void SetLinearGainMaster(const mtsDouble& Scale);
    void SetLinearGainSlave(const mtsDouble& Scale);
    void SetForceLimit(const mtsDouble& FLimit);
    void SetForceMode(const mtsInt& Mode);
    void SetMasterClutch(const mtsBool& commandedClutch);
    void SetSlaveClutch(const mtsBool& commandedClutch);
    void SetMasterSlaveClutch(const mtsBool& commandedClutch);
    void SetForceCoefficient(const mtsDouble& commandedCoefficient);
    void SetCollaborativeControlParameter(const prmCollaborativeControlForce & commandedClutch);
    
protected:
    
    /*!
        DevData struct holds the current values provided by the devSensableHD class and
        the parameters calculated throughout teleoperation. This DevData struct is created
        for each pair of devices being used. 
    */
    struct DevData {
        vctFixedSizeVector<double, 6> ForceMaster;
        vctFixedSizeVector<double, 6> ForceSlave;
        prmCollaborativeControlForce Parameter;
        mtsBool                       MasterClutchGUI;
        mtsBool                       SlaveClutchGUI;
        mtsBool                       MasterSlaveClutchGUI;
        int                           MasterDeviceNo;
        int                           SlaveDeviceNo;
    };

    /*!
        Vectors of DevData and robCollaborativeControlForce objects.
    */
    std::vector<DevData *> DevicePair;
    std::vector<robCollaborativeControlForce *> RobotPair;
    int PairCount;
    mtsInt PairNumber;
};


CMN_DECLARE_SERVICES_INSTANTIATION(devSensableHDMasterSlave);

#endif // _devSensableHDMasterSlave_h

