/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsFaultBase.cpp 3303 2012-01-02 16:33:23Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2012-02-07

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#include <cisstMultiTask/mtsFaultBase.h>

CMN_IMPLEMENT_SERVICES(mtsFaultLocation);
CMN_IMPLEMENT_SERVICES(mtsFaultBase);

//
// mtsFaultLocation
//
void mtsFaultLocation::ToStream(std::ostream & outputStream) const
{
    mtsGenericObject::ToStream(outputStream);
    outputStream << " Process: \""        << this->Process << "\","
                 << " Component: \""      << this->Component << "\", "
                 << " Interface: \""      << this->Interface << "\", "
                 << " Command: \""        << this->Command << "\", "
                 << " Function: \""       << this->Function << "\", "
                 << " EventGenerator: \"" << this->EventGenerator << "\", "
                 << " EventHandler: \""   << this->EventHandler << std::endl;
}

void mtsFaultLocation::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, Process);
    cmnSerializeRaw(outputStream, Component);
    cmnSerializeRaw(outputStream, Interface);
    cmnSerializeRaw(outputStream, Command);
    cmnSerializeRaw(outputStream, Function);
    cmnSerializeRaw(outputStream, EventGenerator);
    cmnSerializeRaw(outputStream, EventHandler);
}

void mtsFaultLocation::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, Process);
    cmnDeSerializeRaw(inputStream, Component);
    cmnDeSerializeRaw(inputStream, Interface);
    cmnDeSerializeRaw(inputStream, Command);
    cmnDeSerializeRaw(inputStream, Function);
    cmnDeSerializeRaw(inputStream, EventGenerator);
    cmnDeSerializeRaw(inputStream, EventHandler);
}

//
// mtsFaultBase
//
mtsFaultBase::mtsFaultBase(void) : mtsGenericObject(),
    FaultName("NONAME"),
    FaultLayer(mtsFaultBase::LAYER_INVALID), FaultTimestamp(0.0),
    FaultType(mtsFaultBase::FAULT_INVALID), FaultMagnitude(0.0)
{
}

mtsFaultBase::mtsFaultBase(const std::string & faultName, FaultTypes faultType) 
    : mtsGenericObject(),
      FaultName(faultName),
      FaultTimestamp(0.0),
      FaultType(faultType), 
      FaultMagnitude(0.0)
{
    if (faultType & FAULT_SYSTEM_MASK)         FaultLayer = LAYER_SYSTEM;
    else if (faultType & FAULT_PROCESS_MASK)   FaultLayer = LAYER_PROCESS;
    else if (faultType & FAULT_COMPONENT_MASK) FaultLayer = LAYER_COMPONENT;
    else if (faultType & FAULT_INTERFACE_MASK) FaultLayer = LAYER_INTERFACE;
    else if (faultType & FAULT_EXECUTION_MASK) FaultLayer = LAYER_EXECUTION;
    else {
        std::stringstream ss;
        ss << "mtsFaultBase: invalid fault type for fault \"" << faultName << "\"";
        CMN_LOG_CLASS_INIT_ERROR << ss << std::endl;
        cmnThrow(std::string(ss.str()));
    }
}

mtsFaultBase::~mtsFaultBase()
{
}

mtsFaultLocation & mtsFaultBase::GetFaultLocation(void) {
    return FaultLocation;
}

void mtsFaultBase::SetFaultTimestamp(double timestamp) {
    FaultTimestamp = timestamp;
}

double mtsFaultBase::GetFaultTimestamp(void) {
    return FaultTimestamp;
}

void mtsFaultBase::ToStream(std::ostream & outputStream) const
{
    mtsGenericObject::ToStream(outputStream);

    outputStream << " Name: \"" << this->FaultName << "\"";
    outputStream << " Layer: ";
    switch (FaultLayer) {
        case mtsFaultBase::LAYER_SYSTEM:    outputStream << "SYSTEM"; break;
        case mtsFaultBase::LAYER_PROCESS:   outputStream << "PROCESS"; break;
        case mtsFaultBase::LAYER_COMPONENT: outputStream << "COMPONENT"; break;
        case mtsFaultBase::LAYER_INTERFACE: outputStream << "INTERFACE"; break;
        case mtsFaultBase::LAYER_EXECUTION: outputStream << "EXECUTION"; break;
        case mtsFaultBase::LAYER_INVALID:   
        default:                            outputStream << "INVALID";
    }
    outputStream << ", Location: [" << this->FaultLocation << "]";
    outputStream << ", Timestamp: " << this->FaultTimestamp;
    outputStream << ", Fault type: ";
    switch (this->FaultType) {
        case mtsFaultBase::FAULT_SYSTEM_PROCESS: 
            outputStream << "System/Process"; break;
        case mtsFaultBase::FAULT_SYSTEM_CONNECTION: 
            outputStream << "System/Connection"; break;
        case mtsFaultBase::FAULT_SYSTEM_NETWORK: 
            outputStream << "System/Network"; break;
        case mtsFaultBase::FAULT_SYSTEM_FROM_PROCESS_LAYER: 
            outputStream << "System/FromSubLayer"; break;
        // Process layer
        case mtsFaultBase::FAULT_PROCESS_COMPONENT:
            outputStream << "Process/Component"; break;
        case mtsFaultBase::FAULT_PROCESS_FROM_COMPONENT_LAYER:
            outputStream << "Process/FromSubLayer"; break;
        // Component layer
        case mtsFaultBase::FAULT_COMPONENT_FUNCTIONAL:
            outputStream << "Component/Functional"; break;
        case mtsFaultBase::FAULT_COMPONENT_NONFUNCTIONAL:
            outputStream << "Component/Nonfunctional"; break;
        case mtsFaultBase::FAULT_COMPONENT_FROM_INTERFACE_LAYER:
            outputStream << "Component/FromSubLayer"; break;
        // Interface layer
        case mtsFaultBase::FAULT_INTERFACE_ONCONNECT:
            outputStream << "Interface/OnConnect"; break;
        case mtsFaultBase::FAULT_INTERFACE_POSTCONNECT:
            outputStream << "Interface/PostConnect"; break;
        // Execution layer
        case mtsFaultBase::FAULT_EXECUTION_PERFORMANCE:
            outputStream << "Execution/Performance"; break;
        case mtsFaultBase::FAULT_EXECUTION_NETWORK:
            outputStream << "Execution/Network"; break;
        case mtsFaultBase::FAULT_EXECUTION_INVALID_PAYLOAD:
            outputStream << "Execution/InvalidPayload"; break;
        // INVALID
        case mtsFaultBase::FAULT_INVALID:
        default:
            outputStream << "Invalid";
    }
    outputStream << ", Fault magnitude: " << this->FaultMagnitude;
}

void mtsFaultBase::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);

    cmnSerializeRaw(outputStream, this->FaultName);
    cmnSerializeRaw(outputStream, this->FaultLayer);
    FaultLocation.SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, this->FaultTimestamp);
    cmnSerializeRaw(outputStream, this->FaultType);
    cmnSerializeRaw(outputStream, this->FaultMagnitude);
}

void mtsFaultBase::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);

    cmnDeSerializeRaw(inputStream, this->FaultName);
    cmnDeSerializeRaw(inputStream, this->FaultLayer);
    FaultLocation.DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, this->FaultTimestamp);
    cmnDeSerializeRaw(inputStream, this->FaultType);
    cmnDeSerializeRaw(inputStream, this->FaultMagnitude);
}

