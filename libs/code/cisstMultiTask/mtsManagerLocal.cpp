/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2009-12-07

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsManagerLocal.h>

#include <cisstCommon/cmnThrow.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstOSAbstraction/osaGetTime.h>
#include <cisstOSAbstraction/osaSocket.h>

#include <cisstMultiTask/mtsConfig.h>
#include <cisstMultiTask/mtsManagerGlobal.h>
#include <cisstMultiTask/mtsTaskContinuous.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsTaskFromCallback.h>
#include <cisstMultiTask/mtsTaskFromSignal.h>

#if CISST_MTS_HAS_ICE
#include <cisstMultiTask/mtsComponentProxy.h>
#include <cisstMultiTask/mtsManagerProxyClient.h>
#include <cisstMultiTask/mtsManagerProxyServer.h>
#endif

CMN_IMPLEMENT_SERVICES(mtsManagerLocal);
CMN_IMPLEMENT_SERVICES(mtsManagerLocalInterface);

// Typedef to use 'component' instead of 'device'
typedef mtsDevice mtsComponent;

// Static variable definition
mtsManagerLocal * mtsManagerLocal::Instance;
bool mtsManagerLocal::UnitTestEnabled = false;
bool mtsManagerLocal::UnitTestNetworkProxyEnabled = false;

#define DEFAULT_PROCESS_NAME "LCM"

mtsManagerLocal::mtsManagerLocal(void)
#if 0
    , JGraphSocket(osaSocket::TCP)
#endif
{
    Initialize();

    if (!UnitTestEnabled) {
        TimeServer.SetTimeOrigin();
    }

#if 0
    JGraphSocketConnected = false;

    // Try to connect to the JGraph application software (Java program).
    // Note that the JGraph application also sends event messages back via the socket,
    // though we don't currently read them. To do this, it would be best to implement
    // the TaskManager as a periodic task.
    JGraphSocketConnected = JGraphSocket.Connect("127.0.0.1", 4444);
    if (JGraphSocketConnected) {
        osaSleep(1.0 * cmn_s);  // need to wait or JGraph server will not start properly
    } else {
        CMN_LOG_CLASS_INIT_WARNING << "Failed to connect to JGraph server" << std::endl;
    }
#endif

    // In standalone mode, process name is set as DEFAULT_PROCESS_NAME by
    // default since there is only one instance of local task manager.
    ProcessName = DEFAULT_PROCESS_NAME;

    CMN_LOG_CLASS_INIT_VERBOSE << "Local component manager: STANDALONE mode" << std::endl;

    // In standalone mode, the global component manager is an instance of
    // mtsManagerGlobal that runs in the same process in which this local
    // component manager runs.
    mtsManagerGlobal * globalManager = new mtsManagerGlobal;

    // Register process name to the global component manager
    if (!globalManager->AddProcess(ProcessName)) {
        cmnThrow(std::runtime_error("Failed to register process name to the global component manager"));
    }

    // Register process object to the global component manager
    if (!globalManager->AddProcessObject(this)) {
        cmnThrow(std::runtime_error("Failed to register process object to the global component manager"));
    }

    ManagerGlobal = globalManager;
}

#if CISST_MTS_HAS_ICE
mtsManagerLocal::mtsManagerLocal(const std::string & globalComponentManagerIP,
                                 const std::string & thisProcessName,
                                 const std::string & thisProcessIP)
                                 : ProcessName(thisProcessName),
                                   GlobalComponentManagerIP(globalComponentManagerIP),
                                   ProcessIP(thisProcessIP)
{
    Initialize();

    if (!UnitTestEnabled) {
        TimeServer.SetTimeOrigin();
    }

#if 0
    JGraphSocketConnected = false;

    // Try to connect to the JGraph application software (Java program).
    // Note that the JGraph application also sends event messages back via the socket,
    // though we don't currently read them. To do this, it would be best to implement
    // the TaskManager as a periodic task.
    JGraphSocketConnected = JGraphSocket.Connect("127.0.0.1", 4444);
    if (JGraphSocketConnected) {
        osaSleep(1.0 * cmn_s);  // need to wait or JGraph server will not start properly
    } else {
        CMN_LOG_CLASS_INIT_WARNING << "Failed to connect to JGraph server" << std::endl;
    }
#endif
    // If process ip is not specified (""), the first ip address detected is used as this process ip
    if (ProcessIP == "") {
        std::vector<std::string> ipAddresses;
        osaSocket::GetLocalhostIP(ipAddresses);
        ProcessIP = ipAddresses[0];
    }

    // If process name is not specified (""), the ip address is used as this process name.
    if (ProcessName == "") {
        ProcessName = ProcessIP;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "Local component manager     : NETWORK mode" << std::endl;
    CMN_LOG_CLASS_INIT_VERBOSE << "Global component manager IP : " << GlobalComponentManagerIP << std::endl;
    CMN_LOG_CLASS_INIT_VERBOSE << "This process name           : " << ProcessName << std::endl;
    CMN_LOG_CLASS_INIT_VERBOSE << "This process IP             : " << ProcessIP << std::endl;

    // Generate an endpoint string to connect to the global component manager
    std::stringstream ss;
    ss << mtsManagerProxyServer::GetManagerCommunicatorID()
       << ":default -h " << GlobalComponentManagerIP
       << " -p " << mtsManagerProxyServer::GetGCMPortNumberAsString();

    // In network mode, the gobal component manager is a network (ICE) proxy
    // client of type mtsManagerProxyClient which transfers data to/from the
    // GCM across network.
    mtsManagerProxyClient * globalComponentManagerProxy = new mtsManagerProxyClient(ss.str());

    // Run and connect to the global component manager
    if (!globalComponentManagerProxy->Start(this)) {
        cmnThrow(std::runtime_error("Failed to initialize global component manager proxy"));
    }

    // Register process name to the global component manager.
    if (!globalComponentManagerProxy->AddProcess(ProcessName)) {
        cmnThrow(std::runtime_error("Failed to register process name to the global component manager"));
    }

    // In network mode, a process object doesn't need to be registered
    // to the global component manager.

    ManagerGlobal = globalComponentManagerProxy;
}
#endif

mtsManagerLocal::~mtsManagerLocal()
{
    // If ManagerGlobal is not NULL, it means Cleanup() has not been called
    // before. Thus, it needs to be called here for safe and clean termination.
    if (ManagerGlobal) {
        Cleanup();
    }
}

void mtsManagerLocal::Initialize(void)
{
    __os_init();
    ComponentMap.SetOwner(*this);
}

void mtsManagerLocal::Cleanup(void)
{
    //JGraphSocket.Close();
    //JGraphSocketConnected = false;

    if (ManagerGlobal) {
        delete ManagerGlobal;
        ManagerGlobal = NULL;
    }

    ComponentMap.DeleteAll();

    __os_exit();
}

#if !CISST_MTS_HAS_ICE
mtsManagerLocal * mtsManagerLocal::GetInstance(void)
{
    if (!Instance) {
        Instance = new mtsManagerLocal;
    }

    return Instance;
}
#else
mtsManagerLocal * mtsManagerLocal::GetInstance(const std::string & globalComponentManagerIP,
                                               const std::string & thisProcessName,
                                               const std::string & thisProcessIP)
{
    if (!Instance) {
        // If no argument is specified, this local component manager runs in standalone mode.
        if (globalComponentManagerIP == "" && thisProcessName == "" && thisProcessIP == "") {
            Instance = new mtsManagerLocal;
            CMN_LOG_INIT_WARNING << "WARNING: Inter-process communication support is disabled" << std::endl;
        } else {
            Instance = new mtsManagerLocal(globalComponentManagerIP, thisProcessName, thisProcessIP);
        }
    }

    return Instance;
}
#endif

bool mtsManagerLocal::AddComponent(mtsComponent * component)
{
    if (!component) {
        CMN_LOG_CLASS_RUN_ERROR << "AddComponent: invalid component" << std::endl;
        return false;
    }

    const std::string componentName = component->GetName();

    // Try to register new component to the global component manager first.
    if (!ManagerGlobal->AddComponent(ProcessName, componentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddComponent: failed to add component: " << componentName << std::endl;
        return false;
    }

    // Register all the existing required interfaces and provided interfaces to
    // the global component manager and mart them as registered.
    if (!RegisterInterfaces(component)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddComponent: failed to register interfaces" << std::endl;
        return false;
    }

    CMN_LOG_CLASS_RUN_VERBOSE << "AddComponent: "
        << "successfully added component to the global component manager: " << componentName << std::endl;

    bool success;
    ComponentMapChange.Lock();
    success = ComponentMap.AddItem(componentName, component);
    ComponentMapChange.Unlock();

    if (!success) {
        CMN_LOG_CLASS_RUN_ERROR << "AddComponent: "
            << "failed to add component to local component manager: " << componentName << std::endl;
        return false;
    }

    CMN_LOG_CLASS_RUN_VERBOSE << "AddComponent: "
        << "successfully added component to local component manager: " << componentName << std::endl;

    return true;
}

bool CISST_DEPRECATED mtsManagerLocal::AddTask(mtsTask * component)
{
    return AddComponent(component);
}

bool CISST_DEPRECATED mtsManagerLocal::AddDevice(mtsDevice * component)
{
    return AddComponent(component);
}

bool mtsManagerLocal::RemoveComponent(mtsComponent * component)
{
    if (!component) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveComponent: invalid argument" << std::endl;
        return false;
    }

    return RemoveComponent(component->GetName());
}

bool mtsManagerLocal::RemoveComponent(const std::string & componentName)
{
    // Notify the global component manager of the removal of this component

    if (!ManagerGlobal->RemoveComponent(ProcessName, componentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveComponent: failed to remove component at global component manager: " << componentName << std::endl;
        return false;
    }

    // Get a component to be removed
    mtsComponent * component = ComponentMap.GetItem(componentName);
    if (!component) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveComponent: failed to get component to be removed: " << componentName << std::endl;
        return false;
    }

    bool success;
    ComponentMapChange.Lock();
    success = ComponentMap.RemoveItem(componentName);
    ComponentMapChange.Unlock();

    if (!success) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveComponent: failed to removed component: " << componentName << std::endl;
        return false;
    }
    else {
        delete component;

        CMN_LOG_CLASS_RUN_VERBOSE << "RemoveComponent: removed component: " << componentName << std::endl;
    }

    return true;
}

std::vector<std::string> mtsManagerLocal::GetNamesOfComponents(void) const
{
    return ComponentMap.GetNames();
}

