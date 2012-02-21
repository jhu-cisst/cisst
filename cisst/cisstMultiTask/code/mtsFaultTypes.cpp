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

//-----------------------------------------------------------------------------
//  System Layer
//
//  System Layer - Faults From Process Layer
mtsFaultSystemFromSubLayer::mtsFaultSystemFromSubLayer(void) 
    : mtsFaultBase("System-Sublayer", BaseType::FAULT_SYSTEM_FROM_PROCESS_LAYER)
{
    this->FaultLocation.Process        = "";
    this->FaultLocation.Component      = "";
    this->FaultLocation.Interface      = "";
    this->FaultLocation.Command        = "";
    this->FaultLocation.Function       = "";
    this->FaultLocation.EventGenerator = "";
    this->FaultLocation.EventHandler   = "";
}

//-----------------------------------------------------------------------------
//  Process Layer
//
//  Process Layer - Faults From Component Layer
mtsFaultProcessFromSubLayer::mtsFaultProcessFromSubLayer(const std::string & targetProcessName)
    : mtsFaultBase("Process-Sublayer", BaseType::FAULT_PROCESS_FROM_COMPONENT_LAYER)
{
    this->FaultLocation.Process        = targetProcessName;
    this->FaultLocation.Component      = "";
    this->FaultLocation.Interface      = "";
    this->FaultLocation.Command        = "";
    this->FaultLocation.Function       = "";
    this->FaultLocation.EventGenerator = "";
    this->FaultLocation.EventHandler   = "";
}

//-----------------------------------------------------------------------------
//  Component Layer
//
//  Component Layer - Functional Integrity - Thread Periodicity
mtsFaultComponentThreadPeriodicity::mtsFaultComponentThreadPeriodicity(
    const std::string & targetProcessName, const std::string & targetComponentName)
    : mtsFaultBase("Component-Periodicity", BaseType::FAULT_COMPONENT_FUNCTIONAL)
{
    this->FaultLocation.Process        = targetProcessName;
    this->FaultLocation.Component      = targetComponentName;
    this->FaultLocation.Interface      = "";
    this->FaultLocation.Command        = "";
    this->FaultLocation.Function       = "";
    this->FaultLocation.EventGenerator = "";
    this->FaultLocation.EventHandler   = "";
}

//  Component Layer - Faults From Interface Layer
mtsFaultComponentFromSubLayer::mtsFaultComponentFromSubLayer(
    const std::string & targetProcessName, const std::string & targetComponentName)
    : mtsFaultBase("Component-Sublayer", BaseType::FAULT_COMPONENT_FROM_INTERFACE_LAYER)
{
    this->FaultLocation.Process        = targetProcessName;
    this->FaultLocation.Component      = targetComponentName;
    this->FaultLocation.Interface      = "";
    this->FaultLocation.Command        = "";
    this->FaultLocation.Function       = "";
    this->FaultLocation.EventGenerator = "";
    this->FaultLocation.EventHandler   = "";
}

//-----------------------------------------------------------------------------
//  Interface Layer
//
//  Interface Layer - Faults From Execution Layer
mtsFaultInterfaceFromSubLayer::mtsFaultInterfaceFromSubLayer(
    const std::string & targetProcessName, 
    const std::string & targetComponentName, 
    const std::string & targetInterfaceName)
    : mtsFaultBase("Component-Sublayer", BaseType::FAULT_INTERFACE_FROM_EXECUTION_LAYER)
{
    this->FaultLocation.Process        = targetProcessName;
    this->FaultLocation.Component      = targetComponentName;
    this->FaultLocation.Interface      = targetInterfaceName;
    this->FaultLocation.Command        = "";
    this->FaultLocation.Function       = "";
    this->FaultLocation.EventGenerator = "";
    this->FaultLocation.EventHandler   = "";
}
