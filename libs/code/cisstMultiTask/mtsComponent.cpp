/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

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
#include <cisstMultiTask/mtsParameterTypes.h>

#include <cisstOSAbstraction/osaGetTime.h>
#include <cisstOSAbstraction/osaSleep.h>

mtsComponent::mtsComponent(const std::string & componentName):
    Name(componentName),
    InterfacesProvidedOrOutput("InterfacesProvided"),
    InterfacesRequiredOrInput("InterfacesRequiredOrInput"),
    StateTables("StateTables")

{
    Initialize();
}


mtsComponent::mtsComponent(void):
    InterfacesProvidedOrOutput("InterfacesProvided"),
    InterfacesRequiredOrInput("InterfacesRequiredOrInput"),
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

    InterfacesProvidedOrOutput.SetOwner(*this);
    InterfacesRequiredOrInput.SetOwner(*this);
    this->StateTables.SetOwner(*this);

    InterfaceProvidedToManager = 0;
}


mtsComponent::mtsComponent(const mtsComponent & other):
    cmnGenericObject(other)
{
    cmnThrow("Class mtsComponent: copy constructor for mtsComponent should never be called");
}


mtsComponent::~mtsComponent()
{
    if (this->LoDMultiplexerStreambuf) {
        this->LoDMultiplexerStreambuf->RemoveAllChannels();
        delete this->LoDMultiplexerStreambuf;
        this->LoDMultiplexerStreambuf = 0;
    }
    if (this->LogFile) {
        this->LogFile->close();
        delete this->LogFile;
    }

    if (ManagerComponentServices) {
        delete ManagerComponentServices;
    }
}


const std::string & mtsComponent::GetName(void) const
{
    return this->Name;
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


void mtsComponent::Start(void)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Start: default start method for component \""
                               << this->GetName() << "\"" << std::endl;
    this->State = mtsComponentState::ACTIVE;
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
    this->State = mtsComponentState::FINISHED;
}


void mtsComponent::Configure(const std::string & CMN_UNUSED(filename))
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Configure: default start method for component \""
                               << this->GetName() << "\"" << std::endl;
}


std::vector<std::string> mtsComponent::GetNamesOfInterfacesProvidedOrOutput(void) const
{
    return InterfacesProvidedOrOutput.GetNames();
}


std::vector<std::string> mtsComponent::GetNamesOfInterfacesProvided(void) const
{
    std::vector<std::string> names;
    InterfacesProvidedListType::const_iterator iterator = InterfacesProvided.begin();
    const InterfacesProvidedListType::const_iterator end = InterfacesProvided.end();
    for (;
         iterator != end;
         ++iterator) {
        names.push_back((*iterator)->GetName());
    }
    return names;
}


std::vector<std::string> mtsComponent::GetNamesOfInterfacesOutput(void) const
{
    std::vector<std::string> names;
    InterfacesOutputListType::const_iterator iterator = InterfacesOutput.begin();
    const InterfacesOutputListType::const_iterator end = InterfacesOutput.end();
    for (;
         iterator != end;
         ++iterator) {
        names.push_back((*iterator)->GetName());
    }
    return names;
}