std::vector<std::string> CISST_DEPRECATED mtsManagerLocal::GetNamesOfTasks(void) const
{
    mtsComponent * component;
    std::vector<std::string> namesOfTasks;

    ComponentMapType::const_iterator it = ComponentMap.begin();
    const ComponentMapType::const_iterator itEnd = ComponentMap.end();
    for (; it != itEnd; ++it) {
        component = dynamic_cast<mtsTask*>(it->second);
        if (component) {
            namesOfTasks.push_back(it->first);
        }
    }

    return namesOfTasks;
}

std::vector<std::string> CISST_DEPRECATED mtsManagerLocal::GetNamesOfDevices(void) const
{
    mtsDevice * component;
    std::vector<std::string> namesOfDevices;

    ComponentMapType::const_iterator it = ComponentMap.begin();
    const ComponentMapType::const_iterator itEnd = ComponentMap.end();
    for (; it != itEnd; ++it) {
        component = dynamic_cast<mtsTask*>(it->second);
        if (!component) {
            namesOfDevices.push_back(it->first);
        }
    }

    return namesOfDevices;
}

void mtsManagerLocal::GetNamesOfComponents(std::vector<std::string> & namesOfComponents) const
{
    ComponentMap.GetNames(namesOfComponents);
}

void CISST_DEPRECATED mtsManagerLocal::GetNamesOfDevices(std::vector<std::string>& namesOfDevices) const
{
    mtsDevice * component;

    ComponentMapType::const_iterator it = ComponentMap.begin();
    const ComponentMapType::const_iterator itEnd = ComponentMap.end();
    for (; it != itEnd; ++it) {
        component = dynamic_cast<mtsTask*>(it->second);
        if (!component) {
            namesOfDevices.push_back(it->first);
        }
    }
}

void CISST_DEPRECATED mtsManagerLocal::GetNamesOfTasks(std::vector<std::string>& namesOfTasks) const
{
    mtsDevice * component;

    ComponentMapType::const_iterator it = ComponentMap.begin();
    const ComponentMapType::const_iterator itEnd = ComponentMap.end();
    for (; it != itEnd; ++it) {
        component = dynamic_cast<mtsTask*>(it->second);
        if (component) {
            namesOfTasks.push_back(it->first);
        }
    }
}

#if CISST_MTS_HAS_ICE
void mtsManagerLocal::GetNamesOfCommands(std::vector<std::string>& namesOfCommands,
                                         const std::string & componentName, 
                                         const std::string & providedInterfaceName,
                                         const std::string & CMN_UNUSED(listenerID))
{
    ProvidedInterfaceDescription desc;
    if (!GetProvidedInterfaceDescription(componentName, providedInterfaceName, desc)) {
        return;
    }

    std::string name;
    for (unsigned int i = 0; i < desc.CommandsVoid.size(); ++i) {
        name = "V) ";
        name += desc.CommandsVoid[i].Name;
        namesOfCommands.push_back(name);
    }
    for (unsigned int i = 0; i < desc.CommandsWrite.size(); ++i) {
        name = "W) ";
        name += desc.CommandsWrite[i].Name;
        namesOfCommands.push_back(name);
    }
    for (unsigned int i = 0; i < desc.CommandsRead.size(); ++i) {
        name = "R) ";
        name += desc.CommandsRead[i].Name;
        namesOfCommands.push_back(name);
    }
    for (unsigned int i = 0; i < desc.CommandsQualifiedRead.size(); ++i) {
        name = "Q) ";
        name += desc.CommandsQualifiedRead[i].Name;
        namesOfCommands.push_back(name);
    }
}

void mtsManagerLocal::GetNamesOfEventGenerators(std::vector<std::string>& namesOfEventGenerators,
                                                const std::string & componentName, 
                                                const std::string & providedInterfaceName,
                                                const std::string & CMN_UNUSED(listenerID))
{
    ProvidedInterfaceDescription desc;
    if (!GetProvidedInterfaceDescription(componentName, providedInterfaceName, desc)) {
        return;
    }

    std::string name;
    for (unsigned int i = 0; i < desc.EventsVoid.size(); ++i) {
        name = "V) ";
        name += desc.EventsVoid[i].Name;
        namesOfEventGenerators.push_back(name);
    }
    for (unsigned int i = 0; i < desc.EventsWrite.size(); ++i) {
        name = "W) ";
        name += desc.EventsWrite[i].Name;
        namesOfEventGenerators.push_back(name);
    }
}

void mtsManagerLocal::GetNamesOfFunctions(std::vector<std::string>& namesOfFunctions,
                                          const std::string & componentName, 
                                          const std::string & requiredInterfaceName,
                                          const std::string & CMN_UNUSED(listenerID))
{
    RequiredInterfaceDescription desc;
    if (!GetRequiredInterfaceDescription(componentName, requiredInterfaceName, desc)) {
        return;
    }

    std::string name;
    for (unsigned int i = 0; i < desc.FunctionVoidNames.size(); ++i) {
        name = "V) ";
        name += desc.FunctionVoidNames[i];
        namesOfFunctions.push_back(name);
    }
    for (unsigned int i = 0; i < desc.FunctionWriteNames.size(); ++i) {
        name = "W) ";
        name += desc.FunctionWriteNames[i];
        namesOfFunctions.push_back(name);
    }
    for (unsigned int i = 0; i < desc.FunctionReadNames.size(); ++i) {
        name = "R) ";
        name += desc.FunctionReadNames[i];
        namesOfFunctions.push_back(name);
    }
    for (unsigned int i = 0; i < desc.FunctionQualifiedReadNames.size(); ++i) {
        name = "Q) ";
        name += desc.FunctionQualifiedReadNames[i];
        namesOfFunctions.push_back(name);
    }
}

void mtsManagerLocal::GetNamesOfEventHandlers(std::vector<std::string>& namesOfEventHandlers,
                                              const std::string & componentName, 
                                              const std::string & requiredInterfaceName,
                                              const std::string & CMN_UNUSED(listenerID))
{
    RequiredInterfaceDescription desc;
    if (!GetRequiredInterfaceDescription(componentName, requiredInterfaceName, desc)) {
        return;
    }

    std::string name;
    for (unsigned int i = 0; i < desc.EventHandlersVoid.size(); ++i) {
        name = "V) ";
        name += desc.EventHandlersVoid[i].Name;
        namesOfEventHandlers.push_back(name);
    }
    for (unsigned int i = 0; i < desc.EventHandlersWrite.size(); ++i) {
        name = "W) ";
        name += desc.EventHandlersWrite[i].Name;
        namesOfEventHandlers.push_back(name);
    }
}

void mtsManagerLocal::GetDescriptionOfCommand(std::string & description,
                                              const std::string & componentName, 
                                              const std::string & providedInterfaceName, 
                                              const std::string & commandName,
                                              const std::string & CMN_UNUSED(listenerID))
{
    mtsComponent * component = GetComponent(componentName);
    if (!component) return;

    mtsProvidedInterface * providedInterface = component->GetProvidedInterface(providedInterfaceName);
    if (!providedInterface) return;

    // Get command type
    char commandType = *commandName.c_str();
    std::string actualCommandName = commandName.substr(3, commandName.size() - 2);

    description = "Argument type: ";
    switch (commandType) {
        case 'V':
            {
                mtsCommandVoidBase * command = providedInterface->GetCommandVoidMap().GetItem(actualCommandName);
                if (!command) {
                    description = "No void command found";
                    return;
                }
                description += "(none)";
            }
            break;
        case 'W':
            {
                mtsCommandWriteBase * command = providedInterface->GetCommandWriteMap().GetItem(actualCommandName);
                if (!command) {
                    description = "No write command found";
                    return;
                }
                description += command->GetArgumentClassServices()->GetName();
            }
            break;
        case 'R':
            {
                mtsCommandReadBase * command = providedInterface->GetCommandRead(actualCommandName);
                if (!command) {
                    description = "No read command found";
                    return;
                }
                description += command->GetArgumentClassServices()->GetName();
            }
            break;
        case 'Q':
            {
                mtsCommandQualifiedReadBase * command = providedInterface->GetCommandQualifiedRead(actualCommandName);
                if (!command) {
                    description = "No qualified read command found";
                    return;
                }
                description = "Argument1 type: ";
                description += command->GetArgument1ClassServices()->GetName();
                description += "\nArgument2 type: ";
                description += command->GetArgument2ClassServices()->GetName();
            }
            break;
        default:
            description = "Failed to get command description";
            return;
    }
}

void mtsManagerLocal::GetDescriptionOfEventGenerator(std::string & description,
                                                     const std::string & componentName, 
                                                     const std::string & providedInterfaceName, 
                                                     const std::string & eventGeneratorName,
                                                     const std::string & CMN_UNUSED(listenerID))
{
    mtsComponent * component = GetComponent(componentName);
    if (!component) return;

    mtsProvidedInterface * providedInterface = component->GetProvidedInterface(providedInterfaceName);
    if (!providedInterface) return;

    // Get event generator type
    char eventGeneratorType = *eventGeneratorName.c_str();
    std::string actualEventGeneratorName = eventGeneratorName.substr(3, eventGeneratorName.size() - 2);

    description = "Argument type: ";
    switch (eventGeneratorType) {
        case 'V':
            {
                mtsCommandVoidBase * eventGenerator = providedInterface->EventVoidGenerators.GetItem(actualEventGeneratorName);
                if (!eventGenerator) {
                    description = "No void event generator found";
                    return;
                }
                description += "(none)";
            }
            break;
        case 'W':
            {
                mtsCommandWriteBase * eventGenerator = providedInterface->EventWriteGenerators.GetItem(actualEventGeneratorName);
                if (!eventGenerator) {
                    description = "No write event generator found";
                    return;
                }
                description += eventGenerator->GetArgumentClassServices()->GetName();
            }
            break;
        default:
            description = "Failed to get event generator description";
            return;
    }
}

