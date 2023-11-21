/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Min Yang Jung
  Created on: 2010-08-29

  (C) Copyright 2010-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsConfig.h>
#include <cisstMultiTask/mtsManagerComponentClient.h>
#include <cisstMultiTask/mtsManagerComponentServer.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsInterfaceOutput.h>
#include <cisstMultiTask/mtsInterfaceInput.h>
#include <cisstOSAbstraction/osaGetTime.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstOSAbstraction/osaDynamicLoader.h>
#include <cisstCommon/cmnUnits.h>

CMN_IMPLEMENT_SERVICES_DERIVED(mtsManagerComponentClient, mtsManagerComponentBase);

mtsManagerComponentClient::mtsManagerComponentClient(const std::string & componentName)
    : mtsManagerComponentBase(componentName),
      MCSReady(false),
      InterfaceComponentFunctionMap("InterfaceComponentFunctionMap")
{
    InterfaceComponentFunctionMap.SetOwner(*this);
}

mtsManagerComponentClient::~mtsManagerComponentClient()
{
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

bool mtsManagerComponentClient::ConnectLocally(const std::string & clientComponentName, const std::string & clientInterfaceName,
                                               const std::string & serverComponentName, const std::string & serverInterfaceName,
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

    // first, try to figure out if the interface is either provided or output
    mtsInterfaceProvided * serverInterfaceProvided = serverComponent->GetInterfaceProvided(serverInterfaceName);
    mtsInterfaceOutput * serverInterfaceOutput = serverComponent->GetInterfaceOutput(serverInterfaceName);
    if (!serverInterfaceProvided && !serverInterfaceOutput) {
        // test for swapped interfaces
        // TODO: This duplicates a test already done in the GCM
        CMN_LOG_CLASS_INIT_DEBUG << "ConnectLocally: looking for provided/output interface in first component as well" << std::endl;
        serverInterfaceProvided = clientComponent->GetInterfaceProvided(clientInterfaceName);
        serverInterfaceOutput = clientComponent->GetInterfaceOutput(clientInterfaceName);
        if (!serverInterfaceProvided && !serverInterfaceOutput) {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectLocally: failed to get provided/output interface \"" << serverInterfaceName << "\""
                                     << " in component \"" << serverComponentName << "\"" << std::endl;
            return false;
        } else {
            CMN_LOG_CLASS_INIT_DEBUG << "ConnectLocally: Swapping client/server" << std::endl;
            return ConnectLocally(serverComponentName, serverInterfaceName, clientComponentName, clientInterfaceName, clientProcessName);
        }
    }

    // Now, handle the connections.  First, we look for connection between required/provided interfaces.  Then, we look
    // for connection between input/output interfaces.
    if (serverInterfaceProvided) {
        mtsInterfaceRequired * clientInterfaceRequired = clientComponent->GetInterfaceRequired(clientInterfaceName);
        if (!clientInterfaceRequired) {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectLocally: failed to connect interfaces: "
                                     << clientComponentName << ":" << clientInterfaceName << " - "
                                     << serverComponentName << ":" << serverInterfaceName
                                     << ", client does not have required interface." << std::endl;
            return false;
        }
        bool success = false;
        // If the server is this component (ManagerComponentClient), or if the server component is not active,
        // we can use the previous implementation (mtsInterfaceRequired::ConnectTo), which directly calls the methods.
        // Note that we could use the StateChange mutex to make sure that the state
        // does not change during execution of this method, but that is unlikely.
        if ((serverComponentName == GetName()) ||
            (serverComponentName == mtsManagerComponentBase::GetNameOfManagerComponentServer()) ||   // PK TEMP
            !serverComponent->IsRunning()) {
            success = clientInterfaceRequired->ConnectTo(serverInterfaceProvided);
        }
        else {
            InterfaceComponentFunctionType * serverFunctionSet = InterfaceComponentFunctionMap.GetItem(serverComponentName);
            if (!serverFunctionSet) {
                CMN_LOG_CLASS_INIT_ERROR << "ConnectLocally: failed to connect interfaces: "
                                         << clientComponentName << ":" << clientInterfaceName << " - "
                                         << serverComponentName << ":" << serverInterfaceName
                                         << ", failed to get function set for " << serverComponentName << std::endl;
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
            mtsEndUserInterfaceArg endUserInterfaceArg(reinterpret_cast<size_t>(serverInterfaceProvided), clientInterfaceName, 0);

#if (CISST_OS == CISST_LINUX_XENOMAI && CISST_MTS_64BIT)
            {
                // See void mtsComponent::InterfaceInternalCommands_GetEndUserInterface()
                // 11/20/13: Patch for cisstDataGenerator
                //endUserInterfaceArg.EndUserInterface = endUserInterfaceArg.OriginalInterface->GetEndUserInterface(clientInterfaceName);
                mtsInterfaceProvided * originalInterface = reinterpret_cast<mtsInterfaceProvided*>(endUserInterfaceArg.OriginalInterface);
                CMN_ASSERT(originalInterface);
                endUserInterfaceArg.EndUserInterface =
                    reinterpret_cast<size_t>(originalInterface->GetEndUserInterface(clientInterfaceName));
            }
#else
            serverFunctionSet->GetEndUserInterface(endUserInterfaceArg, endUserInterfaceArg);
#endif

            mtsInterfaceProvided * endUserInterface = reinterpret_cast<mtsInterfaceProvided*>(endUserInterfaceArg.EndUserInterface);
            if (!endUserInterface) {
                CMN_LOG_CLASS_RUN_ERROR << "ConnectLocally: failed to get end-user interface for " << serverComponentName << std::endl;
                return false;
            }
            success = clientInterfaceRequired->BindCommands(endUserInterface);
            mtsEventHandlerList eventList(endUserInterface);
            clientInterfaceRequired->GetEventList(eventList);

#if (CISST_OS == CISST_LINUX_XENOMAI && CISST_MTS_64BIT)
            {
                // From void mtsInterfaceProvided::AddObserverList(const mtsEventHandlerList & argin, mtsEventHandlerList & argout)
                size_t i;
                for (i = 0; i < eventList.VoidEvents.size(); i++) {
                    eventList.VoidEvents[i].Result = eventList.Provided->AddObserver(eventList.VoidEvents[i].EventName, eventList.VoidEvents[i].HandlerPointer);
                }
                for (i = 0; i < eventList.WriteEvents.size(); i++) {
                    eventList.WriteEvents[i].Result = eventList.Provided->AddObserver(eventList.WriteEvents[i].EventName, eventList.WriteEvents[i].HandlerPointer);
                }
            }
#else
            serverFunctionSet->AddObserverList(eventList, eventList);
#endif
            if (!clientInterfaceRequired->CheckEventList(eventList))
                success = false;
        }
        if (success) {
            CMN_LOG_CLASS_INIT_VERBOSE << "ConnectLocally: successfully connected required/provided: "
                                       << clientComponentName << ":" << clientInterfaceName << " - "
                                       << serverComponentName << ":" << serverInterfaceName << std::endl;
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectLocally: failed to connect required/provided: "
                                     << clientComponentName << ":" << clientInterfaceName << " - "
                                     << serverComponentName << ":" << serverInterfaceName << std::endl;
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
            if (mtsManagerComponentBase::IsNameOfInterfaceGCMProvided(serverInterfaceName)) {
                if (!MCS->AddNewClientProcess(clientProcessName)) {
                    CMN_LOG_CLASS_INIT_ERROR << "ConnectLocally: failed to create new set of InterfaceGCM function objects: "
                                             << clientProcessName << std::endl;
                    return false;
                }
            }
        }

    }
    else {  // Input/Output connection
        if (!serverInterfaceOutput) {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectLocally: failed to connect interfaces: "
                                     << clientComponentName << ":" << clientInterfaceName << " - "
                                     << serverComponentName << ":" << serverInterfaceName
                                     << ", server does not have provided or output interface." << std::endl;
            return false;
        }
        mtsInterfaceInput * clientInterfaceInput = clientComponent->GetInterfaceInput(clientInterfaceName);
        if (!clientInterfaceInput) {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectLocally: failed to connect interfaces: "
                                     << clientComponentName << ":" << clientInterfaceName << " - "
                                     << serverComponentName << ":" << serverInterfaceName
                                     << ", client does not have input interface." << std::endl;
            return false;
        }
        if (clientInterfaceInput->ConnectTo(serverInterfaceOutput)) {
            CMN_LOG_CLASS_INIT_VERBOSE << "ConnectLocally: component \""
                                       << this->GetName()
                                       << "\" input interface \"" << clientInterfaceName
                                       << "\" successfully connected to output interface \""
                                       << serverInterfaceName << "\"" << std::endl;
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectLocally: component \""
                                     << this->GetName()
                                     << "\" input interface \"" << clientInterfaceName
                                     << "\" failed to connect to output interface \""
                                     << serverInterfaceName << "\"" << std::endl;
            return false;
        }
    }

    return true;
}


// This implementation of DisconnectLocally does not rely on any data saved about the connection, such as the end-user
// interface pointer or the connection id.  I think it would be better to first look up this information.
bool mtsManagerComponentClient::DisconnectLocally(const std::string & clientComponentName, const std::string & clientInterfaceName,
                                                  const std::string & serverComponentName, const std::string & serverInterfaceName)
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

    mtsInterfaceProvided * serverInterfaceProvided = serverComponent->GetInterfaceProvided(serverInterfaceName);
    mtsInterfaceOutput * serverInterfaceOutput = serverComponent->GetInterfaceOutput(serverInterfaceName);
    if (!serverInterfaceProvided && !serverInterfaceOutput) {
        // test for swapped interfaces
        // TODO: This duplicates a test already done in the GCM
        CMN_LOG_CLASS_INIT_DEBUG << "DisconnectLocally: looking for provided/output interface in first component as well" << std::endl;
        serverInterfaceProvided = clientComponent->GetInterfaceProvided(clientInterfaceName);
        serverInterfaceOutput = clientComponent->GetInterfaceOutput(clientInterfaceName);
        if (!serverInterfaceProvided && !serverInterfaceOutput) {
            CMN_LOG_CLASS_INIT_ERROR << "DisconnectLocally: failed to get provided/output interface \"" << serverInterfaceName << "\""
                                     << " in component \"" << serverComponentName << "\"" << std::endl;
            return false;
        } else {
            CMN_LOG_CLASS_INIT_DEBUG << "DisconnectLocally: Swapping client/server" << std::endl;
            return DisconnectLocally(serverComponentName, serverInterfaceName, clientComponentName, clientInterfaceName);
        }
    }

    // Now, handle the disconnection.  First, we look for connection between required/provided interfaces.  Then, we look
    // for connection between input/output interfaces.
    if (serverInterfaceProvided) {
        mtsInterfaceRequired *clientInterfaceRequired = clientComponent->GetInterfaceRequired(clientInterfaceName);
        if (!clientInterfaceRequired) {
            CMN_LOG_CLASS_RUN_ERROR << "DisconnectLocally: failed to disconnect interfaces: "
                                     << clientComponentName << ":" << clientInterfaceName << " - "
                                     << serverComponentName << ":" << serverInterfaceName
                                     << ", client does not have required interface." << std::endl;
            return false;
        }
        // Following check should not be necessary -- serverInterfaceProvided should always be the original interface
        // because we obtained it via a string query.
        if (serverInterfaceProvided->GetOriginalInterface()) {
            CMN_LOG_CLASS_RUN_ERROR << "DisconnectLocally: failed to disconnect interfaces: "
                                     << clientComponentName << ":" << clientInterfaceName << " - "
                                     << serverComponentName << ":" << serverInterfaceName
                                     << ", did not get original interface." << std::endl;
            return false;
        }
        // Now, get the end-user interface for this client
        mtsInterfaceProvided *endUserInterface = serverInterfaceProvided->FindEndUserInterfaceByName(clientInterfaceName);
        if (!endUserInterface) {
            CMN_LOG_CLASS_RUN_ERROR << "DisconnectLocally: failed to disconnect interfaces: "
                                     << clientComponentName << ":" << clientInterfaceName << " - "
                                     << serverComponentName << ":" << serverInterfaceName
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
            if (serverInterfaceProvided->RemoveEndUserInterface(endUserInterface, clientInterfaceName) != 0)
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
            mtsEndUserInterfaceArg endUserInterfaceArg(reinterpret_cast<size_t>(serverInterfaceProvided),
                                                       clientInterfaceName,
                                                       reinterpret_cast<size_t>(endUserInterface));
            serverFunctionSet->RemoveEndUserInterface(endUserInterfaceArg, endUserInterfaceArg);
            if (endUserInterfaceArg.EndUserInterface != 0) {
                CMN_LOG_CLASS_RUN_WARNING << "DisconnectLocally: failed to remove end-user interface for " << serverComponentName << std::endl;
                success = false;
            }
        }
        if (success) {
            CMN_LOG_CLASS_INIT_VERBOSE << "DisconnectLocally: successfully disconnected required/provided: "
                                       << clientComponentName << ":" << clientInterfaceName << " - "
                                       << serverComponentName << ":" << serverInterfaceName << std::endl;
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "DisconnectLocally: failed to disconnect required/provided: "
                                     << clientComponentName << ":" << clientInterfaceName << " - "
                                     << serverComponentName << ":" << serverInterfaceName << std::endl;
            return false;
        }
    }
    else {  // Input/Output connection
        if (!serverInterfaceOutput) {
            CMN_LOG_CLASS_INIT_ERROR << "DisconnectLocally: failed to disconnect interfaces: "
                                     << clientComponentName << ":" << clientInterfaceName << " - "
                                     << serverComponentName << ":" << serverInterfaceName
                                     << ", server does not have provided or output interface." << std::endl;
            return false;
        }
        mtsInterfaceInput *clientInterfaceInput = clientComponent->GetInterfaceInput(clientInterfaceName);
        if (!clientInterfaceInput) {
            CMN_LOG_CLASS_INIT_ERROR << "DisconnectLocally: failed to disconnect interfaces: "
                                     << clientComponentName << ":" << clientInterfaceName << " - "
                                     << serverComponentName << ":" << serverInterfaceName
                                     << ", client does not have input interface." << std::endl;
            return false;
        }
        if (clientInterfaceInput->Disconnect()) {
            CMN_LOG_CLASS_INIT_VERBOSE << "DisconnectLocally: component \""
                                       << this->GetName()
                                       << "\" input interface \"" << clientInterfaceName
                                       << "\" successfully disconnected from output interface \""
                                       << serverInterfaceName << "\"" << std::endl;
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "DisconnectLocally: component \""
                                     << this->GetName()
                                     << "\" input interface \"" << clientInterfaceName
                                     << "\" failed to disconnect from output interface \""
                                     << serverInterfaceName << "\"" << std::endl;
            return false;
        }
    }

    // Special handling for connections which MCC is involved with
    //
    // Remove InterfaceComponentRequired instance (InterfaceComponentRequired - InterfaceInternalProvided)
    if (mtsManagerComponentBase::IsNameOfInterfaceInternalProvided(serverInterfaceName)) {
        const std::string nameOfInterfaceComponentRequired = GetNameOfInterfaceComponentRequiredFor(serverComponentName);
        if (!RemoveInterfaceRequired(nameOfInterfaceComponentRequired)) {
            CMN_LOG_CLASS_INIT_ERROR << "DisconnectLocally: failed to disconnect interfaces: "
                                     << clientComponentName << ":" << clientInterfaceName << " - "
                                     << serverComponentName << ":" << serverInterfaceName
                                     << ", failed to remove InterfaceComponent's required interface: "
                                     << "\"" << nameOfInterfaceComponentRequired << "\"" << std::endl;
            return false;
        }
        if (!DisconnectCleanup(serverComponentName)) {
            CMN_LOG_CLASS_INIT_ERROR << "DisconnectLocally: failed to disconnect interfaces: "
                                     << clientComponentName << ":" << clientInterfaceName << " - "
                                     << serverComponentName << ":" << serverInterfaceName
                                     << ", failed to clean up InterfaceComponent's required interface" << std::endl;
            return false;
        }
    }

    return true;
}

bool mtsManagerComponentClient::DisconnectCleanup(const std::string & componentName)
{
    // Get instance of InterfaceComponent's required interface that corresponds to
    // "componentName"
    InterfaceComponentFunctionType * functionSet = InterfaceComponentFunctionMap.GetItem(componentName);
    if (!functionSet) {
        CMN_LOG_CLASS_RUN_ERROR << "DisconnectCleanup: failed to get function set for component \"" << componentName << "\"" << std::endl;
        return false;
    }

    // MJ: This might need to be protected as mutex
    InterfaceComponentFunctionMap.RemoveItem(componentName);
    delete functionSet;

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
    std::string interfaceName = mtsManagerComponentBase::GetNameOfInterfaceComponentProvided();
    // Return if provided interface already exists
    if (GetInterfaceProvided(interfaceName)) {
        return true;
    }
    mtsInterfaceProvided * provided = AddInterfaceProvided(interfaceName);
    if (!provided) {
        CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceComponent: failed to add \"Component\" provided interface: " << interfaceName << std::endl;
        return false;
    }

    provided->AddCommandWriteReturn(&mtsManagerComponentClient::InterfaceComponentCommands_ComponentCreate,
                                    this, mtsManagerComponentBase::CommandNames::ComponentCreate);
    provided->AddCommandWrite(&mtsManagerComponentClient::InterfaceComponentCommands_ComponentConfigure,
                              this, mtsManagerComponentBase::CommandNames::ComponentConfigure);
    provided->AddCommandWriteReturn(&mtsManagerComponentClient::InterfaceComponentCommands_ComponentConnectNew,
                                    this, mtsManagerComponentBase::CommandNames::ComponentConnect);
    provided->AddCommandWriteReturn(&mtsManagerComponentClient::InterfaceComponentCommands_ComponentDisconnectNew,
                                    this, mtsManagerComponentBase::CommandNames::ComponentDisconnect);
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
    provided->AddCommandQualifiedRead(&mtsManagerComponentClient::InterfaceComponentCommands_GetListOfComponentClasses,
                              this, mtsManagerComponentBase::CommandNames::GetListOfComponentClasses);
    provided->AddCommandQualifiedRead(&mtsManagerComponentClient::InterfaceComponentCommands_GetInterfaceProvidedDescription,
                                      this, mtsManagerComponentBase::CommandNames::GetInterfaceProvidedDescription);
    provided->AddCommandQualifiedRead(&mtsManagerComponentClient::InterfaceComponentCommands_GetInterfaceRequiredDescription,
                                      this, mtsManagerComponentBase::CommandNames::GetInterfaceRequiredDescription);
    provided->AddCommandQualifiedRead(&mtsManagerComponentClient::InterfaceComponentCommands_LoadLibrary,
                                      this, mtsManagerComponentBase::CommandNames::LoadLibrary);
    provided->AddCommandWrite(&mtsManagerComponentClient::InterfaceComponentCommands_EnableLogForwarding,
                              this, mtsManagerComponentBase::CommandNames::EnableLogForwarding);
    provided->AddCommandWrite(&mtsManagerComponentClient::InterfaceComponentCommands_DisableLogForwarding,
                              this, mtsManagerComponentBase::CommandNames::DisableLogForwarding);
    provided->AddCommandQualifiedRead(&mtsManagerComponentClient::InterfaceComponentCommands_GetLogForwardingStates,
                              this, mtsManagerComponentBase::CommandNames::GetLogForwardingStates);
    provided->AddCommandQualifiedRead(&mtsManagerComponentClient::InterfaceComponentCommands_GetAbsoluteTimeDiffs,
                                      this, mtsManagerComponentBase::CommandNames::GetAbsoluteTimeDiffs);

    provided->AddEventWrite(this->InterfaceComponentEvents_AddComponent,
                            mtsManagerComponentBase::EventNames::AddComponent, mtsDescriptionComponent());
    provided->AddEventWrite(this->InterfaceComponentEvents_ChangeState,
                            mtsManagerComponentBase::EventNames::ChangeState, mtsComponentStateChange());
    provided->AddEventWrite(this->InterfaceComponentEvents_AddConnection,
                            mtsManagerComponentBase::EventNames::AddConnection, mtsDescriptionConnection());
    provided->AddEventWrite(this->InterfaceComponentEvents_RemoveConnection,
                            mtsManagerComponentBase::EventNames::RemoveConnection, mtsDescriptionConnection());
    provided->AddCommandRead(&mtsManagerComponentClient::GetAbsoluteTimeInSeconds, this, "GetAbsoluteTimeInSeconds");  // DEPRECATED



    CMN_LOG_CLASS_INIT_VERBOSE << "AddInterfaceComponent: successfully added \"Component\" interfaces" << std::endl;

    // Create an end user interface for internal invocations (via GeneralInterface). These internal invocations
    // are made by methods of this class, so we cannot assume that there is only one thread. Thus, to achieve thread-safety
    // we need to use osaMutex.
#if 0  // PK TEMP 7/18/2011
    mtsInterfaceRequired * interfaceRequired = this->AddInterfaceRequired("Self", MTS_OPTIONAL);
    interfaceRequired->AddFunction(mtsManagerComponentBase::CommandNames::ComponentConnect,
                                   GeneralInterface.ComponentConnect);
    mtsInterfaceProvided * interfaceProvidedToSelf = provided->GetEndUserInterface("Self");
    if (interfaceProvidedToSelf) {
        if (!interfaceRequired->ConnectTo(interfaceProvidedToSelf)) {
            CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceComponent: failed to connect \"Self\" required interface to \"Self\" provided interface" << std::endl;
            return false;
        }
    }
#else
    mtsInterfaceProvided *interfaceProvidedToSelf = provided->GetEndUserInterface("Self");
    if (interfaceProvidedToSelf) {
        GeneralInterface.ComponentConnectNew.Bind(interfaceProvidedToSelf->GetCommandWriteReturn(
                                                  mtsManagerComponentBase::CommandNames::ComponentConnect));
        GeneralInterface.ComponentConnect.Bind(interfaceProvidedToSelf->GetCommandWrite(
                                               mtsManagerComponentBase::CommandNames::ComponentConnect));
    }
#endif

    return true;
}


bool mtsManagerComponentClient::AddInterfaceLCM(void)
{
    // Add required interface
    std::string interfaceName = mtsManagerComponentBase::GetNameOfInterfaceLCMRequired();
    mtsInterfaceRequired * required = AddInterfaceRequired(interfaceName);
    if (!required) {
        CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceLCM: failed to add \"LCM\" required interface: " << interfaceName << std::endl;
        return false;
    }
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentCreate,
                          InterfaceLCMFunction.ComponentCreate);
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentConfigure,
                          InterfaceLCMFunction.ComponentConfigure);
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentConnect,
                          InterfaceLCMFunction.ComponentConnectNew);
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentDisconnect,
                          InterfaceLCMFunction.ComponentDisconnectNew);
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
    required->AddFunction(mtsManagerComponentBase::CommandNames::LoadLibrary,
                          InterfaceLCMFunction.LoadLibrary);
    required->AddFunction(mtsManagerComponentBase::CommandNames::PrintLog,
                          InterfaceLCMFunction.PrintLog);
    required->AddFunction(mtsManagerComponentBase::CommandNames::EnableLogForwarding,
                          InterfaceLCMFunction.EnableLogForwarding);
    required->AddFunction(mtsManagerComponentBase::CommandNames::DisableLogForwarding,
                          InterfaceLCMFunction.DisableLogForwarding);
    required->AddFunction(mtsManagerComponentBase::CommandNames::GetLogForwardingStates,
                          InterfaceLCMFunction.GetLogForwardingStates);
    required->AddFunction(mtsManagerComponentBase::CommandNames::GetAbsoluteTimeDiffs,
                          InterfaceLCMFunction.GetAbsoluteTimeDiffs);
    required->AddFunction(mtsManagerComponentBase::CommandNames::GetNamesOfProcesses,
                          InterfaceLCMFunction.GetNamesOfProcesses);
    required->AddFunction(mtsManagerComponentBase::CommandNames::GetNamesOfComponents,
                          InterfaceLCMFunction.GetNamesOfComponents);
    required->AddFunction(mtsManagerComponentBase::CommandNames::GetNamesOfInterfaces,
                          InterfaceLCMFunction.GetNamesOfInterfaces);
    required->AddFunction(mtsManagerComponentBase::CommandNames::GetListOfConnections,
                          InterfaceLCMFunction.GetListOfConnections);
    required->AddFunction(mtsManagerComponentBase::CommandNames::GetListOfComponentClasses,
                          InterfaceLCMFunction.GetListOfComponentClasses);
    required->AddFunction(mtsManagerComponentBase::CommandNames::GetInterfaceProvidedDescription,
                          InterfaceLCMFunction.GetInterfaceProvidedDescription);
    required->AddFunction(mtsManagerComponentBase::CommandNames::GetInterfaceRequiredDescription,
                          InterfaceLCMFunction.GetInterfaceRequiredDescription);
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
    required->AddEventHandlerVoid(&mtsManagerComponentClient::HandleMCSReadyEvent, this,
                                   mtsManagerComponentBase::EventNames::MCSReady, MTS_EVENT_NOT_QUEUED);

    // Add provided interface
    interfaceName = mtsManagerComponentBase::GetNameOfInterfaceLCMProvided();
    mtsInterfaceProvided * provided = AddInterfaceProvided(interfaceName);
    if (!provided) {
        CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceLCM: failed to add \"LCM\" required interface: " << interfaceName << std::endl;
        return false;
    }
    provided->AddCommandWriteReturn(&mtsManagerComponentClient::InterfaceLCMCommands_ComponentCreate,
                                    this, mtsManagerComponentBase::CommandNames::ComponentCreate);
    provided->AddCommandWrite(&mtsManagerComponentClient::InterfaceLCMCommands_ComponentConfigure,
                             this, mtsManagerComponentBase::CommandNames::ComponentConfigure);
    provided->AddCommandWriteReturn(&mtsManagerComponentClient::InterfaceLCMCommands_ComponentConnectNew,
                                    this, mtsManagerComponentBase::CommandNames::ComponentConnect);
    provided->AddCommandWriteReturn(&mtsManagerComponentClient::InterfaceLCMCommands_ComponentDisconnectNew,
                                    this, mtsManagerComponentBase::CommandNames::ComponentDisconnect);
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
    provided->AddCommandQualifiedRead(&mtsManagerComponentClient::InterfaceLCMCommands_GetInterfaceProvidedDescription,
                             this, mtsManagerComponentBase::CommandNames::GetInterfaceProvidedDescription);
    provided->AddCommandQualifiedRead(&mtsManagerComponentClient::InterfaceLCMCommands_GetInterfaceRequiredDescription,
                             this, mtsManagerComponentBase::CommandNames::GetInterfaceRequiredDescription);
    provided->AddCommandQualifiedRead(&mtsManagerComponentClient::InterfaceLCMCommands_LoadLibrary,
                             this, mtsManagerComponentBase::CommandNames::LoadLibrary);
    provided->AddCommandRead(&mtsManagerComponentClient::InterfaceLCMCommands_GetListOfComponentClasses,
                             this, mtsManagerComponentBase::CommandNames::GetListOfComponentClasses);
    provided->AddCommandWrite(&mtsManagerComponentClient::InterfaceLCMCommands_SetLogForwarding,
                             this, mtsManagerComponentBase::CommandNames::SetLogForwarding);
    provided->AddCommandRead(&mtsManagerComponentClient::InterfaceLCMCommands_GetLogForwardingState,
                             this, mtsManagerComponentBase::CommandNames::GetLogForwardingState);
    provided->AddCommandRead(&mtsManagerComponentClient::InterfaceLCMCommands_GetAbsoluteTimeInSeconds,
                             this, mtsManagerComponentBase::CommandNames::GetAbsoluteTimeInSeconds);
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

    const std::string interfaceName = mtsManagerComponentBase::GetNameOfInterfaceComponentRequiredFor(clientComponentName);
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
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentCreate,
                          newFunctionSet->ComponentCreate);
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentStart,
                          newFunctionSet->ComponentStartOther);
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

