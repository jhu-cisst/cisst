/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Min Yang Jung
  Created on: 2010-03-05

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include "SignalGeneratorTask.h"

#include <cisstCommon/cmnConstants.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

// required to implement the class services, see cisstCommon
CMN_IMPLEMENT_SERVICES(SignalGenerator);

SignalGenerator::SignalGenerator(const std::string & taskName, double period) : mtsTaskPeriodic(taskName, period)
{
    this->StateTable.AddData(PosGet, "PosData");

    mtsInterfaceProvided * provided = AddInterfaceProvided("p1");
    if (provided) {
        provided->AddCommandReadState(this->StateTable, this->PosGet, "ReadPos");
    }
}

void SignalGenerator::Startup(void)
{
    // To test
    std::cout << "Number of data: " << PosGet.GetNumberOfScalars() << std::endl;
    for (size_t i = 0; i < PosGet.GetNumberOfScalars(); ++i) {
        std::cout << "Data names: " << PosGet.GetScalarName(i) << std::endl;
    }
}

void SignalGenerator::Run(void)
{
    ProcessQueuedCommands();

    static int x = 0;

    // Read and print out current data
    //for (unsigned int i = 0; i < PosGet.GetNumberOfScalars(); ++i) {
    //    std::cout << PosGet.GetScalarName(i) << ": " << PosGet.GetDataAsDouble(i) << std::endl;
    //}

    // Update the current values
    double newValue;
    std::stringstream ss;
    for (unsigned int i = 0; i < PosGet.GetNumberOfScalars(); ++i) {
        //newValue = sin((double)x) * (100.0 + 10.0 * (double)i) / 100.0;
        newValue = sin(2 * cmnPI * static_cast<double>(this->GetTick()) * Period / 2.0)
            * (100.0 + 10.0 * (double)i) / 100.0;
        if (i == PosGet.GetNumberOfScalars() - 1) {
            newValue *= 10; // to test GCM visualizer with a signal of large amplitude
        }
        ss << newValue << " ";
    }
    x++;

    vctFrm3 newPosition;
    newPosition.FromStreamRaw(ss);
    PosGet.SetPosition(newPosition);
}