void mtsManagerLocal::GetDescriptionOfFunction(std::string & description,
                                               const std::string & componentName, 
                                               const std::string & requiredInterfaceName, 
                                               const std::string & functionName,
                                               const std::string & CMN_UNUSED(listenerID))
{
    mtsComponent * component = GetComponent(componentName);
    if (!component) return;

    mtsRequiredInterface * requiredInterface = component->GetRequiredInterface(requiredInterfaceName);
    if (!requiredInterface) return;

    // Get function type
    char functionType = *functionName.c_str();
    std::string actualFunctionName = functionName.substr(3, functionName.size() - 2);

    description = "Resource argument type: ";
    switch (functionType) {
        case 'V':
            {
                mtsRequiredInterface::CommandInfo<mtsCommandVoidBase> * function = requiredInterface->CommandPointersVoid.GetItem(actualFunctionName);
                if (!function) {
                    description = "No void function found";
                    return;
                }
                description += "(none)";
            }
            break;
        case 'W':
            {
                mtsRequiredInterface::CommandInfo<mtsCommandWriteBase> * function = requiredInterface->CommandPointersWrite.GetItem(actualFunctionName);
                if (!function) {
                    description = "No write function found";
                    return;
                }
                if (*function->CommandPointer) {
                    description += (*function->CommandPointer)->GetArgumentClassServices()->GetName();
                } else {
                    description += "(unbounded write function)";
                }
            }
            break;
        case 'R':
            {
                mtsRequiredInterface::CommandInfo<mtsCommandReadBase> * function = requiredInterface->CommandPointersRead.GetItem(actualFunctionName);
                if (!function) {
                    description = "No read function found";
                    return;
                }
                if (*function->CommandPointer) {
                    description += (*function->CommandPointer)->GetArgumentClassServices()->GetName();
                } else {
                    description += "(unbounded read function)";
                }
            }
            break;
        case 'Q':
            {
                mtsRequiredInterface::CommandInfo<mtsCommandQualifiedReadBase> * function = requiredInterface->CommandPointersQualifiedRead.GetItem(actualFunctionName);
                if (!function) {
                    description = "No qualified read function found";
                    return;
                }
                if (*function->CommandPointer) {
                    description = "Resource argument1 type: ";
                    description += (*function->CommandPointer)->GetArgument1ClassServices()->GetName();
                    description += "\nResource argument2 type: ";
                    description += (*function->CommandPointer)->GetArgument2ClassServices()->GetName();
                } else {
                    description = "Resource argument1 type: ";
                    description += "(unbounded qualified read function)";
                    description += "\nResource argument2 type: ";
                    description += "(unbounded qualified read function)";
                }
                
            }
            break;
        default:
            description = "Failed to get function description";
            return;
    }
}

void mtsManagerLocal::GetDescriptionOfEventHandler(std::string & description,
                                                   const std::string & componentName, 
                                                   const std::string & requiredInterfaceName, 
                                                   const std::string & eventHandlerName,
                                                   const std::string & CMN_UNUSED(listenerID))
{
    mtsComponent * component = GetComponent(componentName);
    if (!component) return;

    mtsRequiredInterface * requiredInterface = component->GetRequiredInterface(requiredInterfaceName);
    if (!requiredInterface) return;

    // Get event handler type
    char eventHandlerType = *eventHandlerName.c_str();
    std::string actualEventHandlerName = eventHandlerName.substr(3, eventHandlerName.size() - 2);

    description = "Argument type: ";
    switch (eventHandlerType) {
        case 'V':
            {
                mtsCommandVoidBase * command = requiredInterface->EventHandlersVoid.GetItem(actualEventHandlerName);
                if (!command) {
                    description = "No void event handler found";
                    return;
                }
                description += "(none)";
            }
            break;
        case 'W':
            {
                mtsCommandWriteBase * command = requiredInterface->EventHandlersWrite.GetItem(actualEventHandlerName);
                if (!command) {
                    description = "No write event handler found";
                    return;
                }
                description += command->GetArgumentClassServices()->GetName();
            }
            break;
        default:
            description = "Failed to get event handler description";
            return;
    }
}

void mtsManagerLocal::GetArgumentInformation(std::string & argumentName,
                                             std::vector<std::string> & signalNames,
                                             const std::string & componentName, 
                                             const std::string & providedInterfaceName, 
                                             const std::string & commandName,
                                             const std::string & CMN_UNUSED(listenerID))
{
    mtsComponent * component = GetComponent(componentName);
    if (!component) return;

    mtsProvidedInterface * providedInterface = component->GetProvidedInterface(providedInterfaceName);
    if (!providedInterface) return;

    // Get argument name
    mtsCommandReadBase * command;
    char commandType = *commandName.c_str();
    std::string actualCommandName = commandName.substr(3, commandName.size() - 2);

    switch (commandType) {
        case 'V':
            argumentName = "Cannot visualize void command";
            return;
        case 'W':
            argumentName = "Cannot visualize write command";
            return;
        case 'Q':
            argumentName = "Cannot visualize q.read command";
            return;
        case 'R':
            command = providedInterface->GetCommandRead(actualCommandName);
            if (!command) {
                argumentName = "No read command found";
                return;
            }
            argumentName = command->GetArgumentClassServices()->GetName();
            break;
        default:
            argumentName = "Failed to get argument information";
            return;
    }

    // Get argument prototype
    const mtsGenericObject * argument = command->GetArgumentPrototype();
    if (!argument) {
        argumentName = "Failed to get argument";
        return;
    }

    // Get signal information
    const int signalCount = argument->GetNumberOfScalar();
    for (unsigned int i = 0; i < argument->GetNumberOfScalar(); ++i) {
        signalNames.push_back(argument->GetScalarName(i));
    }
}

void mtsManagerLocal::GetValuesOfCommand(SetOfValues & values,
                                         const std::string & componentName,
                                         const std::string & providedInterfaceName, 
                                         const std::string & commandName,
                                         const std::string & CMN_UNUSED(listenerID))
{
    mtsComponent * component = GetComponent(componentName);
    if (!component) return;

    mtsProvidedInterface * providedInterface = component->GetProvidedInterface(providedInterfaceName);
    if (!providedInterface) return;

    // Get argument name
    char commandType = *commandName.c_str();
    std::string actualCommandName = commandName.substr(3, commandName.size() - 2);
    mtsCommandReadBase * command = providedInterface->GetCommandRead(actualCommandName);
    if (!command) {
        CMN_LOG_CLASS_RUN_ERROR << "GetValuesOfCommand: no command found: " << actualCommandName << std::endl;
        return;
    };

    // Get argument prototype
    mtsGenericObject * argument = dynamic_cast<mtsGenericObject*>(command->GetArgumentClassServices()->Create());
    if (!argument) {
        CMN_LOG_CLASS_RUN_ERROR << "GetValuesOfCommand: failed to create temporary argument" << std::endl;
        return;
    }
    
    // Execute read command
    command->Execute(*argument);

    // Get current values with timestamps
    ValuePair value;
    Values valueSet;
    double relativeTime;
    values.clear();
    const int signalCount = argument->GetNumberOfScalar();
    for (unsigned int i = 0; i < argument->GetNumberOfScalar(); ++i) {
        value.Value = argument->GetScalarAsDouble(i);
        argument->GetTimestamp(relativeTime);
        TimeServer.RelativeToAbsolute(relativeTime, value.Timestamp);

        valueSet.push_back(value);
    }
    values.push_back(valueSet);

    delete argument;
}

#endif

mtsComponent * mtsManagerLocal::GetComponent(const std::string & componentName) const
{
    return ComponentMap.GetItem(componentName);
}

mtsTask * mtsManagerLocal::GetComponentAsTask(const std::string & componentName) const
{
    mtsTask * componentTask = NULL;

    mtsDevice * component = ComponentMap.GetItem(componentName);
    if (component) {
        componentTask = dynamic_cast<mtsTask*>(component);
    }

    return componentTask;
}

mtsTask CISST_DEPRECATED * mtsManagerLocal::GetTask(const std::string & taskName)
{
    return GetComponentAsTask(taskName);
}

mtsDevice CISST_DEPRECATED * mtsManagerLocal::GetDevice(const std::string & deviceName)
{
    return ComponentMap.GetItem(deviceName);
}

bool mtsManagerLocal::FindComponent(const std::string & componentName) const
{
    return (GetComponent(componentName) != NULL);
}

void mtsManagerLocal::CreateAll(void)
{
    mtsTask * componentTask;

    ComponentMapChange.Lock();

    ComponentMapType::const_iterator it = ComponentMap.begin();
    const ComponentMapType::const_iterator itEnd = ComponentMap.end();

    for (; it != itEnd; ++it) {
        // Skip components of mtsDevice type
        componentTask = dynamic_cast<mtsTask*>(it->second);
        if (!componentTask) continue;

        // Note that the order of dynamic casts matters to figure out
        // a type of an original task since tasks have multiple inheritance.

        // mtsTaskPeriodic type component
        componentTask = dynamic_cast<mtsTaskPeriodic*>(it->second);
        if (componentTask) {
            componentTask->Create();
            continue;
        }

        // mtsTaskFromSignal type component
        componentTask = dynamic_cast<mtsTaskFromSignal*>(it->second);
        if (componentTask) {
            componentTask->Create();
            continue;
        }

        // mtsTaskContinuous type component
        componentTask = dynamic_cast<mtsTaskContinuous*>(it->second);
        if (componentTask) {
            componentTask->Create();
            continue;
        }

        // mtsTaskFromCallback type component
        componentTask = dynamic_cast<mtsTaskFromCallback*>(it->second);
        if (componentTask) {
            componentTask->Create();
            continue;
        }
    }

    ComponentMapChange.Unlock();
}

