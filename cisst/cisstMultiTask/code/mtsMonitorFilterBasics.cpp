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

CMN_IMPLEMENT_SERVICES(mtsMonitorFilterBypass);
CMN_IMPLEMENT_SERVICES(mtsMonitorFilterTrendVel);
CMN_IMPLEMENT_SERVICES(mtsMonitorFilterVectorize);
CMN_IMPLEMENT_SERVICES(mtsMonitorFilterNorm);

//-----------------------------------------------------------------------------
//  Filter Name Definitions
//
#define DEFINE_MONITOR_FILTER_NAMES(_filterName)\
const std::string NameOfFilter##_filterName = #_filterName;
DEFINE_MONITOR_FILTER_NAMES(Bypass);
DEFINE_MONITOR_FILTER_NAMES(TrendVel);
DEFINE_MONITOR_FILTER_NAMES(Vectorize);
DEFINE_MONITOR_FILTER_NAMES(Norm);
/*
DEFINE_MONITOR_FILTER_NAMES(Sampling);
DEFINE_MONITOR_FILTER_NAMES(Min);
DEFINE_MONITOR_FILTER_NAMES(Max);
DEFINE_MONITOR_FILTER_NAMES(Avg);
DEFINE_MONITOR_FILTER_NAMES(Std);
*/
#undef DEFINE_MONITOR_FILTER_NAMES

//-----------------------------------------------------------------------------
//  Bypass Filter
//
// DO NOT USE DEFAULT CONSTRUCTOR
mtsMonitorFilterBypass::mtsMonitorFilterBypass()
    : mtsMonitorFilterBase(BaseType::INVALID, ::NameOfFilterBypass)
{
    this->Enable(false);
}

mtsMonitorFilterBypass::mtsMonitorFilterBypass(
    BaseType::FILTER_TYPE filterType, const std::string & inputName, BaseType::SignalElement::SIGNAL_TYPE signalType)
    : mtsMonitorFilterBase(filterType, ::NameOfFilterBypass),
      SignalType(signalType)
{
    // Define inputs
    this->AddInputSignal(inputName, signalType);

    // Define outputs
    std::string outputName(inputName);
    outputName += ":Bypass";
    this->AddOutputSignal(outputName, signalType);
}

mtsMonitorFilterBypass::~mtsMonitorFilterBypass()
{
}

void mtsMonitorFilterBypass::DoFiltering(bool debug)
{
    if (!this->IsEnabled()) return;

    // Fetch new value from state table and update output value
    double timestamp;
    if (SignalType == BaseType::SignalElement::SCALAR) {
        InputSignals[0]->Placeholder = StateTable->GetNewValue(InputSignals[0]->GetStateDataId(), timestamp);
        OutputSignals[0]->Placeholder = InputSignals[0]->Placeholder; // bypass
    } else {
        InputSignals[0]->PlaceholderVector = StateTable->GetNewValue(InputSignals[0]->GetStateDataId(), timestamp);
        OutputSignals[0]->PlaceholderVector = InputSignals[0]->PlaceholderVector;
    }

    if (debug) {
        if (SignalType == BaseType::SignalElement::SCALAR) {
            std::cout << this->GetFilterName() << "\t" << InputSignals[0]->GetName() << ": " 
                << InputSignals[0]->Placeholder << ", " << OutputSignals[0]->Placeholder << std::endl;
        } else {
            std::cout << this->GetFilterName() << "\t" << InputSignals[0]->GetName() << ": " 
                << InputSignals[0]->PlaceholderVector << ", " << OutputSignals[0]->PlaceholderVector << std::endl;
        }
    }
}

void mtsMonitorFilterBypass::ToStream(std::ostream & outputStream) const
{
    outputStream << "Filter Name: " << this->GetFilterName() << " ("
                 << (SignalType == BaseType::SignalElement::SCALAR ? "SCALAR" : "VECTOR")
                 << "), " << (this->IsEnabled() ? "Enabled" : "Disabled")
                 << ", Input: \"" << InputSignals[0]->GetName() << "\", Output: \"" << OutputSignals[0]->GetName() << "\"";
}


//-----------------------------------------------------------------------------
//  Trend Velocity Filter
//
// DO NOT USE DEFAULT CONSTRUCTOR
mtsMonitorFilterTrendVel::mtsMonitorFilterTrendVel()
    : mtsMonitorFilterBase(BaseType::INVALID, ::NameOfFilterTrendVel),
      OldValueScalar(0.0), OldTimestamp(0.0), OldValueVectorInitialized(false)
{
    this->Enable(false);
}

