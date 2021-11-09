/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsInterfaceOutput.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsInterfaceInput.h>
#include <cisstMultiTask/mtsManagerComponentBase.h>
#include <cisstMultiTask/mtsManagerComponentServices.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsParameterTypes.h>

#include <cisstOSAbstraction/osaGetTime.h>
#include <cisstOSAbstraction/osaSleep.h>

mtsComponent::mtsComponent(const std::string & componentName):
    Name(componentName),
    InterfacesProvided("InterfacesProvided"),
    InterfacesOutput("InterfacesOutput"),
    InterfacesRequired("InterfacesRequired"),
    InterfacesInput("InterfacesInput"),
    StateTables("StateTables")

{
    Initialize();
}


mtsComponent::mtsComponent(void):
    InterfacesProvided("InterfacesProvided"),
    InterfacesOutput("InterfacesOutput"),
    InterfacesRequired("InterfacesRequired"),
    InterfacesInput("InterfacesInput"),
    StateTables("StateTables")
{
    Initialize();
}

void mtsComponent::Initialize(void)
{
    UseSeparateLogFileFlag = false;
    LoDMultiplexerStreambuf = 0;
    LogFile = 0;

    ManagerComponentServices = 0;

    InterfacesProvided.SetOwner(*this);
    InterfacesRequired.SetOwner(*this);
    InterfacesOutput.SetOwner(*this);
    InterfacesInput.SetOwner(*this);
    this->StateTables.SetOwner(*this);

    InterfaceProvidedToManager = 0;

    ReplayMode = false;
}


mtsComponent::mtsComponent(const mtsComponent & other):
    cmnGenericObject(other)
{
    cmnThrow("Class mtsComponent: copy constructor for mtsComponent should never be called");
}


mtsComponent::~mtsComponent()
{
    KillSeparateLogFile();

    if (ManagerComponentServices) {
        delete ManagerComponentServices;
    }
}


#if CISST_HAS_JSON
void mtsComponent::ConfigureJSON(const Json::Value & configuration)
{
    const Json::Value jsonLog = configuration["log"];
    if (!jsonLog.empty()) {
        const Json::Value jsonAllow = jsonLog["allow"];
        if (!jsonAllow.empty()) {
            const std::string allow = jsonAllow.asString();
            const std::string className = Services()->GetName();
            if (allow == "none") {
                cmnLogger::SetMaskClass(className, CMN_LOG_ALLOW_NONE);
            } else if (allow == "errors") {
                cmnLogger::SetMaskClass(className, CMN_LOG_ALLOW_ERRORS);
            } else if (allow == "errors-and-warnings") {
                cmnLogger::SetMaskClass(className, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
            } else if (allow == "verbose") {
                cmnLogger::SetMaskClass(className, CMN_LOG_ALLOW_VERBOSE);
            } else if (allow == "debug") {
                cmnLogger::SetMaskClass(className, CMN_LOG_ALLOW_DEBUG);
            } else if (allow == "all") {
                cmnLogger::SetMaskClass(className, CMN_LOG_ALLOW_ALL);
            } else {
                CMN_LOG_CLASS_INIT_ERROR << "ConfigureJSON: failed to configure \""
                                         << this->GetName()
                                         << "\", the \"log\":\"allow\" value must be one of: "
                                         << "none, errors, errors-and-warnings, verbose, debug, all.  We found: \""
                                         << allow << "\"" << std::endl;
            }

        }
        const Json::Value jsonSeparateFile = jsonLog["separate-file"];
        if (!jsonSeparateFile.empty()) {
            if (jsonSeparateFile.isBool()) {
                if (jsonSeparateFile.asBool()) {
                    UseSeparateLogFileDefaultWithDate();
                }
            } else if (jsonSeparateFile.isString()) {
                UseSeparateLogFile(jsonSeparateFile.asString());
            }
        }
    }
}
#endif


const std::string & mtsComponent::GetName(void) const
{
    return this->Name;
}


void mtsComponent::GetName(std::string & placeHolder) const
{
    placeHolder = this->Name;
}


void mtsComponent::SetName(const std::string & componentName)
{
    this->Name = componentName;
}


void mtsComponent::Create(void)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Create: default create method for component \""
                               << this->GetName() << "\"" << std::endl;
    this->State = mtsComponentState::READY;
}


bool mtsComponent::CreateAndWait(double timeoutInSeconds)
{
    this->Create();
    return this->WaitForState(mtsComponentState::READY, timeoutInSeconds);
}


void mtsComponent::Start(void)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Start: default start method for component \""
                               << this->GetName() << "\"" << std::endl;
    this->Startup();
    this->State = mtsComponentState::ACTIVE;
}


bool mtsComponent::StartAndWait(double timeoutInSeconds)
{
    this->Start();
    return this->WaitForState(mtsComponentState::ACTIVE, timeoutInSeconds);
}


void mtsComponent::Suspend(void)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Suspend: default suspend method for component \""
                               << this->GetName() << "\"" << std::endl;
    this->State = mtsComponentState::READY;
}


void mtsComponent::Kill(void)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Kill: default kill method for component \""
                               << this->GetName() << "\"" << std::endl;
    this->Cleanup();
    this->State = mtsComponentState::FINISHED;
}


bool mtsComponent::KillAndWait(double timeoutInSeconds)
{
    this->Kill();
    return this->WaitForState(mtsComponentState::FINISHED, timeoutInSeconds);
}


void mtsComponent::Configure(const std::string & CMN_UNUSED(filename))
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Configure: default start method for component \""
                               << this->GetName() << "\"" << std::endl;
}


std::vector<std::string> mtsComponent::GetNamesOfInterfacesProvidedOrOutput(void) const
{
    std::vector<std::string> all = InterfacesProvided.GetNames();
    std::vector<std::string> added = InterfacesOutput.GetNames();
    all.insert(all.end(), added.begin(), added.end());
    return all;
}


