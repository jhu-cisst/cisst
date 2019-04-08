/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung, Peter Kazanzides
  Created on: 2010-08-29

  (C) Copyright 2010-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaGetTime.h>
#include <cisstMultiTask/mtsInterfaceCommon.h>
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
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::ComponentConfigure,
                                               ServiceComponentManagement.Configure);
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::ComponentConnect,
                                               ServiceComponentManagement.ConnectNew);
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::ComponentDisconnect,
                                               ServiceComponentManagement.DisconnectNew);
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
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::ComponentGetState,
                                               ServiceComponentManagement.GetState);
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::LoadLibrary,
                                               ServiceComponentManagement.LoadLibrary);
        // Log services
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::EnableLogForwarding,
                                               ServiceLogManagement.EnableLogForwarding);
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::DisableLogForwarding,
                                               ServiceLogManagement.DisableLogForwarding);
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::GetLogForwardingStates,
                                               ServiceLogManagement.GetLogForwardingStates);

        // Getter services
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::GetNamesOfProcesses,
                                               ServiceGetters.GetNamesOfProcesses);
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::GetNamesOfComponents,
                                               ServiceGetters.GetNamesOfComponents);
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::GetNamesOfInterfaces,
                                               ServiceGetters.GetNamesOfInterfaces);
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::GetListOfConnections,
                                               ServiceGetters.GetListOfConnections);
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::GetListOfComponentClasses,
                                               ServiceGetters.GetListOfComponentClasses);
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::GetInterfaceProvidedDescription,
                                               ServiceGetters.GetInterfaceProvidedDescription);
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::GetInterfaceRequiredDescription,
                                               ServiceGetters.GetInterfaceRequiredDescription);
        InternalInterfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::GetAbsoluteTimeDiffs,
                                               ServiceGetters.GetAbsoluteTimeDiffs);

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


bool mtsManagerComponentServices::ComponentCreate(const std::string & className, const std::string & componentName) const
{
    std::string processName = mtsManagerLocal::GetInstance()->GetProcessName();
    return ComponentCreate(processName, className, componentName);
}


bool mtsManagerComponentServices::ComponentCreate(const std::string & processName,
                                                  const std::string & className,
                                                  const std::string & componentName) const
{
    mtsDescriptionComponent componentDescription;
    componentDescription.ProcessName   = processName;
    componentDescription.ClassName     = className;
    componentDescription.ComponentName = componentName;

    // call blocking command
    bool result;
    mtsExecutionResult executionResult = ServiceComponentManagement.Create(componentDescription, result);

    // check if command was sent properly
    if (!executionResult.IsOK()) {
        CMN_LOG_CLASS_RUN_ERROR << "ComponentCreate: failed to execute command \"Create\" (error "
                                << executionResult << ")" << std::endl;
        return false;
    }

    if (result == false) {
        CMN_LOG_CLASS_RUN_ERROR << "ComponentCreate: failed to create component: " << componentDescription << std::endl;
        return false;
    }

    CMN_LOG_CLASS_RUN_VERBOSE << "ComponentCreate: successfully created component: " << componentDescription << std::endl;
    return true;
}


bool mtsManagerComponentServices::ComponentCreate(const std::string & className, const mtsGenericObject & constructorArg) const
{
    std::string processName = mtsManagerLocal::GetInstance()->GetProcessName();
    return ComponentCreate(processName, className, constructorArg);
}