mtsMonitorFilterTrendVel::mtsMonitorFilterTrendVel(
    BaseType::FILTER_TYPE filterType, 
    const std::string & inputName,
    BaseType::SignalElement::SIGNAL_TYPE signalType)
    : mtsMonitorFilterBase(filterType, ::NameOfFilterTrendVel),
      SignalType(signalType),
      OldValueScalar(0.0), OldTimestamp(0.0), OldValueVectorInitialized(false)
{
    // Define inputs
    this->AddInputSignal(inputName, signalType);

    // Define outputs
    std::string outputName(inputName);
    outputName += ":Vel";
    this->AddOutputSignal(outputName, signalType);
}

mtsMonitorFilterTrendVel::~mtsMonitorFilterTrendVel()
{
}

void mtsMonitorFilterTrendVel::DoFiltering(bool debug)
{
    if (!this->IsEnabled()) return;

    double timestamp, deltaT;

    if (SignalType == BaseType::SignalElement::SCALAR) {
        // Fetch new value from state table
        InputSignals[0]->Placeholder = StateTable->GetNewValue(InputSignals[0]->GetStateDataId(), timestamp);

        // Update output value (velocity)
        deltaT = timestamp - OldTimestamp;
        if (deltaT == 0.0) {
            OutputSignals[0]->Placeholder = 0.0;
        } else {
            OutputSignals[0]->Placeholder = (InputSignals[0]->Placeholder - OldValueScalar) / deltaT;
        }

        // Remember last values
        OldValueScalar = InputSignals[0]->Placeholder;
        OldTimestamp = timestamp;
    } else {
        // Fetch new values from state table
        InputSignals[0]->PlaceholderVector = StateTable->GetNewValue(InputSignals[0]->GetStateDataId(), timestamp);

        // Update output values (velocity)
        deltaT = timestamp - OldTimestamp;
        if (deltaT == 0.0) {
            OutputSignals[0]->PlaceholderVector.SetAll(0.0);
        } else {
            if (!OldValueVectorInitialized) {
                OldValueVector = InputSignals[0]->PlaceholderVector;
                OldValueVectorInitialized = true;
            }
            OutputSignals[0]->PlaceholderVector = (InputSignals[0]->PlaceholderVector - OldValueVector) / deltaT;
        }

        // Remember last values
        OldValueVector = InputSignals[0]->PlaceholderVector;
        OldTimestamp = timestamp;
    }

    if (debug) {
        if (SignalType == BaseType::SignalElement::SCALAR) {
            std::cout << this->GetFilterName() << "\t" << InputSignals[0]->GetName() << ": " 
                    << InputSignals[0]->Placeholder << ", " << OutputSignals[0]->Placeholder << std::endl;
        } else {
            std::cout << this->GetFilterName() << "\t" << InputSignals[0]->GetName() << ": " 
                    << InputSignals[0]->PlaceholderVector << ", " << OutputSignals[0]->PlaceholderVector << std::endl;
        }
    }
}

void mtsMonitorFilterTrendVel::ToStream(std::ostream & outputStream) const
{
    outputStream << "Filter Name: " << this->GetFilterName() << " ("
                 << (SignalType == BaseType::SignalElement::SCALAR ? "SCALAR" : "VECTOR")
                 << "), " << (this->IsEnabled() ? "Enabled" : "Disabled") << ", "
                 << "Input: \"" << InputSignals[0]->GetName() << "\", Output: \"" << OutputSignals[0]->GetName() << "\"";
}


//-----------------------------------------------------------------------------
//  Vectorize Filter
//
// DO NOT USE DEFAULT CONSTRUCTOR
mtsMonitorFilterVectorize::mtsMonitorFilterVectorize()
    : mtsMonitorFilterBase(BaseType::INVALID, ::NameOfFilterVectorize)
{
    this->Enable(false);
}

mtsMonitorFilterVectorize::mtsMonitorFilterVectorize(
    BaseType::FILTER_TYPE filterType, 
    const BaseType::SignalNamesType & inputNames)
    : mtsMonitorFilterBase(filterType, ::NameOfFilterVectorize)
{
    InputSize = inputNames.size();

    // Define inputs
    for (size_t i = 0; i < InputSize; ++i) {
        this->AddInputSignal(inputNames[i], BaseType::SignalElement::SCALAR);
    }

    // Define outputs
    std::stringstream ss;
    ss << "Vector" << this->FilterUID;
    AddOutputSignal(ss.str(), BaseType::SignalElement::VECTOR);

    GetOutputSignalElement(0)->PlaceholderVector.SetSize(InputSize);
    GetOutputSignalElement(0)->PlaceholderVector.SetAll(0.0);
}