std::vector<std::string> mtsComponent::GetNamesOfInterfacesProvided(void) const
{
    return InterfacesProvided.GetNames();
}


std::vector<std::string> mtsComponent::GetNamesOfInterfacesOutput(void) const
{
    return InterfacesOutput.GetNames();
}


bool mtsComponent::InterfaceExists(const std::string & interfaceName, cmnLogLevel lod) const
{
    return (InterfaceProvidedOrOutputExists(interfaceName, lod)
            || InterfaceRequiredOrInputExists(interfaceName, lod));
}


bool mtsComponent::InterfaceProvidedOrOutputExists(const std::string & interfaceName, cmnLogLevel lod) const
{
    if (this->InterfacesProvided.FindItem(interfaceName)) {
        CMN_LOG_CLASS(lod) << "InterfaceExists: found a provided interface with name \"" << interfaceName << "\"" << std::endl;
        return true;
    }
    if (this->InterfacesOutput.FindItem(interfaceName)) {
        CMN_LOG_CLASS(lod) << "InterfaceExists: found an output interface with name \"" << interfaceName << "\"" << std::endl;
        return true;
    }
    return false;
}


bool mtsComponent::InterfaceRequiredOrInputExists(const std::string & interfaceName, cmnLogLevel lod) const
{
    if (this->InterfacesRequired.FindItem(interfaceName)) {
        CMN_LOG_CLASS(lod) << "InterfaceExists: found a required interface with name \"" << interfaceName << "\"" << std::endl;
        return true;
    }
    if (this->InterfacesInput.FindItem(interfaceName)) {
        CMN_LOG_CLASS(lod) << "InterfaceExists: found an input interface with name \"" << interfaceName << "\"" << std::endl;
        return true;
    }
    return false;
}


mtsInterfaceProvided * mtsComponent::AddInterfaceProvided(const std::string & interfaceProvidedName,
                                                          mtsInterfaceQueueingPolicy queueingPolicy)
{
    mtsInterfaceProvided * interfaceProvided =
        this->AddInterfaceProvidedWithoutSystemEvents(interfaceProvidedName,
                                                      queueingPolicy);
    if (interfaceProvided) {
        interfaceProvided->AddSystemEvents();
    }
    return interfaceProvided;
}


mtsInterfaceProvided * mtsComponent::AddInterfaceProvidedWithoutSystemEvents(const std::string & interfaceProvidedName,
                                                                             mtsInterfaceQueueingPolicy queueingPolicy,
                                                                             bool isProxy)
{
    if (this->InterfaceProvidedOrOutputExists(interfaceProvidedName, CMN_LOG_LEVEL_INIT_ERROR)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceProvided: component " << this->GetName() << " already has an interface named \""
                                 << interfaceProvidedName << "\"" << std::endl;
        return 0;
    }
    mtsInterfaceProvided * interfaceProvided;
    if ((queueingPolicy == MTS_COMPONENT_POLICY)
        || (queueingPolicy == MTS_COMMANDS_SHOULD_NOT_BE_QUEUED)) {
        interfaceProvided = new mtsInterfaceProvided(interfaceProvidedName, this, MTS_COMMANDS_SHOULD_NOT_BE_QUEUED, 0, isProxy);
    } else {
        CMN_LOG_CLASS_INIT_WARNING << "AddInterfaceProvided: adding provided interface \"" << interfaceProvidedName
                                   << "\" with policy MTS_COMMANDS_SHOULD_BE_QUEUED to component \""
                                   << this->GetName() << "\", make sure you call ProcessQueuedCommands to empty the queues" << std::endl;
        interfaceProvided = new mtsInterfaceProvided(interfaceProvidedName, this, MTS_COMMANDS_SHOULD_BE_QUEUED, 0, isProxy);
    }
    if (interfaceProvided) {
        if (InterfacesProvided.AddItem(interfaceProvidedName, interfaceProvided, CMN_LOG_LEVEL_INIT_ERROR)) {
            return interfaceProvided;
        }
        CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceProvided: component " << this->GetName() << " unable to add interface \""
                                 << interfaceProvidedName << "\"" << std::endl;
        delete interfaceProvided;
        return 0;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceProvided: component " << this->GetName() << " unable to create interface \""
                             << interfaceProvidedName << "\"" << std::endl;
    return 0;
}


mtsInterfaceOutput * mtsComponent::AddInterfaceOutput(const std::string & interfaceOutputName)
{
    CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceOutput: adding output interface \"" << interfaceOutputName
                             << "\" to component \"" << this->GetName()
                             << "\" can't be performed from mtsComponent, this method must be derived." << std::endl;
    return 0;
}


mtsInterfaceProvided *
mtsComponent::GetInterfaceProvided(const std::string & interfaceProvidedName) const
{
    return InterfacesProvided.GetItem(interfaceProvidedName, CMN_LOG_LEVEL_RUN_VERBOSE);
}


mtsInterfaceOutput *
mtsComponent::GetInterfaceOutput(const std::string & interfaceOutputName) const
{
    return InterfacesOutput.GetItem(interfaceOutputName, CMN_LOG_LEVEL_RUN_VERBOSE);
}


size_t mtsComponent::GetNumberOfInterfacesProvided(void) const
{
    return InterfacesProvided.size();
}


size_t mtsComponent::GetNumberOfInterfacesOutput(void) const
{
    return InterfacesOutput.size();
}


bool mtsComponent::RemoveInterfaceProvided(const std::string & interfaceProvidedName, const bool skipDisconnect)
{
    mtsInterfaceProvided * interfaceProvided = GetInterfaceProvided(interfaceProvidedName);
    if (!interfaceProvided) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceProvided: no provided interface found: \""
                                << interfaceProvidedName << "\"" << std::endl;
        return false;
    }

    // This check might be not necessary -- interfaceProvided should always be the original interface.
    if (interfaceProvided->GetOriginalInterface()) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceProvided: failed to remove provided interface \""
                                << interfaceProvidedName << "\""
                                << ", did not get original interface." << std::endl;
        return false;
    }

    if (!skipDisconnect) {
        // Disconnect all active connections
        int userCount = interfaceProvided->GetNumberOfEndUsers();
        int removedUserCount = 0;
        const std::string serverComponentName = GetName();
        const std::string serverInterfaceProvidedName = interfaceProvidedName;
        std::string clientComponentName, clientInterfaceRequiredName;
        mtsComponent * clientComponent = 0;
        mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();

        // Get a list of user names (names of required interfaces)
        std::vector<std::string> userNames = interfaceProvided->GetListOfUserNames();
        // Get a list of components in the same process
        std::vector<std::string> componentNames = LCM->GetNamesOfComponents();

        std::vector<std::string>::const_iterator it = userNames.begin();
        const std::vector<std::string>::const_iterator itEnd = userNames.end();
        for (; it != itEnd; ++it) {
            clientInterfaceRequiredName = *it;
            // Look for component which owns the required interface
            std::vector<std::string>::const_iterator _it = componentNames.begin();
            const std::vector<std::string>::const_iterator _itEnd = componentNames.end();
            for (; _it != _itEnd; ++_it) {
                clientComponent = LCM->GetComponent(*_it);
                if (!clientComponent) continue;
                if (clientComponent->GetInterfaceRequired(clientInterfaceRequiredName)) {
                    // MJ: Don't use MCC/MCS service here because some of internal connections
                    // can be possibly disconnected.
                    if (!LCM->Disconnect(clientComponentName, clientInterfaceRequiredName,
                                         serverComponentName, serverInterfaceProvidedName))
                        {
                            CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceProvided: failed to remove provided interface \""
                                                    << interfaceProvidedName << "\""
                                                    << ", failed to disconnect interfaces: "
                                                    << clientComponentName << ":" << clientInterfaceRequiredName << " - "
                                                    << serverComponentName << ":" << serverInterfaceProvidedName << std::endl;
                            return false;
                        } else {
                        ++removedUserCount;
                    }
                    break;
                }
            }
        }

        if (userCount != removedUserCount) {
            CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceProvided: failed to remove provided interface \""
                                    << interfaceProvidedName << "\""
                                    << ", some of connections are not removed."
                                    << std::endl;
        }
    }

    if (!InterfacesProvided.RemoveItem(interfaceProvidedName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceProvided: failed to remove provided interface \""
                                << interfaceProvidedName << "\"" << std::endl;
        return false;
    }

    delete interfaceProvided;
    CMN_LOG_CLASS_RUN_VERBOSE << "RemoveInterfaceProvided: removed provided interface \""
                              << interfaceProvidedName << "\"" << std::endl;
    return true;
}


