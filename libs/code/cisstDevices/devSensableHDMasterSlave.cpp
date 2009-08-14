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
    DevicesStruct.SetSize(1);
    DevicesStruct(0) = new DevData;
    // Initialize the vectors and variables
    InitializeVariables(0);
    if(DevicesVector(0)->Name == firstDeviceName) {
        DevicesStruct(0)->MasterDeviceNo = DevicesVector(0)->DeviceNumber;
        DevicesStruct(0)->SlaveDeviceNo = DevicesVector(1)->DeviceNumber;
    } else {
        DevicesStruct(0)->MasterDeviceNo = DevicesVector(1)->DeviceNumber;
        DevicesStruct(0)->SlaveDeviceNo = DevicesVector(0)->DeviceNumber;
    }
    PairNumber = 1;
    // Initialize provided interfaces
    SetupTeleoperationInterfaces(firstDeviceName, secondDeviceName, 0);
}

devSensableHDMasterSlave::devSensableHDMasterSlave(const std::string & taskName,
                                                   const std::string & firstDeviceName,
                                                   const std::string & secondDeviceName,
                                                   const std::string & thirdDeviceName,
                                                   const std::string & fourthDeviceName):
    devSensableHD(taskName, firstDeviceName, secondDeviceName, 
                  thirdDeviceName, fourthDeviceName, true, true, true, true)
{
    DevicesStruct.SetSize(2);
    DevicesStruct(0) = new DevData;
    DevicesStruct(1) = new DevData;
    DevicesStruct(0)->MasterDeviceNo = 0;
    DevicesStruct(0)->SlaveDeviceNo = 1;
    DevicesStruct(1)->MasterDeviceNo = 2;
    DevicesStruct(1)->SlaveDeviceNo = 3;
    
    // Initialize the vectors and variables
    int index = 0;
    for(index; index < 2; index++) {
        InitializeVariables(index);
    }   
    SetupTeleoperationInterfaces(firstDeviceName, secondDeviceName, 0);
    //SetupTeleoperationInterfaces(thirdDeviceName, fourthDeviceName, 1);
    PairNumber = 2;
}

void devSensableHDMasterSlave::InitializeVariables(int index)
{
    DevicesStruct(index)->ClutchOffset.SetAll(0.0);
    DevicesStruct(index)->LeftClutchOffset.SetAll(0.0);
    DevicesStruct(index)->RightClutchOffset.SetAll(0.0);
    DevicesStruct(index)->LeftClutchMSOffset.SetAll(0.0);
    DevicesStruct(index)->RightClutchMSOffset.SetAll(0.0);
    DevicesStruct(index)->WorkspaceOffset.SetAll(0.0);
    DevicesStruct(index)->Error.SetAll(0.0);
    DevicesStruct(index)->ForceMaster.SetAll(0.0);
    DevicesStruct(index)->ForceSlave.SetAll(0.0);
    DevicesStruct(index)->ScaleFactor = 0.15;
    DevicesStruct(index)->FMax = 40.0;
    DevicesStruct(index)->ForceMode = 0;
    DevicesStruct(index)->clutchMode = 0;
    DevicesStruct(index)->ForceMasterCoefficient = 1.0;
    DevicesStruct(index)->firstIteration = true;
    DevicesStruct(index)->MasterClutch = false;
    DevicesStruct(index)->SlaveClutch = false;
    DevicesStruct(index)->MasterSlaveClutch = false;
    DevicesStruct(index)->clutchDone = false;
    DevicesStruct(index)->bothClutched = false;
    DevicesStruct(index)->clutchOffsetAdd = false;
}