bool mtsManagerComponentClient::CanForwardLog(void) const
{
    return (InterfaceLCMFunction.PrintLog.IsValid() && MCSReady);
}

bool mtsManagerComponentClient::ForwardLog(const mtsLogMessage & log) const
{
    mtsExecutionResult ret = InterfaceLCMFunction.PrintLog(log);
    if ((ret.GetResult() != mtsExecutionResult::COMMAND_SUCCEEDED))// &&
         //(ret.GetResult() != mtsExecutionResult::COMMAND_QUEUED) )
    {
        return false;
    }

    return true;
}

bool mtsManagerComponentClient::Connect(const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
                                        const std::string & serverComponentName, const std::string & serverInterfaceProvidedName)
{
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    const std::string processName = LCM->GetProcessName();
    mtsDescriptionConnection connectionDescription(processName, clientComponentName, clientInterfaceRequiredName,
                                                   processName, serverComponentName, serverInterfaceProvidedName);
    bool result = true;
    if (!IsRunning()) {
#if CISST_MTS_NEW
        InterfaceComponentCommands_ComponentConnectNew(connectionDescription, result);
#else
        InterfaceComponentCommands_ComponentConnect(connectionDescription /*, result*/);
#endif
    } else {
#if CISST_MTS_NEW
        if (!GeneralInterface.ComponentConnectNew.IsValid()) {
            CMN_LOG_CLASS_INIT_WARNING << "Connect: GeneralInterface not yet valid, initializing." << std::endl;
            AddInterfaceComponent();
            if (!GeneralInterface.ComponentConnectNew.IsValid()) {
                CMN_LOG_CLASS_INIT_ERROR << "Connect: Failed to initialize ComponentConnectNew." << std::endl;
                return false;
            }
        }
#else
        if (!GeneralInterface.ComponentConnect.IsValid()) {
            CMN_LOG_CLASS_INIT_WARNING << "Connect: GeneralInterface not yet valid, initializing." << std::endl;
            AddInterfaceComponent();
            if (!GeneralInterface.ComponentConnect.IsValid()) {
                CMN_LOG_CLASS_INIT_ERROR << "Connect: Failed to initialize ComponentConnect." << std::endl;
                return false;
            }
        }
#endif
        // note that we have a mutex around a blocking command, ...
        GeneralInterface.Mutex.Lock();
        CMN_LOG_CLASS_INIT_DEBUG << "Connect: Calling ComponentConnect for " << connectionDescription << std::endl;
        //if (!byPassInterface) {
#if CISST_MTS_NEW
        GeneralInterface.ComponentConnectNew(connectionDescription, result);
#else
        GeneralInterface.ComponentConnect(connectionDescription /*, result*/);
#endif
        //} else {
        //std::cerr << "------------------------- bypass for " << connectionDescription << std::endl;
        //InterfaceComponentCommands_ComponentConnect(connectionDescription, result);
        //}
        GeneralInterface.Mutex.Unlock();
    }
    return result;
}