mtsInterfaceRequired *
mtsComponent::GetInterfaceRequired(const std::string & interfaceRequiredName)
{
    return InterfacesRequired.GetItem(interfaceRequiredName, CMN_LOG_LEVEL_RUN_VERBOSE);
}


mtsInterfaceInput *
mtsComponent::GetInterfaceInput(const std::string & interfaceInputName) const
{
    return InterfacesInput.GetItem(interfaceInputName, CMN_LOG_LEVEL_RUN_VERBOSE);
}


size_t mtsComponent::GetNumberOfInterfacesRequired(void) const
{
    return InterfacesRequired.size();
}


size_t mtsComponent::GetNumberOfInterfacesInput(void) const
{
    return InterfacesInput.size();
}


bool mtsComponent::RemoveInterfaceRequired(const std::string & interfaceRequiredName, const bool skipDisconnect)
{
    // MJ: Note that this method should be called only by the GCM so that the LCM and the GCM can
    // synchronize its internal information even though the LCM doesn't notify this change to the GCM.
    mtsInterfaceRequired * interfaceRequired = GetInterfaceRequired(interfaceRequiredName);
    if (!interfaceRequired) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceRequired: no required interface found: \""
                                << interfaceRequiredName << "\"" << std::endl;
        return false;
    }

    if (!skipDisconnect) {
        mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
        mtsInterfaceProvided * serverInterfaceProvided = const_cast<mtsInterfaceProvided*>(interfaceRequired->GetConnectedInterface());
        // If this required interface has an established connection, disconnect it first using MCC.
        if (serverInterfaceProvided) {
            const std::string clientComponentName = GetName();
            const std::string clientInterfaceRequiredName = interfaceRequiredName;
            const std::string serverComponentName = serverInterfaceProvided->GetComponent()->GetName();
            const std::string serverInterfaceProvidedName = serverInterfaceProvided->GetName();

            // MJ: Don't use MCC/MCS service here because some of internal connections
            // can be possibly disconnected.
            if (!LCM->Disconnect(clientComponentName, clientInterfaceRequiredName,
                                 serverComponentName, serverInterfaceProvidedName))
                {
                    CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceRequired: failed to remove required interface \""
                                            << interfaceRequiredName << "\""
                                            << ", failed to disconnect interfaces: "
                                            << clientComponentName << ":" << clientInterfaceRequiredName << " - "
                                            << serverComponentName << ":" << serverInterfaceProvidedName << std::endl;
                    return false;
                }
        }
    }

    // Now clean up internal data structures
    if (!InterfacesRequired.RemoveItem(interfaceRequiredName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceRequired: failed to remove required interface \""
                                << interfaceRequiredName << "\"" << std::endl;
        return false;
    }

    delete interfaceRequired;
    CMN_LOG_CLASS_RUN_VERBOSE << "RemoveInterfaceRequired: removed required interface \""
                              << interfaceRequiredName << "\"" << std::endl;
    return true;
}


