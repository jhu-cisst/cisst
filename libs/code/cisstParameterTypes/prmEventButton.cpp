/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet, Rajesh Kumar
  Created on:   2008-04-08

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstParameterTypes/prmEventButton.h>

prmEventButton::~prmEventButton()
{
}

void prmEventButton::ToStream(std::ostream & outputStream) const
{
    BaseType::ToStream(outputStream);
    switch (this->Type()) {
        case prmEventButton::PRESSED:
            outputStream << " EventButton PRESSED";
            break;
        case prmEventButton::RELEASED:
            outputStream << " EventButton RELEASED";
            break;
        case prmEventButton::CLICKED:
            outputStream << " EventButton CLICKED";
            break;
        case prmEventButton::DOUBLE_CLICKED:
            outputStream << " EventButton DOUBLE_CLICKED";
            break;
        default:
            outputStream << " EventButton of unknown type, probably not yet set." << std::endl;
    }
}

void prmEventButton::ToStreamRaw(std::ostream & outputStream, const char delimiter,
                                 bool headerOnly, const std::string & headerPrefix) const {
    BaseType::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
    outputStream << delimiter;
    if (headerOnly) {
        outputStream << headerPrefix << "-type";
    } else {
        outputStream << this->Type();
    }
}

void prmEventButton::SerializeRaw(std::ostream & outputStream) const 
{
    BaseType::SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, this->TypeMember);
}

void prmEventButton::DeSerializeRaw(std::istream & inputStream) 
{
    BaseType::DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, this->TypeMember);
}
