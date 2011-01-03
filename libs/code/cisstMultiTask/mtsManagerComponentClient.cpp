/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2010-08-29

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsManagerComponentClient.h>
#include <cisstMultiTask/mtsManagerComponentServer.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsInterfaceOutput.h>
#include <cisstMultiTask/mtsInterfaceInput.h>
#include <cisstOSAbstraction/osaGetTime.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstCommon/cmnUnits.h>

CMN_IMPLEMENT_SERVICES(mtsManagerComponentClient);

mtsManagerComponentClient::mtsManagerComponentClient(const std::string & componentName)
    : mtsManagerComponentBase(componentName),
      InterfaceComponentFunctionMap("InterfaceComponentFunctionMap")
{
    InterfaceComponentFunctionMap.SetOwner(*this);
}

mtsManagerComponentClient::~mtsManagerComponentClient()
{
}

std::string mtsManagerComponentClient::GetNameOfManagerComponentClient(const std::string & processName)
{
    std::string name(processName);
    name += mtsManagerComponentBase::ComponentNames::ManagerComponentClientSuffix;

    return name;
}

std::string mtsManagerComponentClient::GetNameOfInterfaceComponentRequired(const std::string & userComponentName)
{
    std::string interfaceName = mtsManagerComponentBase::InterfaceNames::InterfaceComponentRequired;
    interfaceName += "For";
    interfaceName += userComponentName;

    return interfaceName;
}

void mtsManagerComponentClient::Startup(void)
{
   CMN_LOG_CLASS_INIT_VERBOSE << "MCC starts" << std::endl;
}

void mtsManagerComponentClient::Run(void)
{
    mtsManagerComponentBase::Run();
}

void mtsManagerComponentClient::Cleanup(void)
{
}

bool mtsManagerComponentClient::CreateAndAddNewComponent(const std::string & className, const std::string & componentName)
{
    // Try to create component as requested
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();

    mtsComponent * newComponent = LCM->CreateComponentDynamically(className, componentName);
    if (!newComponent) {
        CMN_LOG_CLASS_RUN_ERROR << "CreateAndAddNewComponent: failed to create component: " 
            << "\"" << componentName << "\" of type \"" << className << "\"" << std::endl;
        return false;
    }

    // Add new component with supoprt for the dynamic component control
    if (!LCM->AddComponent(newComponent)) {
        CMN_LOG_CLASS_RUN_ERROR << "CreateAndAddNewComponent: failed to add component: "
            << "\"" << componentName << "\" of type \"" << className << "\"" << std::endl;
        return false;
    }

    CMN_LOG_CLASS_RUN_VERBOSE << "CreateAndAddNewComponent: successfully added component: "
        << "\"" << componentName << "\" of type \"" << className << "\"" << std::endl;

    return true;
}

