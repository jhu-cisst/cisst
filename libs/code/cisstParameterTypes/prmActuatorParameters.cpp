/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
Author(s):	Marcin Balicki
Created on:   2008-08-08

(C) Copyright 2008 Johns Hopkins University (JHU), All Rights
Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/
#include <cisstParameterTypes/prmActuatorParameters.h>

void prmActuatorParameters::SetSize(size_type size){

    ForwardLimitMember.SetSize(size);
    ReverseLimitMember.SetSize(size);
    MaxVelocityMember.SetSize(size);
    MaxAccelerationMember.SetSize(size);
    MaxDecelerationMember.SetSize(size);
    LimitSwitchDeccelarationMember.SetSize(size);
    Counts_per_mmMember.SetSize(size);
    KpMember.SetSize(size);
    KdMember.SetSize(size);
    KiMember.SetSize(size);
    ILMember.SetSize(size);
    FVMember.SetSize(size);
    FAMember.SetSize(size);
    TorqueLimitMember.SetSize(size);
    TorqueLimitPeakMember.SetSize(size);
    TorqueOffsetMember.SetSize(size);
}