mtsMonitorFilterVectorize::~mtsMonitorFilterVectorize()
{
}

void mtsMonitorFilterVectorize::DoFiltering(bool debug)
{
    if (!this->IsEnabled()) return;

    double timestamp;
    for (size_t i = 0; i < InputSize; ++i) {
        // Fetch new values from state table
        InputSignals[i]->Placeholder = StateTable->GetNewValue(InputSignals[i]->GetStateDataId(), timestamp);
        // Update output vector
        OutputSignals[0]->PlaceholderVector(i) = InputSignals[i]->Placeholder;
    }

    if (debug) {
        std::cout << this->GetFilterName() << "\t" << InputSignals[0]->GetName() << ": " 
                  << InputSignals[0]->Placeholder << ", " << OutputSignals[0]->PlaceholderVector << std::endl;
    }
}

void mtsMonitorFilterVectorize::ToStream(std::ostream & outputStream) const
{
    outputStream << "Filter Name: " << this->GetFilterName()
                 << ", " << (this->IsEnabled() ? "Enabled" : "Disabled") << ", "
                 << ", Input (" << InputSize << "): ";
    for (size_t i = 0; i < InputSize; ++i) {
        outputStream << "\"" << InputSignals[i]->GetName() << "\", ";
    }
    outputStream << "Output: ";
    outputStream << "\"" << OutputSignals[0]->GetName() << "\"";
}


//-----------------------------------------------------------------------------
//  Norm Filter
//
// DO NOT USE DEFAULT CONSTRUCTOR
mtsMonitorFilterNorm::mtsMonitorFilterNorm()
    : mtsMonitorFilterBase(BaseType::INVALID, ::NameOfFilterNorm)
{
    this->Enable(false);
}

mtsMonitorFilterNorm::mtsMonitorFilterNorm(
    BaseType::FILTER_TYPE filterType, 
    const std::string & inputName,
    mtsMonitorFilterNorm::NORM_TYPE normType)
    : mtsMonitorFilterBase(filterType, ::NameOfFilterNorm),
      NormType(normType)
{
    // Define inputs
    this->AddInputSignal(inputName, BaseType::SignalElement::VECTOR);

    // Define outputs
    std::stringstream ss;
    ss << "Norm" << this->FilterUID;
    AddOutputSignal(ss.str(), BaseType::SignalElement::SCALAR);
}

mtsMonitorFilterNorm::~mtsMonitorFilterNorm()
{
}

void mtsMonitorFilterNorm::DoFiltering(bool debug)
{
    if (!this->IsEnabled()) return;

    // Fetch new values from state table
    double timestamp;
    InputSignals[0]->PlaceholderVector = StateTable->GetNewValue(InputSignals[0]->GetStateDataId(), timestamp);
    // Update output vector
    switch (NormType) {
    case mtsMonitorFilterNorm::L1NORM:
        OutputSignals[0]->Placeholder = InputSignals[0]->PlaceholderVector.L1Norm();
        break;
    case mtsMonitorFilterNorm::L2NORM:
        OutputSignals[0]->Placeholder = InputSignals[0]->Placeholder; // MJ FIXME: Implement L2Norm
        break;
    case mtsMonitorFilterNorm::LINFNORM:
        OutputSignals[0]->Placeholder = InputSignals[0]->PlaceholderVector.LinfNorm();
        break;
    }

    if (debug) {
        std::cout << this->GetFilterName() << "\t" << InputSignals[0]->GetName() << ": " 
                  << InputSignals[0]->PlaceholderVector << " => " << OutputSignals[0]->Placeholder << std::endl;
    }
}

void mtsMonitorFilterNorm::ToStream(std::ostream & outputStream) const
{
    outputStream << "Filter Name: " << this->GetFilterName()
                 << ", " << (this->IsEnabled() ? "Enabled" : "Disabled") << ", "
                 << ", Input: \"" << InputSignals[0]->GetName() << "\""
                 << ", Output: \"" << OutputSignals[0]->GetName() << "\"";
}