bool mtsManagerComponentClient::IsLocalProcess(const std::string &procName) const
{
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    const std::string nameOfThisLCM = LCM->GetProcessName();
    return (LCM->GetConfiguration() == mtsManagerLocal::LCM_CONFIG_STANDALONE) || (nameOfThisLCM == procName);
}

void mtsManagerComponentClient::InterfaceComponentCommands_ComponentCreate(const mtsDescriptionComponent & componentDescription, bool & result)
{
    if (IsLocalProcess(componentDescription.ProcessName))
    {
        InterfaceLCMCommands_ComponentCreate(componentDescription, result);
        return;
    }
    mtsExecutionResult executionResult = InterfaceLCMFunction.ComponentCreate(componentDescription, result);
    if (!executionResult.IsOK()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_ComponentCreate: failed to execute \"Component Create\" command ("
                                << executionResult << ")" << std::endl;
    }
}


void mtsManagerComponentClient::InterfaceComponentCommands_ComponentConfigure(const mtsDescriptionComponent & arg)
{
    if (IsLocalProcess(arg.ProcessName))
        InterfaceLCMCommands_ComponentConfigure(arg);
    else {
        if (InterfaceLCMFunction.ComponentConfigure.IsValid())
            //InterfaceLCMFunction.ComponentConfigure.ExecuteBlocking(arg);
            InterfaceLCMFunction.ComponentConfigure(arg);
        else
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_ComponentConfigure: failed to execute \"Component Configure\"" << std::endl;
    }
}


