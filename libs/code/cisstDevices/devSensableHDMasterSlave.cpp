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
    // Initialize the vectors and variables
    ClutchOffset.SetAll(0.0);
    LeftClutchOffset.SetAll(0.0);
    RightClutchOffset.SetAll(0.0);
    LeftClutchMSOffset.SetAll(0.0);
    RightClutchMSOffset.SetAll(0.0);
    WorkspaceOffset.SetAll(0.0);
    Error.SetAll(0.0);
    ForceMaster.SetAll(0.0);
    ForceSlave.SetAll(0.0);
    ScaleFactor = 0.15;
    FMax = 40.0;
    ForceMode = 0;
    clutchMode = 0;
    ForceMasterCoefficient = 1.0;
    firstIteration = true;
    MasterClutch = false;
    SlaveClutch = false;
    MasterSlaveClutch = false;
    clutchDone = false;
    bothClutched = false;
    clutchOffsetAdd = false;

    // Initialize provided interfaces
    mtsProvidedInterface * providedInterface;
    providedInterface = AddProvidedInterface("ProvidesSensableHDMasterSlave");

    StateTable.AddData(ScaleFactor, "ScaleFactor");
    StateTable.AddData(FMax, "ForceLimit");
    StateTable.AddData(MasterClutch, "MasterClutch");
    StateTable.AddData(SlaveClutch, "SlaveClutch");
    StateTable.AddData(MasterSlaveClutch, "MasterSlaveClutch");
    StateTable.AddData(ForceMode, "ForceMode");
    StateTable.AddData(ForceMasterCoefficient, "ForceCoefficient");

    providedInterface->AddCommandReadState(StateTable, ScaleFactor, "GetScaleFactor");
    providedInterface->AddCommandReadState(StateTable, FMax, "GetForceLimit");
    providedInterface->AddCommandReadState(StateTable, MasterClutch, "GetMasterClutch");
    providedInterface->AddCommandReadState(StateTable, SlaveClutch, "GetSlaveClutch");
    providedInterface->AddCommandReadState(StateTable, MasterSlaveClutch, "GetMasterSlaveClutch");
    providedInterface->AddCommandReadState(StateTable, ForceMode, "GetForceMode");
    providedInterface->AddCommandReadState(StateTable, ForceMasterCoefficient, "GetForceCoefficient");

    providedInterface->AddCommandWrite(&devSensableHDMasterSlave::SetScaleFactor, 
                                        this, "SetScaleFactor", ScaleFactor);
    providedInterface->AddCommandWrite(&devSensableHDMasterSlave::SetForceLimit, 
                                        this, "SetForceLimit", FMax);
    providedInterface->AddCommandWrite(&devSensableHDMasterSlave::SetMasterClutch, 
                                        this, "SetMasterClutch", MasterClutch);
    providedInterface->AddCommandWrite(&devSensableHDMasterSlave::SetSlaveClutch, 
                                        this, "SetSlaveClutch", SlaveClutch);
    providedInterface->AddCommandWrite(&devSensableHDMasterSlave::SetMasterSlaveClutch, 
                                        this, "SetMasterSlaveClutch", MasterSlaveClutch);
    providedInterface->AddCommandWrite(&devSensableHDMasterSlave::SetForceMode, 
                                        this, "SetForceMode", ForceMode);
    providedInterface->AddCommandWrite(&devSensableHDMasterSlave::SetForceCoefficient, 
                                        this, "SetForceCoefficient", ForceMasterCoefficient);

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
    ForceMaster.SetAll(0.0);
    ForceSlave.SetAll(0.0);

    //If the first iteration
    if(firstIteration) {
        firstIteration = false;
        // Compute the initial offset between the two devices
        p1 = DevicesVector(0)->PositionCartesian;
        p2 = DevicesVector(1)->PositionCartesian;
        WorkspaceOffset.DifferenceOf(p1.Position().Translation(), p2.Position().Translation());
    }

    // If clutching the first device
    if((DevicesVector(0)->Clutch == true || MasterClutch == true) && DevicesVector(1)->Clutch == false) {
        // Get the positions of the second device
        p2 = DevicesVector(1)->PositionCartesian;

        if(clutchOffsetAdd == true) {
            p2Goal.Subtract(RightClutchMSOffset);
            clutchOffsetAdd = false;
        }
        // Compute the current force (F = kp * (secondDeviceGoal - secondDeviceLastPosition) )
        ForceSlave.XYZ().DifferenceOf(p2Goal, p2.Position().Translation());
        ForceSlave.XYZ().Multiply(ScaleFactor);

        // Cap the forces, apply ratchet effect or apply the current force
        //  on the slave device depending on the force mode
        ForceFeedNormSlave = ForceSlave.XYZ().Norm();
        if(ForceFeedNormSlave >= (FMax / 4.0)) {
            if(ForceMode == 0) {
                p2Goal.Assign(p2.Position().Translation());
            } else if (ForceMode == 1) {
                ForceSlave.Divide(FMax / ForceFeedNormSlave);
            }
        }

        // Apply the force computed to the slave device, apply zero force to the master device
        DevicesVector(1)->ForceCartesian.SetForce(ForceSlave);
        DevicesVector(0)->ForceCartesian.SetAll(0.0, true);
        
        // Compute the offset of the master device from its original position
        p1Clutched = DevicesVector(0)->PositionCartesian;
        LeftClutchOffset.DifferenceOf(p1Clutched.Position().Translation(), p1.Position().Translation());
        
        clutchMode = 1;
        clutchDone = true;
    } else if((DevicesVector(1)->Clutch == true || SlaveClutch == true) && DevicesVector(0)->Clutch == false) {
        // Get the positions of the first device
        p1 = DevicesVector(0)->PositionCartesian;

        // Compute the current force (F = kp * (firstDeviceGoal - firstDeviceLastPosition) )
        if(clutchOffsetAdd == true) {
            p1Goal.Add(LeftClutchMSOffset);
            clutchOffsetAdd = false;
        }
        ForceMaster.XYZ().DifferenceOf(p1Goal, p1.Position().Translation());
        ForceMaster.XYZ().Multiply(ScaleFactor);

        // Cap the forces, apply ratchet effect or apply the current force
        //  on the slave device depending on the force mode
        ForceFeedNormMaster = ForceMaster.XYZ().Norm();
        if(ForceFeedNormMaster >= (FMax / 4.0)) {
            if(ForceMode == 0) {
                p1Goal.Assign(p1.Position().Translation());
            } else if (ForceMode == 1) {
                ForceMaster.Divide(FMax / ForceFeedNormMaster);
            }
        }

        // Apply the force computed to the master device, apply zero force to the slave device
        DevicesVector(0)->ForceCartesian.SetForce(ForceMaster);
        DevicesVector(1)->ForceCartesian.SetAll(0.0, true);
        
        // Compute the offset of the slave device from its original position
        p2Clutched = DevicesVector(1)->PositionCartesian;
        RightClutchOffset.DifferenceOf(p2Clutched.Position().Translation(), p2.Position().Translation());
        
        clutchMode = 2;   
        clutchDone = true;
    } else if ((DevicesVector(0)->Clutch == true && DevicesVector(1)->Clutch == true) || MasterSlaveClutch == true) {
        // Set both devices' forces to 0
        DevicesVector(0)->ForceCartesian.SetAll(0.0, true);
        DevicesVector(1)->ForceCartesian.SetAll(0.0, true);

        // Get the positions of both devices
        p1Clutched = DevicesVector(0)->PositionCartesian;
        p2Clutched = DevicesVector(1)->PositionCartesian;
        
        // Compute left and right clutch offsets
        LeftClutchMSOffset.DifferenceOf(p1Clutched.Position().Translation(), p1.Position().Translation());
        RightClutchMSOffset.DifferenceOf(p2Clutched.Position().Translation(), p2.Position().Translation());
        
        clutchOffsetAdd = bothClutched = clutchDone = true;
    } else {
        // Read in the current positions of the two devices
        p1 = DevicesVector(0)->PositionCartesian;
        p2 = DevicesVector(1)->PositionCartesian;

        // If clutching is done
        if(clutchMode != 0 && clutchDone == true) {
            clutchDone = false;
            // Add the clutch offset to the current offset
            if(clutchMode == 1) {
                WorkspaceOffset.Add(LeftClutchOffset);
            } 
            if(clutchMode == 2) {
                WorkspaceOffset.Subtract(RightClutchOffset);
            }
            if(bothClutched == true) {
                WorkspaceOffset.DifferenceOf(p1.Position().Translation(), p2.Position().Translation());
                bothClutched = false;
            }
            LeftClutchOffset.SetAll(0.0);
            RightClutchOffset.SetAll(0.0);
        }
        
        //p2R = p2 + WorkspaceOffset
        p2R.Position().Translation().SumOf(p2.Position().Translation(), WorkspaceOffset);
        //Error = p2R - p1
        Error.DifferenceOf(p2R.Position().Translation(), p1.Position().Translation());
        Error.Multiply(0.5);
        //p2RGoal = p2R - 1/2(Error)
        p2RGoal.DifferenceOf(p2R.Position().Translation(), Error);
        //p1Goal = p1 + 1/2(Error)
        p1Goal.SumOf(p1.Position().Translation(), Error);
        //p2Goal = p2 - 1/2(Error)
        p2Goal.DifferenceOf(p2.Position().Translation(), Error);

        // ForceMaster = kp * (p1Goal - p1)
        // ForceSlave = kp * (p2RGoal - p2R)
        ForceMaster.XYZ().DifferenceOf(p1Goal, p1.Position().Translation());
        ForceSlave.XYZ().DifferenceOf(p2RGoal, p2R.Position().Translation());

        // Cap the forces, apply ratchet effect or apply the current force
        // on the master and on the slave device depending on the force mode
        //if(||ForceMaster|| > ForceLimit) { ForceMaster /= (FLimit / ||ForceMaster||)
        ForceFeedNormMaster = ForceMaster.XYZ().Norm();
        if(ForceFeedNormMaster >= FMax) {
            if(ForceMode == 0) {
                WorkspaceOffset.DifferenceOf(p1.Position().Translation(), p2.Position().Translation());
            } else if(ForceMode == 1) {
                ForceMaster.Divide(FMax / ForceFeedNormMaster);
            }
        }

        ForceFeedNormSlave = ForceSlave.XYZ().Norm();
        if(ForceFeedNormSlave >= FMax) {
            if(ForceMode == 0) {
                WorkspaceOffset.DifferenceOf(p1.Position().Translation(), p2.Position().Translation());
            } else if(ForceMode == 1) {
                ForceSlave.Divide(FMax / ForceFeedNormSlave);
            }
        }

        // Apply the scale factor (kp) to the forces
        ForceMaster.Multiply(ScaleFactor);
        ForceSlave.Multiply(ScaleFactor);

        //Apply the coefficient
        ForceMaster.Multiply(ForceMasterCoefficient);

        // Set force to the prm type
        firstDeviceForce.SetForce(ForceMaster);
        secondDeviceForce.SetForce(ForceSlave);
        
        // Assign the forces on the master and slave devices
        DevicesVector(0)->ForceCartesian = firstDeviceForce;
        DevicesVector(1)->ForceCartesian = secondDeviceForce;
    }    
}