mtsInterfaceProvided * mtsComponent::AddInterfaceProvided(const std::string & interfaceProvidedName,
                                                          mtsInterfaceQueueingPolicy queueingPolicy)
{
    mtsInterfaceProvided * interfaceProvided;
    if ((queueingPolicy == MTS_COMPONENT_POLICY)
        || (queueingPolicy == MTS_COMMANDS_SHOULD_NOT_BE_QUEUED)) {
        interfaceProvided = new mtsInterfaceProvided(interfaceProvidedName, this, MTS_COMMANDS_SHOULD_NOT_BE_QUEUED);
    } else {
        CMN_LOG_CLASS_INIT_WARNING << "AddInterfaceProvided: adding provided interface \"" << interfaceProvidedName
                                   << "\" with policy MTS_COMMANDS_SHOULD_BE_QUEUED to component \""
                                   << this->GetName() << "\", make sure you call ProcessQueuedCommands to empty the queues" << std::endl;
        interfaceProvided = new mtsInterfaceProvided(interfaceProvidedName, this, MTS_COMMANDS_SHOULD_BE_QUEUED);
    }
    if (interfaceProvided) {
        if (InterfacesProvidedOrOutput.AddItem(interfaceProvidedName, interfaceProvided, CMN_LOG_LEVEL_INIT_ERROR)) {
            InterfacesProvided.push_back(interfaceProvided);
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


mtsInterfaceProvidedOrOutput *
mtsComponent::GetInterfaceProvidedOrOutput(const std::string & interfaceProvidedOrOutputName)
{
    return InterfacesProvidedOrOutput.GetItem(interfaceProvidedOrOutputName, CMN_LOG_LEVEL_RUN_VERBOSE);
}


mtsInterfaceProvided *
mtsComponent::GetInterfaceProvided(const std::string & interfaceProvidedName) const
{
    return dynamic_cast<mtsInterfaceProvided *>(InterfacesProvidedOrOutput.GetItem(interfaceProvidedName, CMN_LOG_LEVEL_RUN_VERBOSE));
}


mtsInterfaceOutput *
mtsComponent::GetInterfaceOutput(const std::string & interfaceOutputName) const
{
    return dynamic_cast<mtsInterfaceOutput *>(InterfacesProvidedOrOutput.GetItem(interfaceOutputName));
}


size_t mtsComponent::GetNumberOfInterfacesProvided(void) const
{
    return InterfacesProvided.size();
}


size_t mtsComponent::GetNumberOfInterfacesOutput(void) const
{
    return InterfacesOutput.size();
}


bool mtsComponent::RemoveInterfaceProvided(const std::string & interfaceProvidedName)
{
    mtsInterfaceProvided * interfaceProvided = GetInterfaceProvided(interfaceProvidedName);
    if (!interfaceProvided) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceProvided: no provided interface found: \""
                                << interfaceProvidedName << "\"" << std::endl;
        return false;
    }

    // MJUNG: right now we don't consider the "safe disconnection" issue as
    // follows (not complete) but have to deal with them in the future.
    //
    // \todo Need to remove (clean up as well) all provided interface
    // instances which were created by this interface.
    //
    // \todo Need to add clean-up codes before deleting interface object
    // itself through "safe disconnection" mechanism. Otherwise, a connected
    // required interface might try to use provided interface's resource which
    // are already deallocated and invalidated.
    // The "safe disconection" mechanism should include (at least)
    // - To disable required interface so that users cannot use it any more
    // - To disable provided interface and instances that it generated
    // - (something more...)
    // - To disconnect the connection that the provided interface was related to
    // - To deallocate provided interface and instances that it generated
    // Things to consider:
    // - Thread-safety issue between caller thread and main thread (main thread
    //   uses component's inner structure(s) to process commands and events)
    // - Safe clean-up to avoid run-time crash

    // MJUNG: this code is NOT thread safe.
    if (!InterfacesProvidedOrOutput.RemoveItem(interfaceProvidedName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceProvided: failed to remove provided interface \""
                                << interfaceProvidedName << "\"" << std::endl;
        return false;
    }

    bool removed = false;
    InterfacesProvidedListType::iterator it = InterfacesProvided.begin();
    const InterfacesProvidedListType::const_iterator itEnd = InterfacesProvided.end();
    for (; it != itEnd; ++it) {
        // MJUNG: this code is NOT thread safe.
        if (*it == interfaceProvided) {
            InterfacesProvided.erase(it);
            removed = true;
            break;
        }
    }

    if (!removed) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceProvided: failed to remove provided interface \""
                                 << interfaceProvidedName << "\" from list" << std::endl;
        return false;
    }

    delete interfaceProvided;

    CMN_LOG_CLASS_RUN_VERBOSE << "RemoveInterfaceProvided: removed provided interface \""
                              << interfaceProvidedName << "\"" << std::endl;
    return true;
}


bool mtsComponent::RemoveInterfaceOutput(const std::string & interfaceOutputName)
{
    mtsInterfaceOutput * interfaceOutput = GetInterfaceOutput(interfaceOutputName);
    if (!interfaceOutput) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceOutput: no output interface found: \""
                                << interfaceOutputName << "\"" << std::endl;
        return false;
    }

    if (!InterfacesProvidedOrOutput.RemoveItem(interfaceOutputName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceOutput: failed to remove output interface \""
                                << interfaceOutputName << "\"" << std::endl;
        return false;
    }

    bool removed = false;
    InterfacesOutputListType::iterator it = InterfacesOutput.begin();
    const InterfacesOutputListType::const_iterator itEnd = InterfacesOutput.end();
    for (; it != itEnd; ++it) {
        if (*it == interfaceOutput) {
            InterfacesOutput.erase(it);
            removed = true;
            break;
        }
    }

    if (!removed) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceOutput: failed to remove output interface \""
                                 << interfaceOutputName << "\" from list" << std::endl;
        return false;
    }

    delete interfaceOutput;
    CMN_LOG_CLASS_RUN_VERBOSE << "RemoveInterfaceOutput: removed output interface \""
                              << interfaceOutputName << "\"" << std::endl;
    return true;
}


mtsInterfaceRequiredOrInput *
mtsComponent::GetInterfaceRequiredOrInput(const std::string & interfaceRequiredOrInputName)
{
    return InterfacesRequiredOrInput.GetItem(interfaceRequiredOrInputName);
}


mtsInterfaceRequired *
mtsComponent::GetInterfaceRequired(const std::string & interfaceRequiredName)
{
    return dynamic_cast<mtsInterfaceRequired *>(InterfacesRequiredOrInput.GetItem(interfaceRequiredName));
}


mtsInterfaceInput *
mtsComponent::GetInterfaceInput(const std::string & interfaceInputName) const
{
    return dynamic_cast<mtsInterfaceInput *>(InterfacesRequiredOrInput.GetItem(interfaceInputName));
}


size_t mtsComponent::GetNumberOfInterfacesRequired(void) const
{
    return InterfacesRequired.size();
}


size_t mtsComponent::GetNumberOfInterfacesInput(void) const
{
    return InterfacesInput.size();
}


bool mtsComponent::RemoveInterfaceRequired(const std::string & interfaceRequiredName)
{
    mtsInterfaceRequired * interfaceRequired = GetInterfaceRequired(interfaceRequiredName);
    if (!interfaceRequired) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceRequired: no required interface found: \""
                                << interfaceRequiredName << "\"" << std::endl;
        return false;
    }

    // MJUNG: see comments in mtsComponent::RemoveInterfaceProvided() for
    // potential thread-safety issues

    // MJUNG: this code is NOT thread safe.
    if (!InterfacesRequiredOrInput.RemoveItem(interfaceRequiredName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceRequired: failed to remove required interface \""
                                 << interfaceRequiredName << "\"" << std::endl;
        return false;
    }

    bool removed = false;
    InterfacesRequiredListType::iterator it = InterfacesRequired.begin();
    const InterfacesRequiredListType::const_iterator itEnd = InterfacesRequired.end();
    for (; it != itEnd; ++it) {
        if (*it == interfaceRequired) {
            // MJUNG: this code is NOT thread safe.
            InterfacesRequired.erase(it);
            removed = true;
            break;
        }
    }

    if (!removed) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceRequired: failed to remove required interface \""
                                 << interfaceRequiredName << "\" from list" << std::endl;
        return false;
    }

    delete interfaceRequired;

    CMN_LOG_CLASS_RUN_VERBOSE << "RemoveInterfaceRequired: removed required interface \""
                              << interfaceRequiredName << "\"" << std::endl;
    return true;
}


