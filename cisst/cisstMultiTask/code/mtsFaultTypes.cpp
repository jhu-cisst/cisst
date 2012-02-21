/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsFaultTypes.cpp 3303 2012-01-02 16:33:23Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2012-02-07

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#include <cisstMultiTask/mtsFaultTypes.h>

//
//  System Layer
//
//-----------------------------------------------------------------------------
//  System Layer - Faults From Process Layer
//
CMN_IMPLEMENT_SERVICES(mtsFaultSystemFromSubLayer);

mtsFaultSystemFromSubLayer::mtsFaultSystemFromSubLayer(void) 
    : mtsFaultBase("System-Sublayer", BaseType::FAULT_SYSTEM_FROM_PROCESS_LAYER)
{
    SetFaultLocation();
}

mtsFaultSystemFromSubLayer::~mtsFaultSystemFromSubLayer()
{
}

void mtsFaultSystemFromSubLayer::SetFaultLocation(void)
{
    this->FaultLocation.Process        = "";
    this->FaultLocation.Component      = "";
    this->FaultLocation.Interface      = "";
    this->FaultLocation.Command        = "";
    this->FaultLocation.Function       = "";
    this->FaultLocation.EventGenerator = "";
    this->FaultLocation.EventHandler   = "";
}

void mtsFaultSystemFromSubLayer::ToStream(std::ostream & outputStream) const
{
    BaseType::ToStream(outputStream);
}

void mtsFaultSystemFromSubLayer::SerializeRaw(std::ostream & outputStream) const
{
    BaseType::SerializeRaw(outputStream);
}

void mtsFaultSystemFromSubLayer::DeSerializeRaw(std::istream & inputStream)
{
    BaseType::DeSerializeRaw(inputStream);
}

//
//  Process Layer
//
//-----------------------------------------------------------------------------
//  Process Layer - Faults From Component Layer
//
CMN_IMPLEMENT_SERVICES(mtsFaultProcessFromSubLayer);

mtsFaultProcessFromSubLayer::mtsFaultProcessFromSubLayer(void) 
    : mtsFaultBase("Process-Sublayer", BaseType::FAULT_PROCESS_FROM_COMPONENT_LAYER),
      TargetProcessName("")
{
}

mtsFaultProcessFromSubLayer::mtsFaultProcessFromSubLayer(const std::string & targetProcessName)
    : mtsFaultBase("Process-Sublayer", BaseType::FAULT_PROCESS_FROM_COMPONENT_LAYER),
      TargetProcessName(targetProcessName)
{
    SetFaultLocation();
}

mtsFaultProcessFromSubLayer::~mtsFaultProcessFromSubLayer()
{
}

void mtsFaultProcessFromSubLayer::SetFaultLocation(void)
{
    this->FaultLocation.Process        = TargetProcessName;
    this->FaultLocation.Component      = "";
    this->FaultLocation.Interface      = "";
    this->FaultLocation.Command        = "";
    this->FaultLocation.Function       = "";
    this->FaultLocation.EventGenerator = "";
    this->FaultLocation.EventHandler   = "";
}

void mtsFaultProcessFromSubLayer::ToStream(std::ostream & outputStream) const
{
    BaseType::ToStream(outputStream);
    outputStream << "Process: " << TargetProcessName;
}

void mtsFaultProcessFromSubLayer::SerializeRaw(std::ostream & outputStream) const
{
    BaseType::SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, TargetProcessName);
}

void mtsFaultProcessFromSubLayer::DeSerializeRaw(std::istream & inputStream)
{
    BaseType::DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, TargetProcessName);
}

//
//  Component Layer
//
//-----------------------------------------------------------------------------
//  Component Layer - Functional Integrity - Thread Periodicity
//
CMN_IMPLEMENT_SERVICES(mtsFaultComponentThreadPeriodicity);

mtsFaultComponentThreadPeriodicity::mtsFaultComponentThreadPeriodicity(void) 
    : mtsFaultBase("Thread Periodicity", BaseType::FAULT_COMPONENT_FUNCTIONAL),
      TargetProcessName(""), TargetComponentName("")
{
}

mtsFaultComponentThreadPeriodicity::mtsFaultComponentThreadPeriodicity(
    const std::string & targetProcessName, const std::string & targetComponentName)
    : mtsFaultBase("Thread Periodicity", BaseType::FAULT_COMPONENT_FUNCTIONAL),
      TargetProcessName(targetProcessName), TargetComponentName(targetComponentName)
{
    SetFaultLocation();
}

mtsFaultComponentThreadPeriodicity::~mtsFaultComponentThreadPeriodicity()
{
}

void mtsFaultComponentThreadPeriodicity::SetFaultLocation(void)
{
    this->FaultLocation.Process        = TargetProcessName;
    this->FaultLocation.Component      = TargetComponentName;
    this->FaultLocation.Interface      = "";
    this->FaultLocation.Command        = "";
    this->FaultLocation.Function       = "";
    this->FaultLocation.EventGenerator = "";
    this->FaultLocation.EventHandler   = "";
}

void mtsFaultComponentThreadPeriodicity::ToStream(std::ostream & outputStream) const
{
    BaseType::ToStream(outputStream);
    outputStream << "Process: " << TargetProcessName << ", Component: " << TargetComponentName;
}