bool mtsManagerComponentClient::ConnectLocally(const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
                                               const std::string & serverComponentName, const std::string & serverInterfaceProvidedName,
                                               const std::string & clientProcessName)
{
    // At this point, it is guaranteed that all components and interfaces exist
    // in the same process because the global component manager has already
    // created all proxy objects needed.
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    mtsComponent * clientComponent = LCM->GetComponent(clientComponentName);
    if (!clientComponent) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectLocally: failed to get client component: \"" << clientComponentName << "\"" << std::endl;
        return false;
    }

    mtsComponent * serverComponent = LCM->GetComponent(serverComponentName);
    if (!serverComponent) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectLocally: failed to get server component: \"" << serverComponentName << "\"" << std::endl;
        return false;
    }

    mtsInterfaceProvidedOrOutput * serverInterfaceProvidedOrOutput = serverComponent->GetInterfaceProvidedOrOutput(serverInterfaceProvidedName);
    if (!serverInterfaceProvidedOrOutput) {
        // test for swapped interfaces
        // TODO: This duplicates a test already done in the GCM
        CMN_LOG_CLASS_INIT_DEBUG << "ConnectLocally: looking for provided/output interface in first component as well" << std::endl;
        serverInterfaceProvidedOrOutput = clientComponent->GetInterfaceProvidedOrOutput(clientInterfaceRequiredName);
        if (!serverInterfaceProvidedOrOutput) {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectLocally: failed to get provided/output interface \"" << serverInterfaceProvidedName << "\""
                                     << " in component \"" << serverComponentName << "\"" << std::endl;
            return false;
        } else {
            CMN_LOG_CLASS_INIT_DEBUG << "ConnectLocally: Swapping client/server" << std::endl;
            return ConnectLocally(serverComponentName, serverInterfaceProvidedName, clientComponentName, clientInterfaceRequiredName, clientProcessName);
        }
    }

    // Now, handle the connections.  First, we look for connection between required/provided interfaces.  Then, we look
    // for connection between input/output interfaces.
    mtsInterfaceProvided *serverInterfaceProvided = dynamic_cast<mtsInterfaceProvided *>(serverInterfaceProvidedOrOutput);
    if (serverInterfaceProvided) {
        mtsInterfaceRequired *clientInterfaceRequired = clientComponent->GetInterfaceRequired(clientInterfaceRequiredName);
        if (!clientInterfaceRequired) {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectLocally: failed to connect interfaces: "
                                     << clientComponentName << ":" << clientInterfaceRequiredName << " - "
                                     << serverComponentName << ":" << serverInterfaceProvidedName
                                     << ", client does not have required interface." << std::endl;
            return false;
        }
        bool success = false;
        // If the server is this component (ManagerComponentClient), or if this component is not active,
        // we can directly call the methods. Note that we could use the StateChange mutex to make sure that the state
        // does not change during execution of this method, but that is unlikely.
        if ((serverComponentName == GetName()) || !IsRunning()) {
            mtsInterfaceProvided *endUserInterface = serverInterfaceProvided->GetEndUserInterface(clientInterfaceRequiredName);
            success = clientInterfaceRequired->BindCommands(endUserInterface);
            mtsEventHandlerList eventList(endUserInterface);
            clientInterfaceRequired->GetEventList(eventList);
            endUserInterface->AddObserverList(eventList, eventList);
            if (!clientInterfaceRequired->CheckEventList(eventList))
                success = false;
        }
        else {
            InterfaceComponentFunctionType * serverFunctionSet = InterfaceComponentFunctionMap.GetItem(serverComponentName);
            if (!serverFunctionSet) {
                CMN_LOG_CLASS_RUN_ERROR << "ConnectLocally: failed to get function set for " << serverComponentName << std::endl;
                return false;
            }
            if (!serverFunctionSet->GetEndUserInterface.IsValid()) {
                CMN_LOG_CLASS_RUN_ERROR << "ConnectLocally: GetEndUserInterface invalid for " << serverComponentName << std::endl;
                return false;
            }
            if (!serverFunctionSet->AddObserverList.IsValid()) {
                CMN_LOG_CLASS_RUN_ERROR << "ConnectLocally: AddObserverList invalid for " << serverComponentName << std::endl;
                return false;
            }
            mtsEndUserInterfaceArg endUserInterfaceArg(serverInterfaceProvided, clientInterfaceRequiredName);
            serverFunctionSet->GetEndUserInterface(endUserInterfaceArg, endUserInterfaceArg);
            mtsInterfaceProvided *endUserInterface = endUserInterfaceArg.EndUserInterface;
            if (!endUserInterface) {
                CMN_LOG_CLASS_RUN_ERROR << "ConnectLocally: failed to get end-user interface for " << serverComponentName << std::endl;
                return false;
            }
            success = clientInterfaceRequired->BindCommands(endUserInterface);
            mtsEventHandlerList eventList(endUserInterface);
            clientInterfaceRequired->GetEventList(eventList);
            serverFunctionSet->AddObserverList(eventList, eventList);
            if (!clientInterfaceRequired->CheckEventList(eventList))
                success = false;
        }
        if (success) {
            CMN_LOG_CLASS_INIT_VERBOSE << "ConnectLocally: successfully connected required/provided: "
                                       << clientComponentName << ":" << clientInterfaceRequiredName << " - "
                                       << serverComponentName << ":" << serverInterfaceProvidedName << std::endl;
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectLocally: failed to connect required/provided: "
                                     << clientComponentName << ":" << clientInterfaceRequiredName << " - "
                                     << serverComponentName << ":" << serverInterfaceProvidedName << std::endl;
            return false;
        }

        // Post-connect processing to handle the special case 1:
        // When the manager component server's provided interface (InterfaceGCM's
        // provided interface) gets connected with a manager component client's
        // required interface (InterfaceLCM's required interface) and a process
        // name of LCM that owns the manager component client is unknown to the
        // manager component server, a new set of function objects needs to be
        // created so that manager component server can handle multiple manager
        // component clients, i.e., multiple processes.
        mtsManagerComponentServer * MCS = dynamic_cast<mtsManagerComponentServer*>(serverComponent);
        if (MCS) {
            if (serverInterfaceProvidedName == mtsManagerComponentBase::InterfaceNames::InterfaceGCMProvided) {
                if (!MCS->AddNewClientProcess(clientProcessName)) {
                    CMN_LOG_CLASS_INIT_ERROR << "ConnectLocally: failed to create new set of InterfaceGCM function objects: "
                        << clientProcessName << std::endl;
                    return false;
                }
            }
        }

    }
    else {  // Input/Output connection
        mtsInterfaceOutput *serverInterfaceOutput = dynamic_cast<mtsInterfaceOutput *>(serverInterfaceProvidedOrOutput);
        if (!serverInterfaceOutput) {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectLocally: failed to connect interfaces: "
                                     << clientComponentName << ":" << clientInterfaceRequiredName << " - "
                                     << serverComponentName << ":" << serverInterfaceProvidedName
                                     << ", server does not have provided or output interface." << std::endl;
            return false;
        }
        mtsInterfaceInput *clientInterfaceInput = clientComponent->GetInterfaceInput(clientInterfaceRequiredName);
        if (!clientInterfaceInput) {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectLocally: failed to connect interfaces: "
                                     << clientComponentName << ":" << clientInterfaceRequiredName << " - "
                                     << serverComponentName << ":" << serverInterfaceProvidedName
                                     << ", client does not have input interface." << std::endl;
            return false;
        }
        if (clientInterfaceInput->ConnectTo(serverInterfaceOutput)) {
            CMN_LOG_CLASS_INIT_VERBOSE << "ConnectLocally: component \""
                                       << this->GetName()
                                       << "\" input interface \"" << clientInterfaceRequiredName
                                       << "\" successfully connected to output interface \""
                                       << serverInterfaceProvidedName << "\"" << std::endl;
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectLocally: component \""
                                     << this->GetName()
                                     << "\" input interface \"" << clientInterfaceRequiredName
                                     << "\" failed to connect to output interface \""
                                     << serverInterfaceProvidedName << "\"" << std::endl;
            return false;
        }
    }

    return true;
}

