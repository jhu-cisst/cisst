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
#include <cisstMultiTask/mtsFaultDetectorThresholding.h>

CMN_IMPLEMENT_SERVICES(mtsMonitorFilterBypass);
CMN_IMPLEMENT_SERVICES(mtsMonitorFilterTrendVel);
CMN_IMPLEMENT_SERVICES(mtsMonitorFilterVectorize);
CMN_IMPLEMENT_SERVICES(mtsMonitorFilterNorm);
CMN_IMPLEMENT_SERVICES(mtsMonitorFilterArithmetic);
CMN_IMPLEMENT_SERVICES(mtsMonitorFilterAverage);

//-----------------------------------------------------------------------------
//  Filter Name Definitions
//
#define DEFINE_MONITOR_FILTER_NAMES(_filterName)\
const std::string NameOfFilter##_filterName = #_filterName;
DEFINE_MONITOR_FILTER_NAMES(Bypass);
DEFINE_MONITOR_FILTER_NAMES(TrendVel);
DEFINE_MONITOR_FILTER_NAMES(Vectorize);
DEFINE_MONITOR_FILTER_NAMES(Norm);
DEFINE_MONITOR_FILTER_NAMES(Arithmetic);
DEFINE_MONITOR_FILTER_NAMES(Average);
/*
DEFINE_MONITOR_FILTER_NAMES(Sampling);
DEFINE_MONITOR_FILTER_NAMES(Min);
DEFINE_MONITOR_FILTER_NAMES(Max);
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
    std::stringstream ss;
    ss << "Bypass:" << inputName << ":" << this->FilterUID;
    AddOutputSignal(ss.str(), signalType);
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
        InputSignals[0]->Placeholder = StateTable->GetNewValueScalar(InputSignals[0]->GetStateDataId(), timestamp);
        OutputSignals[0]->Placeholder = InputSignals[0]->Placeholder; // bypass
    } else {
        InputSignals[0]->PlaceholderVector = StateTable->GetNewValueVector(InputSignals[0]->GetStateDataId(), timestamp);
        OutputSignals[0]->PlaceholderVector = InputSignals[0]->PlaceholderVector;
    }

    if (debug) {
        if (SignalType == BaseType::SignalElement::SCALAR) {
            std::cout << this->GetFilterName() << "\t" << InputSignals[0]->GetName() << ": " 
                << InputSignals[0]->Placeholder << " => " << OutputSignals[0]->Placeholder << std::endl;
        } else {
            std::cout << this->GetFilterName() << "\t" << InputSignals[0]->GetName() << ": " 
                << InputSignals[0]->PlaceholderVector << " => " << OutputSignals[0]->PlaceholderVector << std::endl;
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
    std::stringstream ss;
    ss << "TrendVel:" << inputName << ":" << this->FilterUID;
    AddOutputSignal(ss.str(), signalType);
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
        InputSignals[0]->Placeholder = StateTable->GetNewValueScalar(InputSignals[0]->GetStateDataId(), timestamp);

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
        InputSignals[0]->PlaceholderVector = StateTable->GetNewValueVector(InputSignals[0]->GetStateDataId(), timestamp);

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
                    << InputSignals[0]->Placeholder << " => " << OutputSignals[0]->Placeholder << std::endl;
        } else {
            std::cout << this->GetFilterName() << "\t" << InputSignals[0]->GetName() << ": " 
                    << InputSignals[0]->PlaceholderVector << " => " << OutputSignals[0]->PlaceholderVector << std::endl;
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
    : mtsMonitorFilterBase(BaseType::INVALID, ::NameOfFilterVectorize),
      InputSize(0)
{
    this->Enable(false);
}

mtsMonitorFilterVectorize::mtsMonitorFilterVectorize(
    BaseType::FILTER_TYPE filterType, 
    const BaseType::SignalNamesType & inputNames)
    : mtsMonitorFilterBase(filterType, ::NameOfFilterVectorize),
      InputSize(inputNames.size())
{
    // Define inputs
    for (size_t i = 0; i < InputSize; ++i) {
        this->AddInputSignal(inputNames[i], BaseType::SignalElement::SCALAR);
    }

    // Define outputs
    std::stringstream ss;
    ss << "Vectorize:" << inputNames.size() << ":" << this->FilterUID;
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
        InputSignals[i]->Placeholder = StateTable->GetNewValueScalar(InputSignals[i]->GetStateDataId(), timestamp);
        // Update output vector
        OutputSignals[0]->PlaceholderVector(i) = InputSignals[i]->Placeholder;
    }

    if (debug) {
        std::cout << this->GetFilterName() << "\t";
        for (size_t i = 0; i < InputSize; ++i) {
            std::cout << InputSignals[i]->GetName() << ": " << InputSignals[i]->Placeholder << ", ";
        }
        std::cout << " => " << OutputSignals[0]->PlaceholderVector << std::endl;
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
    ss << "Norm:" << inputName << ":" << this->FilterUID;
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
    InputSignals[0]->PlaceholderVector = StateTable->GetNewValueVector(InputSignals[0]->GetStateDataId(), timestamp);
    // Update output vector
    switch (NormType) {
    case mtsMonitorFilterNorm::L1NORM:
        OutputSignals[0]->Placeholder = InputSignals[0]->PlaceholderVector.L1Norm();
        break;
    case mtsMonitorFilterNorm::L2NORM:
        OutputSignals[0]->Placeholder = sqrt(InputSignals[0]->PlaceholderVector.DotProduct(InputSignals[0]->PlaceholderVector));
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


//-----------------------------------------------------------------------------
//  Arithmetic Filter
//
// DO NOT USE DEFAULT CONSTRUCTOR
mtsMonitorFilterArithmetic::mtsMonitorFilterArithmetic()
    : mtsMonitorFilterBase(BaseType::INVALID, ::NameOfFilterArithmetic),
      OperationType(INVALID),
      IsOffsetScalar(true), OffsetScalar(0.0), OffsetVector()
{
    this->Enable(false);
}

mtsMonitorFilterArithmetic::mtsMonitorFilterArithmetic(BaseType::FILTER_TYPE filterType, 
                                                       OPERATION_TYPE operationType,
                                                       const std::string & input1Name,
                                                       BaseType::SignalElement::SIGNAL_TYPE input1Type,
                                                       const BaseType::PlaceholderVectorType input2)
    : mtsMonitorFilterBase(filterType, ::NameOfFilterArithmetic),
      OperationType(operationType),
      IsOffsetScalar(false), OffsetScalar(0.0), OffsetVector(input2)
{
    // Define inputs
    this->AddInputSignal(input1Name, input1Type);

    // Define outputs
    std::stringstream ss;
    ss << "Arithmetic:" << input1Name << ":" << this->FilterUID;
    AddOutputSignal(ss.str(), BaseType::SignalElement::VECTOR);

    IsInputScalar = (input1Type == BaseType::SignalElement::SCALAR);
    IsOutputScalar = false;
}

mtsMonitorFilterArithmetic::mtsMonitorFilterArithmetic(BaseType::FILTER_TYPE filterType, 
                                                       OPERATION_TYPE operationType,
                                                       const std::string & input1Name,
                                                       BaseType::SignalElement::SIGNAL_TYPE input1Type,
                                                       const BaseType::PlaceholderType input2)
    : mtsMonitorFilterBase(filterType, ::NameOfFilterArithmetic),
      OperationType(operationType),
      IsOffsetScalar(true), OffsetScalar(input2), OffsetVector()
{
    // Define inputs
    this->AddInputSignal(input1Name, input1Type);

    // Define outputs
    std::stringstream ss;
    ss << "Arithmetic:" << input1Name << ":" << this->FilterUID;
    AddOutputSignal(ss.str(), input1Type);

    IsInputScalar = (input1Type == BaseType::SignalElement::SCALAR);
    IsOutputScalar = IsInputScalar;
}

mtsMonitorFilterArithmetic::~mtsMonitorFilterArithmetic()
{
}

void mtsMonitorFilterArithmetic::DoFiltering(bool debug)
{
    if (!this->IsEnabled()) return;

    // Fetch new values from state table
    double timestamp;
    if (IsInputScalar)
        InputSignals[0]->Placeholder = StateTable->GetNewValueScalar(InputSignals[0]->GetStateDataId(), timestamp);
    else
        InputSignals[0]->PlaceholderVector = StateTable->GetNewValueVector(InputSignals[0]->GetStateDataId(), timestamp);

    // Update output vector
    if (IsOutputScalar) {
        CMN_ASSERT(IsInputScalar);
        CMN_ASSERT(IsOffsetScalar);
        OutputSignals[0]->Placeholder = InputSignals[0]->Placeholder - OffsetScalar;
    } else {
        if (IsInputScalar) {
            CMN_ASSERT(!IsOffsetScalar);
            OutputSignals[0]->PlaceholderVector = InputSignals[0]->Placeholder - OffsetVector;
        } else {
            if (IsOffsetScalar)
                OutputSignals[0]->PlaceholderVector = InputSignals[0]->PlaceholderVector - OffsetScalar;
            else
                OutputSignals[0]->PlaceholderVector = InputSignals[0]->PlaceholderVector - OffsetVector;
        }
    }

    if (debug) {
        std::cout << this->GetFilterName() << "\t" << InputSignals[0]->GetName() << ": "; 
        if (IsInputScalar)
            std::cout << InputSignals[0]->Placeholder;
        else
            std::cout << InputSignals[0]->PlaceholderVector;
        std::cout << " => ";
        if (IsOutputScalar)
            std::cout << OutputSignals[0]->Placeholder;
        else
            std::cout << OutputSignals[0]->PlaceholderVector;
        std::cout << std::endl;
    }
}

void mtsMonitorFilterArithmetic::ToStream(std::ostream & outputStream) const
{
    outputStream << "Filter Name: " << this->GetFilterName()
                 << ", " << (this->IsEnabled() ? "Enabled" : "Disabled") << ", "
                 << ", Input: \"" << InputSignals[0]->GetName() << "\""
                 << ", Output: \"" << OutputSignals[0]->GetName() << "\"";
}


//-----------------------------------------------------------------------------
//  Mean (Moving Average) Filter
//
// DO NOT USE DEFAULT CONSTRUCTOR
mtsMonitorFilterAverage::mtsMonitorFilterAverage()
    : mtsMonitorFilterBase(BaseType::INVALID, ::NameOfFilterAverage),
      SignalType(BaseType::SignalElement::SCALAR),
      Lambda(0.0), OldValueInitialized(false)
{
    this->Enable(false);
}

mtsMonitorFilterAverage::mtsMonitorFilterAverage(
    BaseType::FILTER_TYPE filterType, 
    const std::string & inputName,
    BaseType::SignalElement::SIGNAL_TYPE inputType,
    double lambda)
    : mtsMonitorFilterBase(filterType, ::NameOfFilterAverage),
      SignalType(inputType),
      Lambda(lambda), OldValueInitialized(false)
{
    // Define inputs
    this->AddInputSignal(inputName, inputType);

    // Define outputs
    std::stringstream ss;
    ss << "Avg:" << inputName << ":" << this->FilterUID;
    AddOutputSignal(ss.str(), inputType);
}

mtsMonitorFilterAverage::~mtsMonitorFilterAverage()
{
}

void mtsMonitorFilterAverage::DoFiltering(bool debug)
{
    if (!this->IsEnabled()) return;

    // Fetch new values from state table
    double timestamp;
    if (SignalType == BaseType::SignalElement::SCALAR) {
        // Fetch new value from state table
        InputSignals[0]->Placeholder = StateTable->GetNewValueScalar(InputSignals[0]->GetStateDataId(), timestamp);

        // Update output value (EWMA)
        if (!OldValueInitialized) {
            OldValueScalar = InputSignals[0]->Placeholder;
            OutputSignals[0]->Placeholder = OldValueScalar;
            OldValueInitialized = true;
        } else {
            OutputSignals[0]->Placeholder = Lambda * InputSignals[0]->Placeholder + (1.0 - Lambda) * OldValueScalar;
            OldValueScalar = OutputSignals[0]->Placeholder;
        }
    } else {
        // Fetch new values from state table
        InputSignals[0]->PlaceholderVector = StateTable->GetNewValueVector(InputSignals[0]->GetStateDataId(), timestamp);

        // Update output values (EWMA)
        if (!OldValueInitialized) {
            OldValueVector = InputSignals[0]->PlaceholderVector;
            OutputSignals[0]->PlaceholderVector = OldValueVector;
            OldValueInitialized = true;
        } else {
            OutputSignals[0]->PlaceholderVector = Lambda * OldValueVector + (1.0 - Lambda) * InputSignals[0]->PlaceholderVector;
            OldValueVector = OutputSignals[0]->PlaceholderVector;
        }
    }

    if (debug) {
        if (SignalType == BaseType::SignalElement::SCALAR)
            std::cout << this->GetFilterName() << "\t" << InputSignals[0]->GetName() << ": " 
                      << InputSignals[0]->Placeholder << " => " << OutputSignals[0]->Placeholder << std::endl;
        else
            std::cout << this->GetFilterName() << "\t" << InputSignals[0]->GetName() << ": " 
                      << InputSignals[0]->PlaceholderVector << " => " << OutputSignals[0]->PlaceholderVector << std::endl;
    }
}

void mtsMonitorFilterAverage::ToStream(std::ostream & outputStream) const
{
    outputStream << "Filter Name: " << this->GetFilterName()
                 << ", " << (this->IsEnabled() ? "Enabled" : "Disabled") << ", "
                 << ", Input: \"" << InputSignals[0]->GetName() << "\""
                 << ", Output: \"" << OutputSignals[0]->GetName() << "\"";
}


