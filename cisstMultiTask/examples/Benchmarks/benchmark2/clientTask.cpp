/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2009-09-09

  (C) Copyright 2009-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include "clientTask.h"
#include "configuration.h"

#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsManagerLocal.h>

CMN_IMPLEMENT_SERVICES(clientTask);

clientTask::clientTask(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 5000),
    NumberOfSamplesSkipped(0),
	NumberOfSamplesCollected(0),
    BenchmarkCompleted(false)
{
    // to communicate with the interface of the resource
    mtsInterfaceRequired * requiredInterface = AddInterfaceRequired("Required");
    if (requiredInterface) {
        requiredInterface->AddFunction("QualifiedRead", QualifiedReadFunction);
    }

	// Get a pointer to the time server
    TimeServer = &mtsManagerLocal::GetInstance()->GetTimeServer();

	// Reserve memory space for results
    Results.SetSize(confNumberOfSamples);
}


void clientTask::Configure(const std::string & CMN_UNUSED(filename))
{}


void clientTask::Startup(void)
{}

void clientTask::Run(void)
{
    if (BenchmarkCompleted) return;

    // Skip some samples at start-up.
    if (NumberOfSamplesSkipped++ <= confNumberOfSamplesToSkip) return;

	// Placeholders to send and receive time information.
	mtsDouble currentTic, previousTic;

	// Keep the current relative time.
	currentTic.Data = TimeServer->GetRelativeTime();

	// Execute the qualified read command across a network.
	QualifiedReadFunction(currentTic, previousTic);

    // Obtain the elapsed time and keep it.
	const double elapsedTime = TimeServer->GetRelativeTime() - previousTic;
    Results.Element(NumberOfSamplesCollected++) = elapsedTime;

	if (NumberOfSamplesCollected == confNumberOfSamples) {        
        ShowResults();
        BenchmarkCompleted = true;
    } else {
        if (NumberOfSamplesCollected % (confNumberOfSamples / 20) == 0) {
            std::cout << ".";
        }
    }
}

bool clientTask::IsBenchmarkCompleted(void) const
{
    return BenchmarkCompleted;
}

void clientTask::ShowResults()
{
    double average = Results.SumOfElements() / Results.size();
    double min = 0.0;
    double max = 0.0;
    Results.MinAndMaxElement(min, max);

    std::cout << std::endl << std::endl
              << "--------------------------------------------------------------------" << std::endl
              << "Size of elements used (in bytes) : " << sizeof(mtsDouble) << std::endl
              << "Number of samples: " << NumberOfSamplesCollected << std::endl
              << "avg (ms) : " << cmnInternalTo_ms(average) << std::endl
              << "std (ms) : " << cmnInternalTo_ms(StandardDeviation(Results)) << std::endl
              << "min (ms) : " << cmnInternalTo_ms(min) << std::endl
              << "max (ms) : " << cmnInternalTo_ms(max) << std::endl;
}
