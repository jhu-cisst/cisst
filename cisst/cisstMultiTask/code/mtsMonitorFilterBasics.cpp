/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsMonitorBasics.cpp 3303 2012-01-02 16:33:23Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2012-01-07

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#include <cisstMultiTask/mtsStateTable.h>
#include <cisstMultiTask/mtsMonitorFilterBasics.h>

//-----------------------------------------------------------------------------
//  Filter Name Definitions
//
#define DEFINE_MONITOR_FILTER_NAMES(_filterName)\
const std::string NameOfFilter##_filterName = #_filterName;
DEFINE_MONITOR_FILTER_NAMES(Bypass);
#undef DEFINE_MONITOR_FILTER_NAMES

//-----------------------------------------------------------------------------
//  Bypass Filter
//
CMN_IMPLEMENT_SERVICES(mtsMonitorFilterBypass);

// DO NOT USE DEFAULT CONSTRUCTOR
mtsMonitorFilterBypass::mtsMonitorFilterBypass()
    : mtsMonitorFilterBase(::NameOfFilterBypass)
{
    this->Enable(false);
}

mtsMonitorFilterBypass::mtsMonitorFilterBypass(const std::string & inputName)
    : mtsMonitorFilterBase(::NameOfFilterBypass)
{
    // Define inputs
    this->AddInputSignal(inputName);

    // Define outputs
    std::string outputName(inputName);
    outputName += ":Bypass";
    this->AddOutputSignal(outputName);
}

mtsMonitorFilterBypass::~mtsMonitorFilterBypass()
{
}

void mtsMonitorFilterBypass::DoFiltering(void)
{
    if (!this->IsEnabled()) return;

    // Fetch new value from state table
    InputSignals[0]->Placeholder = StateTable->GetNewValue(InputSignals[0]->GetStateDataId());

    // Update output value (bypass)
    OutputSignals[0]->Placeholder = InputSignals[0]->Placeholder;

#if 1 // MJ TEST
    static int a = 0;
    std::cout << a << ": " << GetFilterName() << ", " << (this->IsEnabled() ? "Enabled" : "Disabled") << std::endl;

    for (size_t i = 0; i < OutputSignals.size(); ++i) {
        std::cout << "[" << i << "] " << InputSignals[0]->Placeholder << ", " << OutputSignals[0]->Placeholder << std::endl;
    }
#endif
}

void mtsMonitorFilterBypass::ToStream(std::ostream & outputStream) const
{
    outputStream << "mtsFilter Name: " << this->GetFilterName() << ", " << (this->IsEnabled() ? "Enabled" : "Disabled");
}

