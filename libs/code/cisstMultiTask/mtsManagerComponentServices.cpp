/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung, Peter Kazanzides
  Created on: 2010-08-29

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsManagerComponentServices.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsManagerComponentBase.h>

CMN_IMPLEMENT_SERVICES(mtsManagerComponentServices)

// Constructor
mtsManagerComponentServices::mtsManagerComponentServices(mtsInterfaceRequired * internalInterfaceRequired)
    : InternalInterfaceRequired(internalInterfaceRequired)
{}

bool mtsManagerComponentServices::InitializeInterfaceInternalRequired(void)
{
    if (InternalInterfaceRequired) {
        // Dynamic component composition (DCC) services
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::ComponentCreate, 
                                               ServiceComponentManagement.Create);
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::ComponentConnect, 
                                               ServiceComponentManagement.Connect);
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::ComponentDisconnect, 
                                               ServiceComponentManagement.Disconnect);
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::ComponentStart, 
                                               ServiceComponentManagement.Start);
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::ComponentStop, 
                                               ServiceComponentManagement.Stop);
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::ComponentResume, 
                                               ServiceComponentManagement.Resume);
        // Getter services
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::GetNamesOfProcesses, 
                                               ServiceGetters.GetNamesOfProcesses);
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::GetNamesOfComponents, 
                                               ServiceGetters.GetNamesOfComponents);
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::GetNamesOfInterfaces, 
                                               ServiceGetters.GetNamesOfInterfaces);
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::GetListOfConnections, 
                                               ServiceGetters.GetListOfConnections);

        // Event receivers
        InternalInterfaceRequired->AddEventReceiver(mtsManagerComponentBase::EventNames::AddComponent,
                                                    EventReceivers.AddComponent);
        InternalInterfaceRequired->AddEventReceiver(mtsManagerComponentBase::EventNames::AddConnection,
                                                    EventReceivers.AddConnection);
        InternalInterfaceRequired->AddEventReceiver(mtsManagerComponentBase::EventNames::RemoveConnection,
                                                    EventReceivers.RemoveConnection);
        InternalInterfaceRequired->AddEventReceiver(mtsManagerComponentBase::EventNames::ChangeState,
                                                    EventReceivers.ChangeState);
    }

    return (InternalInterfaceRequired != 0);
}

bool mtsManagerComponentServices::RequestComponentCreate(const std::string & className, const std::string & componentName) const
{
    std::string processName = mtsManagerLocal::GetInstance()->GetProcessName();
    return RequestComponentCreate(processName, className, componentName);
}

bool mtsManagerComponentServices::RequestComponentCreate(
    const std::string& processName, const std::string & className, const std::string & componentName) const
{
    if (!ServiceComponentManagement.Create.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "RequestComponentCreate: invalid function - has not been bound to command" << std::endl;
        return false;
    }

    mtsDescriptionComponent arg;
    arg.ProcessName   = processName;
    arg.ClassName     = className;
    arg.ComponentName = componentName;

    // MJ: TODO: change this with blocking command
    ServiceComponentManagement.Create(arg);

    CMN_LOG_CLASS_RUN_VERBOSE << "RequestComponentCreate: requested component creation: " << arg << std::endl;

    return true;
}

bool mtsManagerComponentServices::RequestComponentConnect(
    const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
    const std::string & serverComponentName, const std::string & serverInterfaceProvidedName) const
{
    const std::string thisProcessName = mtsManagerLocal::GetInstance()->GetProcessName();
    return RequestComponentConnect(thisProcessName, clientComponentName, clientInterfaceRequiredName,
                                   thisProcessName, serverComponentName, serverInterfaceProvidedName);
}

bool mtsManagerComponentServices::RequestComponentConnect(
    const std::string & clientProcessName,
    const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
    const std::string & serverProcessName,
    const std::string & serverComponentName, const std::string & serverInterfaceProvidedName) const
{
    if (!ServiceComponentManagement.Connect.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "RequestComponentConnect: invalid function - has not been bound to command" << std::endl;
        return false;
    }

    mtsDescriptionConnection arg;
    arg.Client.ProcessName   = clientProcessName;
    arg.Client.ComponentName = clientComponentName;
    arg.Client.InterfaceName = clientInterfaceRequiredName;
    arg.Server.ProcessName   = serverProcessName;
    arg.Server.ComponentName = serverComponentName;
    arg.Server.InterfaceName = serverInterfaceProvidedName;
    arg.ConnectionID = -1;  // not yet assigned

    // MJ: TODO: change this with blocking command
    ServiceComponentManagement.Connect(arg);

    CMN_LOG_CLASS_RUN_VERBOSE << "RequestComponentConnect: requested component connection: " << arg << std::endl;

    return true;
}

bool mtsManagerComponentServices::RequestComponentDisconnect(
    const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
    const std::string & serverComponentName, const std::string & serverInterfaceProvidedName) const
{
    const std::string thisProcessName = mtsManagerLocal::GetInstance()->GetProcessName();
    return RequestComponentDisconnect(thisProcessName, clientComponentName, clientInterfaceRequiredName,
                                      thisProcessName, serverComponentName, serverInterfaceProvidedName);
}

bool mtsManagerComponentServices::RequestComponentDisconnect(
    const std::string & clientProcessName,
    const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
    const std::string & serverProcessName,
    const std::string & serverComponentName, const std::string & serverInterfaceProvidedName) const
{
    if (!ServiceComponentManagement.Disconnect.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "RequestComponentDisconnect: invalid function - has not been bound to command" << std::endl;
        return false;
    }

    mtsDescriptionConnection arg;
    arg.Client.ProcessName   = clientProcessName;
    arg.Client.ComponentName = clientComponentName;
    arg.Client.InterfaceName = clientInterfaceRequiredName;
    arg.Server.ProcessName   = serverProcessName;
    arg.Server.ComponentName = serverComponentName;
    arg.Server.InterfaceName = serverInterfaceProvidedName;
    arg.ConnectionID = -1;  // not needed

    // MJ: TODO: change this with blocking command
    ServiceComponentManagement.Disconnect(arg);

    CMN_LOG_CLASS_RUN_VERBOSE << "RequestComponentDisconnect: requested component disconnection: " << arg << std::endl;

    return true;
}