void mtsManagerComponentClient::InterfaceComponentCommands_ComponentConnect(const mtsDescriptionConnection & connectionDescription /*, bool & result*/)
{
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    const std::string nameOfThisLCM = LCM->GetProcessName();
    if (LCM->GetConfiguration() == mtsManagerLocal::LCM_CONFIG_STANDALONE ||
        (nameOfThisLCM == connectionDescription.Client.ProcessName &&
         nameOfThisLCM == connectionDescription.Server.ProcessName))
    {
        InterfaceLCMCommands_ComponentConnect(connectionDescription /*, result*/);
        return;
    }
    mtsExecutionResult executionResult = InterfaceLCMFunction.ComponentConnect(connectionDescription /*, result*/);
    if (!executionResult.IsOK()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_ComponentConnect: failed to execute \"Component Component\" command ("
                                << executionResult << ")" << std::endl;
    }
}


void mtsManagerComponentClient::InterfaceComponentCommands_ComponentConnectNew(const mtsDescriptionConnection & connectionDescription, bool & result)
{
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    const std::string nameOfThisLCM = LCM->GetProcessName();
    if (LCM->GetConfiguration() == mtsManagerLocal::LCM_CONFIG_STANDALONE ||
        (nameOfThisLCM == connectionDescription.Client.ProcessName &&
         nameOfThisLCM == connectionDescription.Server.ProcessName))
    {
        InterfaceLCMCommands_ComponentConnectNew(connectionDescription, result);
        return;
    }
    mtsExecutionResult executionResult = InterfaceLCMFunction.ComponentConnectNew(connectionDescription, result);
    if (!executionResult.IsOK()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_ComponentConnectNew: failed to execute \"Component Component\" command ("
                                << executionResult << ")" << std::endl;
    }
}