void mtsManagerLocal::StartAll(void)
{
    // Get the current thread id in order to check if any task will use the current thread.
    // If so, start that task for last.
    const osaThreadId threadId = osaGetCurrentThreadId();

    mtsTask * componentTask, * componentTaskTemp;

    ComponentMapChange.Lock();

    ComponentMapType::const_iterator it = ComponentMap.begin();
    const ComponentMapType::const_iterator itEnd = ComponentMap.end();
    ComponentMapType::const_iterator itLastTask = ComponentMap.end();

    for (; it != ComponentMap.end(); ++it) {
        // Skip components of mtsDevice type
        componentTaskTemp = dynamic_cast<mtsTask*>(it->second);
        if (!componentTaskTemp) continue;

        // Note that the order of dynamic casts matters to figure out
        // a type of an original task since tasks have multiple inheritances.

        // mtsTaskPeriodic type component
        componentTaskTemp = dynamic_cast<mtsTaskPeriodic*>(it->second);
        if (componentTaskTemp) {
            componentTask = componentTaskTemp;
        } else {
            // mtsTaskFromSignal type component
            componentTaskTemp = dynamic_cast<mtsTaskFromSignal*>(it->second);
            if (componentTaskTemp) {
                componentTask = componentTaskTemp;
            } else {
                // mtsTaskContinuous type component
                componentTaskTemp = dynamic_cast<mtsTaskContinuous*>(it->second);
                if (componentTaskTemp) {
                    componentTask = componentTaskTemp;
                } else {
                    // mtsTaskFromCallback type component
                    componentTaskTemp = dynamic_cast<mtsTaskFromCallback*>(it->second);
                    if (componentTaskTemp) {
                        componentTask = componentTaskTemp;
                    } else {
                        componentTask = 0;
                        CMN_LOG_CLASS_RUN_ERROR << "StartAll: invalid component: unknown mtsTask type" << std::endl;
                        continue;
                    }
                }
            }
        }

        // Check if the task will use the current thread.
        if (componentTask->Thread.GetId() == threadId) {
            CMN_LOG_CLASS_INIT_WARNING << "StartAll: component \"" << it->first << "\" uses current thread, will start last." << std::endl;
            if (itLastTask != ComponentMap.end()) {
                CMN_LOG_CLASS_RUN_ERROR << "StartAll: multiple tasks using current thread (only first will be started)." << std::endl;
            } else {
                itLastTask = it;
            }
        } else {
            CMN_LOG_CLASS_INIT_DEBUG << "StartAll: starting task \"" << it->first << "\"" << std::endl;
            componentTask->Start();  // If task will not use current thread, start it immediately.
        }
    }

    if (itLastTask != ComponentMap.end()) {
        // mtsTaskPeriodic type component
        componentTaskTemp = dynamic_cast<mtsTaskPeriodic*>(itLastTask->second);
        if (componentTaskTemp) {
            componentTask = componentTaskTemp;
        } else {
            // mtsTaskFromSignal type component
            componentTaskTemp = dynamic_cast<mtsTaskFromSignal*>(itLastTask->second);
            if (componentTaskTemp) {
                componentTask = componentTaskTemp;
            } else {
                // mtsTaskContinuous type component
                componentTaskTemp = dynamic_cast<mtsTaskContinuous*>(itLastTask->second);
                if (componentTaskTemp) {
                    componentTask = componentTaskTemp;
                } else {
                    // mtsTaskFromCallback type component
                    componentTaskTemp = dynamic_cast<mtsTaskFromCallback*>(itLastTask->second);
                    if (componentTaskTemp) {
                        componentTask = componentTaskTemp;
                    } else {
                        componentTask = NULL;
                        CMN_LOG_CLASS_RUN_ERROR << "StartAll: invalid component: unknown mtsTask type (last component)" << std::endl;
                    }
                }
            }
        }

        if (componentTask) {
            componentTask->Start();
        }
    }

    ComponentMapChange.Unlock();
}

void mtsManagerLocal::KillAll(void)
{
    mtsTask *componentTask, *componentTaskTemp;

    ComponentMapChange.Lock();

    ComponentMapType::const_iterator it = ComponentMap.begin();
    const ComponentMapType::const_iterator itEnd = ComponentMap.end();
    for (; it != itEnd; ++it) {
        // mtsTaskPeriodic type component
        componentTaskTemp = dynamic_cast<mtsTaskPeriodic*>(it->second);
        if (componentTaskTemp) {
            componentTask = componentTaskTemp;
        } else {
            // mtsTaskFromSignal type component
            componentTaskTemp = dynamic_cast<mtsTaskFromSignal*>(it->second);
            if (componentTaskTemp) {
                componentTask = componentTaskTemp;
            } else {
                // mtsTaskContinuous type component
                componentTaskTemp = dynamic_cast<mtsTaskContinuous*>(it->second);
                if (componentTaskTemp) {
                    componentTask = componentTaskTemp;
                } else {
                    // mtsTaskFromCallback type component
                    componentTaskTemp = dynamic_cast<mtsTaskFromCallback*>(it->second);
                    if (componentTaskTemp) {
                        componentTask = componentTaskTemp;
                    } else {
                        componentTask = NULL;
                        CMN_LOG_CLASS_RUN_ERROR << "KillAll: invalid component: unknown mtsTask type" << std::endl;
                        continue;
                    }
                }
            }
        }
        componentTask->Kill();
    }

    ComponentMapChange.Unlock();
}