void devSensableHDMasterSlave::SetScaleFactor(const mtsDouble & Scale)
{
    ScaleFactor = Scale;
}

void devSensableHDMasterSlave::SetForceLimit(const mtsDouble& FLimit)
{
    FMax = FLimit;
}

void devSensableHDMasterSlave::SetForceMode(const mtsInt& Mode)
{
    ForceMode = Mode;
}

void devSensableHDMasterSlave::SetMasterClutch(const mtsBool& commandedClutch)
{
    MasterClutch = commandedClutch;
}

void devSensableHDMasterSlave::SetSlaveClutch(const mtsBool& commandedClutch)
{
    SlaveClutch = commandedClutch;
}

void devSensableHDMasterSlave::SetMasterSlaveClutch(const mtsBool& commandedClutch)
{
    MasterSlaveClutch = commandedClutch;
}

void devSensableHDMasterSlave::SetForceCoefficient(const mtsDouble& commandedCoefficient)
{
    ForceMasterCoefficient = commandedCoefficient;
}

void devSensableHDMasterSlave::IncrementScaleFactor(void)
{
    ScaleFactor *= 1.25;
}

void devSensableHDMasterSlave::DecrementScaleFactor(void)
{
    ScaleFactor *= 0.8;
}

void devSensableHDMasterSlave::IncrementForceLimit(void)
{
    FMax *= 1.25;
}

void devSensableHDMasterSlave::DecrementForceLimit(void)
{
    FMax *= 0.8;
}