void mtsManagerComponentClient::InterfaceComponentCommands_ComponentDisconnect(const mtsDescriptionConnection & arg)
{
    /*
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
    */
    // MJ: Don't use short cut -- every configuration change in the LCM should be reported to the GCM
    // and the change should be initiated and controlled by the GCM.
    if (!InterfaceLCMFunction.ComponentDisconnect.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_ComponentDisconnect: failed to execute \"Component Disconnect\"" << std::endl;
        return;
    }

    //InterfaceLCMFunction.ComponentDisconnect.ExecuteBlocking(arg);
    InterfaceLCMFunction.ComponentDisconnect(arg);
}

void mtsManagerComponentClient::InterfaceComponentCommands_ComponentDisconnectNew(const mtsDescriptionConnection & arg, bool & result)
{
    if (!InterfaceLCMFunction.ComponentDisconnectNew.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_ComponentDisconnectNew: failed to execute \"Component Disconnect\"" << std::endl;
        result = false;
        return;
    }

    InterfaceLCMFunction.ComponentDisconnectNew(arg, result);
}

void mtsManagerComponentClient::InterfaceComponentCommands_ComponentStart(const mtsComponentStatusControl & arg)
{
    if (IsLocalProcess(arg.ProcessName))
    {
        mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
        // Check if the component specified exists
        if (!LCM->GetComponent(arg.ComponentName)) {
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_ComponentStart: did not find component \""
                                    << arg.ComponentName << "\"" << std::endl;
            return;
        }

        InterfaceLCMCommands_ComponentStart(arg);
        return;
    } else {
        if (!InterfaceLCMFunction.ComponentStart.IsValid()) {
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_ComponentStart: could not start component \""
                                    << arg.ComponentName << "\"" << std::endl;
            return;
        }
        //InterfaceLCMFunction.ComponentStart.ExecuteBlocking(arg);
        InterfaceLCMFunction.ComponentStart(arg);
    }
}