mtsInterfaceRequired * mtsComponent::AddInterfaceRequiredExisting(const std::string & interfaceRequiredName,
                                                                  mtsInterfaceRequired * interfaceRequired)
{
    if (this->InterfaceRequiredOrInputExists(interfaceRequiredName, CMN_LOG_LEVEL_INIT_ERROR)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceRequired: component " << this->GetName() << " already has an interface named \""
                                 << interfaceRequiredName << "\"" << std::endl;
        return 0;
    }
    if (InterfacesRequired.AddItem(interfaceRequiredName, interfaceRequired)) {
        return interfaceRequired;
    }
    return 0;
}


mtsInterfaceRequired * mtsComponent::AddInterfaceRequiredUsingMailbox(const std::string & interfaceRequiredName,
                                                                      mtsMailBox * mailBox,
                                                                      mtsRequiredType required)
{
    if (this->InterfaceRequiredOrInputExists(interfaceRequiredName, CMN_LOG_LEVEL_INIT_ERROR)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceRequired: component " << this->GetName() << " already has an interface named \""
                                 << interfaceRequiredName << "\"" << std::endl;
        return 0;
    }
    mtsInterfaceRequired * interfaceRequired = new mtsInterfaceRequired(interfaceRequiredName, this, mailBox, required);
    if (interfaceRequired) {
        if (InterfacesRequired.AddItem(interfaceRequiredName, interfaceRequired)) {
            return interfaceRequired;
        }
        CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceRequired: unable to add interface \""
                                 << interfaceRequiredName << "\"" << std::endl;
        if (interfaceRequired) {
            delete interfaceRequired;
        }
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceRequired: unable to create interface for \""
                                 << interfaceRequiredName << "\"" << std::endl;
    }
    return 0;
}


mtsInterfaceRequired * mtsComponent::AddInterfaceRequired(const std::string & interfaceRequiredName,
                                                          mtsRequiredType required)
{
    if (this->InterfaceRequiredOrInputExists(interfaceRequiredName, CMN_LOG_LEVEL_INIT_ERROR)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceRequired: component " << this->GetName() << " already has an interface named \""
                                 << interfaceRequiredName << "\"" << std::endl;
        return 0;
    }
    mtsInterfaceRequired * interfaceRequired =
        this->AddInterfaceRequiredWithoutSystemEventHandlers(interfaceRequiredName,
                                                             required);
    if (interfaceRequired) {
        interfaceRequired->AddSystemEventHandlers();
    }
    return interfaceRequired;
}


mtsInterfaceRequired * mtsComponent::AddInterfaceRequiredWithoutSystemEventHandlers(const std::string & interfaceRequiredName,
                                                                                    mtsRequiredType required)
{
    // by default, no mailbox for base component, events are not queued
    return this->AddInterfaceRequiredUsingMailbox(interfaceRequiredName, 0, required);
}


mtsInterfaceInput * mtsComponent::AddInterfaceInput(const std::string & interfaceInputName)
{
    CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceInput: adding input interface \"" << interfaceInputName
                             << "\" to component \"" << this->GetName()
                             << "\" can't be performed from mtsComponent, this method must be derived." << std::endl;
    return 0;
}


