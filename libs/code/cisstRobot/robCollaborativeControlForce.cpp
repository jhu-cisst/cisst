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

void robCollaborativeControlForce::ParameterType::ToStream(std::ostream & outputStream) const
{
    outputStream << "\nForce Mode: " << this->ForceModeMember
                 << "\nFeedback Ratio: " << this->ForceFeedbackRatioMember
                 << "\nLinear Gain: " << this->LinearGainMember
                 << "\nForce Limit Ratio: " << this->ForceLimitMember;
}   

void robCollaborativeControlForce::ParameterType::ToStreamRaw(std::ostream & outputStream, const char delimiter,
                                                              bool headerOnly, const std::string & headerPrefix) const {
    if(headerOnly) {
        outputStream << headerPrefix << "Force Mode" << delimiter 
                     << headerPrefix << "Feedback Ratio" << delimiter 
                     << headerPrefix << "Linear Gain" << delimiter
                     << headerPrefix << "Force Limit Ratio";
    } else {
        outputStream  << this->ForceModeMember << delimiter
                      << this->ForceFeedbackRatioMember << delimiter
                      << this->LinearGainMember << delimiter
                      << this->ForceLimitMember;
    }
                                                                  
}

void robCollaborativeControlForce::ParameterType::SerializeRaw(std::ostream & outputStream) const 
{
    cmnSerializeRaw(outputStream, this->ForceModeMember);
    cmnSerializeRaw(outputStream, this->ForceFeedbackRatioMember);
    cmnSerializeRaw(outputStream, this->LinearGainMember);
    cmnSerializeRaw(outputStream, this->ForceLimitMember);
}

void robCollaborativeControlForce::ParameterType::DeSerializeRaw(std::istream & inputStream) 
{
    cmnDeSerializeRaw(inputStream, this->ForceModeMember);
    cmnDeSerializeRaw(inputStream, this->ForceFeedbackRatioMember);
    cmnDeSerializeRaw(inputStream, this->LinearGainMember);
    cmnDeSerializeRaw(inputStream, this->ForceLimitMember);
}


