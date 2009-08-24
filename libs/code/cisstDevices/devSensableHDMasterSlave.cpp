/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: devSensableHDMasterSlave.cpp 557 2009-07-17 20:39:06Z gsevinc1 $

  Author(s): Gorkem Sevinc, Anton Deguet
  Created on: 2008-07-17

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
    // Initialize the struct and class vectors
    RobotPair.resize(1);
    DevicePair.resize(1);
    DevicePair[0] = new DevData;
    RobotPair[0] = new robCollaborativeControlForce();
    
    // Assign device numbers
    if(DevicesVector[0]->Name == firstDeviceName) {
        DevicePair[0]->MasterDeviceNo = DevicesVector(0)->DeviceNumber;
        DevicePair[0]->SlaveDeviceNo = DevicesVector(1)->DeviceNumber;
    } else {
        DevicePair[0]->MasterDeviceNo = DevicesVector(1)->DeviceNumber;
        DevicePair[0]->SlaveDeviceNo = DevicesVector(0)->DeviceNumber;
    }

    PairNumber = 0;
    PairCount = 1;
    // Initialize provided interfaces
    SetupTeleoperationInterfaces(firstDeviceName, secondDeviceName, PairNumber);
}

devSensableHDMasterSlave::devSensableHDMasterSlave(const std::string & taskName,
                                                   const std::string & firstDeviceName,
                                                   const std::string & secondDeviceName,
                                                   const std::string & thirdDeviceName,
                                                   const std::string & fourthDeviceName):
    devSensableHD(taskName, firstDeviceName, secondDeviceName, 
                  thirdDeviceName, fourthDeviceName, true, true, true, true)
{
    // Initialize the class and struct vectors
    RobotPair.resize(2);
    DevicePair.resize(2);
    DevicePair[0] = new DevData;
    DevicePair[1] = new DevData;
    RobotPair[0] = new robCollaborativeControlForce();
    RobotPair[1] = new robCollaborativeControlForce();

    // Assign device numbers, * hard coded for now *
    DevicePair[0]->MasterDeviceNo = 0;
    DevicePair[0]->SlaveDeviceNo = 1;
    DevicePair[1]->MasterDeviceNo = 2;
    DevicePair[1]->SlaveDeviceNo = 3;
    
    // Pair Number assigns which pair is being used for the GUI. 
    // Change this value to test the other pair, default is the first pair.
    PairNumber = 0;
    PairCount = 2;
    //Initialize provided interfaces
    SetupTeleoperationInterfaces(firstDeviceName, secondDeviceName, PairNumber);
}

void devSensableHDMasterSlave::SetupTeleoperationInterfaces(const std::string & firstDeviceName, 
                                                            const std::string & secondDeviceName,
                                                            int pair)
{
    // Create the provided interface
    mtsProvidedInterface * providedInterface;
    std::string providedInterfaceName = "TeleoperationParameters" + firstDeviceName + secondDeviceName;
    providedInterface = AddProvidedInterface(providedInterfaceName);

    // Initialize Values
    DevData * pairData;
    pairData = DevicePair[pair];
    pairData->ForceLimit = 40.0;
    pairData->ScaleFactor = 0.15;
    pairData->ForceCoefficient = 1.0;
    pairData->ForceMode = 0;
    pairData->MasterClutchGUI = pairData->SlaveClutchGUI = pairData->MasterSlaveClutchGUI = false;

    // Add values to the state table
    StateTable.AddData(pairData->ScaleFactor, "ScaleFactor");
    StateTable.AddData(pairData->ForceLimit, "ForceLimit");
    StateTable.AddData(pairData->MasterClutchGUI, "MasterClutch");
    StateTable.AddData(pairData->SlaveClutchGUI, "SlaveClutch");
    StateTable.AddData(pairData->MasterSlaveClutchGUI, "MasterSlaveClutch");
    StateTable.AddData(pairData->ForceMode, "ForceMode");
    StateTable.AddData(pairData->ForceCoefficient, "ForceCoefficient");

    // Add read functions to the interface
    providedInterface->AddCommandReadState(StateTable, pairData->ScaleFactor, "GetScaleFactor");
    providedInterface->AddCommandReadState(StateTable, pairData->ForceLimit, "GetForceLimit");
    providedInterface->AddCommandReadState(StateTable, pairData->MasterClutchGUI, "GetMasterClutch");
    providedInterface->AddCommandReadState(StateTable, pairData->SlaveClutchGUI, "GetSlaveClutch");
    providedInterface->AddCommandReadState(StateTable, pairData->MasterSlaveClutchGUI, "GetMasterSlaveClutch");
    providedInterface->AddCommandReadState(StateTable, pairData->ForceMode, "GetForceMode");
    providedInterface->AddCommandReadState(StateTable, pairData->ForceCoefficient, "GetForceCoefficient");

    // Add write functions to the interface
    providedInterface->AddCommandWrite(&devSensableHDMasterSlave::SetScaleFactor, 
                                        this, "SetScaleFactor", pairData->ScaleFactor);
    providedInterface->AddCommandWrite(&devSensableHDMasterSlave::SetForceLimit, 
                                        this, "SetForceLimit", pairData->ForceLimit);
    providedInterface->AddCommandWrite(&devSensableHDMasterSlave::SetMasterClutch, 
                                        this, "SetMasterClutch", pairData->MasterClutchGUI);
    providedInterface->AddCommandWrite(&devSensableHDMasterSlave::SetSlaveClutch, 
                                        this, "SetSlaveClutch", pairData->SlaveClutchGUI);
    providedInterface->AddCommandWrite(&devSensableHDMasterSlave::SetMasterSlaveClutch, 
                                        this, "SetMasterSlaveClutch", pairData->MasterSlaveClutchGUI);
    providedInterface->AddCommandWrite(&devSensableHDMasterSlave::SetForceMode, 
                                        this, "SetForceMode", pairData->ForceMode);
    providedInterface->AddCommandWrite(&devSensableHDMasterSlave::SetForceCoefficient, 
                                        this, "SetForceCoefficient", pairData->ForceCoefficient);
}


