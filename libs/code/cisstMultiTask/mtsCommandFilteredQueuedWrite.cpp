/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id:$

  Author(s):  Peter Kazanzides

  (C) Copyright 2005-2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstMultiTask/mtsCommandFilteredQueuedWrite.h>
#include <cisstMultiTask/mtsCommandQualifiedReadBase.h>

mtsCommandFilteredQueuedWrite::mtsCommandFilteredQueuedWrite(mtsCommandQualifiedReadBase * actualFilter,
                                                             mtsCommandWriteBase * actualCommand):
    BaseType(0, actualCommand, 0), ActualFilter(actualFilter)
{
    // PK: is there a better way to do this?
    if (actualFilter) {
        FilterOutput = dynamic_cast<mtsGenericObject *>(actualFilter->GetArgument2Prototype()->Services()->Create());
    }
}


mtsCommandFilteredQueuedWrite::mtsCommandFilteredQueuedWrite(mtsMailBox * mailBox,
                                                             mtsCommandQualifiedReadBase * actualFilter,
                                                             mtsCommandWriteBase * actualCommand, size_t size):
    BaseType(mailBox, actualCommand, size),
    ActualFilter(actualFilter)
{
    // PK: is there a better way to do this?
    if (actualFilter) {
        FilterOutput = dynamic_cast<mtsGenericObject *>(actualFilter->GetArgument2Prototype()->Services()->Create());
    }
}


// ArgumentsQueue destructor should get called
mtsCommandFilteredQueuedWrite::~mtsCommandFilteredQueuedWrite()
{
    if (FilterOutput) {
        delete FilterOutput;
    }
}


mtsCommandFilteredQueuedWrite * mtsCommandFilteredQueuedWrite::Clone(mtsMailBox * mailBox,
                                                                     size_t size) const
{
    return new mtsCommandFilteredQueuedWrite(mailBox, this->ActualFilter, this->ActualCommand, size);
}


const mtsGenericObject * mtsCommandFilteredQueuedWrite::GetArgumentPrototype(void) const
{
    return this->ActualFilter->GetArgument1Prototype();
}


mtsExecutionResult mtsCommandFilteredQueuedWrite::Execute(const mtsGenericObject & argument, mtsBlockingType blocking)
{
    if (this->IsEnabled()) {
#if 1
        // First, call the filter (qualified read)
        mtsExecutionResult result = ActualFilter->Execute(argument, *FilterOutput);
        if (result != mtsExecutionResult::DEV_OK) {
            return result;
        }
        // Next, queue the write command
        return BaseType::Execute(*FilterOutput, blocking);
#else   // PK Proposal
        return classInstance->PreMethod(argument, *filterOutput, ActualFilter, blocking);
#endif
    }
    return mtsExecutionResult::DISABLED;
}
