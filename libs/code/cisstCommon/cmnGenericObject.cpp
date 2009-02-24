/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2005-08-11

  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassServices.h>


std::string cmnGenericObject::ToString(void) const {
    std::stringstream outputStream;
    ToStream(outputStream);
    return outputStream.str();
}

  
void cmnGenericObject::ToStream(std::ostream & outputStream) const {
    outputStream << "Default output for class: " << Services()->GetName();
}


void cmnGenericObject::SerializeRaw(std::ostream & CMN_UNUSED(outputStream)) const {
    CMN_LOG_CLASS(5) << "No serialization implemented for: " << Services()->GetName();
}


void cmnGenericObject::DeSerializeRaw(std::istream & CMN_UNUSED(inputStream)) {
    CMN_LOG_CLASS(5) << "No de-serialization implemented for: " << Services()->GetName();
}

