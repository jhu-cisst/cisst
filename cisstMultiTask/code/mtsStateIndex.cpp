/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Peter Kazanzides
  Created on: 2004-05-04

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstMultiTask/mtsStateIndex.h>


void mtsStateIndex::ToStream(std::ostream & outputStream) const
{
    outputStream << "Index = " << TimeIndex << ", Ticks = " << Ticks()
                 << ", Length = " << BufferLength;
}


void mtsStateIndex::ToStreamRaw(std::ostream & outputStream, const char delimiter,
                                bool headerOnly, const std::string & headerPrefix) const
{
    mtsGenericObject::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
    outputStream << delimiter;
    if (headerOnly) {
        outputStream << headerPrefix << "-timeindex" << delimiter
                     << headerPrefix << "-timeticks" << delimiter
                     << headerPrefix << "-bufferlength";
    } else {
        outputStream << this->TimeIndex << delimiter
                     << this->TimeTicks << delimiter
                     << this->BufferLength;
    }
}


void mtsStateIndex::SerializeRaw(std::ostream & outputStream) const
{
    cmnSerializeRaw(outputStream, this->TimeIndex);
    cmnSerializeRaw(outputStream, this->TimeTicks);
    cmnSerializeRaw(outputStream, this->BufferLength);
}


void mtsStateIndex::DeSerializeRaw(std::istream & inputStream)
{
    cmnDeSerializeRaw(inputStream, this->TimeIndex);
    cmnDeSerializeRaw(inputStream, this->TimeTicks);
    cmnDeSerializeRaw(inputStream, this->BufferLength);
}
