/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsMonitorBase.cpp 3303 2012-01-02 16:33:23Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2012-01-07

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#include <cisstCommon/cmnThrow.h>
#include <cisstMultiTask/mtsMonitorFilterBase.h>

int mtsMonitorFilterBase::UID = 0;

const std::string InvalidSignalName = "INVALID";

mtsMonitorFilterBase::mtsMonitorFilterBase(void)
    : FilterUID(-1), // MJ: invalid uid: don't use default constructor
      FilterType(mtsMonitorFilterBase::INVALID),
      FilterName("NONAME"), 
      Enabled(false)
{
}

mtsMonitorFilterBase::mtsMonitorFilterBase(FILTER_TYPE filterType, const std::string & filterName)
    : FilterUID(UID++),
      FilterType(filterType),
      FilterName(filterName),
      Enabled(true)
{
}

mtsMonitorFilterBase::~mtsMonitorFilterBase()
{
    for (size_t i = 0; i < InputSignals.size(); ++i) {
        delete InputSignals[i];
    }
    for (size_t i = 0; i < OutputSignals.size(); ++i) {
        delete OutputSignals[i];
    }
}

bool mtsMonitorFilterBase::AddInputSignal(const std::string & signalName)
{
    for (size_t i = 0; i < InputSignals.size(); ++i) {
        if (InputSignals[i]->GetName() == signalName) {
            CMN_LOG_CLASS_RUN_ERROR << "AddInputSignal: failed to add input signal (duplicate name): \"" << signalName << "\"" << std::endl;
            return false;
        }
    }

    SignalElement * newSignal = new SignalElement(signalName);
    InputSignals.push_back(newSignal);

    return true;
}

bool mtsMonitorFilterBase::AddOutputSignal(const std::string & signalName)
{
    for (size_t i = 0; i < OutputSignals.size(); ++i) {
        if (OutputSignals[i]->GetName() == signalName) {
            CMN_LOG_CLASS_RUN_ERROR << "AddOutputSignal: failed to add output signal (duplicate name): \"" << signalName << "\"" << std::endl;
            return false;
        }
    }

    SignalElement * newSignal = new SignalElement(signalName);
    OutputSignals.push_back(newSignal);

    return true;
}

std::string mtsMonitorFilterBase::GetInputSignalName(size_t index, bool withUID) const
{
    if (index >= InputSignals.size()) {
        CMN_LOG_CLASS_RUN_ERROR << "GetInputSignalName: index out of range (input signal count: " << InputSignals.size() << ")" << std::endl;
        return InvalidSignalName;
    }

    if (!withUID)
        return InputSignals[index]->GetName();
    else {
        std::stringstream ss;
        ss << InputSignals[index]->GetName() << FilterUID;
        return ss.str();
    }
}

std::string mtsMonitorFilterBase::GetOutputSignalName(size_t index, bool withUID) const
{
    if (index >= OutputSignals.size()) {
        CMN_LOG_CLASS_RUN_ERROR << "GetOutputSignalName: index out of range (output signal count: " << OutputSignals.size() << ")" << std::endl;
        return InvalidSignalName;
    }

    if (!withUID)
        return OutputSignals[index]->GetName();
    else {
        std::stringstream ss;
        ss << OutputSignals[index]->GetName() << FilterUID;
        return ss.str();
    }
}

mtsMonitorFilterBase::SignalNamesType mtsMonitorFilterBase::GetInputSignalNames(void) const
{
    SignalNamesType names;
    for (size_t i = 0; i < InputSignals.size(); ++i) {
        names.push_back(InputSignals[i]->GetName());
    }
    return names;
}

mtsMonitorFilterBase::SignalNamesType mtsMonitorFilterBase::GetOutputSignalNames(void) const
{
    SignalNamesType names;
    for (size_t i = 0; i < OutputSignals.size(); ++i) {
        names.push_back(OutputSignals[i]->GetName());
    }
    return names;
}

mtsMonitorFilterBase::SignalElement * mtsMonitorFilterBase::GetOutputSignalElement(size_t index) const
{
    if (index >= OutputSignals.size()) return 0;

    return OutputSignals[index];
}

mtsMonitorFilterBase::SignalElement * mtsMonitorFilterBase::GetInputSignalElement(size_t index) const
{
    if (index >= InputSignals.size()) return 0;

    return InputSignals[index];
}