bool mtsManagerLocal::Connect(const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
                              const std::string & serverComponentName, const std::string & serverProvidedInterfaceName)
{
    // Make sure all interfaces created so far are registered to the GCM.
    if (!RegisterInterfaces(clientComponentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "Connect: failed to register interfaces: " << clientComponentName << std::endl;
        return false;
    }
    if (!RegisterInterfaces(serverComponentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "Connect: failed to register interfaces: " << serverComponentName << std::endl;
        return false;
    }

    unsigned int connectionID = ManagerGlobal->Connect(ProcessName,
        ProcessName, clientComponentName, clientRequiredInterfaceName,
        ProcessName, serverComponentName, serverProvidedInterfaceName);

    if (connectionID == static_cast<unsigned int>(mtsManagerGlobalInterface::CONNECT_ERROR)) {
        CMN_LOG_CLASS_RUN_ERROR << "Connect: failed to get connection id from the Global Component Manager: "
            << clientComponentName << ":" << clientRequiredInterfaceName << " - "
            << serverComponentName << ":" << serverProvidedInterfaceName << std::endl;
        return false;
    }

    int ret = ConnectLocally(clientComponentName, clientRequiredInterfaceName,
                             serverComponentName, serverProvidedInterfaceName);
    if (ret == -1) {
        CMN_LOG_CLASS_RUN_ERROR << "Connect: failed to establish local connection: "
            << clientComponentName << ":" << clientRequiredInterfaceName << " - "
            << serverComponentName << ":" << serverProvidedInterfaceName << std::endl;
        return false;
    }

    // Notify the GCM of successful local connection
    if (!ManagerGlobal->ConnectConfirm(connectionID)) {
        CMN_LOG_CLASS_RUN_ERROR << "Connect: failed to notify GCM of this connection" << std::endl;

        if (!Disconnect(clientComponentName, clientRequiredInterfaceName, serverComponentName, serverProvidedInterfaceName)) {
            CMN_LOG_CLASS_RUN_ERROR << "Connect: clean up error: disconnection failed" << std::endl;
        }
    }

    return true;
}

#if CISST_MTS_HAS_ICE
std::vector<std::string> mtsManagerLocal::GetIPAddressList(void)
{
    std::vector<std::string> ipAddresses;
    osaSocket::GetLocalhostIP(ipAddresses);

    return ipAddresses;
}

void mtsManagerLocal::GetIPAddressList(std::vector<std::string> & ipAddresses)
{
    osaSocket::GetLocalhostIP(ipAddresses);
}

bool mtsManagerLocal::Connect(
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName)
{
    // Prevent this method from being used to connect two local interfaces
    if (clientProcessName == serverProcessName) {
        return Connect(clientComponentName, clientRequiredInterfaceName, serverComponentName, serverProvidedInterfaceName);
    }

    // Make sure all interfaces created so far are registered to the GCM.
    if (!RegisterInterfaces(clientComponentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "Connect: failed to register interfaces: " << clientComponentName << std::endl;
        return false;
    }
    if (!RegisterInterfaces(serverComponentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "Connect: failed to register interfaces: " << serverComponentName << std::endl;
        return false;
    }

    // Inform the global component manager of the fact that a new connection is
    // to be established.
    unsigned int connectionID = ManagerGlobal->Connect(ProcessName,
        clientProcessName, clientComponentName, clientRequiredInterfaceName,
        serverProcessName, serverComponentName, serverProvidedInterfaceName);

    if (connectionID == static_cast<unsigned int>(mtsManagerGlobalInterface::CONNECT_ERROR)) {
        CMN_LOG_CLASS_RUN_ERROR << "Connect: Global Component Manager failed to reserve connection: "
            << clientProcessName << ":" << clientComponentName << ":" << clientRequiredInterfaceName << " - "
            << serverProcessName << ":" << serverComponentName << ":" << serverProvidedInterfaceName << std::endl;
        return false;
    }

    // Connect() can be called by two different processes: either by a client
    // process or by a server process. Whichever calls Connect(), the connection
    // result is identical.
    bool isConnectCalledByClientProcess;

    // If this local component has a client component
    if (ProcessName == clientProcessName) {
        isConnectCalledByClientProcess = true;
    }
    // If this local component has a server component
    else if (ProcessName == serverProcessName) {
        isConnectCalledByClientProcess = false;
    }
    // This should not be the case: two external component cannot be connected.
    else {
        CMN_LOG_CLASS_RUN_ERROR << "Connect: Cannot connect two external components." << std::endl;
        return false;
    }

    // At this point, server and client process have the identical set of
    // components because the GCM has injected proxy components and interfaces
    // as needed.

    // If client process calls Connect(),
    // - Create a network proxy server of type mtsComponentInterfaceProxyServer.
    // - Register its access information (endpoint string) to the GCM.
    // - Let server process begin connection process via the GCM.
    // - Inform the GCM that the connection is successfully established and
    //   becomes active.
    if (isConnectCalledByClientProcess) {
        if (!ConnectClientSideInterface(connectionID,
                clientProcessName, clientComponentName, clientRequiredInterfaceName,
                serverProcessName, serverComponentName, serverProvidedInterfaceName))
        {
            CMN_LOG_CLASS_RUN_ERROR << "Connect: Failed to connect at client process" << std::endl;

            if (!Disconnect(clientProcessName, clientComponentName, clientRequiredInterfaceName,
                            serverProcessName, serverComponentName, serverProvidedInterfaceName))
            {
                CMN_LOG_CLASS_RUN_ERROR << "Connect: clean up error: disconnection failed";
            }
            return false;
        }
    }
    // If server process calls Connect(), let client process initiate connection
    // process via the GCM.
    else {
        if (!ManagerGlobal->InitiateConnect(connectionID,
                clientProcessName, clientComponentName, clientRequiredInterfaceName,
                serverProcessName, serverComponentName, serverProvidedInterfaceName))
        {
            CMN_LOG_CLASS_RUN_ERROR << "Connect: Failed to initiate connection" << std::endl;

            if (!Disconnect(clientProcessName, clientComponentName, clientRequiredInterfaceName,
                            serverProcessName, serverComponentName, serverProvidedInterfaceName))
            {
                CMN_LOG_CLASS_RUN_ERROR << "Connect: clean up (disconnect failed) error";
            }
            return false;
        }
    }

    CMN_LOG_CLASS_RUN_VERBOSE << "Connect: successfully established remote connection" << std::endl;

    return true;
}
#endif

int mtsManagerLocal::ConnectLocally(
    const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
    const std::string & serverComponentName, const std::string & serverProvidedInterfaceName)
{
    // At this point, it is guaranteed that all components and interfaces exist
    // in the same process because the global component manager has already
    // injected proxy objects as needed.

    mtsComponent * clientComponent = GetComponent(clientComponentName);
    if (!clientComponent) {
        CMN_LOG_CLASS_RUN_ERROR << "ConnectLocally: failed to get client component: " << clientComponentName << std::endl;
        return -1;
    }

    mtsComponent * serverComponent = GetComponent(serverComponentName);
    if (!serverComponent) {
        CMN_LOG_CLASS_RUN_ERROR << "ConnectLocally: failed to get server component: " << serverComponentName << std::endl;
        return -1;
    }

    mtsProvidedInterface * serverProvidedInterface = serverComponent->GetProvidedInterface(serverProvidedInterfaceName);
    if (!serverProvidedInterface) {
        CMN_LOG_CLASS_RUN_ERROR << "ConnectLocally: failed to find provided interface: "
            << serverComponentName << ":" << serverProvidedInterfaceName << std::endl;
        return -1;
    }

    // If a server component is a proxy, it should create a new instance of
    // provided interface proxy and clone all the proxies in it. This is crucial
    // for thread-safe data communication over networks.
    unsigned int providedInterfaceInstanceID = 0;
    mtsProvidedInterface * providedInterfaceInstance = NULL;

#if CISST_MTS_HAS_ICE
    const bool isServerComponentProxy = IsProxyComponent(serverComponentName);
    if (isServerComponentProxy) {
        mtsComponentProxy * serverComponentProxy = dynamic_cast<mtsComponentProxy *>(serverComponent);
        if (!serverComponentProxy) {
            CMN_LOG_CLASS_RUN_ERROR << "ConnectLocally: invalid type of server component: " << serverComponentName << std::endl;
            return -1;
        }

        // Issue a new resource user id and clone command proxy objects
        providedInterfaceInstance = serverComponentProxy->CreateProvidedInterfaceInstance(serverProvidedInterface, providedInterfaceInstanceID);
        if (!providedInterfaceInstance) {
            CMN_LOG_CLASS_RUN_ERROR << "ConnectLocally: failed to create provided interface proxy instance: "
                << clientComponentName << ":" << clientRequiredInterfaceName << std::endl;
            return -1;
        }

        /* TODO: How to resolve a problem of receiving events two times??
        // Disable event void (see mtsCommandBase.h for detailed comments)
        mtsDeviceInterface::EventVoidMapType::const_iterator itVoid =
            providedInterfaceInstance->EventVoidGenerators.begin();
        const mtsDeviceInterface::EventVoidMapType::const_iterator itVoidEnd =
            providedInterfaceInstance->EventVoidGenerators.end();
        for (; itVoid != itVoidEnd; ++itVoid) {
            itVoid->second->DisableEvent();
        }

        // Disable event write
        mtsDeviceInterface::EventWriteMapType::const_iterator itWrite =
            providedInterfaceInstance->EventWriteGenerators.begin();
        const mtsDeviceInterface::EventWriteMapType::const_iterator itWriteEnd =
            providedInterfaceInstance->EventWriteGenerators.end();
        for (; itWrite != itWriteEnd; ++itWrite) {
            itWrite->second->DisableEvent();
        }
        */

        CMN_LOG_CLASS_RUN_VERBOSE << "ConnectLocally: "
            << "created provided interface proxy instance: id = " << providedInterfaceInstanceID << std::endl;
    }
#endif

    // if providedInterfaceInstance is NULL, this is local connection between
    // two original interfaces.
    if (!providedInterfaceInstance) {
        providedInterfaceInstance = serverProvidedInterface;
    }

    // Connect two interfaces
    if (!clientComponent->ConnectRequiredInterface(clientRequiredInterfaceName, providedInterfaceInstance)) {
        CMN_LOG_CLASS_RUN_ERROR << "ConnectLocally: failed to connect interfaces: "
            << clientComponentName << ":" << clientRequiredInterfaceName << " - "
            << serverComponentName << ":" << serverProvidedInterfaceName << std::endl;
        return -1;
    }

    CMN_LOG_CLASS_RUN_VERBOSE << "ConnectLocally: successfully connected: "
            << clientComponentName << ":" << clientRequiredInterfaceName << " - "
            << serverComponentName << ":" << serverProvidedInterfaceName << std::endl;

    return providedInterfaceInstanceID;
}

bool mtsManagerLocal::Disconnect(const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
                                 const std::string & serverComponentName, const std::string & serverProvidedInterfaceName)
{
    bool success = ManagerGlobal->Disconnect(
        ProcessName, clientComponentName, clientRequiredInterfaceName,
        ProcessName, serverComponentName, serverProvidedInterfaceName);

    if (!success) {
        CMN_LOG_CLASS_RUN_ERROR << "Disconnect: disconnection failed." << std::endl;
        return false;
    }

    //
    // TODO: LOCAL DISCONNECT!!! (e.g. disable all commands and events, and all the other
    // resource clean-up and disconnection chores)
    //

    CMN_LOG_CLASS_RUN_VERBOSE << "Disconnect: successfully disconnected." << std::endl;

    return true;
}

void CISST_DEPRECATED mtsManagerLocal::ToStream(std::ostream & CMN_UNUSED(outputStream)) const
{
#if 0
    TaskMapType::const_iterator taskIterator = TaskMap.begin();
    const TaskMapType::const_iterator taskEndIterator = TaskMap.end();
    outputStream << "List of tasks: name and address" << std::endl;
    for (; taskIterator != taskEndIterator; ++taskIterator) {
        outputStream << "  Task: " << taskIterator->first << ", address: " << taskIterator->second << std::endl;
    }
    DeviceMapType::const_iterator deviceIterator = DeviceMap.begin();
    const DeviceMapType::const_iterator deviceEndIterator = DeviceMap.end();
    outputStream << "List of devices: name and address" << std::endl;
    for (; deviceIterator != deviceEndIterator; ++deviceIterator) {
        outputStream << "  Device: " << deviceIterator->first << ", adress: " << deviceIterator->second << std::endl;
    }
    AssociationSetType::const_iterator associationIterator = AssociationSet.begin();
    const AssociationSetType::const_iterator associationEndIterator = AssociationSet.end();
    outputStream << "Associations: task::requiredInterface associated to device/task::requiredInterface" << std::endl;
    for (; associationIterator != associationEndIterator; ++associationIterator) {
        outputStream << "  " << associationIterator->first.first << "::" << associationIterator->first.second << std::endl
                     << "  -> " << associationIterator->second.first << "::" << associationIterator->second.second << std::endl;
    }
#endif
}

void CISST_DEPRECATED mtsManagerLocal::ToStreamDot(std::ostream & CMN_UNUSED(outputStream)) const
{
#if 0
    std::vector<std::string> providedInterfacesAvailable, requiredInterfacesAvailable;
    std::vector<std::string>::const_iterator stringIterator;
    unsigned int clusterNumber = 0;
    // dot header
    outputStream << "/* Automatically generated by cisstMultiTask, mtsTaskManager::ToStreamDot.\n"
                 << "   Use Graphviz utility \"dot\" to generate a graph of tasks/devices interactions. */"
                 << std::endl;
    outputStream << "digraph mtsTaskManager {" << std::endl;
    // create all nodes for tasks
    TaskMapType::const_iterator taskIterator = TaskMap.begin();
    const TaskMapType::const_iterator taskEndIterator = TaskMap.end();
    for (; taskIterator != taskEndIterator; ++taskIterator) {
        outputStream << "subgraph cluster" << clusterNumber << "{" << std::endl
                     << "node[style=filled,color=white,shape=box];" << std::endl
                     << "style=filled;" << std::endl
                     << "color=lightgrey;" << std::endl;
        clusterNumber++;
        outputStream << taskIterator->first
                     << " [label=\"Task:\\n" << taskIterator->first << "\"];" << std::endl;
        providedInterfacesAvailable = taskIterator->second->GetNamesOfProvidedInterfaces();
        for (stringIterator = providedInterfacesAvailable.begin();
             stringIterator != providedInterfacesAvailable.end();
             stringIterator++) {
            outputStream << taskIterator->first << "providedInterface" << *stringIterator
                         << " [label=\"Provided interface:\\n" << *stringIterator << "\"];" << std::endl;
            outputStream << taskIterator->first << "providedInterface" << *stringIterator
                         << "->" << taskIterator->first << ";" << std::endl;
        }
        requiredInterfacesAvailable = taskIterator->second->GetNamesOfRequiredInterfaces();
        for (stringIterator = requiredInterfacesAvailable.begin();
             stringIterator != requiredInterfacesAvailable.end();
             stringIterator++) {
            outputStream << taskIterator->first << "requiredInterface" << *stringIterator
                         << " [label=\"Required interface:\\n" << *stringIterator << "\"];" << std::endl;
            outputStream << taskIterator->first << "->"
                         << taskIterator->first << "requiredInterface" << *stringIterator << ";" << std::endl;
        }
        outputStream << "}" << std::endl;
    }
    // create all nodes for devices
    DeviceMapType::const_iterator deviceIterator = DeviceMap.begin();
    const DeviceMapType::const_iterator deviceEndIterator = DeviceMap.end();
    for (; deviceIterator != deviceEndIterator; ++deviceIterator) {
        outputStream << "subgraph cluster" << clusterNumber << "{" << std::endl
                     << "node[style=filled,color=white,shape=box];" << std::endl
                     << "style=filled;" << std::endl
                     << "color=lightgrey;" << std::endl;
        clusterNumber++;
        outputStream << deviceIterator->first
                     << " [label=\"Device:\\n" << deviceIterator->first << "\"];" << std::endl;
        providedInterfacesAvailable = deviceIterator->second->GetNamesOfProvidedInterfaces();
        for (stringIterator = providedInterfacesAvailable.begin();
             stringIterator != providedInterfacesAvailable.end();
             stringIterator++) {
            outputStream << deviceIterator->first << "providedInterface" << *stringIterator
                         << " [label=\"Provided interface:\\n" << *stringIterator << "\"];" << std::endl;
            outputStream << deviceIterator->first << "providedInterface" << *stringIterator
                         << "->" << deviceIterator->first << ";" << std::endl;
        }
        outputStream << "}" << std::endl;
    }
    // create edges
    AssociationSetType::const_iterator associationIterator = AssociationSet.begin();
    const AssociationSetType::const_iterator associationEndIterator = AssociationSet.end();
    for (; associationIterator != associationEndIterator; ++associationIterator) {
        outputStream << associationIterator->first.first << "requiredInterface" << associationIterator->first.second
                     << "->"
                     << associationIterator->second.first << "providedInterface" << associationIterator->second.second
                     << ";" << std::endl;
    }
    // end of file
    outputStream << "}" << std::endl;
#endif
}




bool mtsManagerLocal::RegisterInterfaces(mtsDevice * component)
{
    if (!component) {
        return false;
    }

    const std::string componentName = component->GetName();

    mtsRequiredInterface * requiredInterface;
    std::vector<std::string> interfaceNames = component->GetNamesOfRequiredInterfaces();
    for (unsigned int i = 0; i < interfaceNames.size(); ++i) {
        requiredInterface = component->GetRequiredInterface(interfaceNames[i]);
        if (!requiredInterface) {
            CMN_LOG_CLASS_RUN_ERROR << "RegisterInterfaces: NULL required interface detected: " << interfaceNames[i] << std::endl;
            return false;
        } else {
            if (requiredInterface->GetRegistered()) {
                continue;
            }
        }

        if (!ManagerGlobal->AddRequiredInterface(ProcessName, componentName, interfaceNames[i], false)) {
            CMN_LOG_CLASS_RUN_ERROR << "RegisterInterfaces: failed to add required interface: "
                << componentName << ":" << interfaceNames[i] << std::endl;
            return false;
        }

        requiredInterface->SetRegistered();
    }

    mtsProvidedInterface * providedInterface;
    interfaceNames = component->GetNamesOfProvidedInterfaces();
    for (unsigned int i = 0; i < interfaceNames.size(); ++i) {
        providedInterface = component->GetProvidedInterface(interfaceNames[i]);
        if (!providedInterface) {
            CMN_LOG_CLASS_RUN_ERROR << "RegisterInterfaces: NULL provided interface detected: " << interfaceNames[i] << std::endl;
            return false;
        } else {
            if (providedInterface->GetRegistered()) {
                continue;
            }
        }

        if (!ManagerGlobal->AddProvidedInterface(ProcessName, componentName, interfaceNames[i], false)) {
            CMN_LOG_CLASS_RUN_ERROR << "RegisterInterfaces: failed to add provided interface: "
                << componentName << ":" << interfaceNames[i] << std::endl;
            return false;
        }

        providedInterface->SetRegistered();
    }

    return true;
}


bool mtsManagerLocal::RegisterInterfaces(const std::string & componentName)
{
    mtsComponent * component = GetComponent(componentName);
    if (!component) {
        CMN_LOG_CLASS_RUN_ERROR << "RegistereInterfaces: invalid component name: " << componentName << std::endl;
        return false;
    }

    return RegisterInterfaces(component);
}



#if CISST_MTS_HAS_ICE
bool mtsManagerLocal::Disconnect(
    const std::string & clientProcessName,
    const std::string & clientComponentName,
    const std::string & clientRequiredInterfaceName,
    const std::string & serverProcessName,
    const std::string & serverComponentName,
    const std::string & serverProvidedInterfaceName)
{
    bool success = ManagerGlobal->Disconnect(
        clientProcessName, clientComponentName, clientRequiredInterfaceName,
        serverProcessName, serverComponentName, serverProvidedInterfaceName);

    if (!success) {
        CMN_LOG_CLASS_RUN_ERROR << "Disconnect: disconnection failed." << std::endl;
        return false;
    }

    //
    // TODO: LOCAL DISCONNECT!!!
    //

    CMN_LOG_CLASS_RUN_VERBOSE << "Disconnect: successfully disconnected." << std::endl;

    return true;
}

bool mtsManagerLocal::GetProvidedInterfaceDescription(
    const std::string & componentName, const std::string & providedInterfaceName,
    ProvidedInterfaceDescription & providedInterfaceDescription, const std::string & CMN_UNUSED(listenerID))
{
    // Get component specified
    mtsComponent * component = GetComponent(componentName);
    if (!component) {
        CMN_LOG_CLASS_RUN_ERROR << "GetProvidedInterfaceDescription: no component \""
            << componentName << "\" found in local component manager \"" << ProcessName << "\"" << std::endl;
        return false;
    }

    // Get provided interface specified
    mtsDeviceInterface * providedInterface = component->GetProvidedInterface(providedInterfaceName);
    if (!providedInterface) {
        CMN_LOG_CLASS_RUN_ERROR << "GetProvidedInterfaceDescription: no provided interface \""
            << providedInterfaceName << "\" found in component \"" << componentName << "\"" << std::endl;
        return false;
    }

    // Extract complete information about all commands and event generators in
    // a provided interface. Argument prototypes are fetched with serialization.
    providedInterfaceDescription.ProvidedInterfaceName = providedInterfaceName;

    mtsComponentProxy::ExtractProvidedInterfaceDescription(providedInterface, providedInterfaceDescription);

    return true;
}

bool mtsManagerLocal::GetRequiredInterfaceDescription(
    const std::string & componentName, const std::string & requiredInterfaceName,
    RequiredInterfaceDescription & requiredInterfaceDescription, const std::string & CMN_UNUSED(listenerID))
{
    // Get the component instance specified
    mtsComponent * component = GetComponent(componentName);
    if (!component) {
        CMN_LOG_CLASS_RUN_ERROR << "GetRequiredInterfaceDescription: no component \""
            << componentName << "\" found in local component manager \"" << ProcessName << "\"" << std::endl;
        return false;
    }

    // Get the provided interface specified
    mtsRequiredInterface * requiredInterface = component->GetRequiredInterface(requiredInterfaceName);
    if (!requiredInterface) {
        CMN_LOG_CLASS_RUN_ERROR << "GetRequiredInterfaceDescription: no provided interface \""
            << requiredInterfaceName << "\" found in component \"" << componentName << "\"" << std::endl;
        return false;
    }

    // Extract complete information about all functions and event handlers in
    // a required interface. Argument prototypes are fetched with serialization.
    requiredInterfaceDescription.RequiredInterfaceName = requiredInterfaceName;

    mtsComponentProxy::ExtractRequiredInterfaceDescription(requiredInterface, requiredInterfaceDescription);

    return true;
}

bool mtsManagerLocal::CreateComponentProxy(const std::string & componentProxyName, const std::string & CMN_UNUSED(listenerID))
{
    // Create a component proxy
    mtsComponent * newComponent = new mtsComponentProxy(componentProxyName);

    bool success = AddComponent(newComponent);
    if (!success) {
        delete newComponent;
        return false;
    }

    return true;
}

bool mtsManagerLocal::RemoveComponentProxy(const std::string & componentProxyName, const std::string & CMN_UNUSED(listenerID))
{
    return RemoveComponent(componentProxyName);
}

bool mtsManagerLocal::CreateProvidedInterfaceProxy(
    const std::string & serverComponentProxyName,
    const ProvidedInterfaceDescription & providedInterfaceDescription, const std::string & CMN_UNUSED(listenerID))
{
    const std::string providedInterfaceName = providedInterfaceDescription.ProvidedInterfaceName;

    // Get current component proxy. If none, returns false because a component
    // proxy should be created before an interface proxy is created.
    mtsComponent * serverComponent = GetComponent(serverComponentProxyName);
    if (!serverComponent) {
        CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: "
            << "no component proxy found: " << serverComponentProxyName << std::endl;
        return false;
    }

    // Downcasting to its original type
    mtsComponentProxy * serverComponentProxy = dynamic_cast<mtsComponentProxy*>(serverComponent);
    if (!serverComponentProxy) {
        CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: "
            << "invalid component proxy: " << serverComponentProxyName << std::endl;
        return false;
    }

    // Create provided interface proxy.
    if (!serverComponentProxy->CreateProvidedInterfaceProxy(providedInterfaceDescription)) {
        CMN_LOG_CLASS_RUN_VERBOSE << "CreateProvidedInterfaceProxy: "
            << "failed to create Provided interface proxy: " << serverComponentProxyName << ":"
            << providedInterfaceName << std::endl;
        return false;
    }

    // Inform the global component manager of the creation of provided interface proxy
    if (!ManagerGlobal->AddProvidedInterface(ProcessName, serverComponentProxyName, providedInterfaceName, true))
    {
        CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: "
            << "failed to add provided interface proxy to global component manager: "
            << ProcessName << ":" << serverComponentProxyName << ":" << providedInterfaceName << std::endl;
        return false;
    }

    CMN_LOG_CLASS_RUN_VERBOSE << "CreateProvidedInterfaceProxy: "
        << "successfully created Provided interface proxy: " << serverComponentProxyName << ":"
        << providedInterfaceName << std::endl;

    return true;
}

bool mtsManagerLocal::CreateRequiredInterfaceProxy(
    const std::string & clientComponentProxyName, const RequiredInterfaceDescription & requiredInterfaceDescription, const std::string & CMN_UNUSED(listenerID))
{
    const std::string requiredInterfaceName = requiredInterfaceDescription.RequiredInterfaceName;

    // Get current component proxy. If none, returns false because a component
    // proxy should be created before an interface proxy is created.
    mtsComponent * clientComponent = GetComponent(clientComponentProxyName);
    if (!clientComponent) {
        CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: "
            << "no component proxy found: " << clientComponentProxyName << std::endl;
        return false;
    }

    // Downcasting to its orginal type
    mtsComponentProxy * clientComponentProxy = dynamic_cast<mtsComponentProxy*>(clientComponent);
    if (!clientComponentProxy) {
        CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: "
            << "invalid component proxy: " << clientComponentProxyName << std::endl;
        return false;
    }

    // Create required interface proxy
    if (!clientComponentProxy->CreateRequiredInterfaceProxy(requiredInterfaceDescription)) {
        CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: "
            << "failed to create required interface proxy: " << clientComponentProxyName << ":"
            << requiredInterfaceName << std::endl;
        return false;
    }

    // Inform the global component manager of the creation of provided interface proxy
    if (!ManagerGlobal->AddRequiredInterface(ProcessName, clientComponentProxyName, requiredInterfaceName, true))
    {
        CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: "
            << "failed to add required interface proxy to global component manager: "
            << ProcessName << ":" << clientComponentProxyName << ":" << requiredInterfaceName << std::endl;
        return false;
    }

    CMN_LOG_CLASS_RUN_VERBOSE << "CreateRequiredInterfaceProxy: "
        << "successfully created required interface proxy: " << clientComponentProxyName << ":"
        << requiredInterfaceName << std::endl;

    return true;
}

bool mtsManagerLocal::RemoveProvidedInterfaceProxy(
    const std::string & clientComponentProxyName, const std::string & providedInterfaceProxyName, const std::string & CMN_UNUSED(listenerID))
{
    mtsComponent * clientComponent = GetComponent(clientComponentProxyName);
    if (!clientComponent) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveProvidedInterfaceProxy: can't find client component: " << clientComponentProxyName << std::endl;
        return false;
    }

    mtsComponentProxy * clientComponentProxy = dynamic_cast<mtsComponentProxy*>(clientComponent);
    if (!clientComponentProxy) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveProvidedInterfaceProxy: client component is not a proxy: " << clientComponentProxyName << std::endl;
        return false;
    }

    // Check a number of required interfaces using (connecting to) this provided interface.
    mtsProvidedInterface * providedInterfaceProxy = clientComponentProxy->GetProvidedInterface(providedInterfaceProxyName);
    if (!providedInterfaceProxy) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveProvidedInterfaceProxy: can't get provided interface proxy.: " << providedInterfaceProxyName << std::endl;
        return false;
    }

    // Remove provided interface proxy only when user counter is zero.
    if (--providedInterfaceProxy->UserCounter == 0) {
        // Remove provided interface from component proxy.
        if (!clientComponentProxy->RemoveProvidedInterfaceProxy(providedInterfaceProxyName)) {
            CMN_LOG_CLASS_RUN_ERROR << "RemoveProvidedInterfaceProxy: failed to remove provided interface proxy: " << providedInterfaceProxyName << std::endl;
            return false;
        }

        CMN_LOG_CLASS_RUN_VERBOSE << "RemoveProvidedInterfaceProxy: removed provided interface: "
            << clientComponentProxyName << ":" << providedInterfaceProxyName << std::endl;
    } else {
        CMN_LOG_CLASS_RUN_VERBOSE << "RemoveProvidedInterfaceProxy: decreased active user counter. current counter: "
            << providedInterfaceProxy->UserCounter << std::endl;
    }

    return true;
}

bool mtsManagerLocal::RemoveRequiredInterfaceProxy(
    const std::string & serverComponentProxyName, const std::string & requiredInterfaceProxyName, const std::string & CMN_UNUSED(listenerID))
{
    mtsComponent * serverComponent = GetComponent(serverComponentProxyName);
    if (!serverComponent) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveRequiredInterfaceProxy: can't find server component: " << serverComponentProxyName << std::endl;
        return false;
    }

    mtsComponentProxy * serverComponentProxy = dynamic_cast<mtsComponentProxy*>(serverComponent);
    if (!serverComponentProxy) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveRequiredInterfaceProxy: server component is not a proxy: " << serverComponentProxyName << std::endl;
        return false;
    }

    // Remove required interface from component proxy.
    if (!serverComponentProxy->RemoveRequiredInterfaceProxy(requiredInterfaceProxyName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveRequiredInterfaceProxy: failed to remove required interface proxy: " << requiredInterfaceProxyName << std::endl;
        return false;
    }

    CMN_LOG_CLASS_RUN_VERBOSE << "RemoveRequiredInterfaceProxy: removed required interface: "
        << serverComponentProxyName << ":" << requiredInterfaceProxyName << std::endl;

    return true;
}