// This implementation of DisconnectLocally does not rely on any data saved about the connection, such as the end-user
// interface pointer or the connection id.  I think it would be better to first look up this information.
bool mtsManagerComponentClient::DisconnectLocally(const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
                                                 const std::string & serverComponentName, const std::string & serverInterfaceProvidedName,
                                                 const std::string & clientProcessName)
{
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    mtsComponent * clientComponent = LCM->GetComponent(clientComponentName);
    if (!clientComponent) {
        CMN_LOG_CLASS_INIT_ERROR << "DisconnectLocally: failed to get client component: \"" << clientComponentName << "\"" << std::endl;
        return false;
    }

    mtsComponent * serverComponent = LCM->GetComponent(serverComponentName);
    if (!serverComponent) {
        CMN_LOG_CLASS_INIT_ERROR << "DisconnectLocally: failed to get server component: \"" << serverComponentName << "\"" << std::endl;
        return false;
    }

    mtsInterfaceProvidedOrOutput * serverInterfaceProvidedOrOutput = serverComponent->GetInterfaceProvidedOrOutput(serverInterfaceProvidedName);
    if (!serverInterfaceProvidedOrOutput) {
        // test for swapped interfaces
        // TODO: This duplicates a test already done in the GCM
        CMN_LOG_CLASS_INIT_DEBUG << "DisconnectLocally: looking for provided/output interface in first component as well" << std::endl;
        serverInterfaceProvidedOrOutput = clientComponent->GetInterfaceProvidedOrOutput(clientInterfaceRequiredName);
        if (!serverInterfaceProvidedOrOutput) {
            CMN_LOG_CLASS_INIT_ERROR << "DisconnectLocally: failed to get provided/output interface \"" << serverInterfaceProvidedName << "\""
                                     << " in component \"" << serverComponentName << "\"" << std::endl;
            return false;
        } else {
            CMN_LOG_CLASS_INIT_DEBUG << "DisconnectLocally: Swapping client/server" << std::endl;
            return DisconnectLocally(serverComponentName, serverInterfaceProvidedName, clientComponentName, clientInterfaceRequiredName, clientProcessName);
        }
    }

    // Now, handle the disconnection.  First, we look for connection between required/provided interfaces.  Then, we look
    // for connection between input/output interfaces.
    mtsInterfaceProvided *serverInterfaceProvided = dynamic_cast<mtsInterfaceProvided *>(serverInterfaceProvidedOrOutput);
    if (serverInterfaceProvided) {
        mtsInterfaceRequired *clientInterfaceRequired = clientComponent->GetInterfaceRequired(clientInterfaceRequiredName);
        if (!clientInterfaceRequired) {
            CMN_LOG_CLASS_RUN_ERROR << "DisconnectLocally: failed to disconnect interfaces: "
                                     << clientComponentName << ":" << clientInterfaceRequiredName << " - "
                                     << serverComponentName << ":" << serverInterfaceProvidedName
                                     << ", client does not have required interface." << std::endl;
            return false;
        }
        // Following check should not be necessary -- serverInterfaceProvided should always be the original interface
        // because we obtained it via a string query.
        if (serverInterfaceProvided->GetOriginalInterface()) {
            CMN_LOG_CLASS_RUN_ERROR << "DisconnectLocally: failed to disconnect interfaces: "
                                     << clientComponentName << ":" << clientInterfaceRequiredName << " - "
                                     << serverComponentName << ":" << serverInterfaceProvidedName
                                     << ", did not get original interface." << std::endl;
            return false;
        }
        // Now, get the end-user interface for this client
        mtsInterfaceProvided *endUserInterface = serverInterfaceProvided->FindEndUserInterfaceByName(clientInterfaceRequiredName);
        if (!endUserInterface) {
            CMN_LOG_CLASS_RUN_ERROR << "DisconnectLocally: failed to disconnect interfaces: "
                                     << clientComponentName << ":" << clientInterfaceRequiredName << " - "
                                     << serverComponentName << ":" << serverInterfaceProvidedName
                                     << ", could not find end-user interface." << std::endl;
            return false;
        }
        bool success = false;
        // If the server is this component (ManagerComponentClient), or if this component is not active,
        // we can directly call the methods. Note that we could use the StateChange mutex to make sure that the state
        // does not change during execution of this method, but that is unlikely.
        if ((serverComponentName == GetName()) || !IsRunning()) {
            mtsEventHandlerList eventList(serverInterfaceProvided);
            clientInterfaceRequired->GetEventList(eventList);
            serverInterfaceProvided->RemoveObserverList(eventList, eventList);
            success = clientInterfaceRequired->CheckEventList(eventList);
            // Now, pause/stop the client component.  In the future, the component could be left
            // running if the required interface is MTS_OPTIONAL.
            clientComponent->Suspend();
            clientInterfaceRequired->DetachCommands();
            if (serverInterfaceProvided->RemoveEndUserInterface(endUserInterface, clientInterfaceRequiredName) != 0)
                success = false;
        }
        else {
            InterfaceComponentFunctionType * serverFunctionSet = InterfaceComponentFunctionMap.GetItem(serverComponentName);
            if (!serverFunctionSet) {
                CMN_LOG_CLASS_RUN_ERROR << "DisconnectLocally: failed to get function set for " << serverComponentName << std::endl;
                return false;
            }
            if (!serverFunctionSet->RemoveEndUserInterface.IsValid()) {
                CMN_LOG_CLASS_RUN_ERROR << "DisconnectLocally: RemoveEndUserInterface invalid for " << serverComponentName << std::endl;
                return false;
            }
            if (!serverFunctionSet->RemoveObserverList.IsValid()) {
                CMN_LOG_CLASS_RUN_ERROR << "DisconnectLocally: RemoveObserverList invalid for " << serverComponentName << std::endl;
                return false;
            }
#if 0
            if (!serverFunctionSet->ComponentStop.IsValid()) {
                CMN_LOG_CLASS_RUN_ERROR << "DisconnectLocally: ComponentStop invalid for " << serverComponentName << std::endl;
                return false;
            }
#endif
            mtsEventHandlerList eventList(serverInterfaceProvided);
            clientInterfaceRequired->GetEventList(eventList);
            serverFunctionSet->RemoveObserverList(eventList, eventList);
            success = clientInterfaceRequired->CheckEventList(eventList);
            // Now, pause/stop the client component.  In the future, the component could be left
            // running if the required interface is MTS_OPTIONAL.
            clientComponent->Suspend(); // Could instead use serverFunctionSet->ComponentStop
            clientInterfaceRequired->DetachCommands();
            mtsEndUserInterfaceArg endUserInterfaceArg(serverInterfaceProvided, clientInterfaceRequiredName, endUserInterface);
            serverFunctionSet->RemoveEndUserInterface(endUserInterfaceArg, endUserInterfaceArg);
            if (endUserInterfaceArg.EndUserInterface != 0) {
                CMN_LOG_CLASS_RUN_WARNING << "DisconnectLocally: failed to remove end-user interface for " << serverComponentName << std::endl;
                success = false;
            }
        }
        if (success) {
            CMN_LOG_CLASS_INIT_VERBOSE << "DisconnectLocally: successfully disconnected required/provided: "
                                       << clientComponentName << ":" << clientInterfaceRequiredName << " - "
                                       << serverComponentName << ":" << serverInterfaceProvidedName << std::endl;
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "DisconnectLocally: failed to disconnect required/provided: "
                                     << clientComponentName << ":" << clientInterfaceRequiredName << " - "
                                     << serverComponentName << ":" << serverInterfaceProvidedName << std::endl;
            return false;
        }
    }
    else {  // Input/Output connection
        mtsInterfaceOutput *serverInterfaceOutput = dynamic_cast<mtsInterfaceOutput *>(serverInterfaceProvidedOrOutput);
        if (!serverInterfaceOutput) {
            CMN_LOG_CLASS_INIT_ERROR << "DisconnectLocally: failed to disconnect interfaces: "
                                     << clientComponentName << ":" << clientInterfaceRequiredName << " - "
                                     << serverComponentName << ":" << serverInterfaceProvidedName
                                     << ", server does not have provided or output interface." << std::endl;
            return false;
        }
        mtsInterfaceInput *clientInterfaceInput = clientComponent->GetInterfaceInput(clientInterfaceRequiredName);
        if (!clientInterfaceInput) {
            CMN_LOG_CLASS_INIT_ERROR << "DisconnectLocally: failed to disconnect interfaces: "
                                     << clientComponentName << ":" << clientInterfaceRequiredName << " - "
                                     << serverComponentName << ":" << serverInterfaceProvidedName
                                     << ", client does not have input interface." << std::endl;
            return false;
        }
        if (clientInterfaceInput->Disconnect()) {
            CMN_LOG_CLASS_INIT_VERBOSE << "DisconnectLocally: component \""
                                       << this->GetName()
                                       << "\" input interface \"" << clientInterfaceRequiredName
                                       << "\" successfully disconnected from output interface \""
                                       << serverInterfaceProvidedName << "\"" << std::endl;
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "DisconnectLocally: component \""
                                     << this->GetName()
                                     << "\" input interface \"" << clientInterfaceRequiredName
                                     << "\" failed to disconnect from output interface \""
                                     << serverInterfaceProvidedName << "\"" << std::endl;
            return false;
        }
    }

    return true;
}