bool mtsManagerComponentServices::ComponentCreate(const std::string & processName, const std::string & className,
                                                  const mtsGenericObject & constructorArg) const
{
    if (!ServiceComponentManagement.Create.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "ComponentCreate: invalid function - has not been bound to command" << std::endl;
        return false;
    }

    mtsDescriptionComponent componentDescription;
    componentDescription.ProcessName   = processName;
    componentDescription.ClassName     = className;
    componentDescription.ComponentName = "(serialized)";
    std::stringstream buffer;
    cmnSerializer serializer(buffer);
    serializer.Serialize(constructorArg);
    componentDescription.ConstructorArgSerialized = buffer.str();

    // call blocking command
    bool result;
    mtsExecutionResult executionResult = ServiceComponentManagement.Create(componentDescription, result);

    // check if command was sent properly
    if (!executionResult.IsOK()) {
        CMN_LOG_CLASS_RUN_ERROR << "ComponentCreate: failed to execute command \"Create\" (error "
                                << executionResult << ")" << std::endl;
        return false;
    }

    if (result == false) {
        CMN_LOG_CLASS_RUN_ERROR << "ComponentCreate: failed to create component: " << componentDescription << std::endl;
        return false;
    }

    CMN_LOG_CLASS_RUN_VERBOSE << "ComponentCreate: successfully created component: " << componentDescription << std::endl;
    return true;
}


bool mtsManagerComponentServices::ComponentConfigure(const std::string & componentName,
                                                     const std::string & configString) const
{
    std::string processName = mtsManagerLocal::GetInstance()->GetProcessName();
    return ComponentConfigure(processName, componentName, configString);
}

bool mtsManagerComponentServices::ComponentConfigure(
    const std::string& processName, const std::string & componentName, const std::string & configString) const
{
    if (!ServiceComponentManagement.Configure.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "ComponentConfigure: invalid function - has not been bound to command" << std::endl;
        return false;
    }

    mtsDescriptionComponent arg;
    arg.ProcessName   = processName;
    arg.ComponentName = componentName;
    // For now, use ConstructorArgSerialized
    arg.ConstructorArgSerialized = configString;

    // MJ: TODO: change this with blocking command
    ServiceComponentManagement.Configure(arg);

    return true;
}


bool mtsManagerComponentServices::Connect(const std::string & clientComponentName,
                                          const std::string & clientInterfaceName,
                                          const std::string & serverComponentName,
                                          const std::string & serverInterfaceName) const
{
    const std::string thisProcessName = mtsManagerLocal::GetInstance()->GetProcessName();
    return Connect(thisProcessName, clientComponentName, clientInterfaceName,
                   thisProcessName, serverComponentName, serverInterfaceName);
}


bool mtsManagerComponentServices::Connect(const std::string & clientProcessName,
                                          const std::string & clientComponentName,
                                          const std::string & clientInterfaceName,
                                          const std::string & serverProcessName,
                                          const std::string & serverComponentName,
                                          const std::string & serverInterfaceName) const
{
    mtsDescriptionConnection connectionDescription;
    connectionDescription.Client.ProcessName   = clientProcessName;
    connectionDescription.Client.ComponentName = clientComponentName;
    connectionDescription.Client.InterfaceName = clientInterfaceName;
    connectionDescription.Server.ProcessName   = serverProcessName;
    connectionDescription.Server.ComponentName = serverComponentName;
    connectionDescription.Server.InterfaceName = serverInterfaceName;

    return Connect(connectionDescription);
}


bool mtsManagerComponentServices::Connect(const mtsDescriptionConnection & connectionDescription) const
{
    // Make a copy because the parameter is const
    mtsDescriptionConnection conn(connectionDescription);
    conn.ConnectionID = InvalidConnectionID;

#if CISST_MTS_NEW
    // call blocking command
    bool result;
    mtsExecutionResult executionResult = ServiceComponentManagement.ConnectNew(conn, result);
#else
    mtsExecutionResult executionResult = ServiceComponentManagement.Connect(conn /*, result*/);
#endif

    // check is command was sent properly
    if (!executionResult.IsOK()) {
        CMN_LOG_CLASS_RUN_ERROR << "Connect: failed to execute command \"Connect\" (error "
                                << executionResult << ")" << std::endl;
        return false;
    }
#if CISST_MTS_NEW
    if (result == false) {
        CMN_LOG_CLASS_RUN_ERROR << "Connect: failed to connect: " << connectionDescription << std::endl;
        return false;
    }
#endif
    CMN_LOG_CLASS_RUN_VERBOSE << "Connect: successfully connected: " << connectionDescription << std::endl;
    return true;
}