int mtsManagerLocal::GetCurrentInterfaceCount(const std::string & componentName, const std::string & CMN_UNUSED(listenerID))
{
    // Check if the component specified exists
    mtsComponent * component = GetComponent(componentName);
    if (!component) {
        CMN_LOG_CLASS_RUN_ERROR << "GetCurrentInterfaceCount: no component found: " << componentName << " on " << ProcessName << std::endl;
        return -1;
    }

    const unsigned int numOfProvidedInterfaces = component->ProvidedInterfaces.size();
    const unsigned int numOfRequiredInterfaces = component->RequiredInterfaces.size();

    return (const int) (numOfProvidedInterfaces + numOfRequiredInterfaces);
}

void mtsManagerLocal::SetIPAddress(void)
{
    // Fetch all ip addresses available on this machine.
    std::vector<std::string> ipAddresses;
    osaSocket::GetLocalhostIP(ipAddresses);

    for (unsigned int i = 0; i < ipAddresses.size(); ++i) {
        CMN_LOG_CLASS_INIT_VERBOSE << "This machine's IP address : " << ipAddresses[i] << std::endl;
    }

    ProcessIPList.insert(ProcessIPList.begin(), ipAddresses.begin(), ipAddresses.end());
}

bool mtsManagerLocal::SetProvidedInterfaceProxyAccessInfo(
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName,
    const std::string & endpointInfo)
{
    return ManagerGlobal->SetProvidedInterfaceProxyAccessInfo(
        clientProcessName, clientComponentName, clientRequiredInterfaceName,
        serverProcessName, serverComponentName, serverProvidedInterfaceName,
        endpointInfo);
}