void devSensableHDMasterSlave::SetupTeleoperationInterfaces(const std::string & firstDeviceName, 
                                                            const std::string & secondDeviceName,
                                                            int pair)
{
    mtsProvidedInterface * providedInterface;
    std::string providedInterfaceName = "TeleoperationParameters" + firstDeviceName + secondDeviceName;
    providedInterface = AddProvidedInterface(providedInterfaceName);

    DevData * pairData;
    pairData = DevicesStruct(pair);
    StateTable.AddData(pairData->ScaleFactor, "ScaleFactor");
    StateTable.AddData(pairData->FMax, "ForceLimit");
    StateTable.AddData(pairData->MasterClutch, "MasterClutch");
    StateTable.AddData(pairData->SlaveClutch, "SlaveClutch");
    StateTable.AddData(pairData->MasterSlaveClutch, "MasterSlaveClutch");
    StateTable.AddData(pairData->ForceMode, "ForceMode");
    StateTable.AddData(pairData->ForceMasterCoefficient, "ForceCoefficient");

    providedInterface->AddCommandReadState(StateTable, pairData->ScaleFactor, "GetScaleFactor");
    providedInterface->AddCommandReadState(StateTable, pairData->FMax, "GetForceLimit");
    providedInterface->AddCommandReadState(StateTable, pairData->MasterClutch, "GetMasterClutch");
    providedInterface->AddCommandReadState(StateTable, pairData->SlaveClutch, "GetSlaveClutch");
    providedInterface->AddCommandReadState(StateTable, pairData->MasterSlaveClutch, "GetMasterSlaveClutch");
    providedInterface->AddCommandReadState(StateTable, pairData->ForceMode, "GetForceMode");
    providedInterface->AddCommandReadState(StateTable, pairData->ForceMasterCoefficient, "GetForceCoefficient");

    providedInterface->AddCommandWrite(&devSensableHDMasterSlave::SetScaleFactor, 
                                        this, "SetScaleFactor", pairData->ScaleFactor);
    providedInterface->AddCommandWrite(&devSensableHDMasterSlave::SetForceLimit, 
                                        this, "SetForceLimit", pairData->FMax);
    providedInterface->AddCommandWrite(&devSensableHDMasterSlave::SetMasterClutch, 
                                        this, "SetMasterClutch", pairData->MasterClutch);
    providedInterface->AddCommandWrite(&devSensableHDMasterSlave::SetSlaveClutch, 
                                        this, "SetSlaveClutch", pairData->SlaveClutch);
    providedInterface->AddCommandWrite(&devSensableHDMasterSlave::SetMasterSlaveClutch, 
                                        this, "SetMasterSlaveClutch", pairData->MasterSlaveClutch);
    providedInterface->AddCommandWrite(&devSensableHDMasterSlave::SetForceMode, 
                                        this, "SetForceMode", pairData->ForceMode);
    providedInterface->AddCommandWrite(&devSensableHDMasterSlave::SetForceCoefficient, 
                                        this, "SetForceCoefficient", pairData->ForceMasterCoefficient);

    providedInterface->AddCommandVoid(&devSensableHDMasterSlave::IncrementScaleFactor, 
                                        this, "IncrementScaleFactor");
    providedInterface->AddCommandVoid(&devSensableHDMasterSlave::DecrementScaleFactor, 
                                        this, "DecrementScaleFactor");
    providedInterface->AddCommandVoid(&devSensableHDMasterSlave::IncrementForceLimit, 
                                        this, "IncrementForceLimit");
    providedInterface->AddCommandVoid(&devSensableHDMasterSlave::DecrementForceLimit, 
                                        this, "DecrementForceLimit");
}