bool mtsManagerComponentClient::AddInterfaceComponent(void)
{
    // InterfaceComponent's required interface is not created here but is created
    // when a user component with internal interfaces connects to the manager 
    // component client.  
    // See mtsManagerComponentClient::AddNewClientComponent() for the dynamic 
    // creation of required interfaces.

    // Add provided interface to which InterfaceInternal's required interface connects.
    std::string interfaceName = mtsManagerComponentBase::InterfaceNames::InterfaceComponentProvided;
    // Return if provided interface already exists
    if (GetInterfaceProvided(interfaceName))
        return true;
    mtsInterfaceProvided * provided = AddInterfaceProvided(interfaceName);
    if (!provided) {
        CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceComponent: failed to add \"Component\" provided interface: " << interfaceName << std::endl;
        return false;
    }

    provided->AddCommandWrite(&mtsManagerComponentClient::InterfaceComponentCommands_ComponentCreate,
                              this, mtsManagerComponentBase::CommandNames::ComponentCreate);
    provided->AddCommandWrite(&mtsManagerComponentClient::InterfaceComponentCommands_ComponentConnect,
                              this, mtsManagerComponentBase::CommandNames::ComponentConnect);
    provided->AddCommandWrite(&mtsManagerComponentClient::InterfaceComponentCommands_ComponentDisconnect,
                              this, mtsManagerComponentBase::CommandNames::ComponentDisconnect);
    provided->AddCommandWrite(&mtsManagerComponentClient::InterfaceComponentCommands_ComponentStart,
                              this, mtsManagerComponentBase::CommandNames::ComponentStart);
    provided->AddCommandWrite(&mtsManagerComponentClient::InterfaceComponentCommands_ComponentStop,
                              this, mtsManagerComponentBase::CommandNames::ComponentStop);
    provided->AddCommandWrite(&mtsManagerComponentClient::InterfaceComponentCommands_ComponentResume,
                              this, mtsManagerComponentBase::CommandNames::ComponentResume);
    provided->AddCommandQualifiedRead(&mtsManagerComponentClient::InterfaceComponentCommands_ComponentGetState,
                                      this, mtsManagerComponentBase::CommandNames::ComponentGetState);
    provided->AddCommandRead(&mtsManagerComponentClient::InterfaceComponentCommands_GetNamesOfProcesses,
                              this, mtsManagerComponentBase::CommandNames::GetNamesOfProcesses);
    provided->AddCommandQualifiedRead(&mtsManagerComponentClient::InterfaceComponentCommands_GetNamesOfComponents,
                              this, mtsManagerComponentBase::CommandNames::GetNamesOfComponents);
    provided->AddCommandQualifiedRead(&mtsManagerComponentClient::InterfaceComponentCommands_GetNamesOfInterfaces,
                              this, mtsManagerComponentBase::CommandNames::GetNamesOfInterfaces);
    provided->AddCommandRead(&mtsManagerComponentClient::InterfaceComponentCommands_GetListOfConnections,
                              this, mtsManagerComponentBase::CommandNames::GetListOfConnections);
    provided->AddEventWrite(this->InterfaceComponentEvents_AddComponent, 
                            mtsManagerComponentBase::EventNames::AddComponent, mtsDescriptionComponent());
    provided->AddEventWrite(this->InterfaceComponentEvents_ChangeState, 
                            mtsManagerComponentBase::EventNames::ChangeState, mtsComponentStateChange());
    provided->AddEventWrite(this->InterfaceComponentEvents_AddConnection,
                            mtsManagerComponentBase::EventNames::AddConnection, mtsDescriptionConnection());
    provided->AddEventWrite(this->InterfaceComponentEvents_RemoveConnection,
                            mtsManagerComponentBase::EventNames::RemoveConnection, mtsDescriptionConnection());
    
    CMN_LOG_CLASS_INIT_VERBOSE << "AddInterfaceComponent: successfully added \"Component\" interfaces" << std::endl;

    // Create an end user interface for internal invocations (via GeneralInterface). These internal invocations
    // are made by methods of this class, so we cannot assume that there is only one thread. Thus, to achieve thread-safety
    // we need to use osaMutex.
    mtsInterfaceProvided *interfaceProvidedToSelf = provided->GetEndUserInterface("Self");
    if (interfaceProvidedToSelf) {
        GeneralInterface.ComponentConnect.Bind(interfaceProvidedToSelf->GetCommandWrite(
                                               mtsManagerComponentBase::CommandNames::ComponentConnect));
    }

    return true;
}