void devSensableHDMasterSlave::UserControl(void)
{   
    DevData * pairData;
    int index = 0;
    for(index; index < PairCount; index++)
    {
        // Get the device pair data
        pairData = DevicePair[index];

        // Call robCollaborativeControlForce Update function, which carries out the 
        // teleoperation process with the given positions and clutches, then returns
        // two forces
        RobotPair[index]->Update(DevicesVector(pairData->MasterDeviceNo)->PositionCartesian.Position().Translation(), 
                                 DevicesVector(pairData->SlaveDeviceNo)->PositionCartesian.Position().Translation(), 
                                 DevicesVector(pairData->MasterDeviceNo)->Clutch, 
                                 DevicesVector(pairData->SlaveDeviceNo)->Clutch, 
                                 DevicesVector(pairData->MasterDeviceNo)->ForceCartesian.Force(), 
                                 DevicesVector(pairData->SlaveDeviceNo)->ForceCartesian.Force());

        // If the pair has the GUI attached
        if(index == PairNumber) {
            // Update the values through the GUI
            RobotPair[index]->SetParameters(pairData->ForceLimit.Data, pairData->ScaleFactor.Data, 
                                            pairData->ForceCoefficient.Data, pairData->ForceMode.Data, 
                                            pairData->MasterClutchGUI.Data, pairData->SlaveClutchGUI.Data,
                                            pairData->MasterSlaveClutchGUI.Data);
        }
    }
}

void devSensableHDMasterSlave::SetScaleFactor(const mtsDouble & Scale)
{
    DevicePair[PairNumber]->ScaleFactor = Scale;
}

void devSensableHDMasterSlave::SetForceLimit(const mtsDouble& FLimit)
{
    DevicePair[PairNumber]->ForceLimit = FLimit;
}

void devSensableHDMasterSlave::SetForceMode(const mtsInt& Mode)
{
    DevicePair[PairNumber]->ForceMode = Mode;
}

void devSensableHDMasterSlave::SetMasterClutch(const mtsBool& commandedClutch)
{
    DevicePair[PairNumber]->MasterClutchGUI = commandedClutch;
}

void devSensableHDMasterSlave::SetSlaveClutch(const mtsBool& commandedClutch)
{
    DevicePair[PairNumber]->SlaveClutchGUI = commandedClutch;
}

void devSensableHDMasterSlave::SetMasterSlaveClutch(const mtsBool& commandedClutch)
{
    DevicePair[PairNumber]->MasterSlaveClutchGUI = commandedClutch;
}

void devSensableHDMasterSlave::SetForceCoefficient(const mtsDouble& commandedCoefficient)
{
    DevicePair[PairNumber]->ForceCoefficient = commandedCoefficient;
}
