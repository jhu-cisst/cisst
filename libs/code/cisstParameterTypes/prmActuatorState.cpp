/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
Author(s):	Marcin Balicki
Created on:   2008-09-14

(C) Copyright 2008 Johns Hopkins University (JHU), All Rights
Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstParameterTypes/prmActuatorState.h>


prmActuatorState::prmActuatorState(size_type size)
{
    SetSize(size);
}

void prmActuatorState::ToStream(std::ostream & outputStream) const
{
    outputStream << "Position: "    << this->PositionMember
                 << "\nVelocity : "		    << this->VelocityMember
                 << "\nInMotion: "		    << this->InMotionMember
                 << "\nMotorOffMember: "     << this->MotorOffMember
                 << "\nSoftFwdLimitHit: "	<< this->SoftFwdLimitHitMember
                 << "\nSoftRevLimitHit: "	<< this->HardRevLimitHitMember
                 << "\nHardFwdLimitHit: "	<< this->HardFwdLimitHitMember
                 << "\nHardRevLimitHit: "	<< this->SoftRevLimitHitMember
                 << "\nHomeSwitchOn: "	    << this->HomeSwitchOnMember
                 << "\nEStopON: "			<< this->EStopONMember
                 <<"\nIsHomed: "             <<this->IsHomedMember;
}

void prmActuatorState::SetSize(size_type size){

    PositionMember.SetSize(size);
    VelocityMember.SetSize(size);
    InMotionMember.SetSize(size);
    MotorOffMember.SetSize(size);
    SoftFwdLimitHitMember.SetSize(size);
    SoftRevLimitHitMember.SetSize(size);
    HardFwdLimitHitMember.SetSize(size);
    HardRevLimitHitMember.SetSize(size);
    HomeSwitchOnMember.SetSize(size);
    IsHomedMember.SetSize(size);
}

void prmActuatorState::SerializeRaw(std::ostream & outputStream) const
{
    BaseType::SerializeRaw(outputStream);
    PositionMember.SerializeRaw(outputStream);
    VelocityMember.SerializeRaw(outputStream);
    InMotionMember.SerializeRaw(outputStream);
    MotorOffMember.SerializeRaw(outputStream);
    SoftFwdLimitHitMember.SerializeRaw(outputStream);
    SoftRevLimitHitMember.SerializeRaw(outputStream);
    HardFwdLimitHitMember.SerializeRaw(outputStream);
    HardRevLimitHitMember.SerializeRaw(outputStream);
    HomeSwitchOnMember.SerializeRaw(outputStream);
    IsHomedMember.SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream,EStopONMember);
}

void prmActuatorState::DeSerializeRaw(std::istream & inputStream)
{
    BaseType::DeSerializeRaw(inputStream);
    PositionMember.DeSerializeRaw(inputStream);
    VelocityMember.DeSerializeRaw(inputStream);
    InMotionMember.DeSerializeRaw(inputStream);
    MotorOffMember.DeSerializeRaw(inputStream);
    SoftFwdLimitHitMember.DeSerializeRaw(inputStream);
    SoftRevLimitHitMember.DeSerializeRaw(inputStream);
    HardFwdLimitHitMember.DeSerializeRaw(inputStream);
    HardRevLimitHitMember.DeSerializeRaw(inputStream);
    HomeSwitchOnMember.DeSerializeRaw(inputStream);
    IsHomedMember.DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream,EStopONMember);
}