bool mtsManagerComponentClient::AddInterfaceLCM(void)
{
    // Add required interface
    std::string interfaceName = mtsManagerComponentBase::InterfaceNames::InterfaceLCMRequired;
    mtsInterfaceRequired * required = AddInterfaceRequired(interfaceName);
    if (!required) {
        CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceLCM: failed to add \"LCM\" required interface: " << interfaceName << std::endl;
        return false;
    }
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentCreate,
                          InterfaceLCMFunction.ComponentCreate);
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentConnect,
                          InterfaceLCMFunction.ComponentConnect);
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentDisconnect,
                          InterfaceLCMFunction.ComponentDisconnect);
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentStart,
                          InterfaceLCMFunction.ComponentStart);
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentStop,
                          InterfaceLCMFunction.ComponentStop);
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentResume,
                          InterfaceLCMFunction.ComponentResume);
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentGetState,
                          InterfaceLCMFunction.ComponentGetState);
    required->AddFunction(mtsManagerComponentBase::CommandNames::GetNamesOfProcesses,
                          InterfaceLCMFunction.GetNamesOfProcesses);
    required->AddFunction(mtsManagerComponentBase::CommandNames::GetNamesOfComponents,
                          InterfaceLCMFunction.GetNamesOfComponents);
    required->AddFunction(mtsManagerComponentBase::CommandNames::GetNamesOfInterfaces,
                          InterfaceLCMFunction.GetNamesOfInterfaces);
    required->AddFunction(mtsManagerComponentBase::CommandNames::GetListOfConnections,
                          InterfaceLCMFunction.GetListOfConnections);
    // It is not necessary to queue the events because we are just passing them along (it would not
    // hurt to queue them, either).
    required->AddEventHandlerWrite(&mtsManagerComponentClient::HandleAddComponentEvent, this, 
                                   mtsManagerComponentBase::EventNames::AddComponent, MTS_EVENT_NOT_QUEUED);
    required->AddEventHandlerWrite(&mtsManagerComponentClient::HandleChangeStateEvent, this, 
                                   mtsManagerComponentBase::EventNames::ChangeState, MTS_EVENT_NOT_QUEUED);
    required->AddEventHandlerWrite(&mtsManagerComponentClient::HandleAddConnectionEvent, this, 
                                   mtsManagerComponentBase::EventNames::AddConnection, MTS_EVENT_NOT_QUEUED);
    required->AddEventHandlerWrite(&mtsManagerComponentClient::HandleRemoveConnectionEvent, this, 
                                   mtsManagerComponentBase::EventNames::RemoveConnection, MTS_EVENT_NOT_QUEUED);

    // Add provided interface
    interfaceName = mtsManagerComponentBase::InterfaceNames::InterfaceLCMProvided;
    mtsInterfaceProvided * provided = AddInterfaceProvided(interfaceName);
    if (!provided) {
        CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceLCM: failed to add \"LCM\" required interface: " << interfaceName << std::endl;
        return false;
    }
    provided->AddCommandWrite(&mtsManagerComponentClient::InterfaceLCMCommands_ComponentCreate,
                             this, mtsManagerComponentBase::CommandNames::ComponentCreate);
    provided->AddCommandWrite(&mtsManagerComponentClient::InterfaceLCMCommands_ComponentConnect,
                             this, mtsManagerComponentBase::CommandNames::ComponentConnect);
    provided->AddCommandWrite(&mtsManagerComponentClient::InterfaceLCMCommands_ComponentDisconnect,
                             this, mtsManagerComponentBase::CommandNames::ComponentDisconnect);
    provided->AddCommandWrite(&mtsManagerComponentClient::InterfaceLCMCommands_ComponentStart,
                             this, mtsManagerComponentBase::CommandNames::ComponentStart);
    provided->AddCommandWrite(&mtsManagerComponentClient::InterfaceLCMCommands_ComponentStop,
                             this, mtsManagerComponentBase::CommandNames::ComponentStop);
    provided->AddCommandWrite(&mtsManagerComponentClient::InterfaceLCMCommands_ComponentResume,
                             this, mtsManagerComponentBase::CommandNames::ComponentResume);
    provided->AddCommandQualifiedRead(&mtsManagerComponentClient::InterfaceLCMCommands_ComponentGetState,
                             this, mtsManagerComponentBase::CommandNames::ComponentGetState);
    provided->AddEventWrite(this->InterfaceLCMEvents_ChangeState, 
                            mtsManagerComponentBase::EventNames::ChangeState, mtsComponentStateChange());
    CMN_LOG_CLASS_INIT_VERBOSE << "AddInterfaceLCM: successfully added \"LCM\" interfaces" << std::endl;

    return true;
}

bool mtsManagerComponentClient::AddNewClientComponent(const std::string & clientComponentName)
{
    if (InterfaceComponentFunctionMap.FindItem(clientComponentName)) {
        CMN_LOG_CLASS_INIT_VERBOSE << "AddNewClientComponent: component is already known: "
            << "\"" << clientComponentName << "\"" << std::endl;
        return true;
    }

    // Create a new set of function objects
    InterfaceComponentFunctionType * newFunctionSet = new InterfaceComponentFunctionType;

    const std::string interfaceName = GetNameOfInterfaceComponentRequired(clientComponentName);
    mtsInterfaceRequired * required = AddInterfaceRequired(interfaceName);
    if (!required) {
        CMN_LOG_CLASS_INIT_ERROR << "AddNewClientComponent: failed to create \"Component\" required interface: " << interfaceName << std::endl;
        return false;
    }
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentStop,
                          newFunctionSet->ComponentStop);
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentResume,
                          newFunctionSet->ComponentResume);
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentGetState,
                          newFunctionSet->ComponentGetState);
    required->AddFunction(mtsManagerComponentBase::CommandNames::GetEndUserInterface,
                          newFunctionSet->GetEndUserInterface);
    required->AddFunction(mtsManagerComponentBase::CommandNames::AddObserverList,
                          newFunctionSet->AddObserverList);
    required->AddFunction(mtsManagerComponentBase::CommandNames::RemoveEndUserInterface,
                          newFunctionSet->RemoveEndUserInterface);
    required->AddFunction(mtsManagerComponentBase::CommandNames::RemoveObserverList,
                          newFunctionSet->RemoveObserverList);
    required->AddEventHandlerWrite(&mtsManagerComponentClient::HandleChangeStateFromComponent, this, 
                                   mtsManagerComponentBase::EventNames::ChangeState);

    // Remember a required interface (InterfaceComponent's required interface) 
    // to connect it to the provided interface (InterfaceInternals's provided 
    // interface).
    if (!InterfaceComponentFunctionMap.AddItem(clientComponentName, newFunctionSet)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddNewClientComponent: failed to add \"Component\" required interface: " 
            << "\"" << clientComponentName << "\", " << interfaceName << std::endl;
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "AddNewClientComponent: successfully added new client component: " 
        << clientComponentName << std::endl;

    return true;
}