void devSensableHDMasterSlave::UserControl(void)
{   
    DevData * pairData;
    int index = 0;
    for(index; index < PairNumber; index++)
    {
        pairData = DevicesStruct(index);
        pairData->ForceMaster.SetAll(0.0);
        pairData->ForceSlave.SetAll(0.0);

        //If the first iteration
        if(pairData->firstIteration) {
            pairData->firstIteration = false;
            // Compute the initial offset between the two devices
            pairData->p1 = DevicesVector(pairData->MasterDeviceNo)->PositionCartesian;
            pairData->p2 = DevicesVector(pairData->SlaveDeviceNo)->PositionCartesian;
            pairData->WorkspaceOffset.DifferenceOf(pairData->p1.Position().Translation(), pairData->p2.Position().Translation());
        }

        // If clutching the first device
        if((DevicesVector(pairData->MasterDeviceNo)->Clutch == true || pairData->MasterClutch == true) 
                                 && DevicesVector(pairData->SlaveDeviceNo)->Clutch == false) {
            // Get the positions of the second device
            pairData->p2 = DevicesVector(pairData->SlaveDeviceNo)->PositionCartesian;

            if(pairData->clutchOffsetAdd == true) {
                pairData->p2Goal.Subtract(pairData->RightClutchMSOffset);
                pairData->clutchOffsetAdd = false;
            }
            // Compute the current force (F = kp * (secondDeviceGoal - secondDeviceLastPosition) )
            pairData->ForceSlave.XYZ().DifferenceOf(pairData->p2Goal, pairData->p2.Position().Translation());
            pairData->ForceSlave.XYZ().Multiply(pairData->ScaleFactor);

            // Cap the forces, apply ratchet effect or apply the current force
            //  on the slave device depending on the force mode
            pairData->ForceFeedNormSlave = pairData->ForceSlave.XYZ().Norm();
            if(pairData->ForceFeedNormSlave >= (pairData->FMax / 4.0)) {
                if(pairData->ForceMode == 0) {
                    pairData->p2Goal.Assign(pairData->p2.Position().Translation());
                } else if (pairData->ForceMode == 1) {
                    pairData->ForceSlave.Divide(pairData->FMax / pairData->ForceFeedNormSlave);
                }
            }

            // Apply the force computed to the slave device, apply zero force to the master device
            DevicesVector(pairData->SlaveDeviceNo)->ForceCartesian.SetForce(pairData->ForceSlave);
            DevicesVector(pairData->MasterDeviceNo)->ForceCartesian.SetAll(0.0, true);
            
            // Compute the offset of the master device from its original position
            pairData->p1Clutched = DevicesVector(pairData->MasterDeviceNo)->PositionCartesian;
            pairData->LeftClutchOffset.DifferenceOf(pairData->p1Clutched.Position().Translation(), pairData->p1.Position().Translation());
            
            pairData->clutchMode = 1;
            pairData->clutchDone = true;
        } else if((DevicesVector(pairData->SlaveDeviceNo)->Clutch == true || pairData->SlaveClutch == true) 
                                     && DevicesVector(pairData->MasterDeviceNo)->Clutch == false) {
            // Get the positions of the first device
            pairData->p1 = DevicesVector(pairData->MasterDeviceNo)->PositionCartesian;

            // Compute the current force (F = kp * (firstDeviceGoal - firstDeviceLastPosition) )
            if(pairData->clutchOffsetAdd == true) {
                pairData->p1Goal.Add(pairData->LeftClutchMSOffset);
                pairData->clutchOffsetAdd = false;
            }
            pairData->ForceMaster.XYZ().DifferenceOf(pairData->p1Goal, pairData->p1.Position().Translation());
            pairData->ForceMaster.XYZ().Multiply(pairData->ScaleFactor);

            // Cap the forces, apply ratchet effect or apply the current force
            //  on the slave device depending on the force mode
            pairData->ForceFeedNormMaster = pairData->ForceMaster.XYZ().Norm();
            if(pairData->ForceFeedNormMaster >= (pairData->FMax / 4.0)) {
                if(pairData->ForceMode == 0) {
                    pairData->p1Goal.Assign(pairData->p1.Position().Translation());
                } else if (pairData->ForceMode == 1) {
                    pairData->ForceMaster.Divide(pairData->FMax / pairData->ForceFeedNormMaster);
                }
            }

            // Apply the force computed to the master device, apply zero force to the slave device
            DevicesVector(pairData->MasterDeviceNo)->ForceCartesian.SetForce(pairData->ForceMaster);
            DevicesVector(pairData->SlaveDeviceNo)->ForceCartesian.SetAll(0.0, true);
            
            // Compute the offset of the slave device from its original position
            pairData->p2Clutched = DevicesVector(pairData->SlaveDeviceNo)->PositionCartesian;
            pairData->RightClutchOffset.DifferenceOf(pairData->p2Clutched.Position().Translation(), pairData->p2.Position().Translation());
            
            pairData->clutchMode = 2;   
            pairData->clutchDone = true;
        } else if ((DevicesVector(pairData->MasterDeviceNo)->Clutch == true 
                  && DevicesVector(pairData->SlaveDeviceNo)->Clutch == true) || pairData->MasterSlaveClutch == true) {
            // Set both devices' forces to 0
            DevicesVector(pairData->MasterDeviceNo)->ForceCartesian.SetAll(0.0, true);
            DevicesVector(pairData->SlaveDeviceNo)->ForceCartesian.SetAll(0.0, true);

            // Get the positions of both devices
            pairData->p1Clutched = DevicesVector(pairData->MasterDeviceNo)->PositionCartesian;
            pairData->p2Clutched = DevicesVector(pairData->SlaveDeviceNo)->PositionCartesian;
            
            // Compute left and right clutch offsets
            pairData->LeftClutchMSOffset.DifferenceOf(pairData->p1Clutched.Position().Translation(), pairData->p1.Position().Translation());
            pairData->RightClutchMSOffset.DifferenceOf(pairData->p2Clutched.Position().Translation(), pairData->p2.Position().Translation());
            
            pairData->clutchOffsetAdd = pairData->bothClutched = pairData->clutchDone = true;
        } else {
            // Read in the current positions of the two devices
            pairData->p1 = DevicesVector(pairData->MasterDeviceNo)->PositionCartesian;
            pairData->p2 = DevicesVector(pairData->SlaveDeviceNo)->PositionCartesian;

            // If clutching is done
            if(pairData->clutchMode != 0 && pairData->clutchDone == true) {
                pairData->clutchDone = false;
                // Add the clutch offset to the current offset
                if(pairData->clutchMode == 1) {
                    pairData->WorkspaceOffset.Add(pairData->LeftClutchOffset);
                } 
                if(pairData->clutchMode == 2) {
                    pairData->WorkspaceOffset.Subtract(pairData->RightClutchOffset);
                }
                if(pairData->bothClutched == true) {
                    pairData->WorkspaceOffset.DifferenceOf(pairData->p1.Position().Translation(), pairData->p2.Position().Translation());
                    pairData->bothClutched = false;
                }
                pairData->LeftClutchOffset.SetAll(0.0);
                pairData->RightClutchOffset.SetAll(0.0);
            }
            
            //p2R = p2 + WorkspaceOffset
            pairData->p2R.Position().Translation().SumOf(pairData->p2.Position().Translation(), pairData->WorkspaceOffset);
            //Error = p2R - p1
            pairData->Error.DifferenceOf(pairData->p2R.Position().Translation(), pairData->p1.Position().Translation());
            pairData->Error.Multiply(0.5);
            //p2RGoal = p2R - 1/2(Error)
            pairData->p2RGoal.DifferenceOf(pairData->p2R.Position().Translation(), pairData->Error);
            //p1Goal = p1 + 1/2(Error)
            pairData->p1Goal.SumOf(pairData->p1.Position().Translation(), pairData->Error);
            //p2Goal = p2 - 1/2(Error)
            pairData->p2Goal.DifferenceOf(pairData->p2.Position().Translation(),pairData-> Error);

            // ForceMaster = kp * (p1Goal - p1)
            // ForceSlave = kp * (p2RGoal - p2R)
            pairData->ForceMaster.XYZ().DifferenceOf(pairData->p1Goal, pairData->p1.Position().Translation());
            pairData->ForceSlave.XYZ().DifferenceOf(pairData->p2RGoal, pairData->p2R.Position().Translation());

            // Cap the forces, apply ratchet effect or apply the current force
            // on the master and on the slave device depending on the force mode
            //if(||ForceMaster|| > ForceLimit) { ForceMaster /= (FLimit / ||ForceMaster||)
            pairData->ForceFeedNormMaster = pairData->ForceMaster.XYZ().Norm();
            if(pairData->ForceFeedNormMaster >= pairData->FMax) {
                if(pairData->ForceMode == 0) {
                    pairData->WorkspaceOffset.DifferenceOf(pairData->p1.Position().Translation(), pairData->p2.Position().Translation());
                } else if(pairData->ForceMode == 1) {
                    pairData->ForceMaster.Divide(pairData->FMax / pairData->ForceFeedNormMaster);
                }
            }

            pairData->ForceFeedNormSlave = pairData->ForceSlave.XYZ().Norm();
            if(pairData->ForceFeedNormSlave >= pairData->FMax) {
                if(pairData->ForceMode == 0) {
                    pairData->WorkspaceOffset.DifferenceOf(pairData->p1.Position().Translation(), pairData->p2.Position().Translation());
                } else if(pairData->ForceMode == 1) {
                    pairData->ForceSlave.Divide(pairData->FMax / pairData->ForceFeedNormSlave);
                }
            }

            // Apply the scale factor (kp) to the forces
            pairData->ForceMaster.Multiply(pairData->ScaleFactor);
            pairData->ForceSlave.Multiply(pairData->ScaleFactor);

            //Apply the coefficient
            pairData->ForceMaster.Multiply(pairData->ForceMasterCoefficient);

            // Set force to the prm type
            pairData->firstDeviceForce.SetForce(pairData->ForceMaster);
            pairData->secondDeviceForce.SetForce(pairData->ForceSlave);
            
            // Assign the forces on the master and slave devices
            DevicesVector(pairData->MasterDeviceNo)->ForceCartesian = pairData->firstDeviceForce;
            DevicesVector(pairData->SlaveDeviceNo)->ForceCartesian = pairData->secondDeviceForce;
        }  
    }
}

