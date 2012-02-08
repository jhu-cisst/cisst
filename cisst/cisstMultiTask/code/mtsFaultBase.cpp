/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsFaultBase.cpp 3303 2012-01-02 16:33:23Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2012-02-07

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#include <cisstMultiTask/mtsFaultBase.h>

CMN_IMPLEMENT_SERVICES(mtsFaultBase)

mtsFaultBase::mtsFaultBase(void) : mtsGenericObject(),
    Layer(mtsFaultBase::INVALID), Timestamp(0.0),
    Type(mtsFaultBase::FAULT_INVALID), Magnitude(0.0)
{
}

mtsFaultBase::~mtsFaultBase()
{
}

void mtsFaultBase::ToStream(std::ostream & outputStream) const
{
    mtsGenericObject::ToStream(outputStream);
    /* FIXME
    outputStream << ", Message: \"" << std::string(this->Message, this->Length)
                 << "\", Length: " << Length
                 << ", Process: \"" << ProcessName << "\"";
                 */
}

void mtsFaultBase::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);
    /* FIXME
    cmnSerializeRaw(outputStream, this->Length);
    cmnSerializeRaw(outputStream, this->Message);
    cmnSerializeRaw(outputStream, this->ProcessName);
    */
}

void mtsFaultBase::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    /* FIXME
    cmnDeSerializeRaw(inputStream, this->Length);
    cmnDeSerializeRaw(inputStream, this->Message);
    cmnDeSerializeRaw(inputStream, this->ProcessName);
    */
}