bool mtsComponent::RemoveInterfaceInput(const std::string & interfaceInputName)
{
    mtsInterfaceInput * interfaceInput = GetInterfaceInput(interfaceInputName);
    if (!interfaceInput) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceInput: no required interface found: \""
                                << interfaceInputName << "\"" << std::endl;
        return false;
    }

    if (!InterfacesRequiredOrInput.RemoveItem(interfaceInputName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceInput: failed to remove required interface \""
                                << interfaceInputName << "\"" << std::endl;
        return false;
    }

    bool removed = false;
    InterfacesInputListType::iterator it = InterfacesInput.begin();
    const InterfacesInputListType::const_iterator itEnd = InterfacesInput.end();
    for (; it != itEnd; ++it) {
        if (*it == interfaceInput) {
            InterfacesInput.erase(it);
            removed = true;
            break;
        }
    }

    if (!removed) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceInput: failed to remove required interface \""
                                 << interfaceInputName << "\" from list" << std::endl;
        return false;
    }

    delete interfaceInput;
    CMN_LOG_CLASS_RUN_VERBOSE << "RemoveInterfaceInput: removed required interface \""
                              << interfaceInputName << "\"" << std::endl;
    return true;
}


mtsInterfaceRequired * mtsComponent::AddInterfaceRequiredExisting(const std::string & interfaceRequiredName,
                                                                  mtsInterfaceRequired * interfaceRequired) {
    if (InterfacesRequiredOrInput.AddItem(interfaceRequiredName, interfaceRequired)) {
        InterfacesRequired.push_back(interfaceRequired);
        return interfaceRequired;
    }
    return 0;
}


