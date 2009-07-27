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
    WorkspaceOffset.SetAll(0.0);
    Error.SetAll(0.0);
    ForceMaster.SetAll(0.0);
    ForceSlave.SetAll(0.0);
    ScaleFactor = 0.15;
    FMax = 40.0;
    ForceMode = 0;
    firstIteration = true;
    clutch = false;
}


void devSensableHDMasterSlave::UserControl(void)
{   
    ForceMaster.SetAll(0.0);
    ForceSlave.SetAll(0.0);

    //If the first iteration
    if(firstIteration) {
        firstIteration = false;
        // Compute the initial offset between the two devices
        GetPositions();
        WorkspaceOffset.DifferenceOf(p1.Position().Translation(), p2.Position().Translation());
    }

    // If clutching the first device
    if(DevicesVector(0)->Clutch == true) {
        // Get the positions of the second device
        p2 = DevicesVector(1)->PositionCartesian;

        // Compute the current force (F = kp * (secondDeviceGoal - secondDeviceLastPosition) )
        ForceSlave.XYZ().DifferenceOf(p2Goal, p2.Position().Translation());
        ForceSlave.XYZ().Multiply(ScaleFactor);

        // Cap the forces, apply ratchet effect or apply the current force
        //  on the slave device depending on the force mode
        ForceFeedNormSlave = ForceSlave.XYZ().Norm();
        if(ForceFeedNormSlave >= (FMax / 4.0)) {
            if(ForceMode == 0) {
                ForceSlave.Divide(FMax / ForceFeedNormSlave);
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
        ClutchOffset.DifferenceOf(p1Clutched.Position().Translation(), p1.Position().Translation());
                
        clutch = true;
    } else {
        // Read in the current positions of the two devices
        GetPositions();
        // If clutching is done
        if(clutch) {
            clutch = false;
            // Add the clutch offset to the current offset
            WorkspaceOffset.Add(ClutchOffset);
            ClutchOffset.SetAll(0.0);
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

        // Force1 = kp * (p1Goal - p1)
        // Force2 = kp * (p2RGoal - p2)
        ForceMaster.XYZ().DifferenceOf(p1Goal, p1.Position().Translation());
        ForceSlave.XYZ().DifferenceOf(p2RGoal, p2R.Position().Translation());
        
        // Cap the forces, apply ratchet effect or apply the current force
        // on the master and on the slave device depending on the force mode
        //if(||Force1|| > ForceLimit) { Force1 /= (FLimit / ||Force1||)
        ForceFeedNormMaster = ForceMaster.XYZ().Norm();
        if(ForceFeedNormMaster >= FMax) {
            if(ForceMode == 0) {
                ForceMaster.Divide(FMax / ForceFeedNormMaster);
                WorkspaceOffset.DifferenceOf(p1.Position().Translation(), p2.Position().Translation());
            } else if(ForceMode == 1) {
                ForceMaster.Divide(FMax / ForceFeedNormMaster);
            }
        }

        ForceFeedNormSlave = ForceSlave.XYZ().Norm();
        if(ForceFeedNormSlave >= FMax) {
            if(ForceMode == 0) {
                ForceSlave.Divide(FMax / ForceFeedNormSlave);
                WorkspaceOffset.DifferenceOf(p1.Position().Translation(), p2.Position().Translation());
            } else if(ForceMode == 1) {
                ForceSlave.Divide(FMax / ForceFeedNormSlave);
            }
        }

        // Apply the scale factor (kp) to the forces
        ForceMaster.Multiply(ScaleFactor);
        ForceSlave.Multiply(ScaleFactor);

        // Set force to the prm type
        firstDeviceForce.SetForce(ForceMaster);
        secondDeviceForce.SetForce(ForceSlave);
        
        // Assign the forces on the master and slave devices
        DevicesVector(0)->ForceCartesian = firstDeviceForce;
        DevicesVector(1)->ForceCartesian = secondDeviceForce;
    }    
}

void devSensableHDMasterSlave::GetPositions(void)
{
    // Return the positions of both the master and slave devices
    p1 = DevicesVector(0)->PositionCartesian;
    p2 = DevicesVector(1)->PositionCartesian;
} 

void devSensableHDMasterSlave::SetScaleFactor(double Scale)
{
    ScaleFactor = Scale;
}

void devSensableHDMasterSlave::SetForceLimit(double FLimit)
{
    FMax = FLimit;
}

void devSensableHDMasterSlave::IncrementScaleFactor(void)
{
    ScaleFactor *= 1.25;
    printf("- Scale factor: %f\n", ScaleFactor);
}

void devSensableHDMasterSlave::DecrementScaleFactor(void)
{
    ScaleFactor *= 0.8;
    printf("- Scale factor: %f\n", ScaleFactor);
}

void devSensableHDMasterSlave::IncrementForceLimit(void)
{
    FMax *= 1.25;
    printf("- Force Limit: %f\n", FMax);
}

void devSensableHDMasterSlave::DecrementForceLimit(void)
{
    FMax *= 0.8;
    printf("- Force Limit: %f\n", FMax);
}

void devSensableHDMasterSlave::SetForceMode(int Mode)
{
    ForceMode = Mode;
    if(Mode == 0) {
        printf("* Ratchet Mode active.\n");
    } else if(Mode == 1) {
        printf("* Force Capping Mode active.\n");
    } else if(Mode == 2) {
        printf("* infinite Force Mode active.\n");
    }
}