bool mtsManagerComponentServices::Disconnect(
    const std::string & clientComponentName, const std::string & clientInterfaceName,
    const std::string & serverComponentName, const std::string & serverInterfaceName) const
{
    const std::string thisProcessName = mtsManagerLocal::GetInstance()->GetProcessName();
    return Disconnect(thisProcessName, clientComponentName, clientInterfaceName,
                      thisProcessName, serverComponentName, serverInterfaceName);
}

bool mtsManagerComponentServices::Disconnect(
    const std::string & clientProcessName,
    const std::string & clientComponentName, const std::string & clientInterfaceName,
    const std::string & serverProcessName,
    const std::string & serverComponentName, const std::string & serverInterfaceName) const
{
#if CISST_MTS_NEW
    if (!ServiceComponentManagement.DisconnectNew.IsValid()) {
#else
    if (!ServiceComponentManagement.Disconnect.IsValid()) {
#endif
        CMN_LOG_CLASS_RUN_ERROR << "ComponentDisconnect: invalid function - has not been bound to command" << std::endl;
        return false;
    }

    mtsDescriptionConnection arg;
    arg.Client.ProcessName   = clientProcessName;
    arg.Client.ComponentName = clientComponentName;
    arg.Client.InterfaceName = clientInterfaceName;
    arg.Server.ProcessName   = serverProcessName;
    arg.Server.ComponentName = serverComponentName;
    arg.Server.InterfaceName = serverInterfaceName;
    arg.ConnectionID = InvalidConnectionID;  // not needed

    return Disconnect(arg);
}

bool mtsManagerComponentServices::Disconnect(const mtsDescriptionConnection & connection) const
{
    bool result = true;
#if CISST_MTS_NEW
    ServiceComponentManagement.DisconnectNew(connection, result);
#else
    ServiceComponentManagement.Disconnect(connection);
#endif

    CMN_LOG_CLASS_RUN_VERBOSE << "ComponentDisconnect: requested component disconnection: " << connection << std::endl;

    return result;
}

bool mtsManagerComponentServices::Disconnect(ConnectionIDType connectionID) const
{
    // PK TEMP
    return mtsManagerLocal::GetInstance()->Disconnect(connectionID);
}

bool mtsManagerComponentServices::ComponentStart(const std::string & componentName, const double delayInSecond) const
{
    std::string processName = mtsManagerLocal::GetInstance()->GetProcessName();
    return ComponentStart(processName, componentName, delayInSecond);
}

bool mtsManagerComponentServices::ComponentStart(
    const std::string& processName, const std::string & componentName, const double delayInSecond) const
{
    if (!ServiceComponentManagement.Start.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "ComponentStart: invalid function - has not been bound to command" << std::endl;
        return false;
    }

    mtsComponentStatusControl arg;
    arg.ProcessName   = processName;
    arg.ComponentName = componentName;
    arg.DelayInSecond = delayInSecond;
    arg.Command       = mtsComponentStatusControl::COMPONENT_START;

    // MJ: TODO: change this with blocking command
    ServiceComponentManagement.Start(arg);

    CMN_LOG_CLASS_RUN_VERBOSE << "ComponentStart: requested component start: " << arg << std::endl;

    return true;
}

bool mtsManagerComponentServices::ComponentStop(const std::string & componentName, const double delayInSecond) const
{
    std::string processName = mtsManagerLocal::GetInstance()->GetProcessName();
    return ComponentStop(processName, componentName, delayInSecond);
}

bool mtsManagerComponentServices::ComponentStop(
    const std::string& processName, const std::string & componentName, const double delayInSecond) const
{
    if (!ServiceComponentManagement.Stop.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "ComponentStop: invalid function - has not been bound to command" << std::endl;
        return false;
    }

    mtsComponentStatusControl arg;
    arg.ProcessName   = processName;
    arg.ComponentName = componentName;
    arg.DelayInSecond = delayInSecond;
    arg.Command       = mtsComponentStatusControl::COMPONENT_STOP;

    // MJ: TODO: change this with blocking command
    ServiceComponentManagement.Stop(arg);

    CMN_LOG_CLASS_RUN_VERBOSE << "ComponentStop: requested component stop: " << arg << std::endl;

    return true;
}

bool mtsManagerComponentServices::ComponentResume(const std::string & componentName, const double delayInSecond) const
{
    std::string processName = mtsManagerLocal::GetInstance()->GetProcessName();
    return ComponentResume(processName, componentName, delayInSecond);
}

bool mtsManagerComponentServices::ComponentResume(
    const std::string& processName, const std::string & componentName, const double delayInSecond) const
{
    if (!ServiceComponentManagement.Resume.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "ComponentResume: invalid function - has not been bound to command" << std::endl;
        return false;
    }

    mtsComponentStatusControl arg;
    arg.ProcessName   = processName;
    arg.ComponentName = componentName;
    arg.DelayInSecond = delayInSecond;
    arg.Command       = mtsComponentStatusControl::COMPONENT_RESUME;

    // MJ: TODO: change this with blocking command
    ServiceComponentManagement.Resume(arg);

    CMN_LOG_CLASS_RUN_VERBOSE << "ComponentResume: requested component resume: " << arg << std::endl;

    return true;
}

mtsComponentState mtsManagerComponentServices::ComponentGetState(const mtsDescriptionComponent &component) const
{
    mtsComponentState state;
    if (ServiceComponentManagement.GetState.IsValid())
        ServiceComponentManagement.GetState(component, state);
    else
        CMN_LOG_CLASS_RUN_ERROR << "ComponentGetState: invalid function - has not been bound to command" << std::endl;
    return state;
}

std::string mtsManagerComponentServices::ComponentGetState(const std::string componentName) const
{
    std::string processName = mtsManagerLocal::GetInstance()->GetProcessName();
    return ComponentGetState(processName, componentName);
}

std::string mtsManagerComponentServices::ComponentGetState(const std::string & processName,
                                                           const std::string componentName) const
{
    mtsComponentState state = ComponentGetState(mtsDescriptionComponent(processName, componentName));
    return state.HumanReadable();
}

std::vector<std::string> mtsManagerComponentServices::GetNamesOfProcesses(void) const
{
    std::vector<std::string> namesOfProcesses;
    if (ServiceGetters.GetNamesOfProcesses.IsValid())
        ServiceGetters.GetNamesOfProcesses(namesOfProcesses);
    else
        CMN_LOG_CLASS_RUN_ERROR << "GetNamesOfProcesses: invalid function - has not been bound to command" << std::endl;
    return namesOfProcesses;
}

std::vector<std::string> mtsManagerComponentServices::GetNamesOfComponents(const std::string & processName) const
{
    std::vector<std::string> namesOfComponents;
    if (ServiceGetters.GetNamesOfComponents.IsValid())
        ServiceGetters.GetNamesOfComponents(processName, namesOfComponents);
    else
        CMN_LOG_CLASS_RUN_ERROR << "GetNamesOfComponents: invalid function - has not been bound to command" << std::endl;
    return namesOfComponents;
}

bool mtsManagerComponentServices::GetNamesOfInterfaces(
    const std::string & processName, const std::string & componentName,
    std::vector<std::string> & namesOfInterfacesRequired, std::vector<std::string> & namesOfInterfacesProvided) const
{
    if (!ServiceGetters.GetNamesOfInterfaces.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "GetNamesOfInterfaces: invalid function - has not been bound to command" << std::endl;
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

std::vector<mtsDescriptionConnection> mtsManagerComponentServices::GetListOfConnections(void) const
{
    std::vector<mtsDescriptionConnection> listOfConnections;
    if (ServiceGetters.GetListOfConnections.IsValid())
        ServiceGetters.GetListOfConnections(listOfConnections);
    else
        CMN_LOG_CLASS_RUN_ERROR << "GetListOfConnections: invalid function - has not been bound to command" << std::endl;
    return listOfConnections;
}

std::vector<mtsDescriptionComponentClass> mtsManagerComponentServices::GetListOfComponentClasses(void) const
{
    return GetListOfComponentClasses(mtsManagerLocal::GetInstance()->GetProcessName());
}

std::vector<mtsDescriptionComponentClass> mtsManagerComponentServices::GetListOfComponentClasses(const std::string &processName) const
{
    std::vector<mtsDescriptionComponentClass> listOfComponentClasses;
    if (ServiceGetters.GetListOfComponentClasses.IsValid())
        ServiceGetters.GetListOfComponentClasses(processName, listOfComponentClasses);
    else
        CMN_LOG_CLASS_RUN_ERROR << "GetListOfComponentClasses: invalid function - has not been bound to command" << std::endl;
    return listOfComponentClasses;
}

mtsInterfaceProvidedDescription
mtsManagerComponentServices::GetInterfaceProvidedDescription(const std::string & processName,
                                                             const std::string & componentName, const std::string & interfaceName) const
{
    // output arg
    mtsInterfaceProvidedDescription argOut;
    argOut.InterfaceName = "ERROR";

    if (!ServiceGetters.GetInterfaceProvidedDescription.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "GetInterfaceProvidedDescription: invalid function - has not been bound to command" << std::endl;
        return argOut;
    }

    // input arg
    // For now, use mtsDescriptionInterface to pass the request. Probably should create a separate class that consists only
    // of the three required fields.
    mtsDescriptionInterface argIn;
    argIn.ProcessName = processName;
    argIn.ComponentName = componentName;
    argIn.InterfaceProvidedNames.push_back(interfaceName);

    ServiceGetters.GetInterfaceProvidedDescription(argIn, argOut);

    return argOut;
}

mtsInterfaceRequiredDescription
mtsManagerComponentServices::GetInterfaceRequiredDescription(const std::string & processName,
                                                             const std::string & componentName, const std::string & interfaceName) const
{
    // output arg
    mtsInterfaceRequiredDescription argOut;
    argOut.InterfaceName = "ERROR";

    if (!ServiceGetters.GetInterfaceRequiredDescription.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "GetInterfaceRequiredDescription: invalid function - has not been bound to command" << std::endl;
        return argOut;
    }

    // input arg
    // For now, use mtsDescriptionInterface to pass the request. Probably should create a separate class that consists only
    // of the three required fields.
    mtsDescriptionInterface argIn;
    argIn.ProcessName = processName;
    argIn.ComponentName = componentName;
    argIn.InterfaceRequiredNames.push_back(interfaceName);

    ServiceGetters.GetInterfaceRequiredDescription(argIn, argOut);

    return argOut;
}

bool mtsManagerComponentServices::Load(const std::string & fileName) const
{
    return Load(mtsManagerLocal::GetInstance()->GetProcessName(), fileName);
}

bool mtsManagerComponentServices::Load(const std::string & processName, const std::string & fileName) const
{
    mtsDescriptionLoadLibrary argIn(processName, fileName);
    bool result = false;
    ServiceComponentManagement.LoadLibrary(argIn, result);
    return result;
}

void mtsManagerComponentServices::EnableLogForwarding(void)
{
    EnableLogForwarding(GetNamesOfProcesses());
}

void mtsManagerComponentServices::EnableLogForwarding(const std::vector<std::string> &processNames)
{
    ServiceLogManagement.EnableLogForwarding(processNames);
}

void mtsManagerComponentServices::DisableLogForwarding(void)
{
    DisableLogForwarding(GetNamesOfProcesses());
}

void mtsManagerComponentServices::DisableLogForwarding(const std::vector<std::string> &processNames)
{
    ServiceLogManagement.DisableLogForwarding(processNames);
}

void mtsManagerComponentServices::GetLogForwardingStates(stdCharVec & states) const
{
    ServiceLogManagement.GetLogForwardingStates(GetNamesOfProcesses(), states);
}

void mtsManagerComponentServices::GetLogForwardingStates(const stdStringVec & processNames, stdCharVec & states) const
{
    ServiceLogManagement.GetLogForwardingStates(processNames, states);
}

std::vector<double> mtsManagerComponentServices::GetAbsoluteTimeDiffs(const std::vector<std::string> &processNames) const
{
    std::vector<double> processTimes(processNames.size());
    ServiceGetters.GetAbsoluteTimeDiffs(processNames, processTimes);
    return processTimes;
}

bool mtsManagerComponentServices::CheckAndWait(const std::vector<std::string> &list, const std::string &key, double &timeoutInSec,
                                               mtsEventReceiverWrite &eventReceiver)
{
    std::vector<std::string>::const_iterator it = std::find(list.begin(), list.end(), key);
    if (it == list.end()) {
        if (timeoutInSec == 0.0)
            return false;
        double curTime = osaGetTime();
        if (timeoutInSec < 0.0) {
            // Timeout less than 0 means wait indefinitely. Internally, however, we wait with a timeout
            // just in case we miss the event (e.g., due to a race condition).
            CMN_LOG_CLASS_RUN_VERBOSE << "Waiting for " << key << std::endl;
            eventReceiver.WaitWithTimeout(3.0);  // still poll 3 seconds
        }
        else {
            CMN_LOG_CLASS_RUN_VERBOSE << "Waiting for " << key << ", timeout = " << timeoutInSec << std::endl;
            eventReceiver.WaitWithTimeout(timeoutInSec);
            timeoutInSec -= (osaGetTime() - curTime);
            if (timeoutInSec < 0.0)
                timeoutInSec = 0.0;
        }
        return false;
    }
    return true;
}

bool mtsManagerComponentServices::WaitFor(const std::string & processName, double timeoutInSec)
{
    bool found = false;
    while (!found) {
        std::vector<std::string> processList = GetNamesOfProcesses();
        found = CheckAndWait(processList, processName, timeoutInSec, EventReceivers.AddComponent);
        if (!found && (timeoutInSec == 0.0)) {
            processList = GetNamesOfProcesses();
            found = CheckAndWait(processList, processName, timeoutInSec, EventReceivers.AddComponent);
            break;
        }
    }
    return found;
}

bool mtsManagerComponentServices::WaitFor(const std::string & processName, const std::string & componentName,
                                          double timeoutInSec)
{
    bool found = WaitFor(processName, timeoutInSec);
    if (found) {
        found = false;
        while (!found) {
            std::vector<std::string> componentList = GetNamesOfComponents(processName);
            found = CheckAndWait(componentList, componentName, timeoutInSec, EventReceivers.AddComponent);
            if (!found && (timeoutInSec == 0.0)) {
                componentList = GetNamesOfComponents(processName);
                found = CheckAndWait(componentList, componentName, timeoutInSec, EventReceivers.AddComponent);
                break;
            }
        }
    }
    return found;
}

bool mtsManagerComponentServices::WaitFor(const std::string & processName, const std::string & componentName,
                                          const std::string & componentState, double timeoutInSec)
{
    bool found = WaitFor(processName, componentName, timeoutInSec);
    if (found) {
        found = false;
        while (!found) {
            std::vector<std::string> stateList(1);
            stateList.push_back(ComponentGetState(processName, componentName));
            found = CheckAndWait(stateList, componentState, timeoutInSec, EventReceivers.ChangeState);
            if (!found && (timeoutInSec == 0.0)) {
                stateList.clear();
                stateList.push_back(ComponentGetState(processName, componentName));
                found = CheckAndWait(stateList, componentState, timeoutInSec, EventReceivers.ChangeState);
                break;
            }
        }
    }
    return found;
}