void mtsManagerComponentClient::InterfaceComponentCommands_ComponentStop(const mtsComponentStatusControl & arg)
{
    if (IsLocalProcess(arg.ProcessName))
    {
        mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
        // Check if the component specified exists
        if (!LCM->GetComponent(arg.ComponentName)) {
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_ComponentStop: did not find component \""
                                    << arg.ComponentName << "\"" << std::endl;
            return;
        }

        InterfaceLCMCommands_ComponentStop(arg);
        return;
    } else {
        if (!InterfaceLCMFunction.ComponentStop.IsValid()) {
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_ComponentStop: could not stop component \""
                                    << arg.ComponentName << "\"" << std::endl;
            return;
        }
        //InterfaceLCMFunction.ComponentStop.ExecuteBlocking(arg);
        InterfaceLCMFunction.ComponentStop(arg);
    }
}

void mtsManagerComponentClient::InterfaceComponentCommands_ComponentResume(const mtsComponentStatusControl & arg)
{
    if (IsLocalProcess(arg.ProcessName))
    {
        mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
        // Check if the component specified exists
        if (!LCM->GetComponent(arg.ComponentName)) {
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_ComponentResume: did not find component \""
                                    << arg.ComponentName << "\"" << std::endl;
            return;
        }

        InterfaceLCMCommands_ComponentResume(arg);
        return;
    } else {
        if (!InterfaceLCMFunction.ComponentResume.IsValid()) {
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_ComponentResume: could not resume component \""
                                    << arg.ComponentName << "\"" << std::endl;
            return;
        }
        //InterfaceLCMFunction.ComponentResume.ExecuteBlocking(arg);
        InterfaceLCMFunction.ComponentResume(arg);
    }
}

void mtsManagerComponentClient::InterfaceComponentCommands_ComponentGetState(const mtsDescriptionComponent &component,
                                                                             mtsComponentState &state) const
{
    if (IsLocalProcess(component.ProcessName))
    {
        mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
        // Check if the component specified exists
        if (!LCM->GetComponent(component.ComponentName)) {
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_ComponentGetState: did not find component \""
                                    << component.ComponentName << "\"" << std::endl;
            return;
        }

        InterfaceLCMCommands_ComponentGetState(component, state);
        return;
    } else {
        if (!InterfaceLCMFunction.ComponentGetState.IsValid()) {
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_ComponentGetState: could not get state of component \""
                                    << component.ComponentName << "\"" << std::endl;
            return;
        }
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

void mtsManagerComponentClient::InterfaceComponentCommands_GetListOfComponentClasses(const std::string &processName,
                                std::vector <mtsDescriptionComponentClass> & listOfComponentClasses) const
{
    if (!InterfaceLCMFunction.GetListOfComponentClasses.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_GetListOfComponentClasses: failed to execute \"GetListOfComponentClasses\"" << std::endl;
        return;
    }

    InterfaceLCMFunction.GetListOfComponentClasses(processName, listOfComponentClasses);
}

void mtsManagerComponentClient::InterfaceComponentCommands_GetInterfaceProvidedDescription(const mtsDescriptionInterface & intfc,
                                                                                           mtsInterfaceProvidedDescription & description) const
{
    if (IsLocalProcess(intfc.ProcessName))
    {
        InterfaceLCMCommands_GetInterfaceProvidedDescription(intfc, description);
    } else {
        if (!InterfaceLCMFunction.GetInterfaceProvidedDescription.IsValid()) {
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_GetInterfaceProvidedDescription: function not bound to command" << std::endl;
            return;
        }
        InterfaceLCMFunction.GetInterfaceProvidedDescription(intfc, description);
    }
}

void mtsManagerComponentClient::InterfaceComponentCommands_GetInterfaceRequiredDescription(const mtsDescriptionInterface & intfc,
                                                                                           mtsInterfaceRequiredDescription & description) const
{
    if (IsLocalProcess(intfc.ProcessName))
    {
        InterfaceLCMCommands_GetInterfaceRequiredDescription(intfc, description);
    } else {
        if (!InterfaceLCMFunction.GetInterfaceRequiredDescription.IsValid()) {
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_GetInterfaceRequiredDescription: function not bound to command" << std::endl;
            return;
        }
        InterfaceLCMFunction.GetInterfaceRequiredDescription(intfc, description);
    }
}

void mtsManagerComponentClient::InterfaceComponentCommands_LoadLibrary(const mtsDescriptionLoadLibrary &lib, bool &result) const
{
    if (IsLocalProcess(lib.ProcessName))
        InterfaceLCMCommands_LoadLibrary(lib.LibraryName, result);
    else {
        if (!InterfaceLCMFunction.LoadLibrary.IsValid()) {
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_LoadLibrary: function not bound to command" << std::endl;
            return;
        }
        InterfaceLCMFunction.LoadLibrary(lib, result);
    }
}

void mtsManagerComponentClient::InterfaceComponentCommands_EnableLogForwarding(const std::vector<std::string> &processNames)
{
    if (InterfaceLCMFunction.EnableLogForwarding.IsValid())
        InterfaceLCMFunction.EnableLogForwarding(processNames);
    else
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_EnableLogForwarding: function not bound to command" << std::endl;
}

void mtsManagerComponentClient::InterfaceComponentCommands_DisableLogForwarding(const std::vector<std::string> &processNames)
{
    if (InterfaceLCMFunction.DisableLogForwarding.IsValid())
        InterfaceLCMFunction.DisableLogForwarding(processNames);
    else
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_DisableLogForwarding: function not bound to command" << std::endl;
}

void mtsManagerComponentClient::InterfaceComponentCommands_GetLogForwardingStates(const stdStringVec & processNames, stdCharVec & states) const
{
    states.clear();

    // Special case handling
    if (processNames.size() == 1) {
        mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
        if (LCM->GetProcessName() == processNames[0]) {
            states.push_back(LCM->GetLogForwardingState());
            return;
        }
    }

    if (InterfaceLCMFunction.GetLogForwardingStates.IsValid())
        InterfaceLCMFunction.GetLogForwardingStates(processNames, states);
    else
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_GetLogForwardingStates: function not bound to command" << std::endl;
}

void mtsManagerComponentClient::InterfaceComponentCommands_GetAbsoluteTimeDiffs(const std::vector<std::string> &processNames,
                                                                                std::vector<double> &processTimes) const
{
    if (InterfaceLCMFunction.GetAbsoluteTimeDiffs.IsValid())
        InterfaceLCMFunction.GetAbsoluteTimeDiffs(processNames, processTimes);
    else
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceComponentCommands_GetAbsoluteTimeDiffs: function not bound to command" << std::endl;
}

void mtsManagerComponentClient::InterfaceLCMCommands_ComponentCreate(const mtsDescriptionComponent & componentDescription, bool & result)
{
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    if (LCM->GetCurrentMainTask() && (LCM->GetCurrentMainTask() != this)) {
        result = false;
        std::string mainTaskName = LCM->GetCurrentMainTask()->GetName();
        CMN_LOG_CLASS_RUN_VERBOSE << "ComponentCreate: planning to call main task " << mainTaskName
                                  << " to create component " << componentDescription.ComponentName << std::endl;
        InterfaceComponentFunctionType *functionSet = InterfaceComponentFunctionMap.GetItem(mainTaskName);
        if (functionSet) {
            if (functionSet->ComponentCreate.IsValid())
                functionSet->ComponentCreate(componentDescription, result);
            else
                CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_ComponentCreate: failed to find valid function for main task "
                                        << mainTaskName << std::endl;
        }
        else
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_ComponentCreate: failed to find function set for main task "
                                    << mainTaskName << std::endl;
    }
    else {
        // Call method in mtsComponent.cpp
        InterfaceInternalCommands_ComponentCreate(componentDescription, result);
    }
}


void mtsManagerComponentClient::InterfaceLCMCommands_ComponentConfigure(const mtsDescriptionComponent & arg)
{
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    mtsComponent * component = LCM->GetComponent(arg.ComponentName);
    if (!component) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_ComponentConfigure - no component found: "
                                << arg.ComponentName << std::endl;
        return;
    }

    // For now, using ConstructorArgSerialized field.
    component->Configure(arg.ConstructorArgSerialized);
}


void mtsManagerComponentClient::InterfaceLCMCommands_ComponentConnect(const mtsDescriptionConnection & connectionDescription /*, bool & result*/)
{
    // Try to connect interfaces as requested
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();

    // this is a remote connection
    if (connectionDescription.Client.ProcessName != connectionDescription.Server.ProcessName) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_ComponentConnect: Cannot connect to external process without CISST_MTS_HAS_ICE, connection = "
                                << connectionDescription << std::endl;
        // result = false;
        return;
    }

    // local connection
    int connectionId = LCM->ConnectSetup(connectionDescription.Client.ComponentName, connectionDescription.Client.InterfaceName,
                                         connectionDescription.Server.ComponentName, connectionDescription.Server.InterfaceName);
    if (connectionId == InvalidConnectionID) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_ComponentConnect: failed to execute \"Connect Setup\": "
                                << connectionDescription << std::endl;
        // result = false;
        return;
    }

    ConnectLocally(connectionDescription.Client.ComponentName, connectionDescription.Client.InterfaceName,
                   connectionDescription.Server.ComponentName, connectionDescription.Server.InterfaceName,
                   connectionDescription.Client.ProcessName);

    LCM->ConnectNotify(connectionId,
                       connectionDescription.Client.ComponentName, connectionDescription.Client.InterfaceName,
                       connectionDescription.Server.ComponentName, connectionDescription.Server.InterfaceName);

    CMN_LOG_CLASS_RUN_VERBOSE << "InterfaceLCMCommands_ComponentConnect: successfully connected: " << connectionDescription << std::endl;
    // result = true;
}

