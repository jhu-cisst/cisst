/*

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
                 << "\nLinear Gain Master: " << this->LinearGainMasterMember
                 << "\nLinear Gain Slave: " << this->LinearGainSlaveMember
                 << "\nForce Limit Ratio: " << this->PositionLimitMember
                 << "\nMaster to Slave Scale: " << this->MasterToSlaveScaleMember;
}   

void robCollaborativeControlForce::ParameterType::ToStreamRaw(std::ostream & outputStream, const char delimiter,
                                                              bool headerOnly, const std::string & headerPrefix) const {
    if(headerOnly) {
        outputStream << headerPrefix << "Force Mode" << delimiter 
                     << headerPrefix << "Feedback Ratio" << delimiter 
                     << headerPrefix << "Linear Gain" << delimiter
                     << headerPrefix << "Position Limit Ratio";
    } else {
        outputStream  << this->ForceModeMember << delimiter
                      << this->ForceFeedbackRatioMember << delimiter
                      << this->LinearGainMasterMember << delimiter
                      << this->LinearGainSlaveMember << delimiter
                      << this->PositionLimitMember << delimiter
                      << this->MasterToSlaveScaleMember;
    }
                                                                  
}

void robCollaborativeControlForce::ParameterType::SerializeRaw(std::ostream & outputStream) const 
{
    cmnSerializeRaw(outputStream, this->ForceModeMember);
    cmnSerializeRaw(outputStream, this->ForceFeedbackRatioMember);
    cmnSerializeRaw(outputStream, this->LinearGainMasterMember);
    cmnSerializeRaw(outputStream, this->LinearGainSlaveMember);
    cmnSerializeRaw(outputStream, this->PositionLimitMember);
    cmnSerializeRaw(outputStream, this->MasterToSlaveScaleMember);
}

void robCollaborativeControlForce::ParameterType::DeSerializeRaw(std::istream & inputStream) 
{
    cmnDeSerializeRaw(inputStream, this->ForceModeMember);
    cmnDeSerializeRaw(inputStream, this->ForceFeedbackRatioMember);
    cmnDeSerializeRaw(inputStream, this->LinearGainMasterMember);
    cmnDeSerializeRaw(inputStream, this->LinearGainSlaveMember);
    cmnDeSerializeRaw(inputStream, this->PositionLimitMember);
    cmnDeSerializeRaw(inputStream, this->MasterToSlaveScaleMember);
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
    Parameter().LinearGainMaster() = 0.0;
    Parameter().LinearGainSlave() = 0.0;
    Parameter().PositionLimit() = 0.0;
    Parameter().ForceMode() = ParameterType::RAW;
    Parameter().MasterToSlaveScale() = 1.0;
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

robCollaborativeControlForce::robCollaborativeControlForce(double linearGainMaster,
                                                           double linearGainSlave, 
                                                           double forceLimit,
                                                           ParameterType::ForceModeType forceMode,
                                                           double masterToSlaveScale,
                                                           double forceFeedbackRatio)
{
    ClutchOffset.SetAll(0.0);
    LeftClutchOffset.SetAll(0.0);
    RightClutchOffset.SetAll(0.0);
    LeftClutchMSOffset.SetAll(0.0);
    RightClutchMSOffset.SetAll(0.0);
    WorkspaceOffset.SetAll(0.0);
    Error.SetAll(0.0);
    Parameter().LinearGainMaster() = linearGainMaster;
    Parameter().LinearGainSlave() = linearGainSlave;
    Parameter().PositionLimit() = forceLimit;
    Parameter().ForceMode() = forceMode;
    Parameter().MasterToSlaveScale() = 1.0;
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
        ForceSlave.XYZ().Multiply(Parameter().LinearGainSlave());
        ForceMaster.SetAll(0.0);

        // Cap the forces, apply ratchet effect or apply the current force
        //  on the slave device depending on the force mode
        ForceFeedNormSlave = ForceSlave.XYZ().Norm();
        if(ForceFeedNormSlave >= (Parameter().PositionLimit() / 2.0)) {
            switch(Parameter().ForceMode()) {
                case ParameterType::RATCHETED:
                    p2Goal.Assign(p2);
                    break;
                case ParameterType::CAPPED:
                    ForceSlave.Divide(Parameter().PositionLimit() / ForceFeedNormSlave);
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
        ForceMaster.XYZ().Multiply(Parameter().LinearGainMaster());
        ForceSlave.SetAll(0.0);

        // Cap the forces, apply ratchet effect or apply the current force
        //  on the slave device depending on the force mode
        ForceFeedNormMaster = ForceMaster.XYZ().Norm();
        if(ForceFeedNormMaster >= (Parameter().PositionLimit() / 2.0)) {
            switch(Parameter().ForceMode()) {
                case ParameterType::RATCHETED:
                    p1Goal.Assign(p1);
                    break;
                case ParameterType::CAPPED:
                    ForceMaster.Divide(Parameter().PositionLimit() / ForceFeedNormMaster);
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

        double ErrorNorm = Error.Norm();
        if(ErrorNorm > Parameter().PositionLimit()) {
            switch(Parameter().ForceMode()) {
                case ParameterType::RATCHETED:
                    WorkspaceOffset.DifferenceOf(p1, p2);
                    break;
                case ParameterType::CAPPED:
                    Error.Multiply(Parameter().PositionLimit()/ErrorNorm);
                    break;
                default:
                    break;
            }
        }

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

        /*
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
        }*/

        // Apply the scale factor (kp) to the forces
        ForceMaster.Multiply(Parameter().LinearGainMaster());
        //ForceMaster.Multiply(2.0);
        ForceSlave.Multiply(Parameter().LinearGainSlave());
        
        // Apply the Force Feedback ratio on the master arm
        ForceMaster.Multiply(Parameter().ForceFeedbackRatio());
    }  
}