bool mtsManagerLocal::ConnectServerSideInterface(const unsigned int providedInterfaceProxyInstanceID,
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName, const std::string & CMN_UNUSED(listenerID))
{
    // This method is called only by the GCM to connect two local interfaces
    // at server side. In this case, one inteface is an original interface and
    // the other one is a proxy interface.

    std::string serverEndpointInfo, communicatorID;
#if CISST_MTS_HAS_ICE
    int numTrial = 0;
    const int maxTrial = 5;
    mtsComponentProxy * clientComponentProxy = 0;
#endif // CISST_MTS_HAS_ICE

    // Check if this is a server process.
    if (this->ProcessName != serverProcessName) {
        CMN_LOG_CLASS_RUN_ERROR << "ConnectServerSideInterface: this is not the server process: " << serverProcessName << std::endl;
        return false;
    }

    // Get actual names of components (either a client component or a server
    // component is a proxy object).
    std::string actualClientComponentName = mtsManagerGlobal::GetComponentProxyName(clientProcessName, clientComponentName);
    std::string actualServerComponentName = serverComponentName;

    // Connect two local interfaces
    const int ret = ConnectLocally(actualClientComponentName, clientRequiredInterfaceName,
                                   actualServerComponentName, serverProvidedInterfaceName);
    if (ret == -1) {
        CMN_LOG_CLASS_RUN_ERROR << "ConnectServerSideInterface: connectLocally() failed" << std::endl;
        return false;
    } else if (ret != 0) {
        CMN_LOG_CLASS_RUN_ERROR << "ConnectServerSideInterface: connectLocally() failed: should return zero: " << ret << std::endl;
        return false;
    }

    CMN_LOG_CLASS_RUN_VERBOSE << "ConnectServerSideInterface: established local connection at server process: " << ProcessName << std::endl;

    // Get component proxy object. Note that this process is the server process
    // and the client component is a proxy object, not an original component.
    const std::string clientComponentProxyName = mtsManagerGlobal::GetComponentProxyName(clientProcessName, clientComponentName);
    mtsComponent * clientComponent = GetComponent(clientComponentProxyName);
    if (!clientComponent) {
        CMN_LOG_CLASS_RUN_ERROR << "ConnectServerSideInterface: the client component is not a proxy: " << clientComponentProxyName << std::endl;
        goto ConnectServerSideInterfaceError;
    }
    clientComponentProxy = dynamic_cast<mtsComponentProxy *>(clientComponent);
    if (!clientComponentProxy) {
        CMN_LOG_CLASS_RUN_ERROR << "ConnectServerSideInterface: client component is not a proxy: " << clientComponent->GetName() << std::endl;
        goto ConnectServerSideInterfaceError;
    }

    // Fetch access information from the global component manager to connect
    // to interface server proxy. Note that it might be possible that an provided
    // interface proxy server has not started yet. In this case, the conection
    // information is not readily available. To handle this case, required
    // interface proxy client tries fetching the access information from the GCM
    // for five seconds (i.e., five times, sleep for one second per trial).
    // After five seconds pass without success, this method returns false, menas
    // failure.

    // Fecth proxy server's access information from the GCM
    while (++numTrial <= maxTrial) {
        // Try to get server proxy access information
        if (ManagerGlobal->GetProvidedInterfaceProxyAccessInfo(
                clientProcessName, clientComponentName, clientRequiredInterfaceName,
                serverProcessName, serverComponentName, serverProvidedInterfaceName,
                serverEndpointInfo))
        {
            CMN_LOG_CLASS_RUN_VERBOSE << "ConnectServerSideInterface: fetched server proxy access information: "
                << serverEndpointInfo << ", " << communicatorID << std::endl;
            break;
        }

        // Wait for 1 second
        CMN_LOG_CLASS_RUN_VERBOSE << "ConnectServerSideInterface: waiting for server proxy access information to be set... "
            << numTrial << " / " << maxTrial << std::endl;
        osaSleep(1.0 * cmn_s);
    }

    // If this client proxy finally didn't get the access information.
    if (numTrial > maxTrial) {
        CMN_LOG_CLASS_RUN_ERROR << "ConnectServerSideInterface: failed to fetch server proxy access information" << std::endl;
        goto ConnectServerSideInterfaceError;
    }

    // Create and run required interface proxy client
    if (!UnitTestEnabled || (UnitTestEnabled && UnitTestNetworkProxyEnabled)) {
        if (!clientComponentProxy->CreateInterfaceProxyClient(
                clientRequiredInterfaceName, serverEndpointInfo, communicatorID, providedInterfaceProxyInstanceID))
        {
            CMN_LOG_CLASS_RUN_ERROR << "ConnectServerSideInterface: failed to create interface proxy client"
                << ": " << clientComponentProxy->GetName() << std::endl;
            goto ConnectServerSideInterfaceError;
        }

        // Wait for the required interface proxy client to successfully connect to
        // provided interface proxy server.
        numTrial = 0;
        while (++numTrial <= maxTrial) {
            if (clientComponentProxy->IsActiveProxy(clientRequiredInterfaceName, false)) {
                CMN_LOG_CLASS_RUN_VERBOSE << "ConnectServerSideInterface: connected to server proxy" << std::endl;
                break;
            }

            // Wait for some time
            CMN_LOG_CLASS_RUN_VERBOSE << "ConnectServerSideInterface: connecting to server proxy... "
                << numTrial << " / " << maxTrial << std::endl;
            osaSleep(200 * cmn_ms);
        }

        // If this client proxy didn't connect to server proxy
        if (numTrial > maxTrial) {
            CMN_LOG_CLASS_RUN_ERROR << "ConnectServerSideInterface: failed to connect to server proxy" << std::endl;
            goto ConnectServerSideInterfaceError;
        }

        // Now it is guaranteed that two local connections--one at server side
        // and the other one at client side--are successfully established.
        // Event handler IDs can be updated at this moment.

        // Update event handler ID: Set event handlers' IDs in a required interface
        // proxy at server side as event generators' IDs fetched from a provided
        // interface proxy at client side.
        if (!clientComponentProxy->UpdateEventHandlerProxyID(clientComponentName, clientRequiredInterfaceName)) {
            CMN_LOG_CLASS_RUN_ERROR << "ConnectServerSideInterface: failed to update event handler proxy id" << std::endl;
            goto ConnectServerSideInterfaceError;
        }
    }

    return true;

ConnectServerSideInterfaceError:
    if (!Disconnect(clientProcessName, clientComponentName, clientRequiredInterfaceName,
                    serverProcessName, serverComponentName, serverProvidedInterfaceName))
    {
        CMN_LOG_CLASS_RUN_ERROR << "ConnectServerSideInterface: clean up (disconnect failed) error";
    }

    return false;
}

