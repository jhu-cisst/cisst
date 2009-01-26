/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Rajesh Kumar, Anton Deguet
  Created on:   2008-04-10

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstParameterTypes/prmVelocityCartesianGet.h>


CMN_IMPLEMENT_SERVICES(prmVelocityCartesianGet);

prmVelocityCartesianGet::~prmVelocityCartesianGet()
{
}


void prmVelocityCartesianGet::ToStream(std::ostream & outputStream) const
{
    outputStream << "Reference frame: " << this->ReferenceFrameMember
                 << "\nMoving frame: " << this->MovingFrameMember
                 << "\nLinear velocity: " << this->VelocityLinearMember
                 << "\nAngular velocity: " << this->VelocityAngularMember
                 << "\nState Index: " << this->StateIndexMember;
}