bool mtsManagerComponentClient::Connect(const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
                                        const std::string & serverComponentName, const std::string & serverInterfaceProvidedName)
{
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    const std::string processName = LCM->GetProcessName();
    mtsDescriptionConnection arg(processName, clientComponentName, clientInterfaceRequiredName,
                                 processName, serverComponentName, serverInterfaceProvidedName);
    if (!IsRunning()) {
        InterfaceComponentCommands_ComponentConnect(arg);
    }
    else {
        if (!GeneralInterface.ComponentConnect.IsValid()) {
            CMN_LOG_CLASS_INIT_WARNING << "Connect: GeneralInterface not yet valid, initializing." << std::endl;
            AddInterfaceComponent();
            if (!GeneralInterface.ComponentConnect.IsValid()) {
                CMN_LOG_CLASS_INIT_ERROR << "Connect: Failed to initialize ComponentConnect." << std::endl;
                return false;
            }
        }
        GeneralInterface.Mutex.Lock();
        CMN_LOG_CLASS_INIT_WARNING << "mtsManagerComponentClient calling ComponentConnect" << std::endl;
        GeneralInterface.ComponentConnect(arg);
        GeneralInterface.Mutex.Unlock();
        // PK: What about return value? Maybe Connect should be WriteReturn method?
    }
    return true;
}

void mtsManagerComponentClient::InterfaceComponentCommands_ComponentCreate(const mtsDescriptionComponent & arg)
{
    if (!InterfaceLCMFunction.ComponentCreate.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_ComponentCreate: failed to execute \"Component Create\"" << std::endl;
        return;
    }

    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    const std::string nameOfThisLCM = LCM->GetProcessName();
    if (LCM->GetConfiguration() == mtsManagerLocal::LCM_CONFIG_STANDALONE || 
        nameOfThisLCM == arg.ProcessName) 
    {
        InterfaceLCMCommands_ComponentCreate(arg);
        return;
    } else {
        //InterfaceLCMFunction.ComponentCreate.ExecuteBlocking(arg);
        InterfaceLCMFunction.ComponentCreate(arg);
    }
}

void mtsManagerComponentClient::InterfaceComponentCommands_ComponentConnect(const mtsDescriptionConnection & arg)
{
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    const std::string nameOfThisLCM = LCM->GetProcessName();
    if (LCM->GetConfiguration() == mtsManagerLocal::LCM_CONFIG_STANDALONE ||
        (nameOfThisLCM == arg.Client.ProcessName && nameOfThisLCM == arg.Server.ProcessName))
    {
        InterfaceLCMCommands_ComponentConnect(arg);
        return;
    } else {
        if (!InterfaceLCMFunction.ComponentConnect.IsValid()) {
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_ComponentConnect: failed to execute \"Component Connect\"" << std::endl;
            return;
        }
        //InterfaceLCMFunction.ComponentConnect.ExecuteBlocking(arg);
        InterfaceLCMFunction.ComponentConnect(arg);
    }
}

void mtsManagerComponentClient::InterfaceComponentCommands_ComponentDisconnect(const mtsDescriptionConnection & arg)
{
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    const std::string nameOfThisLCM = LCM->GetProcessName();
    if (LCM->GetConfiguration() == mtsManagerLocal::LCM_CONFIG_STANDALONE ||
        (nameOfThisLCM == arg.Client.ProcessName && nameOfThisLCM == arg.Server.ProcessName))
    {
        InterfaceLCMCommands_ComponentDisconnect(arg);
        return;
    } else {
        if (!InterfaceLCMFunction.ComponentDisconnect.IsValid()) {
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_ComponentDsconnect: failed to execute \"Component Disconnect\"" << std::endl;
            return;
        }
        //InterfaceLCMFunction.ComponentDisconnect.ExecuteBlocking(arg);
        InterfaceLCMFunction.ComponentDisconnect(arg);
    }
}

void mtsManagerComponentClient::InterfaceComponentCommands_ComponentStart(const mtsComponentStatusControl & arg)
{
    if (!InterfaceLCMFunction.ComponentStart.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_ComponentStart: failed to execute \"Component Start\"" << std::endl;
        return;
    }

    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    const std::string nameOfThisLCM = LCM->GetProcessName();
    if (LCM->GetConfiguration() == mtsManagerLocal::LCM_CONFIG_STANDALONE ||
        nameOfThisLCM == arg.ProcessName) 
    {
        // Check if the component specified exists
        if (!LCM->GetComponent(arg.ComponentName)) {
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_ComponentStart: failed to execute \"Component Start\"" << std::endl;
            return;
        }

        InterfaceLCMCommands_ComponentStart(arg);
        return;
    } else {
        //InterfaceLCMFunction.ComponentStart.ExecuteBlocking(arg);
        InterfaceLCMFunction.ComponentStart(arg);
    }
}

void mtsManagerComponentClient::InterfaceComponentCommands_ComponentStop(const mtsComponentStatusControl & arg)
{
    if (!InterfaceLCMFunction.ComponentStop.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_ComponentStop: failed to execute \"Component Stop\"" << std::endl;
        return;
    }

    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    const std::string nameOfThisLCM = LCM->GetProcessName();
    if (LCM->GetConfiguration() == mtsManagerLocal::LCM_CONFIG_STANDALONE ||
        LCM->GetProcessName() == arg.ProcessName) 
    {
        // Check if the component specified exists
        if (!LCM->GetComponent(arg.ComponentName)) {
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_ComponentStop: failed to execute \"Component Stop\"" << std::endl;
            return;
        }

        InterfaceLCMCommands_ComponentStop(arg);
        return;
    } else {
        //InterfaceLCMFunction.ComponentStop.ExecuteBlocking(arg);
        InterfaceLCMFunction.ComponentStop(arg);
    }
}

void mtsManagerComponentClient::InterfaceComponentCommands_ComponentResume(const mtsComponentStatusControl & arg)
{
    if (!InterfaceLCMFunction.ComponentResume.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_ComponentResume: failed to execute \"Component Resume\"" << std::endl;
        return;
    }

    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    const std::string nameOfThisLCM = LCM->GetProcessName();
    if (LCM->GetConfiguration() == mtsManagerLocal::LCM_CONFIG_STANDALONE ||
        LCM->GetProcessName() == arg.ProcessName)
    {
        // Check if the component specified exists
        if (!LCM->GetComponent(arg.ComponentName)) {
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_ComponentResume: failed to execute \"Component Resume\"" << std::endl;
            return;
        }

        InterfaceLCMCommands_ComponentResume(arg);
        return;
    } else {
        //InterfaceLCMFunction.ComponentResume.ExecuteBlocking(arg);
        InterfaceLCMFunction.ComponentResume(arg);
    }
}

