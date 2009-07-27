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
    //Initialize the vectors and variables
    ClutchOffset.SetAll(0.0);
    WorkspaceOffset.SetAll(0.0);
    RatchetOffset.SetAll(0.0);
    Error.SetAll(0.0);
    ForceMaster.SetAll(0.0);
    ForceSlave.SetAll(0.0);
    p2Error.SetAll(0.0);
    ScaleFactor = 0.15;
    FMax = 40.0;
    OffsetMultiplier = 0.999;
    firstIteration = true;
    ratchetMaster = ratchetSlave = false;
    clutch = 0;
}


void devSensableHDMasterSlave::UserControl(void)
{   
    ForceMaster.SetAll(0.0);
    ForceSlave.SetAll(0.0);

    //If the first iteration
    if(firstIteration) {
        firstIteration = false;
        //Compute the initial offset between the two devices
        GetPositions();
        WorkspaceOffset.DifferenceOf(p1.Position().Translation(), p2.Position().Translation());
        //RatchetOffset = WorkspaceOffset;
    }

    // If clutching the first device
    if(DevicesVector(0)->Clutch == true) {
        //Get the positions of the second device
        GetPositions(1);
        //Compute the current force (F = kp * (secondDeviceGoal - secondDeviceLastPosition) )
        p2Error.DifferenceOf(p2Goal, p2.Position().Translation());

        ForceSlave.XYZ().ProductOf(p2Error, ScaleFactor);

        ForceFeedNormSlave = ForceSlave.XYZ().Norm();
        if(ForceFeedNormSlave >= (FMax / 4.0)) {
            ForceSlave.Divide(FMax / ForceFeedNormSlave);
            p2Goal.Assign(p2.Position().Translation());
        }

        DevicesVector(1)->ForceCartesian.SetForce(ForceSlave);
        DevicesVector(0)->ForceCartesian.SetAll(0.0, true);
        
        p1Clutched = DevicesVector(0)->PositionCartesian;
        //keep offset while clutching
        ClutchOffset.DifferenceOf(p1Clutched.Position().Translation(), p1.Position().Translation());
                
        clutch = 1;
    } else {
        // Read in the current positions of the two devices
        GetPositions();
        //If clutching is done
        if(clutch != 0) {
            //Add the error on the slave's position to the master
            
            clutch = 0;
            //Add the clutch offset to the current offset
            WorkspaceOffset.Add(ClutchOffset);
            ClutchOffset.SetAll(0.0);
            p1Error.SetAll(0.0);
            p2Error.SetAll(0.0);
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

        p2Goal.DifferenceOf(p2.Position().Translation(), Error);

        //Force1 = kp * (p1Goal - p1)
        //Force2 = kp * (p2RGoal - p2)
        p1Error.DifferenceOf(p1Goal, p1.Position().Translation());
        p2Error.DifferenceOf(p2RGoal, p2R.Position().Translation());
        
        ForceMaster.XYZ().Assign(p1Error);
        ForceSlave.XYZ().Assign(p2Error);
        
        // cap the forces
        // if(||Force1|| > ForceLimit) { Force1 /= (FLimit / ||Force1||)
        ForceFeedNormMaster = ForceMaster.XYZ().Norm();
        if(ForceFeedNormMaster >= FMax) {
            ForceMaster.Divide(FMax / ForceFeedNormMaster);
            WorkspaceOffset.DifferenceOf(p1.Position().Translation(), p2.Position().Translation());
            ratchetMaster = true;
        }
        ForceFeedNormSlave = ForceSlave.XYZ().Norm();
        if(ForceFeedNormSlave >= FMax) {
            ForceSlave.Divide(FMax / ForceFeedNormSlave);
            WorkspaceOffset.DifferenceOf(p1.Position().Translation(), p2.Position().Translation());
            ratchetSlave = true;
        }

        ForceMaster.Multiply(ScaleFactor);
        ForceSlave.Multiply(ScaleFactor);

        // set force to the prm type
        firstDeviceForce.SetForce(ForceMaster);
        secondDeviceForce.SetForce(ForceSlave);
        DevicesVector(0)->ForceCartesian = firstDeviceForce;
        DevicesVector(1)->ForceCartesian = secondDeviceForce;
    }    



    /*
    ForceFeed.SetAll(0.0);
    PosDiff.SetAll(0.0);
 
    firstDevicePos = DevicesVector(0)->PositionCartesian;
    secondDevicePos = DevicesVector(1)->PositionCartesian;

    if(firstIteration)
    {
        firstIteration = false;
        RatchetOffset.DifferenceOf(firstDevicePos.Position().Translation(), secondDevicePos.Position().Translation());
    }

    if(DevicesVector(0)->Clutch == true)
    {
        int i = 0;
        for(i; i<3; i++) {
            ForceFeed(i) = secondDeviceLastForce(i);
        }
        DevicesVector(1)->ForceCartesian.SetForce(ForceFeed);
        
        //DevicesVector(1)->ForceCartesian = secondDeviceForce;
        DevicesVector(0)->ForceCartesian.SetAll(0.0, true);
        ClutchOffset.DifferenceOf(firstDevicePos.Position().Translation(), secondDevicePos.Position().Translation());
    }
    else if(DevicesVector(1)->Clutch == true)
    {
        int i = 0;
        for(i; i<3; i++) {
            ForceFeed(i) = firstDeviceLastForce(i);
        }
        DevicesVector(0)->ForceCartesian.SetForce(ForceFeed);
        
        //DevicesVector(0)->ForceCartesian = firstDeviceForce;
        DevicesVector(1)->ForceCartesian.SetAll(0.0, true);
        ClutchOffset.DifferenceOf(secondDevicePos.Position().Translation(), firstDevicePos.Position().Translation());
    }
    else
    {
        PosDiff.DifferenceOf(secondDevicePos.Position().Translation(), firstDevicePos.Position().Translation());
        RatchetOffset.Multiply(OffsetMultiplier);    

        int i = 0;
        for(i; i<3; i++) {
            //ForceFeed(i) = PosDiff(i) + RatchetOffset(i) + ClutchOffset(i);
            ForceFeed(i) = PosDiff(i) + ClutchOffset(i);
        }
        // cap the forces
        ForceFeedNorm = ForceFeed.Norm();
        if(ForceFeedNorm >= FMax) {
            ForceFeed.Divide(FMax / ForceFeedNorm);
            RatchetOffset.DifferenceOf(firstDevicePos.Position().Translation(), secondDevicePos.Position().Translation());
        }

        ForceFeed.Multiply(ScaleFactor);

        // set force to the prm type
        firstDeviceForce.SetForce(ForceFeed);
        firstDeviceLastForce.DifferenceOf(secondDevicePos.Position().Translation(), 
                                          firstDevicePos.Position().Translation());
        firstDeviceLastForce.Multiply(ScaleFactor);
        ForceFeed.Multiply(-1);
        secondDeviceForce.SetForce(ForceFeed);
        secondDeviceLastForce.DifferenceOf(firstDevicePos.Position().Translation(), 
                                           secondDevicePos.Position().Translation());
        secondDeviceLastForce.Multiply(ScaleFactor);

        
        DevicesVector(0)->ForceCartesian = firstDeviceForce;
        DevicesVector(1)->ForceCartesian = secondDeviceForce;
    } */
}

void devSensableHDMasterSlave::GetPositions(void)
{
     p1 = DevicesVector(0)->PositionCartesian;
     p2 = DevicesVector(1)->PositionCartesian;
}
void devSensableHDMasterSlave::GetPositions(int DeviceNo)
{
    if(DeviceNo == 0) {
        p1 = DevicesVector(DeviceNo)->PositionCartesian;
    } else {
        p2 = DevicesVector(DeviceNo)->PositionCartesian;
    }
}   

void devSensableHDMasterSlave::SetScaleFactor(double Scale)
{
    ScaleFactor = Scale;
}

void devSensableHDMasterSlave::SetForceLimit(double FLimit)
{
    FMax = FLimit;
}

void devSensableHDMasterSlave::SetOffsetMultiplier(double OffMult)
{
    OffsetMultiplier = OffMult;
}

void devSensableHDMasterSlave::IncrementScaleFactor(void)
{
    ScaleFactor *= 1.25;
    printf("Scale factor: %f\n", ScaleFactor);
}

void devSensableHDMasterSlave::DecrementScaleFactor(void)
{
    ScaleFactor *= 0.8;
    printf("Scale factor: %f\n", ScaleFactor);
}

void devSensableHDMasterSlave::IncrementForceLimit(void)
{
    FMax *= 1.25;
    printf("Force Limit: %f\n", FMax);
}

void devSensableHDMasterSlave::DecrementForceLimit(void)
{
    FMax *= 0.8;
    printf("Force Limit: %f\n", FMax);
}