void mtsManagerComponentClient::InterfaceLCMCommands_ComponentConnectNew(const mtsDescriptionConnection & connectionDescription, bool & result)
{
    // Try to connect interfaces as requested
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();

    // this is a remote connection
    if (connectionDescription.Client.ProcessName != connectionDescription.Server.ProcessName) {
#if CISST_MTS_HAS_ICE
        // PK TODO: Need to fix this to be thread-safe
        if (!LCM->Connect(connectionDescription.Client.ProcessName,
                          connectionDescription.Client.ComponentName,
                          connectionDescription.Client.InterfaceName,
                          connectionDescription.Server.ProcessName,
                          connectionDescription.Server.ComponentName,
                          connectionDescription.Server.InterfaceName)) {
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_ComponentConnectNew: failed to execute \"Component Connect\": "
                                    << connectionDescription << std::endl;
            result = false;
        } else {
            CMN_LOG_CLASS_RUN_VERBOSE << "InterfaceLCMCommands_ComponentConnectNew: successfully connected: " << connectionDescription << std::endl;
            result = true;
        }
#else
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_ComponentConnectNew: Cannot connect to external process without CISST_MTS_HAS_ICE, connection = "
                                << connectionDescription << std::endl;
        result = false;
#endif
        return;
    }

    // local connection
    int connectionId = LCM->ConnectSetup(connectionDescription.Client.ComponentName, connectionDescription.Client.InterfaceName,
                                         connectionDescription.Server.ComponentName, connectionDescription.Server.InterfaceName);
    if (connectionId == InvalidConnectionID) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_ComponentConnectNew: failed to execute \"Connect Setup\": "
                                << connectionDescription << std::endl;
        result = false;
        return;
    }

    if (!ConnectLocally(connectionDescription.Client.ComponentName, connectionDescription.Client.InterfaceName,
                        connectionDescription.Server.ComponentName, connectionDescription.Server.InterfaceName,
                        connectionDescription.Client.ProcessName)) {
        result = false;
        return;
    }

    LCM->ConnectNotify(connectionId,
                       connectionDescription.Client.ComponentName, connectionDescription.Client.InterfaceName,
                       connectionDescription.Server.ComponentName, connectionDescription.Server.InterfaceName);

    CMN_LOG_CLASS_RUN_VERBOSE << "InterfaceLCMCommands_ComponentConnectNew: successfully connected: " << connectionDescription << std::endl;
    result = true;
}

void mtsManagerComponentClient::InterfaceLCMCommands_ComponentDisconnect(const mtsDescriptionConnection & arg)
{
    /*
    // Try to disconnect interfaces as requested
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();

    // PK TODO: Would be nice to be able to disconnect using just connectionId (e.g., from ComponentViewer)

    // PK: The following may not be necessary, since the MCS should just send us the local disconnect request
    // (in the network case, one of the components should be a proxy). For now, it is needed just to update the GCM database.
    // See mtsManagerComponentServer::InterfaceGCMCommands_ComponentDisconnect.
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

    // PK TEMP: Don't call following if client or server is a proxy object
    if ((arg.Client.ComponentName.find("Proxy") == std::string::npos) &&
        (arg.Server.ComponentName.find("Proxy") == std::string::npos)) {
        if (!LCM->Disconnect(arg.Client.ComponentName, arg.Client.InterfaceName,
                             arg.Server.ComponentName, arg.Server.InterfaceName))
        {
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_ComponentDisconnect: failed to execute \"Component Disconnect\": " << arg << std::endl;
            return;
        }
    }
    */

    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    if (LCM->FindComponent(arg.Client.ComponentName) && LCM->FindComponent(arg.Server.ComponentName)) {
        if (DisconnectLocally(arg.Client.ComponentName, arg.Client.InterfaceName,
                              arg.Server.ComponentName, arg.Server.InterfaceName))
        {
            CMN_LOG_CLASS_RUN_VERBOSE << "InterfaceLCMCommands_ComponentDisconnect: successfully disconnected: " << arg << std::endl;
        }
    }
}

void mtsManagerComponentClient::InterfaceLCMCommands_ComponentDisconnectNew(const mtsDescriptionConnection & arg, bool & result)
{
    result = true;
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    if (!LCM->FindComponent(arg.Client.ComponentName)) {
        result = false;
        CMN_LOG_CLASS_RUN_WARNING << "InterfaceLCMCommands_ComponentDisconnectNew: did not find client component: "
                                  << arg.Client.ComponentName << std::endl;
    }
    if (!LCM->FindComponent(arg.Server.ComponentName)) {
        result = false;
        CMN_LOG_CLASS_RUN_WARNING << "InterfaceLCMCommands_ComponentDisconnectNew: did not find server component: "
                                  << arg.Server.ComponentName << std::endl;
    }
    if (result) {
        if (DisconnectLocally(arg.Client.ComponentName, arg.Client.InterfaceName,
                              arg.Server.ComponentName, arg.Server.InterfaceName))
        {
            CMN_LOG_CLASS_RUN_VERBOSE << "InterfaceLCMCommands_ComponentDisconnectNew: successfully disconnected: " << arg << std::endl;
        }
        else {
            result = false;
            CMN_LOG_CLASS_RUN_WARNING << "InterfaceLCMCommands_ComponentDisconnectNew: failed to disconnect: " << arg << std::endl;
        }
    }
}

