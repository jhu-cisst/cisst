/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsHistoryBuffer.cpp 3303 2012-01-02 16:33:23Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2012-09-04

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#include <cisstMultiTask/mtsHistoryBuffer.h>

//CMN_IMPLEMENT_SERVICES(mtsHistoryBuffer);

// DO NOT USE DEFAULT CONSTRUCTOR (due to requirement of class services)
mtsHistoryBuffer::mtsHistoryBuffer(): StateTable(0)
{
    CMN_ASSERT(false);
}

mtsHistoryBuffer::mtsHistoryBuffer(mtsStateTable * stateTable)
    : StateTable(stateTable)
{
    CMN_ASSERT(StateTable);
}

void mtsHistoryBuffer::GetNewValueScalar(SF::SignalElement::HistoryBufferIndexType index,
                                         SF::SignalElement::ScalarType & value,
                                         SF::SignalElement::TimestampType & timestamp)
{
    value = StateTable->GetNewValueScalar(index, timestamp);
}

void mtsHistoryBuffer::GetNewValueVector(SF::SignalElement::HistoryBufferIndexType index,
                                         SF::SignalElement::VectorType & value,
                                         SF::SignalElement::TimestampType & timestamp)
{
    StateTable->GetNewValueVector(index, value, timestamp);
}

#if 0
void mtsHistoryBuffer::ToStream(std::ostream & outputStream) const
{
    mtsGenericObject::ToStream(outputStream);
    // MJ: Nothing to print out for now
    //outputStream << " Process: \""        << this->Process << "\","
}

void mtsHistoryBuffer::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);
    // MJ: Nothing to serialize for now
    //cmnSerializeRaw(outputStream, Process);
}

void mtsHistoryBuffer::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    // MJ: Nothing to deserialize for now
    //cmnDeSerializeRaw(inputStream, Process);
}
#endif