void devSensableHDMasterSlave::SetScaleFactor(const mtsDouble & Scale)
{
    DevicesStruct(0)->ScaleFactor = Scale;
}

void devSensableHDMasterSlave::SetForceLimit(const mtsDouble& FLimit)
{
    DevicesStruct(0)->FMax = FLimit;
}

void devSensableHDMasterSlave::SetForceMode(const mtsInt& Mode)
{
    DevicesStruct(0)->ForceMode = Mode;
}

void devSensableHDMasterSlave::SetMasterClutch(const mtsBool& commandedClutch)
{
    DevicesStruct(0)->MasterClutch = commandedClutch;
}

void devSensableHDMasterSlave::SetSlaveClutch(const mtsBool& commandedClutch)
{
    DevicesStruct(0)->SlaveClutch = commandedClutch;
}

void devSensableHDMasterSlave::SetMasterSlaveClutch(const mtsBool& commandedClutch)
{
    DevicesStruct(0)->MasterSlaveClutch = commandedClutch;
}

void devSensableHDMasterSlave::SetForceCoefficient(const mtsDouble& commandedCoefficient)
{
    DevicesStruct(0)->ForceMasterCoefficient = commandedCoefficient;
}

void devSensableHDMasterSlave::IncrementScaleFactor(void)
{
    DevicesStruct(0)->ScaleFactor *= 1.25;
}

void devSensableHDMasterSlave::DecrementScaleFactor(void)
{
    DevicesStruct(0)->ScaleFactor *= 0.8;
}

void devSensableHDMasterSlave::IncrementForceLimit(void)
{
    DevicesStruct(0)->FMax *= 1.25;
}

void devSensableHDMasterSlave::DecrementForceLimit(void)
{
    DevicesStruct(0)->FMax *= 0.8;
}

