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

mtsHistoryBuffer::mtsHistoryBuffer(mtsStateTable * stateTable)
    : FilteringType(ACTIVE), StateTable(stateTable)
{
    CMN_ASSERT(StateTable);
}

void mtsHistoryBuffer::GetNewValueScalar(SF::SignalElement::HistoryBufferIndexType index,
                                         SF::SignalElement::ScalarType & value,
                                         SF::SignalElement::TimestampType & timestamp)
{
    if (FilteringType == ACTIVE)
        value = StateTable->GetNewValueScalar(index, timestamp);
    else {
        value = 0.0;
        timestamp = 0.0;
    }
}

void mtsHistoryBuffer::GetNewValueVector(SF::SignalElement::HistoryBufferIndexType index,
                                         SF::SignalElement::VectorType & value,
                                         SF::SignalElement::TimestampType & timestamp)
{
    if (FilteringType == ACTIVE)
        StateTable->GetNewValueVector(index, value, timestamp);
    else {
        value.clear();
        timestamp = 0.0;
    }
}

void mtsHistoryBuffer::GetNewValueScalar(SF::SignalElement::ScalarType & value,
                                         SF::SignalElement::TimestampType & timestamp)
{
    if (FilteringType == PASSIVE) {
        FetchScalarValue(value);
        // MJ??? : What about timestamp???
    }
    else {
        value = 0.0;
        timestamp = 0.0;
    }
}

void mtsHistoryBuffer::GetNewValueVector(SF::SignalElement::VectorType & value,
                                         SF::SignalElement::TimestampType & timestamp)
{
    if (FilteringType == PASSIVE) {
        FetchVectorValue(value);
        // MJ FIXME: What about timestamp???
    } else {
        value.clear();
        timestamp = 0.0;
    }
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
