/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2009-09-09

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon/cmnConstants.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

#include "serverTask.h"
#include "configuration.h"

// required to implement the class services, see cisstCommon
CMN_IMPLEMENT_SERVICES(serverTask);

unsigned int NumberOfSamplesCollected;

serverTask::serverTask(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 5000)
{
    NumberOfSamplesCollected = 0;

    mtsInterfaceProvided * providedInterface = AddInterfaceProvided("Provided");
    if (providedInterface) {
        providedInterface->AddCommandQualifiedRead(&serverTask::QualifiedRead, this, "QualifiedRead");
    }
}

void serverTask::QualifiedRead(const mtsDouble & tic, mtsDouble & toc) const
{
    toc = tic;

    ++NumberOfSamplesCollected;
}

void serverTask::Startup(void)
{}

void serverTask::Run(void)
{}

bool serverTask::IsBenchmarkCompleted(void) const
{
    return (NumberOfSamplesCollected >= confNumberOfSamples);
}