void mtsManagerComponentClient::InterfaceComponentCommands_ComponentGetState(const mtsDescriptionComponent &component,
                                                                             mtsComponentState &state) const
{
    if (!InterfaceLCMFunction.ComponentGetState.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_ComponentGetState: failed to execute \"Component GetState\"" << std::endl;
        return;
    }
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    const std::string nameOfThisLCM = LCM->GetProcessName();
    if (LCM->GetConfiguration() == mtsManagerLocal::LCM_CONFIG_STANDALONE ||
        LCM->GetProcessName() == component.ProcessName)
    {
        // Check if the component specified exists
        if (!LCM->GetComponent(component.ComponentName)) {
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_ComponentGetState: failed to execute \"Component Resume\"" << std::endl;
            return;
        }

        InterfaceLCMCommands_ComponentGetState(component, state);
        return;
    } else {
        InterfaceLCMFunction.ComponentGetState(component, state);
    }
}

void mtsManagerComponentClient::InterfaceComponentCommands_GetNamesOfProcesses(std::vector<std::string> & names) const
{
    if (!InterfaceLCMFunction.GetNamesOfProcesses.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_GetNamesOfProcesses: failed to execute \"GetNamesOfProcesses\"" << std::endl;
        return;
    }

    InterfaceLCMFunction.GetNamesOfProcesses(names);
}

void mtsManagerComponentClient::InterfaceComponentCommands_GetNamesOfComponents(const std::string & processName,
                                                                                std::vector<std::string> & names) const
{
    if (!InterfaceLCMFunction.GetNamesOfComponents.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_GetNamesOfComponents: failed to execute \"GetNamesOfComponents\"" << std::endl;
        return;
    }

    InterfaceLCMFunction.GetNamesOfComponents(processName, names);
}

void mtsManagerComponentClient::InterfaceComponentCommands_GetNamesOfInterfaces(
    const mtsDescriptionComponent & component, mtsDescriptionInterface & interfaces) const
{
    if (!InterfaceLCMFunction.GetNamesOfInterfaces.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_GetNamesOfInterfaces: failed to execute \"GetNamesOfInterfaces\"" << std::endl;
        return;
    }

    InterfaceLCMFunction.GetNamesOfInterfaces(component, interfaces);
}

void mtsManagerComponentClient::InterfaceComponentCommands_GetListOfConnections(std::vector <mtsDescriptionConnection> & listOfConnections) const
{
    if (!InterfaceLCMFunction.GetListOfConnections.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_GetListOfConnections: failed to execute \"GetListOfConnections\"" << std::endl;
        return;
    }

    InterfaceLCMFunction.GetListOfConnections(listOfConnections);
}

void mtsManagerComponentClient::InterfaceLCMCommands_ComponentCreate(const mtsDescriptionComponent & arg)
{
    // Steps to create a component dynamically :
    // 1. Create a component
    // 2. Add the created component to the local component manager
    // 3. Add internal interfaces to the component (InterfaceInternal).
    // 4. Create InterfaceComponent's required interface which connects to 
    //    InterfaceInternal's provided interface.
    // 5. Connect InterfaceInternal's interfaces to InterfaceComponent's
    //    interfaces.
    if (!CreateAndAddNewComponent(arg.ClassName, arg.ComponentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_ComponentCreate: failed to execute \"ComponentCreate\": " << arg << std::endl;
        return;
    }

    CMN_LOG_CLASS_RUN_VERBOSE << "InterfaceLCMCommands_ComponentCreate: successfully created new component: " << arg << std::endl;
}

void mtsManagerComponentClient::InterfaceLCMCommands_ComponentConnect(const mtsDescriptionConnection & arg)
{
    // Try to connect interfaces as requested
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();

    if (arg.Client.ProcessName != arg.Server.ProcessName) {
#if CISST_MTS_HAS_ICE
        // PK TODO: Need to fix this to be thread-safe
        if (!LCM->Connect(arg.Client.ProcessName, arg.Client.ComponentName, arg.Client.InterfaceName,
                          arg.Server.ProcessName, arg.Server.ComponentName, arg.Server.InterfaceName)) {
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_ComponentConnect: failed to execute \"Component Connect\": " << arg << std::endl;
        }
        else
            CMN_LOG_CLASS_RUN_VERBOSE << "InterfaceLCMCommands_ComponentConnect: successfully connected: " << arg << std::endl;
#else
        CMN_LOG_CLASS_RUN_ERROR << "Cannot connect to external process without CISST_MTS_HAS_ICE, arg = " << arg << std::endl;
#endif
        return;
    }

    int connectionId = LCM->ConnectSetup(arg.Client.ComponentName, arg.Client.InterfaceName,
                                         arg.Server.ComponentName, arg.Server.InterfaceName);
    if (connectionId < 0) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_ComponentConnect: failed to execute \"Component Connect\": " << arg << std::endl;
        return;
    }
    ConnectLocally(arg.Client.ComponentName, arg.Client.InterfaceName,
                   arg.Server.ComponentName, arg.Server.InterfaceName, arg.Client.ProcessName);

    LCM->ConnectNotify(connectionId, arg.Client.ComponentName, arg.Client.InterfaceName,
                                     arg.Server.ComponentName, arg.Server.InterfaceName);

    CMN_LOG_CLASS_RUN_VERBOSE << "InterfaceLCMCommands_ComponentConnect: successfully connected: " << arg << std::endl;
}