bool mtsManagerComponentServices::RequestComponentStart(const std::string & componentName, const double delayInSecond) const
{
    std::string processName = mtsManagerLocal::GetInstance()->GetProcessName();
    return RequestComponentStart(processName, componentName, delayInSecond);
}

bool mtsManagerComponentServices::RequestComponentStart(
    const std::string& processName, const std::string & componentName, const double delayInSecond) const
{
    if (!ServiceComponentManagement.Start.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "RequestComponentStart: invalid function - has not been bound to command" << std::endl;
        return false;
    }

    mtsComponentStatusControl arg;
    arg.ProcessName   = processName;
    arg.ComponentName = componentName;
    arg.DelayInSecond = delayInSecond;
    arg.Command       = mtsComponentStatusControl::COMPONENT_START;

    // MJ: TODO: change this with blocking command
    ServiceComponentManagement.Start(arg);

    CMN_LOG_CLASS_RUN_VERBOSE << "RequestComponentStart: requested component start: " << arg << std::endl;

    return true;
}

bool mtsManagerComponentServices::RequestComponentStop(const std::string & componentName, const double delayInSecond) const
{
    std::string processName = mtsManagerLocal::GetInstance()->GetProcessName();
    return RequestComponentStop(processName, componentName, delayInSecond);
}

bool mtsManagerComponentServices::RequestComponentStop(
    const std::string& processName, const std::string & componentName, const double delayInSecond) const
{
    if (!ServiceComponentManagement.Stop.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "RequestComponentStop: invalid function - has not been bound to command" << std::endl;
        return false;
    }

    mtsComponentStatusControl arg;
    arg.ProcessName   = processName;
    arg.ComponentName = componentName;
    arg.DelayInSecond = delayInSecond;
    arg.Command       = mtsComponentStatusControl::COMPONENT_STOP;

    // MJ: TODO: change this with blocking command
    ServiceComponentManagement.Stop(arg);

    CMN_LOG_CLASS_RUN_VERBOSE << "RequestComponentStop: requested component stop: " << arg << std::endl;

    return true;
}

bool mtsManagerComponentServices::RequestComponentResume(const std::string & componentName, const double delayInSecond) const
{
    std::string processName = mtsManagerLocal::GetInstance()->GetProcessName();
    return RequestComponentResume(processName, componentName, delayInSecond);
}

bool mtsManagerComponentServices::RequestComponentResume(
    const std::string& processName, const std::string & componentName, const double delayInSecond) const
{
    if (!ServiceComponentManagement.Resume.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "RequestComponentResume: invalid function - has not been bound to command" << std::endl;
        return false;
    }

    mtsComponentStatusControl arg;
    arg.ProcessName   = processName;
    arg.ComponentName = componentName;
    arg.DelayInSecond = delayInSecond;
    arg.Command       = mtsComponentStatusControl::COMPONENT_RESUME;

    // MJ: TODO: change this with blocking command
    ServiceComponentManagement.Resume(arg);

    CMN_LOG_CLASS_RUN_VERBOSE << "RequestComponentResume: requested component resume: " << arg << std::endl;

    return true;
}

bool mtsManagerComponentServices::RequestGetNamesOfProcesses(std::vector<std::string> & namesOfProcesses) const
{
    if (!ServiceGetters.GetNamesOfProcesses.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "RequestGetNamesOfProcesses: invalid function - has not been bound to command" << std::endl;
        return false;
    }

    ServiceGetters.GetNamesOfProcesses(namesOfProcesses);

    return true;
}

bool mtsManagerComponentServices::RequestGetNamesOfComponents(
    const std::string & processName, std::vector<std::string> & namesOfComponents) const
{
    if (!ServiceGetters.GetNamesOfComponents.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "RequestGetNamesOfComponents: invalid function - has not been bound to command" << std::endl;
        return false;
    }

    ServiceGetters.GetNamesOfComponents(processName, namesOfComponents);

    return true;
}

bool mtsManagerComponentServices::RequestGetNamesOfInterfaces(
    const std::string & processName, const std::string & componentName,
    std::vector<std::string> & namesOfInterfacesRequired, std::vector<std::string> & namesOfInterfacesProvided) const
{
    if (!ServiceGetters.GetNamesOfInterfaces.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "RequestGetNamesOfInterfaces: invalid function - has not been bound to command" << std::endl;
        return false;
    }

    // input arg
    mtsDescriptionComponent argIn;
    argIn.ProcessName   = processName;
    argIn.ComponentName = componentName;

    // output arg
    mtsDescriptionInterface argOut;

    ServiceGetters.GetNamesOfInterfaces(argIn, argOut);

    namesOfInterfacesRequired = argOut.InterfaceRequiredNames;
    namesOfInterfacesProvided = argOut.InterfaceProvidedNames;

    return true;
}

bool mtsManagerComponentServices::RequestGetListOfConnections(std::vector<mtsDescriptionConnection> & listOfConnections) const
{
    if (!ServiceGetters.GetListOfConnections.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "RequestGetListOfConnections: invalid function - has not been bound to command" << std::endl;
        return false;
    }

    ServiceGetters.GetListOfConnections(listOfConnections);

    return true;
}