mtsInterfaceRequired * mtsComponent::AddInterfaceRequiredUsingMailbox(const std::string & interfaceRequiredName,
                                                                      mtsMailBox * mailBox,
                                                                      mtsRequiredType required)
{
    mtsInterfaceRequired * interfaceRequired = new mtsInterfaceRequired(interfaceRequiredName, mailBox, required);
    if (interfaceRequired) {
        if (InterfacesRequiredOrInput.AddItem(interfaceRequiredName, interfaceRequired)) {
            InterfacesRequired.push_back(interfaceRequired);
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
                                                          mtsRequiredType required) {
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
    if (InterfacesRequiredOrInput.AddItem(interfaceInputName, interfaceInput)) {
        InterfacesInput.push_back(interfaceInput);
        return interfaceInput;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceInputExisting: component \"" << this->GetName()
                             << "\" already has a required or input interface \"" << interfaceInputName
                             << "\"" << std::endl;
    return 0;
}


mtsInterfaceOutput * mtsComponent::AddInterfaceOutputExisting(const std::string & interfaceOutputName,
                                                              mtsInterfaceOutput * interfaceOutput) {
    if (InterfacesProvidedOrOutput.AddItem(interfaceOutputName, interfaceOutput)) {
        InterfacesOutput.push_back(interfaceOutput);
        return interfaceOutput;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceOutputExisting: component \"" << this->GetName()
                             << "\" already has a provided or output interface \"" << interfaceOutputName
                             << "\"" << std::endl;
    return 0;
}


std::vector<std::string> mtsComponent::GetNamesOfInterfacesRequiredOrInput(void) const {
    return InterfacesRequiredOrInput.GetNames();
}


std::vector<std::string> mtsComponent::GetNamesOfInterfacesRequired(void) const
{
    std::vector<std::string> names;
    InterfacesRequiredListType::const_iterator iterator = InterfacesRequired.begin();
    const InterfacesRequiredListType::const_iterator end = InterfacesRequired.end();
    for (;
         iterator != end;
         ++iterator) {
        names.push_back((*iterator)->GetName());
    }
    return names;
}


std::vector<std::string> mtsComponent::GetNamesOfInterfacesInput(void) const
{
    std::vector<std::string> names;
    InterfacesInputListType::const_iterator iterator = InterfacesInput.begin();
    const InterfacesInputListType::const_iterator end = InterfacesInput.end();
    for (;
         iterator != end;
         ++iterator) {
        names.push_back((*iterator)->GetName());
    }
    return names;
}


const mtsInterfaceProvidedOrOutput * mtsComponent::GetInterfaceProvidedOrOutputFor(const std::string & interfaceRequiredOrInputName) {
    mtsInterfaceRequiredOrInput * interfaceRequiredOrInput =
        InterfacesRequiredOrInput.GetItem(interfaceRequiredOrInputName);
    return interfaceRequiredOrInput ? interfaceRequiredOrInput->GetConnectedInterface() : 0;
}

#if 0  // Obsolete
// PK: Following code is obsolete -- connections should be made in mtsManagerComponentClient
// Right now, this is only used for input/output interfaces.
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
size_t mtsComponent::ProcessMailBoxes(InterfacesProvidedListType & interfaces)
{
    size_t numberOfCommands = 0;
    InterfacesProvidedListType::iterator iterator = interfaces.begin();
    const InterfacesProvidedListType::iterator end = interfaces.end();
    for (;
         iterator != end;
         ++iterator) {
        numberOfCommands += (*iterator)->ProcessMailBoxes();
    }
    return numberOfCommands;
}


size_t mtsComponent::ProcessQueuedEvents(void) {
    InterfacesRequiredListType::iterator iterator = InterfacesRequired.begin();
    const InterfacesRequiredListType::iterator end = InterfacesRequired.end();
    size_t numberOfEvents = 0;
    for (;
         iterator != end;
         iterator++) {
        numberOfEvents += (*iterator)->ProcessMailBoxes();
    }
    return numberOfEvents;
}


void mtsComponent::ToStream(std::ostream & outputStream) const
{
    outputStream << "Component name: " << Name << std::endl;
    InterfacesProvidedOrOutput.ToStream(outputStream);
    InterfacesRequiredOrInput.ToStream(outputStream);
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
    mtsInterfaceRequired * required = AddInterfaceRequired(
        mtsManagerComponentBase::InterfaceNames::InterfaceInternalRequired);
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

bool mtsComponent::AddInterfaceInternal(const bool useMangerComponentServices)
{
    // Add required interface
    std::string interfaceName;
    if (useMangerComponentServices) {
        // If a user component needs to use the dynamic component management services,
        // mtsComponent::EnableDynamicComponentManagement() should be called beforehand
        // in the user component's constructor so that the internal required interface and DCC
        // service object is properly initialized.
        // Only validity of such internal structures is checked here.
        mtsInterfaceRequired * required = GetInterfaceRequired(
            mtsManagerComponentBase::InterfaceNames::InterfaceInternalRequired);
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
    interfaceName = mtsManagerComponentBase::InterfaceNames::InterfaceInternalProvided;
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
    provided->AddEventWrite(EventGeneratorChangeState, mtsManagerComponentBase::EventNames::ChangeState,
                            mtsComponentStateChange());

    // Save this interface so that we can call it later (to process the mailboxes)
    InterfaceProvidedToManager = provided;

    return true;
}

// Internal command
void mtsComponent::InterfaceInternalCommands_GetEndUserInterface(const mtsEndUserInterfaceArg & argin,
                                                                 mtsEndUserInterfaceArg &argout)
{
    CMN_ASSERT(argin.OriginalInterface);
    CMN_LOG_CLASS_INIT_VERBOSE << "Starting internal GetEndUserInterface for " << argin.OriginalInterface->GetName() << std::endl;
    argout = argin;  // not really needed
    argout.EndUserInterface = 0;
    argout.EndUserInterface = argin.OriginalInterface->GetEndUserInterface(argin.UserName);
}

void mtsComponent::InterfaceInternalCommands_AddObserverList(const mtsEventHandlerList & argin,
                                                                   mtsEventHandlerList &argout)
{
    CMN_ASSERT(argin.Provided);
    argin.Provided->AddObserverList(argin, argout);
}

void mtsComponent::InterfaceInternalCommands_RemoveEndUserInterface(const mtsEndUserInterfaceArg & argin,
                                                                    mtsEndUserInterfaceArg &argout)
{
    CMN_ASSERT(argin.OriginalInterface);
    argout = argin;  // not really needed
    argout.EndUserInterface = argin.OriginalInterface->RemoveEndUserInterface(argin.EndUserInterface, argin.UserName);
}

void mtsComponent::InterfaceInternalCommands_RemoveObserverList(const mtsEventHandlerList & argin,
                                                                mtsEventHandlerList &argout)
{
    CMN_ASSERT(argin.Provided);
    argin.Provided->RemoveObserverList(argin, argout);
}
