/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):	Anton Deguet
  Created on:	2009-04-13

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstMultiTask/mtsGenericObject.h>

#include <iostream>
#include <iomanip>

bool mtsGenericObject::SetTimestampIfAutomatic(double timestamp) {
    if (this->AutomaticTimestampMember) {
        this->TimestampMember = timestamp;
        return true;
    }
    return false;
}


void mtsGenericObject::ToStream(std::ostream & outputStream) const {
    outputStream << "Timestamp (";
    if (this->AutomaticTimestamp()) {
        outputStream << "auto";
    } else {
        outputStream << "manual";
    }
    outputStream << "): ";

    std::ios_base::fmtflags flags = outputStream.flags();       // Save old flags
    outputStream.setf(std::ios::fixed | std::ios::showpoint);
    outputStream.precision(5);

    outputStream  << this->Timestamp();
    outputStream.flags(flags);                             // Restore old flags

    if (this->Valid()) {
        outputStream << " (valid)";
    } else {
        outputStream << " (invalid)";
    }
}


void mtsGenericObject::ToStreamRaw(std::ostream & outputStream, const char delimiter,
                                   bool headerOnly, const std::string & headerPrefix) const {
    if (headerOnly) {
        outputStream << headerPrefix << "-timestamp" << delimiter
                     << headerPrefix << "-automatic-timestamp" << delimiter
                     << headerPrefix << "-valid";
    } else {
        outputStream << this->Timestamp() << delimiter
                     << this->AutomaticTimestamp() << delimiter
                     << this->Valid();
    }
}


void mtsGenericObject::SerializeRaw(std::ostream & outputStream) const {
    cmnSerializeRaw(outputStream, this->Timestamp());
    cmnSerializeRaw(outputStream, this->AutomaticTimestamp());
    cmnSerializeRaw(outputStream, this->Valid());
}


void mtsGenericObject::DeSerializeRaw(std::istream & inputStream) {
    cmnDeSerializeRaw(inputStream, this->Timestamp());
    cmnDeSerializeRaw(inputStream, this->AutomaticTimestamp());
    cmnDeSerializeRaw(inputStream, this->Valid());
}
