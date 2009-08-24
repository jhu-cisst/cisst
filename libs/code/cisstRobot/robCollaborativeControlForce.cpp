/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: robCollaborativeControlForce.h 683 2009-08-14 21:40:14Z gsevinc1 $

  Author(s): Gorkem Sevinc, Anton Deguet
  Created on: 2009-08-20

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstRobot/robCollaborativeControlForce.h>

robCollaborativeControlForce::robCollaborativeControlForce(void)
{
    ClutchOffset.SetAll(0.0);
    LeftClutchOffset.SetAll(0.0);
    RightClutchOffset.SetAll(0.0);
    LeftClutchMSOffset.SetAll(0.0);
    RightClutchMSOffset.SetAll(0.0);
    WorkspaceOffset.SetAll(0.0);
    Error.SetAll(0.0);
    ScaleFactor() = 0.15;
    FMax() = 40.0;
    ForceMode() = 0;
    clutchMode = 0;
    ForceMasterCoefficient() = 1.0;
    firstIteration = true;
    MasterClutch() = false;
    SlaveClutch() = false;
    MasterSlaveClutch() = false;
    clutchDone = false;
    bothClutched = false;
    clutchOffsetAdd = false;
}

void robCollaborativeControlForce::Update(const vct3 & p1, 
                                          const vct3 & p2,
                                          const bool ClutchMaster,
                                          const bool ClutchSlave,
                                          vctFixedSizeVector<double, 6> & ForceMaster,
                                          vctFixedSizeVector<double, 6> & ForceSlave)
{
    ForceMaster.SetAll(0.0);
    ForceSlave.SetAll(0.0);
    //If the first iteration
    if(firstIteration) {
        firstIteration = false;
        // Compute the initial offset between the two devices
        WorkspaceOffset.DifferenceOf(p1, p2);
    }

    // If clutching the first device
    if((ClutchMaster == true || MasterClutch() == true) && ClutchSlave == false) {
        if(clutchOffsetAdd == true) {
            p2Goal.Subtract(RightClutchMSOffset);
            clutchOffsetAdd = false;
        }
        // Compute the current force (F = kp * (secondDeviceGoal - secondDeviceLastPosition) )
        ForceSlave.XYZ().DifferenceOf(p2Goal, p2);
        ForceSlave.XYZ().Multiply(ScaleFactor());
        ForceMaster.SetAll(0.0);

        // Cap the forces, apply ratchet effect or apply the current force
        //  on the slave device depending on the force mode
        ForceFeedNormSlave = ForceSlave.XYZ().Norm();
        if(ForceFeedNormSlave >= (FMax() / 4.0)) {
            if(ForceMode() == 0) {
                p2Goal.Assign(p2);
            } else if (ForceMode() == 1) {
                ForceSlave.Divide(FMax() / ForceFeedNormSlave);
            }
        }

        // Compute the offset of the master device from its original position
        LeftClutchOffset.DifferenceOf(p1, p1Last);
        
        clutchMode = 1;
        clutchDone = true;
    } else if((ClutchSlave == true || SlaveClutch() == true) && ClutchMaster == false) {
        // Compute the current force (F = kp * (firstDeviceGoal - firstDeviceLastPosition) )
        if(clutchOffsetAdd == true) {
            p1Goal.Add(LeftClutchMSOffset);
            clutchOffsetAdd = false;
        }
        ForceMaster.XYZ().DifferenceOf(p1Goal, p1);
        ForceMaster.XYZ().Multiply(ScaleFactor());
        ForceSlave.SetAll(0.0);

        // Cap the forces, apply ratchet effect or apply the current force
        //  on the slave device depending on the force mode
        ForceFeedNormMaster = ForceMaster.XYZ().Norm();
        if(ForceFeedNormMaster >= (FMax() / 4.0)) {
            if(ForceMode() == 0) {
                p1Goal.Assign(p1);
            } else if (ForceMode() == 1) {
                ForceMaster.Divide(FMax() / ForceFeedNormMaster);
            }
        }

        // Compute the offset of the slave device from its original position
        RightClutchOffset.DifferenceOf(p2, p2Last);
        
        clutchMode = 2;   
        clutchDone = true;
    } else if ((ClutchMaster == true && ClutchSlave == true) || MasterSlaveClutch() == true) {
        // Set both devices' forces to 0
        ForceMaster.SetAll(0.0);
        ForceSlave.SetAll(0.0);
        // Compute left and right clutch offsets
        LeftClutchMSOffset.DifferenceOf(p1, p1Last);
        RightClutchMSOffset.DifferenceOf(p2, p2Last);
        
        clutchOffsetAdd = bothClutched = clutchDone = true;
    } else {
        // Save the positions to temporary positions (for clutching purposes)
        p1Last = p1;
        p2Last = p2;

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
                WorkspaceOffset.DifferenceOf(p1, p2);
                bothClutched = false;
            }
            LeftClutchOffset.SetAll(0.0);
            RightClutchOffset.SetAll(0.0);
        }
        
        //p2R = p2 + WorkspaceOffset
        p2R.SumOf(p2, WorkspaceOffset);
        //Error = p2R - p1
        Error.DifferenceOf(p2R, p1);
        Error.Multiply(0.5);
        //p2RGoal = p2R - 1/2(Error)
        p2RGoal.DifferenceOf(p2R, Error);
        //p1Goal = p1 + 1/2(Error)
        p1Goal.SumOf(p1, Error);
        //p2Goal = p2 - 1/2(Error)
        p2Goal.DifferenceOf(p2, Error);

        // ForceMaster = kp * (p1Goal - p1)
        // ForceSlave = kp * (p2RGoal - p2R)
        ForceMaster.XYZ().DifferenceOf(p1Goal, p1);
        ForceSlave.XYZ().DifferenceOf(p2RGoal, p2R);

        // Cap the forces, apply ratchet effect or apply the current force
        // on the master and on the slave device depending on the force mode
        //if(||ForceMaster|| > ForceLimit) { ForceMaster /= (FLimit / ||ForceMaster||)
        ForceFeedNormMaster = ForceMaster.XYZ().Norm();
        if(ForceFeedNormMaster >= FMax()) {
            if(ForceMode() == 0) {
                WorkspaceOffset.DifferenceOf(p1, p2);
            } else if(ForceMode() == 1) {
                ForceMaster.Divide(FMax() / ForceFeedNormMaster);
            }
        }

        ForceFeedNormSlave = ForceSlave.XYZ().Norm();
        if(ForceFeedNormSlave >= FMax()) {
            if(ForceMode() == 0) {
                WorkspaceOffset.DifferenceOf(p1, p2);
            } else if(ForceMode() == 1) {
                ForceSlave.Divide(FMax() / ForceFeedNormSlave);
            }
        }

        // Apply the scale factor (kp) to the forces
        ForceMaster.Multiply(ScaleFactor());
        ForceSlave.Multiply(ScaleFactor());

        //Apply the coefficient
        ForceMaster.Multiply(ForceMasterCoefficient());
    }  
}

void robCollaborativeControlForce::SetParameters(const double & commandedForceLimit, const double & commandedScaleFactor, 
                                                 const double & commandedForceCoefficient, const int & commandedForceMode, 
                                                 const bool & commandedMasterClutchGUI, const bool & commandedSlaveClutchGUI,
                                                 const bool & commandedMasterSlaveClutchGUI)
{
    FMax() = commandedForceLimit;
    ScaleFactor() = commandedScaleFactor;
    ForceMasterCoefficient() = commandedForceCoefficient;
    ForceMode() = commandedForceMode;
    MasterClutch() = commandedMasterClutchGUI;
    SlaveClutch() = commandedSlaveClutchGUI;
    MasterSlaveClutch() = commandedMasterSlaveClutchGUI;
}