mtsInterfaceInput * mtsComponent::AddInterfaceInputExisting(const std::string & interfaceInputName,
                                                            mtsInterfaceInput * interfaceInput) {
    if (this->InterfaceRequiredOrInputExists(interfaceInputName, CMN_LOG_LEVEL_INIT_ERROR)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceInput: component " << this->GetName() << " already has an interface named \""
                                 << interfaceInputName << "\"" << std::endl;
        return 0;
    }
    if (InterfacesInput.AddItem(interfaceInputName, interfaceInput)) {
        return interfaceInput;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceInputExisting: component \"" << this->GetName()
                             << "\" already has a required or input interface \"" << interfaceInputName
                             << "\"" << std::endl;
    return 0;
}


mtsInterfaceOutput * mtsComponent::AddInterfaceOutputExisting(const std::string & interfaceOutputName,
                                                              mtsInterfaceOutput * interfaceOutput) {
    if (this->InterfaceProvidedOrOutputExists(interfaceOutputName, CMN_LOG_LEVEL_INIT_ERROR)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceOutput: component " << this->GetName() << " already has an interface named \""
                                 << interfaceOutputName << "\"" << std::endl;
        return 0;
    }
    if (InterfacesOutput.AddItem(interfaceOutputName, interfaceOutput)) {
        return interfaceOutput;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceOutputExisting: component \"" << this->GetName()
                             << "\" already has a provided or output interface \"" << interfaceOutputName
                             << "\"" << std::endl;
    return 0;
}


std::vector<std::string> mtsComponent::GetNamesOfInterfacesRequiredOrInput(void) const
{
    std::vector<std::string> all = InterfacesRequired.GetNames();
    std::vector<std::string> added = InterfacesInput.GetNames();
    all.insert(all.end(), added.begin(), added.end());
    return all;
}


std::vector<std::string> mtsComponent::GetNamesOfInterfacesRequired(void) const
{
    return InterfacesRequired.GetNames();
}


std::vector<std::string> mtsComponent::GetNamesOfInterfacesInput(void) const
{
    return InterfacesInput.GetNames();
}


const mtsInterfaceProvided * mtsComponent::GetInterfaceProvidedFor(const std::string & interfaceRequiredName) {
    mtsInterfaceRequired * interfaceRequired =
        InterfacesRequired.GetItem(interfaceRequiredName, CMN_LOG_LEVEL_RUN_VERBOSE);
    return interfaceRequired ? interfaceRequired->GetConnectedInterface() : 0;
}

#if 0  // Obsolete
// PK: Following code is obsolete -- connections should be made in mtsManagerComponentClient
// In retrospect, maybe it was not a good idea to combine Required/Input and Provided/Output interfaces.
bool mtsComponent::ConnectInterfaceRequiredOrInput(const std::string & interfaceRequiredOrInputName,
                                                   mtsInterfaceProvidedOrOutput * interfaceProvidedOrOutput)
{
    mtsInterfaceRequiredOrInput * interfaceRequiredOrInput =
        InterfacesRequiredOrInput.GetItem(interfaceRequiredOrInputName);
    if (interfaceRequiredOrInput) {
        if (interfaceRequiredOrInput->ConnectTo(interfaceProvidedOrOutput)) {
            CMN_LOG_CLASS_INIT_VERBOSE << "ConnectInterfaceRequiredOrInput: component \""
                                       << this->GetName()
                                       << "\" required/input interface \"" << interfaceRequiredOrInputName
                                       << "\" successfully connected to provided/output interface \""
                                       << interfaceProvidedOrOutput->GetName() << "\"" << std::endl;
            return true;
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectInterfaceRequiredOrInput: component \""
                                     << this->GetName()
                                     << "\" required/input interface \"" << interfaceRequiredOrInputName
                                     << "\" failed to connect to provided/output interface \""
                                     << interfaceProvidedOrOutput->GetName() << "\"" << std::endl;
        }
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectInterfaceRequiredOrInput: component \""
                                 << this->GetName()
                                 << "\" doesn't have required/input interface \""
                                 << interfaceRequiredOrInputName << "\"" << std::endl;
    }
    return false;
}
#endif  // OBSOLETE

mtsStateTable * mtsComponent::GetStateTable(const std::string & stateTableName)
{
    return this->StateTables.GetItem(stateTableName, CMN_LOG_LEVEL_INIT_ERROR);
}


bool mtsComponent::AddStateTable(mtsStateTable * existingStateTable, bool addInterfaceProvided)
{
    const std::string tableName = existingStateTable->GetName();
    const std::string interfaceName = "StateTable" + tableName;
    if (!this->StateTables.AddItem(tableName,
                                   existingStateTable,
                                   CMN_LOG_LEVEL_INIT_ERROR)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddStateTable: can't add state table \"" << tableName
                                 << "\" to task \"" << this->GetName() << "\"" << std::endl;
        return false;
    }
    if (addInterfaceProvided) {
        mtsInterfaceProvided * providedInterface = this->AddInterfaceProvided(interfaceName);
        if (!providedInterface) {
            CMN_LOG_CLASS_INIT_ERROR << "AddStateTable: can't add provided interface \"" << interfaceName
                                     << "\" to task \"" << this->GetName() << "\"" << std::endl;
            return false;
        }
        providedInterface->AddCommandWrite(&mtsStateTable::DataCollectionStart,
                                           existingStateTable,
                                           "StartCollection");
        providedInterface->AddCommandWrite(&mtsStateTable::DataCollectionStop,
                                           existingStateTable,
                                           "StopCollection");
        providedInterface->AddCommandRead(&mtsStateTable::GetIndexReader,
                                          existingStateTable,
                                          "GetIndexReader");
        providedInterface->AddEventWrite(existingStateTable->DataCollection.BatchReady,
                                         "BatchReady", mtsStateTable::IndexRange());
        providedInterface->AddEventVoid(existingStateTable->DataCollection.CollectionStarted,
                                        "CollectionStarted");
        providedInterface->AddEventWrite(existingStateTable->DataCollection.CollectionStopped,
                                         "CollectionStopped", mtsUInt());
        providedInterface->AddEventWrite(existingStateTable->DataCollection.Progress,
                                         "Progress", mtsUInt());
    }
    CMN_LOG_CLASS_INIT_DEBUG << "AddStateTable: added state table \"" << tableName
                             << "\" and corresponding interface \"" << interfaceName
                             << "\" to task \"" << this->GetName() << "\"" << std::endl;
    return true;
}


// Execute all commands in the mailbox.  This is just a temporary implementation, where
// all commands in a mailbox are executed before moving on the next mailbox.  The final
// implementation will probably look at timestamps.  We may also want to pass in a
// parameter (enum) to set the mailbox processing policy.
size_t mtsComponent::ProcessMailBoxes(InterfacesProvidedMapType & interfaces)
{
    size_t numberOfCommands = 0;
    InterfacesProvidedMapType::iterator iterator = interfaces.begin();
    const InterfacesProvidedMapType::iterator end = interfaces.end();
    for (;
         iterator != end;
         ++iterator) {
        numberOfCommands += iterator->second->ProcessMailBoxes();
    }
    return numberOfCommands;
}


size_t mtsComponent::ProcessQueuedEvents(void) {
    InterfacesRequiredMapType::iterator iterator = InterfacesRequired.begin();
    const InterfacesRequiredMapType::iterator end = InterfacesRequired.end();
    size_t numberOfEvents = 0;
    for (;
         iterator != end;
         iterator++) {
        numberOfEvents += iterator->second->ProcessMailBoxes();
    }
    return numberOfEvents;
}


void mtsComponent::ToStream(std::ostream & outputStream) const
{
    outputStream << "Component name: " << Name << std::endl;
    InterfacesProvided.ToStream(outputStream);
    InterfacesRequired.ToStream(outputStream);
    InterfacesOutput.ToStream(outputStream);
    InterfacesInput.ToStream(outputStream);
}


void mtsComponent::UseSeparateLogFileDefault(bool forwardToLogger)
{
    std::string filename = this->GetName() + "-log.txt";
    this->UseSeparateLogFile(filename, forwardToLogger);
}


void mtsComponent::UseSeparateLogFileDefaultWithDate(bool forwardToLogger)
{
    std::string currentDateTime;
    osaGetDateTimeString(currentDateTime);
    std::string filename = this->GetName() + "-" + currentDateTime + "-log.txt";
    this->UseSeparateLogFile(filename, forwardToLogger);
}


void mtsComponent::UseSeparateLogFile(const std::string & filename, bool forwardToLogger)
{
    if (this->UseSeparateLogFileFlag) {
        CMN_LOG_CLASS_INIT_ERROR << "UseSeparateLogFile: flag already set for component \""
                                 << this->GetName() << "\", no-op" << std::endl;
        return;
    }
    CMN_LOG_CLASS_INIT_DEBUG << "UseSeparateLogFile: called for component \""
                             << this->GetName() << "\"" << std::endl;
    // create the multiplexer and log file if needed
    if (!this->LoDMultiplexerStreambuf) {
        this->LoDMultiplexerStreambuf = new cmnLogger::StreamBufType();
    }
    // if there is already a log file, remove it from multiplexer, close it and delete it
    if (this->LogFile) {
        this->LoDMultiplexerStreambuf->RemoveChannel(*(this->LogFile));
        this->LogFile->close();
        delete this->LogFile;
        CMN_LOG_CLASS_INIT_DEBUG << "UseSeparateLogFile: closed and removed previous log file for component \""
                                 << this->GetName() << "\"" << std::endl;
    }
    // create a new log file and add it to the multiplexer
    this->LogFile = new std::ofstream();
    this->LogFile->open(filename.c_str());
    if (this->LogFile->is_open()) {
        // set the multiplexer and change flag at the end!
        CMN_LOG_CLASS_INIT_DEBUG << "UseSeparateLogFile: opened log file \"" << filename
                                 << "\" for component \"" << this->GetName() << "\"" << std::endl;
        this->LoDMultiplexerStreambuf->AddChannel(*(this->LogFile), CMN_LOG_ALLOW_ALL);
        if (forwardToLogger) {
            // note that if the component multiplexer already existed
            // and the cmnLogger multiplexer was already added, this
            // line has no effect
            this->LoDMultiplexerStreambuf->AddMultiplexer(cmnLogger::GetMultiplexer());
        }
        this->UseSeparateLogFileFlag = true;
    } else {
        // can't open file, do not switch flag
        CMN_LOG_CLASS_INIT_ERROR << "UseSeparateLogFile: can't open log file \"" << filename
                                 << "\" for component \"" << this->GetName() << "\"" << std::endl;
    }
}


cmnLogger::StreamBufType * mtsComponent::GetLogMultiplexer(void) const
{
    if (this->UseSeparateLogFileFlag) {
        ThisType * nonConstThis = const_cast<ThisType *>(this);
        return nonConstThis->LoDMultiplexerStreambuf;
    }
    return cmnGenericObject::GetLogMultiplexer();
}


void mtsComponent::KillSeparateLogFile(void)
{
    if (this->LoDMultiplexerStreambuf) {
        this->LoDMultiplexerStreambuf->RemoveAllChannels();
        delete this->LoDMultiplexerStreambuf;
        this->LoDMultiplexerStreambuf = 0;
    }
    if (this->LogFile) {
        this->LogFile->close();
        delete this->LogFile;
        this->LogFile = 0;
    }
}


bool mtsComponent::AreAllInterfacesRequiredConnected(const bool log)
{
    // loop through the required interfaces and make sure they are all
    // connected. The method doesn't end if it finds an interface that
    // is not connected so it can log all un-connected interfaces.
    bool allConnected = true;
    InterfacesRequiredMapType::const_iterator requiredIterator = InterfacesRequired.begin();
    const mtsInterfaceProvided * connectedInterface;
    for (;
         requiredIterator != InterfacesRequired.end();
         requiredIterator++) {
        connectedInterface = requiredIterator->second->GetConnectedInterface();
        if (!connectedInterface) {
            if (requiredIterator->second->IsRequired() == MTS_REQUIRED) {
                if (log) {
                    CMN_LOG_CLASS_INIT_ERROR << "AreAllInterfacesRequiredConnected: component \"" << this->GetName()
                                             << "\", void pointer to required/input interface \""
                                             << requiredIterator->first
                                             << "\" (required/input not connected to provided/output)" << std::endl;
                    allConnected = false;
                } else {
                    // no log, we can just abort
                    return false;
                }
            }
            else if (log) {
                CMN_LOG_CLASS_INIT_VERBOSE << "AreAllInterfacesRequiredConnected: component \"" << this->GetName()
                                           << "\", void pointer to optional required/input interface \""
                                           << requiredIterator->first
                                           << "\" (required/input not connected to provided/output)" << std::endl;
            }
        }
    }
    return allConnected;
}


bool mtsComponent::IsRunning(void) const
{
    return (this->State == mtsComponentState::ACTIVE);
}


bool mtsComponent::IsStarted(void) const
{
    return (this->State >= mtsComponentState::READY);
}


bool mtsComponent::IsTerminated(void) const
{
    return (this->State == mtsComponentState::FINISHED);
}


bool mtsComponent::IsEndTask(void) const
{
    return (this->State >= mtsComponentState::FINISHING);
}


const mtsComponentState & mtsComponent::GetState(void) const
{
    return this->State;
}

void mtsComponent::GetState(mtsComponentState &state) const
{
    state = this->State;
}

bool mtsComponent::WaitForState(mtsComponentState desiredState, double timeout)
{
    if (timeout != 0.0) {
        CMN_LOG_CLASS_INIT_VERBOSE << "WaitForState: called for component \"" << this->GetName()
                                   << "\" for state \"" << desiredState << "\" has no effect for mtsComponent" << std::endl;
    }
    return true;
}

mtsInterfaceRequired * mtsComponent::EnableDynamicComponentManagement(void)
{
    // Extend internal required interface (to Manager Component) to include event handlers
    mtsInterfaceRequired * required = AddInterfaceRequired(mtsManagerComponentBase::GetNameOfInterfaceInternalRequired());
    if (!required) {
        CMN_LOG_CLASS_INIT_ERROR << "EnableDynamicComponentManagement: failed to add internal required interface to component "
                                 << "\"" << GetName() << "\"" << std::endl;
        return 0;
    } else {
        CMN_LOG_CLASS_INIT_VERBOSE << "EnableDynamicComponentManagement: successfully added internal required interface" << std::endl;
    }

    // Check if manager component service object has already been created
    if (ManagerComponentServices) {
        CMN_LOG_CLASS_INIT_WARNING << "EnableDynamicComponentManagement: redefine ManagerComponentServices object" << std::endl;
        delete ManagerComponentServices;
    }

    ManagerComponentServices = new mtsManagerComponentServices(required);
    ManagerComponentServices->InitializeInterfaceInternalRequired();

    CMN_LOG_CLASS_INIT_VERBOSE << "EnableDynamicComponentManagement: successfully initialized dynamic component management services" << std::endl;

    return required;
}

bool mtsComponent::AddInterfaceInternal(const bool useManagerComponentServices)
{
    // Add required interface
    std::string interfaceName;
    if (useManagerComponentServices) {
        // If a user component needs to use the dynamic component management services,
        // mtsComponent::EnableDynamicComponentManagement() should be called beforehand
        // in the user component's constructor so that the internal required interface and DCC
        // service object is properly initialized.
        // Only validity of such internal structures is checked here.
        mtsInterfaceRequired * required = GetInterfaceRequired(mtsManagerComponentBase::GetNameOfInterfaceInternalRequired());
        if (!required) {
            CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceInternal: dynamic component management service (1) is not properly initialized" << std::endl;
            return false;
        }
        if (!ManagerComponentServices) {
            CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceInternal: dynamic component management service (2) is not properly initialized" << std::endl;
            return false;
        }
    }

    if (InterfaceProvidedToManager) {
        CMN_LOG_CLASS_INIT_WARNING << "InterfaceProvidedToManager already initialized!" << std::endl;
        return true;
    }
    // Add provided interface (can't be done in constructor)
    interfaceName = mtsManagerComponentBase::GetNameOfInterfaceInternalProvided();
    mtsInterfaceProvided *provided = AddInterfaceProvided(interfaceName);
    if (!provided) {
        CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceInternal: failed to add internal provided interface: " << interfaceName << std::endl;
        return false;
    }
    provided->AddCommandVoid(&mtsComponent::Start,
                             this, mtsManagerComponentBase::CommandNames::ComponentStart, MTS_COMMAND_NOT_QUEUED);
    provided->AddCommandVoid(&mtsComponent::Suspend,
                             this, mtsManagerComponentBase::CommandNames::ComponentStop, MTS_COMMAND_NOT_QUEUED);
    provided->AddCommandVoid(&mtsComponent::Start,
                             this, mtsManagerComponentBase::CommandNames::ComponentResume, MTS_COMMAND_NOT_QUEUED);
    provided->AddCommandRead(&mtsComponent::GetState,
                             this, mtsManagerComponentBase::CommandNames::ComponentGetState);
    provided->AddCommandWriteReturn(&mtsComponent::InterfaceInternalCommands_GetEndUserInterface, this,
                                    mtsManagerComponentBase::CommandNames::GetEndUserInterface);
    provided->AddCommandWriteReturn(&mtsComponent::InterfaceInternalCommands_AddObserverList, this,
                                    mtsManagerComponentBase::CommandNames::AddObserverList);
    provided->AddCommandWriteReturn(&mtsComponent::InterfaceInternalCommands_RemoveEndUserInterface, this,
                                    mtsManagerComponentBase::CommandNames::RemoveEndUserInterface);
    provided->AddCommandWriteReturn(&mtsComponent::InterfaceInternalCommands_RemoveObserverList, this,
                                    mtsManagerComponentBase::CommandNames::RemoveObserverList);
    provided->AddCommandWriteReturn(&mtsComponent::InterfaceInternalCommands_ComponentCreate, this,
                                    mtsManagerComponentBase::CommandNames::ComponentCreate);
    provided->AddCommandWrite(&mtsComponent::InterfaceInternalCommands_ComponentStartOther, this,
                              mtsManagerComponentBase::CommandNames::ComponentStart);
    provided->AddEventWrite(EventGeneratorChangeState, mtsManagerComponentBase::EventNames::ChangeState,
                            mtsComponentStateChange());

    // Save this interface so that we can call it later (to process the mailboxes)
    InterfaceProvidedToManager = provided;

    return true;
}

// Internal command
void mtsComponent::InterfaceInternalCommands_GetEndUserInterface(const mtsEndUserInterfaceArg & argin,
                                                                 mtsEndUserInterfaceArg & argout)
{
    CMN_ASSERT(argin.OriginalInterface);
    argout = argin;  // not really needed
    argout.EndUserInterface = 0;
    argout.EndUserInterface =
        reinterpret_cast<size_t>(reinterpret_cast<mtsInterfaceProvided*>(argin.OriginalInterface)
                                 ->GetEndUserInterface(argin.UserName));
}

void mtsComponent::InterfaceInternalCommands_AddObserverList(const mtsEventHandlerList & argin,
                                                             mtsEventHandlerList & argout)
{
    CMN_ASSERT(argin.Provided);
    argin.Provided->AddObserverList(argin, argout);
}

void mtsComponent::InterfaceInternalCommands_RemoveEndUserInterface(const mtsEndUserInterfaceArg & argin,
                                                                    mtsEndUserInterfaceArg & argout)
{
    CMN_ASSERT(argin.OriginalInterface);
    argout = argin;  // not really needed
    argout.EndUserInterface =
        reinterpret_cast<size_t>(reinterpret_cast<mtsInterfaceProvided*>(argin.OriginalInterface)
                                 ->RemoveEndUserInterface(reinterpret_cast<mtsInterfaceProvided*>(argin.EndUserInterface), argin.UserName));
}

void mtsComponent::InterfaceInternalCommands_RemoveObserverList(const mtsEventHandlerList & argin,
                                                                mtsEventHandlerList & argout)
{
    CMN_ASSERT(argin.Provided);
    argin.Provided->RemoveObserverList(argin, argout);
}

// Code was previously in mtsManagerComponentClient::CreateAndAddNewComponent
void mtsComponent::InterfaceInternalCommands_ComponentCreate(const mtsDescriptionComponent & componentDescription, bool & result)
{
    // Try to create component as requested
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();

    mtsComponent * newComponent = LCM->CreateComponentDynamically(componentDescription.ClassName,
                                                                  componentDescription.ComponentName,
                                                                  componentDescription.ConstructorArgSerialized);
    result = false;
    if (newComponent) {
        if (LCM->AddComponent(newComponent)) {
            CMN_LOG_CLASS_RUN_VERBOSE << GetName() << ": successfully created and added component: "
                                      << "\"" << componentDescription.ComponentName << "\" of type \""
                                      << componentDescription.ClassName << "\"" << std::endl;
            result = true;
        }
        else {
            CMN_LOG_CLASS_RUN_ERROR << GetName() << ": failed to add component: "
                                    << "\"" << componentDescription.ComponentName << "\" of type \""
                                    << componentDescription.ClassName << "\"" << std::endl;
        }
    }
    else {
        CMN_LOG_CLASS_RUN_ERROR << GetName() << ": failed to create component: "
                                << "\"" << componentDescription.ComponentName << "\" of type \""
                                << componentDescription.ClassName << "\"" << std::endl;
    }
}

void mtsComponent::InterfaceInternalCommands_ComponentStartOther(const mtsComponentStatusControl & arg)
{
    mtsManagerLocal *LCM = mtsManagerLocal::GetInstance();
    mtsComponent *component = LCM->GetComponent(arg.ComponentName);
    if (component) {
        if (component->GetState() == mtsComponentState::CONSTRUCTED) {
            // Start an internal thread (if needed)
            component->Create();
            // Wait for internal thread to be created
            osaSleep(arg.DelayInSecond);
        }

        // Start the component
        component->Start();
    }
    else
        CMN_LOG_CLASS_RUN_ERROR << GetName() << ": could not find component " << arg.ComponentName
                                << " to start" << std::endl;
}

bool mtsComponent::SetReplayMode(void) {
    if (!(this->State == mtsComponentState::CONSTRUCTED)) {
        CMN_LOG_CLASS_RUN_ERROR << "SetReplayMode: component \"" << GetName() << "\" failed to set replay mode." << std::endl;
        return false;
    }
    this->ReplayMode = true;
    return true;
}

bool mtsComponent::SetReplayData(const std::string & stateTableName, const std::string & fileName) {
    if (!this->ReplayMode) {
        CMN_LOG_CLASS_RUN_ERROR << "SetReplayData: component \"" << GetName() << "\" not in replay mode." << std::endl;
        return false;
    }

    // find state table and disable AutoAdvance
    mtsStateTable * stateTable = this->StateTables.GetItem(stateTableName);
    if (!stateTable) {
        CMN_LOG_CLASS_RUN_ERROR << "SetReplayData: component \"" << GetName() << "\",  state table \"" << stateTableName << "\" was not found." << std::endl;
        return false;
    }
    stateTable->SetAutomaticAdvance(false);

    // get number of columns, i.e. all elements in state table
    const size_t numberOfElements = stateTable->GetNumberOfElements();
    mtsGenericObject * data = 0;
    // make sure we don't overwrite timestamp when we will advance the state table
    for (size_t index = 0; index < numberOfElements;index++) {
        data = stateTable->GetStateVectorElement(index);
        data->SetAutomaticTimestamp(false);
    }

    std::ifstream input(fileName.c_str());
    if (!input.is_open()) {
        CMN_LOG_CLASS_RUN_ERROR << "SetReplayData: component \"" << GetName() << "\" unable to open file \"" << fileName << "\"." << std::endl;
        return false;
    }

    // code below is adapted from http://stackoverflow.com/questions/3482064/counting-the-number-of-lines-in-a-text-file/3482093#3482093
    // new lines will be skipped unless we stop it from happening:
    input.unsetf(std::ios_base::skipws);

    // count the newlines with an algorithm specialized for counting:
    size_t numberOfLines = std::count(std::istream_iterator<char>(input),
                                      std::istream_iterator<char>(),
                                      '\n');

    // return to the beginning of the file and restore whitespace skipping
    input.seekg(0);
    input.setf(std::ios_base::skipws);

    // resize state table and start loading data
    stateTable->SetSize(numberOfLines);
    for (size_t line = 0; line < numberOfLines; line++) {
        stateTable->Start();
        for (size_t index = 0; index < numberOfElements; index++) {
            data = stateTable->GetStateVectorElement(index);
            std::cerr << "data before de-serialization: " << *data << std::endl;
            if (!data) {
                CMN_LOG_CLASS_RUN_ERROR << "SetReplayData: component \"" << GetName() << "\", state table \""
                                        << stateTableName << "\", failed to retrieve data element " << index << std::endl;
                return false;
            }
            data->FromStreamRaw(input, ',');
            std::cerr << "reader data " << index << " = " << *data << std::endl;
            if (index != numberOfElements-1) {
                char c;
                input >> c;
                if( c != ',') {
                    CMN_LOG_CLASS_RUN_ERROR << "SetReplayData: component \"" << GetName() << "\", error parsing state table \""
                                            << stateTableName << "\" at line " << line << ", element " << index << std::endl;
                }
            }
        }
        stateTable->Advance();
        stateTable->ReplayAdvance();
    }
    return true;
}

bool mtsComponent::SetReplayTime(const double CMN_UNUSED(time)) {
    if (!this->ReplayMode) {
        CMN_LOG_CLASS_RUN_ERROR << "SetReplayTime: component \"" << GetName() << "\" not in replay mode." << std::endl;
        return false;
    }
    return true;
}