void mtsFaultComponentThreadPeriodicity::SerializeRaw(std::ostream & outputStream) const
{
    BaseType::SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, TargetProcessName);
    cmnSerializeRaw(outputStream, TargetComponentName);
}

void mtsFaultComponentThreadPeriodicity::DeSerializeRaw(std::istream & inputStream)
{
    BaseType::DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, TargetProcessName);
    cmnDeSerializeRaw(inputStream, TargetComponentName);
}

//-----------------------------------------------------------------------------
//  Component Layer - Faults From Interface Layer
//
CMN_IMPLEMENT_SERVICES(mtsFaultComponentFromSubLayer);

mtsFaultComponentFromSubLayer::mtsFaultComponentFromSubLayer(void) 
    : mtsFaultBase("Component-Sublayer", BaseType::FAULT_COMPONENT_FROM_INTERFACE_LAYER),
      TargetProcessName(""), TargetComponentName("")
{
}

mtsFaultComponentFromSubLayer::mtsFaultComponentFromSubLayer(
    const std::string & targetProcessName, const std::string & targetComponentName)
    : mtsFaultBase("Component-Sublayer", BaseType::FAULT_COMPONENT_FROM_INTERFACE_LAYER),
      TargetProcessName(targetProcessName), TargetComponentName(targetComponentName)
{
    SetFaultLocation();
}

mtsFaultComponentFromSubLayer::~mtsFaultComponentFromSubLayer()
{
}

void mtsFaultComponentFromSubLayer::SetFaultLocation(void)
{
    this->FaultLocation.Process        = TargetProcessName;
    this->FaultLocation.Component      = TargetComponentName;
    this->FaultLocation.Interface      = "";
    this->FaultLocation.Command        = "";
    this->FaultLocation.Function       = "";
    this->FaultLocation.EventGenerator = "";
    this->FaultLocation.EventHandler   = "";
}

void mtsFaultComponentFromSubLayer::ToStream(std::ostream & outputStream) const
{
    BaseType::ToStream(outputStream);
    outputStream << "Process: " << TargetProcessName << ", Component: " << TargetComponentName;
}

void mtsFaultComponentFromSubLayer::SerializeRaw(std::ostream & outputStream) const
{
    BaseType::SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, TargetProcessName);
    cmnSerializeRaw(outputStream, TargetComponentName);
}

void mtsFaultComponentFromSubLayer::DeSerializeRaw(std::istream & inputStream)
{
    BaseType::DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, TargetProcessName);
    cmnDeSerializeRaw(inputStream, TargetComponentName);
}

//
//  Interface Layer
//
//-----------------------------------------------------------------------------
//  Interface Layer - Faults From Execution Layer
//
CMN_IMPLEMENT_SERVICES(mtsFaultInterfaceFromSubLayer);

mtsFaultInterfaceFromSubLayer::mtsFaultInterfaceFromSubLayer(void) 
    : mtsFaultBase("Interface-Sublayer", BaseType::FAULT_INTERFACE_FROM_EXECUTION_LAYER),
      TargetProcessName(""), TargetComponentName(""), TargetInterfaceName("")
{
}

mtsFaultInterfaceFromSubLayer::mtsFaultInterfaceFromSubLayer(
    const std::string & targetProcessName, 
    const std::string & targetComponentName, 
    const std::string & targetInterfaceName)
    : mtsFaultBase("Component-Sublayer", BaseType::FAULT_COMPONENT_FROM_INTERFACE_LAYER),
      TargetProcessName(targetProcessName), 
      TargetComponentName(targetComponentName), 
      TargetInterfaceName(targetInterfaceName)
{
    SetFaultLocation();
}

mtsFaultInterfaceFromSubLayer::~mtsFaultInterfaceFromSubLayer()
{
}

void mtsFaultInterfaceFromSubLayer::SetFaultLocation(void)
{
    this->FaultLocation.Process        = TargetProcessName;
    this->FaultLocation.Component      = TargetComponentName;
    this->FaultLocation.Interface      = TargetInterfaceName;
    this->FaultLocation.Command        = "";
    this->FaultLocation.Function       = "";
    this->FaultLocation.EventGenerator = "";
    this->FaultLocation.EventHandler   = "";
}

void mtsFaultInterfaceFromSubLayer::ToStream(std::ostream & outputStream) const
{
    BaseType::ToStream(outputStream);
    outputStream << "Process: " << TargetProcessName 
                 << ", Component: " << TargetComponentName
                 << ", Interface: " << TargetInterfaceName;
}

void mtsFaultInterfaceFromSubLayer::SerializeRaw(std::ostream & outputStream) const
{
    BaseType::SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, TargetProcessName);
    cmnSerializeRaw(outputStream, TargetComponentName);
    cmnSerializeRaw(outputStream, TargetInterfaceName);
}

void mtsFaultInterfaceFromSubLayer::DeSerializeRaw(std::istream & inputStream)
{
    BaseType::DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, TargetProcessName);
    cmnDeSerializeRaw(inputStream, TargetComponentName);
    cmnDeSerializeRaw(inputStream, TargetInterfaceName);
}