void mtsManagerComponentClient::InterfaceLCMCommands_ComponentStart(const mtsComponentStatusControl & arg)
{
    // Check if command is for this component (MCC)
    if (arg.ComponentName == this->GetName()) {
        CMN_LOG_CLASS_RUN_WARNING << "ComponentStart for " << arg.ComponentName << " ignored." << std::endl;
        return;
    }
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    if (LCM->GetCurrentMainTask() && (LCM->GetCurrentMainTask() != this)) {
        // If there is a main task, we call Start from there. This is really only necessary if we need
        // to start another task that captures the main thread.
        std::string mainTaskName = LCM->GetCurrentMainTask()->GetName();
        CMN_LOG_CLASS_RUN_VERBOSE << "ComponentStart: planning to call main task " << mainTaskName
                                  << " to start component " << arg.ComponentName << std::endl;
        InterfaceComponentFunctionType *functionSetMain = InterfaceComponentFunctionMap.GetItem(mainTaskName);
        if (functionSetMain) {
            if (functionSetMain->ComponentStartOther.IsValid())
                functionSetMain->ComponentStartOther(arg);
            else
                CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_ComponentStart: failed to find valid function for main task "
                                        << mainTaskName << ", trying to start component " << arg.ComponentName << std::endl;
        }
        else
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_ComponentStart: failed to find function set for main task "
                                    << mainTaskName << std::endl;
    }
    else {
        CMN_LOG_CLASS_RUN_VERBOSE << "ComponentStart: starting component " << arg.ComponentName << " from MCC" << std::endl;
        InterfaceInternalCommands_ComponentStartOther(arg);
    }
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
    if (component.ComponentName == mtsManagerComponentBase::GetNameOfManagerComponentServer()) {
        //For now, always return active for MCS
        state = mtsComponentState::ACTIVE;
        return;
    }
    // Get a set of function objects that are bound to the InterfaceLCM's provided
    // interface.
    InterfaceComponentFunctionType * functionSet =
        InterfaceComponentFunctionMap.GetItem(component.ComponentName, CMN_LOG_LEVEL_NONE);
    if (!functionSet) {
        // MJ: It is possible that the component viewer tries to fetch component state via
        // component services when a new component is added to a system and not all
        // internal connections are ready.  In this case, return INITIALIZING.
        state = mtsComponentState::INITIALIZING;
        CMN_LOG_CLASS_RUN_WARNING << "InterfaceLCMCommands_ComponentGetState: failed to find required interface for component "
                                  << component.ComponentName << std::endl;
        return;
    }
    if (!functionSet->ComponentGetState.IsValid()) {
        state = mtsComponentState::INITIALIZING;
        CMN_LOG_CLASS_RUN_WARNING << "InterfaceLCMCommands_ComponentGetState: failed to execute \"Component GetState\" "
                                  << component << std::endl;
        return;
    }

    functionSet->ComponentGetState(state);
}

void mtsManagerComponentClient::InterfaceLCMCommands_GetInterfaceProvidedDescription(const mtsDescriptionInterface &intfc,
                                                                                     mtsInterfaceProvidedDescription & description) const
{
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    if (intfc.InterfaceProvidedNames.size() < 1) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_GetInterfaceProvidedDescription: provided interface name not specified" << std::endl;
        return;
    }
    if (!LCM->GetInterfaceProvidedDescription(intfc.ComponentName, intfc.InterfaceProvidedNames[0], description)) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_GetInterfaceProvidedDescription: failed to get description for component "
                                << intfc.ComponentName << " provided interface " << intfc.InterfaceProvidedNames[0] << std::endl;
        return;
    }
}

void mtsManagerComponentClient::InterfaceLCMCommands_GetInterfaceRequiredDescription(const mtsDescriptionInterface &intfc,
                                                                                     mtsInterfaceRequiredDescription & description) const
{
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    if (intfc.InterfaceRequiredNames.size() < 1) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_GetInterfaceRequiredDescription: required interface name not specified" << std::endl;
        return;
    }
    if (!LCM->GetInterfaceRequiredDescription(intfc.ComponentName, intfc.InterfaceRequiredNames[0], description)) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceLCMCommands_GetInterfaceRequiredDescription: failed to get description for component "
                                << intfc.ComponentName << " required interface " << intfc.InterfaceRequiredNames[0] << std::endl;
        return;
    }
}

// This function is thread-safe, thus it can be called as a qualified read command
void mtsManagerComponentClient::InterfaceLCMCommands_LoadLibrary(const std::string &fileName, bool &result) const
{
    osaDynamicLoader dl;
    result = dl.Load(fileName.c_str());
}

void mtsManagerComponentClient::InterfaceLCMCommands_GetListOfComponentClasses(
                                std::vector<mtsDescriptionComponentClass> & listOfComponentClasses) const
{
    // Loop through the class register, looking for components that can be created with one argument
    // or derived from mtsComponent (and have dynamic creation enabled).
    cmnClassRegister::const_iterator it = cmnClassRegister::begin();
    while (it != cmnClassRegister::end()) {
        if (it->second->OneArgConstructorAvailable()) {
            // CMN_DYNAMIC_CREATION_ONEARG or CMN_DYNAMIC_CREATION_SETNAME
            mtsDescriptionComponentClass classInfo;
            classInfo.ClassName = it->first;
            const cmnClassServicesBase *argServices = it->second->GetConstructorArgServices();
            if (argServices) {
                classInfo.ArgType = argServices->GetName();
                classInfo.ArgTypeId = argServices->TypeInfoPointer()->name();
            }
            listOfComponentClasses.push_back(classInfo);
        }
        else if (it->second->HasDynamicCreation() && it->second->IsDerivedFrom<mtsComponent>()) {
            // Backward compatibility (CMN_DYNAMIC_CREATION)
            mtsDescriptionComponentClass classInfo;
            classInfo.ClassName = it->first;
            classInfo.ArgType = "std::string";
            classInfo.ArgTypeId = typeid(std::string).name();
            listOfComponentClasses.push_back(classInfo);
        }
        it++;
    }
}

void mtsManagerComponentClient::InterfaceLCMCommands_SetLogForwarding(const bool &state)
{
    if (!state) {
        CMN_LOG_CLASS_RUN_VERBOSE << "Disabling log forwarding" << std::endl;
    }
    mtsManagerLocal::SetLogForwarding(state);
    if (state) {
        CMN_LOG_CLASS_RUN_VERBOSE << "Enabled log forwarding" << std::endl;
    }
}

void mtsManagerComponentClient::InterfaceLCMCommands_GetLogForwardingState(bool & state) const
{
    mtsManagerLocal::GetLogForwardingState(state);
}

void mtsManagerComponentClient::InterfaceLCMCommands_GetAbsoluteTimeInSeconds(double &time) const
{
    time = mtsManagerLocal::GetInstance()->GetTimeServer().GetAbsoluteTimeInSeconds();
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

void mtsManagerComponentClient::HandleMCSReadyEvent(void)
{
    //CMN_LOG_CLASS_INIT_VERBOSE << "MCS ready event" << std::endl;
    //MCSReady = true;
}

// DEPRECATED
void mtsManagerComponentClient::GetAbsoluteTimeInSeconds(mtsDouble &time) const
{
    time = mtsManagerLocal::GetInstance()->GetTimeServer().GetAbsoluteTimeInSeconds();
}