robCollaborativeControlForce::robCollaborativeControlForce(void)
{
    ClutchOffset.SetAll(0.0);
    LeftClutchOffset.SetAll(0.0);
    RightClutchOffset.SetAll(0.0);
    LeftClutchMSOffset.SetAll(0.0);
    RightClutchMSOffset.SetAll(0.0);
    WorkspaceOffset.SetAll(0.0);
    Error.SetAll(0.0);
    Parameter().LinearGain() = 0.15;
    Parameter().ForceLimit() = 40.0;
    Parameter().ForceMode() = ParameterType::RATCHETED;
    ClutchMode = 0;
    Parameter().ForceFeedbackRatio() = 1.0;
    FirstIteration = true;
    ApplicationMasterClutch() = false;
    ApplicationSlaveClutch() = false;
    ApplicationMasterSlaveClutch() = false;
    ClutchDone = false;
    BothClutched = false;
    ClutchOffsetAdd = false;
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
    if(FirstIteration) {
        FirstIteration = false;
        // Compute the initial offset between the two devices
        WorkspaceOffset.DifferenceOf(p1, p2);
    }

    // Validate the Force Feedback ratio such that it is never a value greater than 2.0
    if(Parameter().ForceFeedbackRatio() > 2.0) {
        Parameter().SetForceFeedbackRatio(2.0);
    }

    // If clutching the first device
    if((ClutchMaster == true || ApplicationMasterClutch() == true) && ClutchSlave == false) {
        if(ClutchOffsetAdd == true) {
            p2Goal.Subtract(RightClutchMSOffset);
            ClutchOffsetAdd = false;
        }
        // Compute the current force (F = kp * (secondDeviceGoal - secondDeviceLastPosition) )
        ForceSlave.XYZ().DifferenceOf(p2Goal, p2);
        ForceSlave.XYZ().Multiply(Parameter().LinearGain());
        ForceMaster.SetAll(0.0);

        // Cap the forces, apply ratchet effect or apply the current force
        //  on the slave device depending on the force mode
        ForceFeedNormSlave = ForceSlave.XYZ().Norm();
        if(ForceFeedNormSlave >= (Parameter().ForceLimit() / 4.0)) {
            switch(Parameter().ForceMode()) {
                case ParameterType::RATCHETED:
                    p2Goal.Assign(p2);
                    break;
                case ParameterType::CAPPED:
                    ForceSlave.Divide(Parameter().ForceLimit() / ForceFeedNormSlave);
                    break;
                default:
                    break;
            }
        }

        // Compute the offset of the master device from its original position
        LeftClutchOffset.DifferenceOf(p1, p1Last);
        
        ClutchMode = 1;
        ClutchDone = true;
    } else if((ClutchSlave == true || ApplicationSlaveClutch() == true) && ClutchMaster == false) {
        // Compute the current force (F = kp * (firstDeviceGoal - firstDeviceLastPosition) )
        if(ClutchOffsetAdd == true) {
            p1Goal.Add(LeftClutchMSOffset);
            ClutchOffsetAdd = false;
        }
        ForceMaster.XYZ().DifferenceOf(p1Goal, p1);
        ForceMaster.XYZ().Multiply(Parameter().LinearGain());
        ForceSlave.SetAll(0.0);

        // Cap the forces, apply ratchet effect or apply the current force
        //  on the slave device depending on the force mode
        ForceFeedNormMaster = ForceMaster.XYZ().Norm();
        if(ForceFeedNormMaster >= (Parameter().ForceLimit() / 4.0)) {
            switch(Parameter().ForceMode()) {
                case ParameterType::RATCHETED:
                    p1Goal.Assign(p1);
                    break;
                case ParameterType::CAPPED:
                    ForceMaster.Divide(Parameter().ForceLimit() / ForceFeedNormMaster);
                    break;
                default:
                    break;
            }
        }

        // Compute the offset of the slave device from its original position
        RightClutchOffset.DifferenceOf(p2, p2Last);
        
        ClutchMode = 2;   
        ClutchDone = true;
    } else if ((ClutchMaster == true && ClutchSlave == true) || ApplicationMasterSlaveClutch() == true) {
        // Set both devices' forces to 0
        ForceMaster.SetAll(0.0);
        ForceSlave.SetAll(0.0);
        // Compute left and right clutch offsets
        LeftClutchMSOffset.DifferenceOf(p1, p1Last);
        RightClutchMSOffset.DifferenceOf(p2, p2Last);
        
        ClutchOffsetAdd = BothClutched = ClutchDone = true;
    } else {
        // Save the positions to temporary positions (for clutching purposes)
        p1Last = p1;
        p2Last = p2;

        // If clutching is done
        if(ClutchMode != 0 && ClutchDone == true) {
            ClutchDone = false;
            // Add the clutch offset to the current offset
            if(ClutchMode == 1) {
                WorkspaceOffset.Add(LeftClutchOffset);
            } 
            if(ClutchMode == 2) {
                WorkspaceOffset.Subtract(RightClutchOffset);
            }
            if(BothClutched == true) {
                WorkspaceOffset.DifferenceOf(p1, p2);
                BothClutched = false;
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
        if(ForceFeedNormMaster >= Parameter().ForceLimit()) {
            switch(Parameter().ForceMode()) {
                case ParameterType::RATCHETED:
                    WorkspaceOffset.DifferenceOf(p1, p2);
                    break;
                case ParameterType::CAPPED:
                    ForceMaster.Divide(Parameter().ForceLimit() / ForceFeedNormMaster);
                    break;
                default:
                    break;
            }
        }

        ForceFeedNormSlave = ForceSlave.XYZ().Norm();
        if(ForceFeedNormSlave >= Parameter().ForceLimit()) {
            switch(Parameter().ForceMode()) {
                case ParameterType::RATCHETED:
                    WorkspaceOffset.DifferenceOf(p1, p2);
                    break;
                case ParameterType::CAPPED:
                    ForceSlave.Divide(Parameter().ForceLimit() / ForceFeedNormSlave);
                    break;
                default:
                    break;
            }
        }

        // Apply the scale factor (kp) to the forces
        ForceMaster.Multiply(Parameter().LinearGain());
        ForceSlave.Multiply(Parameter().LinearGain());
        
        // Apply the Force Feedback ratio on the master arm
        ForceMaster.Multiply(Parameter().ForceFeedbackRatio());
    }  
}
