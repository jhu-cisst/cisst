/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsHistoryBuffer.h 3034 2011-10-09 01:53:36Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2012-09-04

  (C) Copyright 2012-2014 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#ifndef _mtsHistoryBuffer_h
#define _mtsHistoryBuffer_h

#include "historyBufferBase.h"
#include "filterBase.h"

//#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstMultiTask/mtsStateTable.h>

#include <cisstMultiTask/mtsExport.h>

//class CISST_EXPORT mtsHistoryBuffer: public mtsGenericObject, public SC::HistoryBufferBase 
class CISST_EXPORT mtsHistoryBuffer: public SC::HistoryBufferBase 
{
    //CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    friend class mtsSafetyCoordinator;

protected:
    /*! Filtering type */
    SC::FilterBase::FilteringType Type;

    /*! For active filtering */
    mtsStateTable * StateTable;

    /*! For passive filtering */
    mtsFunctionRead FetchScalarValue;
    mtsFunctionRead FetchVectorValue;

public:
    mtsHistoryBuffer(const SC::FilterBase::FilteringType type, mtsStateTable * stateTable);
    ~mtsHistoryBuffer() {}

    /*! For active filtering: get latest value from history buffer (direct access) */
    void GetNewValueScalar(SC::SignalElement::HistoryBufferIndexType index,
                           SC::SignalElement::ScalarType & value,
                           SC::TimestampType & timestamp);
    void GetNewValueVector(SC::SignalElement::HistoryBufferIndexType index,
                           SC::SignalElement::VectorType & value,
                           SC::TimestampType & timestamp);

    /*! For passive filtering: fetch latest value from history buffer via
        mts data exchange mechanism (indirect access) */
    void GetNewValueScalar(SC::SignalElement::ScalarType & value,
                           SC::TimestampType & timestamp);
    void GetNewValueVector(SC::SignalElement::VectorType & value,
                           SC::TimestampType & timestamp);

    // Support for "deep" fault inject: modify actual values in the buffer
    void PushNewValueScalar(SC::SignalElement::HistoryBufferIndexType index,
                            SC::SignalElement::ScalarType & value);
    void PushNewValueVector(SC::SignalElement::HistoryBufferIndexType index,
                            const SC::SignalElement::VectorType & value);
    
    
    /*
    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
    */
};

//CMN_DECLARE_SERVICES_INSTANTIATION(mtsHistoryBuffer);

#endif // _mtsHistoryBuffer_h