bool mtsManagerLocal::ConnectClientSideInterface(const unsigned int connectionID,
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName, const std::string & CMN_UNUSED(listenerID))
{
    std::string endpointAccessInfo, communicatorId;

    // Get the actual names of components (either a client component or a server
    // component is a proxy object).
    std::string actualClientComponentName = clientComponentName;
    std::string actualServerComponentName = mtsManagerGlobal::GetComponentProxyName(serverProcessName, serverComponentName);

    // Connect two local components. Internally, this sets [command/function/
    // event handlers/event generators] pointers of which values will be
    // transferred to the peer process to set command IDs and event handler IDs.
    const int providedInterfaceProxyInstanceID =
        ConnectLocally(actualClientComponentName, clientRequiredInterfaceName,
                       actualServerComponentName, serverProvidedInterfaceName);
    if (providedInterfaceProxyInstanceID == -1)
    {
        CMN_LOG_CLASS_RUN_ERROR << "ConnectClientSideInterface: failed to connect two local components: "
            << clientProcessName << ":" << actualClientComponentName << ":" << clientRequiredInterfaceName << " - "
            << serverProcessName << ":" << actualServerComponentName << ":" << serverProvidedInterfaceName << std::endl;
        return false;
    }

    mtsComponent * serverComponent, * clientComponent;
    mtsComponentProxy * serverComponentProxy = 0;

    // Get the components
    serverComponent = GetComponent(actualServerComponentName);
    if (!serverComponent) {
        CMN_LOG_CLASS_RUN_ERROR << "ConnectClientSideInterface: failed to get server component: " << actualServerComponentName << std::endl;
        goto ConnectClientSideInterfaceError;
    }
    clientComponent = GetComponent(actualClientComponentName);
    if (!clientComponent) {
        CMN_LOG_CLASS_RUN_ERROR << "ConnectClientSideInterface: failed to get client component: " << actualClientComponentName << std::endl;
        goto ConnectClientSideInterfaceError;
    }

    // Downcast to get server component proxy
    serverComponentProxy = dynamic_cast<mtsComponentProxy *>(serverComponent);
    if (!serverComponentProxy) {
        CMN_LOG_CLASS_RUN_ERROR << "ConnectClientSideInterface: server component is not a proxy object: " << serverComponent->GetName() << std::endl;
        goto ConnectClientSideInterfaceError;
    }

    // Create and run interface proxy server only if there is no network
    // proxy server running that serves the provided interface with a name of
    // 'serverProvidedInterfaceName.'
    if (!serverComponentProxy->FindInterfaceProxyServer(serverProvidedInterfaceName)) {
        if (!UnitTestEnabled || (UnitTestEnabled && UnitTestNetworkProxyEnabled)) {
            if (!serverComponentProxy->CreateInterfaceProxyServer(
                    serverProvidedInterfaceName, endpointAccessInfo, communicatorId))
            {
                CMN_LOG_CLASS_RUN_ERROR << "ConnectClientSideInterface: failed to create interface proxy server: "
                    << serverComponentProxy->GetName() << std::endl;
                goto ConnectClientSideInterfaceError;
            }
            CMN_LOG_CLASS_RUN_VERBOSE << "ConnectClientSideInterface: successfully created interface proxy server: "
                << serverComponentProxy->GetName() << std::endl;
        }
    }
    // If there is a server proxy already running, fetch and use the access
    // information of it without specifying client interface.
    else {
        if (!ManagerGlobal->GetProvidedInterfaceProxyAccessInfo("", "", "",
                serverProcessName, serverComponentName, serverProvidedInterfaceName,
                endpointAccessInfo))
        {
            CMN_LOG_CLASS_RUN_ERROR << "ConnectClientSideInterface: failed to fecth server proxy access information: "
                << mtsManagerGlobal::GetInterfaceUID(serverProcessName, serverComponentName, serverProvidedInterfaceName) << std::endl;
            goto ConnectClientSideInterfaceError;
        }
    }

    // Inform the global component manager of the access information of this
    // server proxy so that a client proxy of type mtsComponentInterfaceProxyClient
    // can connect to this server proxy later.
    if (!SetProvidedInterfaceProxyAccessInfo(
            clientProcessName, clientComponentName, clientRequiredInterfaceName,
            serverProcessName, serverComponentName, serverProvidedInterfaceName,
            endpointAccessInfo))
    {
        CMN_LOG_CLASS_RUN_ERROR << "ConnectClientSideInterface: failed to set server proxy access information: "
            << serverProvidedInterfaceName << ", " << endpointAccessInfo << std::endl;
        goto ConnectClientSideInterfaceError;
    }
    CMN_LOG_CLASS_RUN_VERBOSE << "ConnectClientSideInterface: successfully set server proxy access information: "
        << serverProvidedInterfaceName << ", " << endpointAccessInfo << std::endl;

    // Make the server process begin connection process via the GCM.
    if (!ManagerGlobal->ConnectServerSideInterface(providedInterfaceProxyInstanceID,
            clientProcessName, clientComponentName, clientRequiredInterfaceName,
            serverProcessName, serverComponentName, serverProvidedInterfaceName))
    {
        CMN_LOG_CLASS_RUN_ERROR << "ConnectClientSideInterface: failed to connect interfaces at server process" << std::endl;
        goto ConnectClientSideInterfaceError;
    }
    CMN_LOG_CLASS_RUN_VERBOSE << "ConnectClientSideInterface: successfully connected server-side interfaces: "
        << clientRequiredInterfaceName << " - " << serverProvidedInterfaceName << std::endl;

    // Now it is guaranteed that two local connections--one at server side
    // and the other one at client side--are successfully established.
    // That is, command IDs and event handler IDs can be updated.

    // Update command ID: Set command proxy IDs in a provided interface proxy at
    // the client side as function IDs fetched from a required interface proxy at
    // the server side so that an original function object at the client process
    // can call an original command at the server process across networks.
    if (!serverComponentProxy->UpdateCommandProxyID(serverProvidedInterfaceName,
                                                    clientComponentName,
                                                    clientRequiredInterfaceName,
                                                    (unsigned int) providedInterfaceProxyInstanceID))
    {
        CMN_LOG_CLASS_RUN_ERROR << "ConnectClientSideInterface: failed to update command proxy id" << std::endl;
        goto ConnectClientSideInterfaceError;
    }

    // Sleep for unit tests which include networking
    if (UnitTestEnabled && UnitTestNetworkProxyEnabled) {
        osaSleep(3);
    }

    // Inform the GCM that the connection is successfully established and
    // becomes active (network proxies are running now and an ICE client
    // proxy is connected to an ICE server proxy).
    if (!ManagerGlobal->ConnectConfirm(connectionID)) {
        CMN_LOG_CLASS_RUN_ERROR << "ConnectClientSideInterface: failed to notify GCM of this connection" << std::endl;
        goto ConnectClientSideInterfaceError;
    }
    CMN_LOG_CLASS_RUN_VERBOSE << "ConnectClientSideInterface: Informed global component manager of successful connection: " << connectionID << std::endl;

    // Register this connection information to a provided interface proxy
    // server so that the proxy server can clean up this connection when a
    // required interface proxy client is detected as disconnected.
    if (!serverComponentProxy->AddConnectionInformation(providedInterfaceProxyInstanceID,
            clientProcessName, clientComponentName, clientRequiredInterfaceName,
            serverProcessName, serverComponentName, serverProvidedInterfaceName))
    {
        CMN_LOG_CLASS_RUN_ERROR << "ConnectClientSideInterface: failed to add connection information" << std::endl;
        goto ConnectClientSideInterfaceError;
    }

    return true;

ConnectClientSideInterfaceError:
    if (!Disconnect(clientProcessName, clientComponentName, clientRequiredInterfaceName,
                    serverProcessName, serverComponentName, serverProvidedInterfaceName))
    {
        CMN_LOG_CLASS_RUN_ERROR << "ConnectClientSideInterface: clean up (disconnect failed) error" << std::endl;
    }

    return false;
}

void mtsManagerLocal::DisconnectGCM()
{
    mtsManagerProxyClient * globalComponentManagerProxy = dynamic_cast<mtsManagerProxyClient*>(ManagerGlobal);
    CMN_ASSERT(globalComponentManagerProxy);

    globalComponentManagerProxy->Stop();
}

void mtsManagerLocal::ReconnectGCM()
{
    mtsManagerProxyClient * globalComponentManagerProxy = dynamic_cast<mtsManagerProxyClient*>(ManagerGlobal);
    CMN_ASSERT(globalComponentManagerProxy);

    if (!globalComponentManagerProxy->Start(this)) {
        CMN_LOG_CLASS_RUN_ERROR << "ReconnectGCM: Start failed" << std::endl;
        return;
    }

    if (!globalComponentManagerProxy->AddProcess(ProcessName)) {
        CMN_LOG_CLASS_RUN_ERROR << "ReconnectGCM: AddProcess failed" << std::endl;
        return;
    }
}
#endif
