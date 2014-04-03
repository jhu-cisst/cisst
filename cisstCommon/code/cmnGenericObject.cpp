/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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
#include <cisstCommon/cmnLogger.h>

#include <sstream>

bool cmnGenericObject::ReconstructFrom(const cmnGenericObject & other) {
    const cmnClassServicesBase * services = this->Services();
#if 0
    // Following are now done in services->Create(this, other)

    // test that both objects are of the same type
    if (services != other.Services()) {
        return false;
    }
    // call destructor on the existing object
    services->Delete(this);
#endif
    // call copy constructor
    services->Create(this, other);
    return true;
}


std::string cmnGenericObject::ToString(void) const {
    std::stringstream outputStream;
    ToStream(outputStream);
    return outputStream.str();
}


void cmnGenericObject::ToStream(std::ostream & outputStream) const {
    outputStream << "Default output for class: " << Services()->GetName();
}


void cmnGenericObject::ToStreamRaw(std::ostream & outputStream, const char CMN_UNUSED(delimiter),
                                   bool CMN_UNUSED(headerOnly), const std::string & CMN_UNUSED(headerPrefix)) const {
    outputStream << Services()->GetName();
}


bool cmnGenericObject::FromStreamRaw(std::istream & CMN_UNUSED(inputStream),
                                     const char CMN_UNUSED(delimiter))
{
    return false;
}


void cmnGenericObject::SerializeRaw(std::ostream & CMN_UNUSED(outputStream)) const {
    CMN_LOG_CLASS_RUN_ERROR << "No serialization implemented for: " << Services()->GetName();
}


void cmnGenericObject::DeSerializeRaw(std::istream & CMN_UNUSED(inputStream)) {
    CMN_LOG_CLASS_RUN_ERROR << "No de-serialization implemented for: " << Services()->GetName();
}


cmnLogger::StreamBufType * cmnGenericObject::GetLogMultiplexer(void) const
{
    return cmnLogger::GetMultiplexer();
}
