/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Rajesh Kumar, Anton Deguet
  Created on:   2008-03-12

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstParameterTypes/prmMotionBase.h>


prmMotionBase::~prmMotionBase()
{
}


void prmMotionBase::SerializeRaw(std::ostream & outputStream) const
{
    BaseType::SerializeRaw(outputStream);
    //! \todo Fix this once serializeRaw is available for BlockingFlagMember
    //this->BlockingFlagMember.SerializeRaw(outputStream);
    //cmnSerializeRaw(outputStream, prmBlocking);
    cmnSerializeRaw(outputStream, BlendingFactorMember);
    cmnSerializeRaw(outputStream, TimeLimitMember);
    cmnSerializeRaw(outputStream, IsPreemptableMember);
    cmnSerializeRaw(outputStream, IsCoordinatedMember);
    cmnSerializeRaw(outputStream, IsGoalOnlyMember);

}

void prmMotionBase::DeSerializeRaw(std::istream & inputStream)
{
    BaseType::DeSerializeRaw(inputStream);
    //! \todo Fix this once serializeRaw is available for BlockingFlagMember
    //this->BlockingFlagMember.DeSerializeRaw(inputStream);
    //cmnDeSerializeRaw(inputStream, prmBlocking);
    cmnDeSerializeRaw(inputStream, BlendingFactorMember);
    cmnDeSerializeRaw(inputStream, TimeLimitMember);
    cmnDeSerializeRaw(inputStream, IsPreemptableMember);
    cmnDeSerializeRaw(inputStream, IsCoordinatedMember);
    cmnDeSerializeRaw(inputStream, IsGoalOnlyMember);
}