void mtsManagerComponentClient::InterfaceLCMCommands_ComponentDisconnect(const mtsDescriptionConnection & arg)
{
    // Try to disconnect interfaces as requested
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();

    // PK TODO: Would be nice to be able to disconnect using just connectionId (e.g., from ComponentViewer)
    if (arg.Client.ProcessName != arg.Server.ProcessName) {
#if CISST_MTS_HAS_ICE
        // PK TODO: Need to fix this to be thread-safe
        if (!LCM->Disconnect(arg.Client.ProcessName, arg.Client.ComponentName, arg.Client.InterfaceName,
                             arg.Server.ProcessName, arg.Server.ComponentName, arg.Server.InterfaceName)) {
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_ComponentDisconnect: failed to execute \"Component Disonnect\": " << arg << std::endl;
        }
        else
            CMN_LOG_CLASS_RUN_VERBOSE << "InterfaceLCMCommands_ComponentDisconnect: successfully disconnected: " << arg << std::endl;
#else
        CMN_LOG_CLASS_RUN_ERROR << "Cannot disconnect from external process without CISST_MTS_HAS_ICE, arg = " << arg << std::endl;
#endif
        return;
    }

    if (!LCM->Disconnect(arg.Client.ComponentName, arg.Client.InterfaceName,
                         arg.Server.ComponentName, arg.Server.InterfaceName))
    {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_ComponentDisconnect: failed to execute \"Component Disconnect\": " << arg << std::endl;
        return;
    }

    if (DisconnectLocally(arg.Client.ComponentName, arg.Client.InterfaceName,
                          arg.Server.ComponentName, arg.Server.InterfaceName, arg.Client.ProcessName))
        CMN_LOG_CLASS_RUN_VERBOSE << "InterfaceLCMCommands_ComponentDisconnect: successfully disconnected: " << arg << std::endl;
}

void mtsManagerComponentClient::InterfaceLCMCommands_ComponentStart(const mtsComponentStatusControl & arg)
{
    // Check if command is for this component (MCC)
    if (arg.ComponentName == this->GetName()) {
        CMN_LOG_CLASS_RUN_WARNING << "ComponentStart for " << arg.ComponentName << " ignored." << std::endl;
        return;
    }
    // Create internal thread (if needed)
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    mtsComponent * component = LCM->GetComponent(arg.ComponentName);
    if (!component) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_ComponentStart - no component found: "
            << arg.ComponentName << std::endl;
        return;
    }

    if (component->GetState() == mtsComponentState::CONSTRUCTED) {    
        // Start an internal thread (if needed)
        component->Create();
        // Wait for internal thread to be created
        osaSleep(arg.DelayInSecond);
    }

    // Start the component
    component->Start();
}

void mtsManagerComponentClient::InterfaceLCMCommands_ComponentStop(const mtsComponentStatusControl & arg)
{
    // Check if command is for this component (MCC)
    if (arg.ComponentName == this->GetName()) {
        CMN_LOG_CLASS_RUN_WARNING << "ComponentStop for " << arg.ComponentName << " ignored." << std::endl;
        return;
    }
    // Get a set of function objects that are bound to the InterfaceLCM's provided
    // interface.
    InterfaceComponentFunctionType * functionSet = InterfaceComponentFunctionMap.GetItem(arg.ComponentName);
    if (!functionSet) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_ComponentStop: failed to execute \"Component Stop\"" << std::endl;
        return;
    }
    if (!functionSet->ComponentStop.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_ComponentStop: failed to execute \"Component Stop\"" << arg << std::endl;
    }

    // MJ: This Component Stop command could be executed through local component 
    // manager but it is not thread safe.  For thread-safe stop/resume, we
    // use the cisstMultiTask's thread-safe command pattern instead.
    //functionSet->ComponentStop.ExecuteBlocking(arg);
    functionSet->ComponentStop();
}

void mtsManagerComponentClient::InterfaceLCMCommands_ComponentResume(const mtsComponentStatusControl & arg)
{
    // Check if command is for this component (MCC)
    if (arg.ComponentName == this->GetName()) {
        CMN_LOG_CLASS_RUN_WARNING << "ComponentResume for " << arg.ComponentName << " ignored." << std::endl;
        return;
    }
    // Create internal thread (if needed)
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    mtsComponent * component = LCM->GetComponent(arg.ComponentName);
    if (!component) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_ComponentResume - no component found: "
            << arg.ComponentName << std::endl;
        return;
    }
    
    // Wait if desired
    osaSleep(arg.DelayInSecond);

    // Resume (Start) the component
    component->Start();
}

void mtsManagerComponentClient::InterfaceLCMCommands_ComponentGetState(const mtsDescriptionComponent &component,
                                                                       mtsComponentState &state) const
{
    // Check if command is for this component (MCC)
    if (component.ComponentName == this->GetName()) {
        //GetState(state);
        //For now, always return active for MCC
        state = mtsComponentState::ACTIVE;
        return;
    }
    // Check if command is for MCS (TODO: should also check process name)
    if (component.ComponentName == mtsManagerComponentBase::ComponentNames::ManagerComponentServer) {
        //For now, always return active for MCS
        state = mtsComponentState::ACTIVE;
        return;
    }
    // Get a set of function objects that are bound to the InterfaceLCM's provided
    // interface.
    InterfaceComponentFunctionType * functionSet = InterfaceComponentFunctionMap.GetItem(component.ComponentName);
    if (!functionSet) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_ComponentGetState: failed to find required interface for component "
                                << component.ComponentName << std::endl;
        return;
    }
    if (!functionSet->ComponentGetState.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_ComponentGetState: failed to execute \"Component GetState\""
                                << component << std::endl;
        return;
    }

    functionSet->ComponentGetState(state);
}

void mtsManagerComponentClient::HandleAddComponentEvent(const mtsDescriptionComponent &component)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "MCC AddComponent event, component = " << component << std::endl;
    // Generate event to connected components
    InterfaceComponentEvents_AddComponent(component);
}

void mtsManagerComponentClient::HandleChangeStateEvent(const mtsComponentStateChange &componentStateChange)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "MCC ChangeState event from MCS, component = " << componentStateChange.ComponentName << std::endl;
    // Generate event to connected components
    InterfaceComponentEvents_ChangeState(componentStateChange);
}

void mtsManagerComponentClient::HandleAddConnectionEvent(const mtsDescriptionConnection &connection)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "MCC AddConnection event, connection = " << connection << std::endl;
    // Generate event to connected components
    InterfaceComponentEvents_AddConnection(connection);
}

void mtsManagerComponentClient::HandleRemoveConnectionEvent(const mtsDescriptionConnection &connection)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "MCC RemoveConnection event, connection = " << connection << std::endl;
    // Generate event to connected components
    InterfaceComponentEvents_RemoveConnection(connection);
}

void mtsManagerComponentClient::HandleChangeStateFromComponent(const mtsComponentStateChange & componentStateChange)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "MCC ChangeState event, component = " << componentStateChange.ComponentName << std::endl;
    // Generate event to connected components
    InterfaceLCMEvents_ChangeState(componentStateChange);
}
