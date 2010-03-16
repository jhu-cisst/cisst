/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: SignalGenerator.cpp 1236 2010-02-26 20:38:21Z adeguet1 $ */

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

#include <cisstCommon/cmnConstants.h>
#include <cisstVector.h>
#include "SignalGeneratorTask.h"

// required to implement the class services, see cisstCommon
CMN_IMPLEMENT_SERVICES(SignalGenerator);

SignalGenerator::SignalGenerator(const std::string & taskName, double period) : mtsTaskPeriodic(taskName, period)
{
    ReadDouble = 10.0;

    ReadVector.SetSize(12);
    for (int i = 0; i < 12; i++) {
        ReadVector(i) = 1.0 * (double) (i + 1);
    }

    this->StateTable.AddData(ReadDouble, "Double");
    this->StateTable.AddData(ReadVector, "Vector");

    // add one interface, this will create an mtsTaskInterface
    mtsProvidedInterface * provided = AddProvidedInterface("p1");
    if (provided) {
        provided->AddCommandReadState(this->StateTable, this->ReadDouble, "ReadDouble");
        provided->AddCommandReadState(this->StateTable, this->ReadVector, "ReadVector");
    }
}

void SignalGenerator::Startup(void)
{
}

void SignalGenerator::Run(void) 
{
    ProcessQueuedCommands();

    static int x = 0;

    ReadDouble = sin((double)x) * 10.0;
    for (int i = 0; i < 12; i++) {
        ReadVector(i) = cos((double)x - cmnPI/2) * 15.0;
    }
    x++;

    // Fetch new values from state table
    mtsProvidedInterface * provided = GetProvidedInterface("p1");
    mtsCommandReadBase * readDoubleCommand = provided->GetCommandRead("ReadDouble");
    mtsCommandReadBase * readVectorCommand = provided->GetCommandRead("ReadVector");

    mtsDouble readDouble;
    mtsDoubleVec readVector;

    readDoubleCommand->Execute(readDouble);
    readVectorCommand->Execute(readVector);

    std::cout << "Double: " << readDouble.Data << std::endl;
    std::cout << "\tVector: ";
    for (int i = 0; i < 12; i++) {
        std::cout << " " << readVector[i];
    }
    std::cout << std::endl;
}
