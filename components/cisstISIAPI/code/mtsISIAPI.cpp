/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Nicolas Padoy
  Created on: 2010-04-06

  (C) Copyright 2010-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstISIAPI/mtsISIAPI.h>

#include <cisstCommon/cmnUnits.h>
#include <cisstOSAbstraction/osaSleep.h>

#include <cisstMultiTask/mtsInterfaceProvided.h>

#include <isi_api.h>


namespace mtsISIAPIUtilities
{
	const ISI_UINT ALL_DOFS[ISI_NUM_MAX_JOINTS] = { 0, 1, 2, 3, 4, 5, 6, 7 };

    ISI_MANIP_INDEX ManipulatorIndexToISI(mtsISIAPI::ManipulatorIndexType index)
    {
        switch (index) {
        case mtsISIAPI::MTML:
            return ISI_MTML1;
        case mtsISIAPI::MTMR:
            return ISI_MTMR1;
        case mtsISIAPI::PSM1:
            return ISI_PSM1;
        case mtsISIAPI::PSM2:
            return ISI_PSM2;
        case mtsISIAPI::PSM3:
            return ISI_PSM3;
        case mtsISIAPI::ECM1:
            return ISI_ECM; // Black Box API dropped the number
        }
        return ISI_NUM_MANIPS;
    }


    mtsISIAPI::ManipulatorIndexType ManipulatorIndexFromISI(ISI_MANIP_INDEX index)
    {
        switch (index) {
        case ISI_MTML1:
            return mtsISIAPI::MTML;
        case ISI_MTMR1:
            return mtsISIAPI::MTMR;
        case ISI_PSM1:
            return mtsISIAPI::PSM1;
        case ISI_PSM2:
            return mtsISIAPI::PSM2;
        case ISI_PSM3:
            return mtsISIAPI::PSM3;
        case ISI_ECM:
            return mtsISIAPI::ECM1; // Black Box API dropped the number
        }
        return mtsISIAPI::NUMBER_MANIPULATORS;
    }


    std::string StatusToString(ISI_STATUS status)
    {
        switch (status) {
        case ISI_SUCCESS:
            return "ISI_SUCCESS";
        case ISI_UNKNOWN_ERROR:
            return "ISI_UNKNOWN_ERROR";
        case ISI_INVALID_INPUT:
            return "ISI_INVALID_INPUT";
        case ISI_INVALID_OPERATION:
            return "ISI_INVALID_OPERATION";
        case ISI_INVALID_CONNECTION:
            return "ISI_INVALID_CONNECTION";
        case ISI_NULL_POINTER_ERROR:
            return "ISI_NULL_POINTER_ERROR";
        case ISI_COMMUNICATION_ERROR:
            return "ISI_COMMUNICATION_ERROR";
        case ISI_AUTHENTICATION_ERROR:
            return "ISI_AUTHENTICATION_ERROR";
        default:
            CMN_LOG_RUN_ERROR << "mtsISIAPIUtilities::StatusToString: unknown status code \"" << status << "\"" << std::endl;
        }
        return "Unknown error code";
    }


    std::string SystemConfigurationToString(const ISI_SYSTEM_CONFIG & systemConfiguration)
    {
        std::stringstream outputStream;
        outputStream << "System name: " << systemConfiguration.system_name << std::endl
                     << "System version: " << systemConfiguration.system_version << std::endl
                     << "Library version: " << systemConfiguration.library_version;
        return outputStream.str();
    }


    void ISICALLBACK StreamCallback(void * userData)
    {
        StreamCallbackInternal(userData);
    }


    void StreamCallbackInternal(void * userData)
    {
        // convert userData pointer
        mtsISIAPI * instance =
            reinterpret_cast<mtsISIAPI *>(userData);
        CMN_ASSERT(instance);
        instance->StreamCallback();
    }


    void ISICALLBACK EventCallback(ISI_MANIP_INDEX isiManipulatorIndex,
                                   ISI_EVENT_ID eventId,
                                   ISI_INT arguments[ISI_NUM_EVENT_ARGS], // introduced in isi_api 1.0.5
                                   void * userData)
    {
        EventCallbackInternal(isiManipulatorIndex, eventId, userData);
    }


    void EventCallbackInternal(int isiManipulatorIndex,
                               int eventId,
                               void * userData)
    {
        // convert userData pointer
        mtsISIAPI * instance =
            reinterpret_cast<mtsISIAPI *>(userData);
        CMN_ASSERT(instance);
        // convert ISI manipulator index
        mtsISIAPI::ManipulatorIndexType manipulatorIndex =
            mtsISIAPIUtilities::ManipulatorIndexFromISI(ISI_MANIP_INDEX(isiManipulatorIndex));
        instance->EventCallback(manipulatorIndex, eventId);
    }


    // Convert ISI_TRANSFORM to vctFrm3
    void FrameFromISI(const ISI_TRANSFORM & input, vctFrm3 & output)
    {
        // ISI_TRANSFORM.pos is a struct with 3 floats, x, y, and z
        output.Translation().Assign(vctFixedSizeConstVectorRef<float, 3, 1>(&(input.pos.x)));
        output.Translation().Multiply(1000.0); // ISI meters to cisst millimeters

        // rotation
        output.Rotation().Column(0).Assign(input.rot.row0.x, input.rot.row1.x, input.rot.row2.x);
        output.Rotation().Column(1).Assign(input.rot.row0.y, input.rot.row1.y, input.rot.row2.y);
        output.Rotation().Column(2).Assign(input.rot.row0.z, input.rot.row1.z, input.rot.row2.z);
    }

	// Convert vctFrm3 to ISI_TRANSFORM
	//TODO: can this be optimized ? (Nico)
    void FrameToISI(const vctFrm3 & input, ISI_TRANSFORM & output)
    {
		output.pos.x = input.Translation().X() / 1000.0;
		output.pos.y = input.Translation().Y() / 1000.0;
		output.pos.z = input.Translation().Z() / 1000.0;

		output.rot.row0.x = input.Rotation()[0][0];
		output.rot.row0.y = input.Rotation()[0][1];
		output.rot.row0.z = input.Rotation()[0][2];
		output.rot.row1.x = input.Rotation()[1][0];
		output.rot.row1.y = input.Rotation()[1][1];
		output.rot.row1.z = input.Rotation()[1][2];
		output.rot.row2.x = input.Rotation()[2][0];
		output.rot.row2.y = input.Rotation()[2][1];
		output.rot.row2.z = input.Rotation()[2][2];
    }


} // end of namespace mtsISIAPIUtilities


mtsISIAPI::ArmData::ArmData(void):
    StateTable(0),
    ProvidedInterface(0)
{}


const double mtsISIAPI::MasterArmData::SelectAngle = 0.2;
const double mtsISIAPI::MasterArmData::ClutchAngle = 0.7;

mtsISIAPI::MasterArmData::MasterArmData(void):
    SelectEventProvidedInterface(0),
    Selected(false),
    ClutchEventProvidedInterface(0),
    ClutchRestAngleNeedsUpdate(false),
    Clutched(false)
{}


mtsISIAPI::SlaveArmData::SlaveArmData(void)
{}


mtsISIAPI::ConsoleData::ConsoleData(void):
    ProvidedInterface(0),
    StandbyProvidedInterface(0),
    ReadyProvidedInterface(0),
    ClutchProvidedInterface(0),
    Clutched(false),
    CameraProvidedInterface(0),
    MastersAsMiceProvidedInterface(0),
    MastersAsMiced(false)
{}


CMN_IMPLEMENT_SERVICES(mtsISIAPI);


mtsISIAPI::mtsISIAPI(const std::string & name, unsigned int rateInHz):
    mtsTaskPeriodic(name,10 * cmn_ms),//TaskFromSignal(name),
    RateInHz(rateInHz)
{
    this->SetupAllInterfaces();
}


mtsISIAPI::~mtsISIAPI()
{
}


void mtsISIAPI::Startup(void)
{
    unsigned int attemptNumber = 1;
    while ((attemptNumber <= 5)
           && (!this->Connected)) {
        this->Connect();
        if (!this->Connected) {
            CMN_LOG_CLASS_INIT_WARNING << "Startup: attempt number " << attemptNumber << " to connect failed for \""
                                       << this->GetName() << "\"" << std::endl;
        }
        attemptNumber++;
        osaSleep(1.0 * cmn_s);
    }
    // check connection
    if (!this->Connected) {
        CMN_LOG_CLASS_INIT_ERROR << "Startup: can not connect \"" << this->GetName() << "\"" << std::endl;
        return;
    }
    // provide some log
    this->LogSystemConfiguration();
    this->LogManipulatorsAndToolsConfiguration();
    // configure and start stream
    this->ConfigureStream();
    this->StartStream();
    // configure events
    this->ConfigureEvents();
}


void mtsISIAPI::Run(void)
{
    this->ProcessQueuedCommands();
    this->ProcessQueuedEvents();
}


void mtsISIAPI::Cleanup(void)
{
    this->StopStream();
    this->Disconnect();
}


bool mtsISIAPI::Connect(void)
{
    ISI_STATUS status;
    status = isi_connect();
    if (status != ISI_SUCCESS) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: connection failed for \"" << this->GetName()
                                 << "\", status: "
                                 << mtsISIAPIUtilities::StatusToString(status) << std::endl;
        this->Connected = false;
    } else {
        CMN_LOG_CLASS_INIT_DEBUG << "Connect: connection succeedeed for \"" << this->GetName()
                                 << "\", status: "
                                 << mtsISIAPIUtilities::StatusToString(status) << std::endl;
        this->Connected = true;
    }
    return this->Connected;
}


bool mtsISIAPI::Disconnect(void)
{
    // check if system is connected
    if (!this->Connected) {
        CMN_LOG_CLASS_RUN_ERROR << "Disconnect: \"" << this->GetName()
                                << "\" not connected." << std::endl;
        return true;
    }
    // API calls
    isi_disconnect();
    this->Connected = false;
    return this->Connected;
}


bool mtsISIAPI::ConfigureStream(void)
{
    // check if system is connected
    if (!this->Connected) {
        CMN_LOG_CLASS_RUN_ERROR << "ConfigureStream: \"" << this->GetName()
                                << "\" not connected." << std::endl;
        return false;
    }
    // API calls
    ISI_STATUS status;
    status = isi_subscribe_all_stream_fields();
    if (status != ISI_SUCCESS) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureStream: subscribe all stream fields failed, status: "
                                 << mtsISIAPIUtilities::StatusToString(status) << std::endl;
        return false;
    }

    // register callback
    status = isi_set_stream_callback(mtsISIAPIUtilities::StreamCallback, this);
    if (status != ISI_SUCCESS) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureStream: failed to set stream callback, status: "
                                 << mtsISIAPIUtilities::StatusToString(status) << std::endl;
        return false;
    }

    return true;
}


void mtsISIAPI::StreamCallback(void)
{
    // API calls
    mtsISIAPI::ManipulatorIndexType index;
    ISI_MANIP_INDEX isiIndex;
    ISI_STATUS status;
    ISI_STREAM_FIELD streamData;
    vctDynamicConstVectorRef<float> jointRef;
    size_t numberOfJoints;
    bool eventCriterion;
    ArmData * arm;
    MasterArmData * masterArm;
    bool atLeastOneMasterSelected = false;
    prmEventButton buttonPayload;

    // subscribe to the API stream
    for (index = MTML;
         index < CONSOLE;
         index = static_cast<ManipulatorIndexType>(index + 1)) {
        // get local indices and pointers setup
        isiIndex = mtsISIAPIUtilities::ManipulatorIndexToISI(index);
        arm = this->Arms[index];
        arm->StateTable->Start();
        // joint values for all manipulators
        status = isi_get_stream_field(isiIndex, ISI_JOINT_VALUES, &streamData);
        if (status != ISI_SUCCESS) {
            CMN_LOG_CLASS_RUN_ERROR << "StreamCallback: get stream field failed for ISI_JOINT_VALUES, manipulator \""
                                    << ManipulatorIndexToString(index) << "\", status: "
                                    << mtsISIAPIUtilities::StatusToString(status) << std::endl;
        } else {
            numberOfJoints = GetNumberOfJoints(index);
            // check size of data
            if (streamData.count != numberOfJoints) {
                CMN_LOG_CLASS_RUN_ERROR << "StreamCallback: received wrong number of elements for ISI_JOINT_VALUES, manipulator \""
                                        << ManipulatorIndexToString(index) << "\", expected "
                                        << numberOfJoints << ", received "
                                        << streamData.count << std::endl;
            } else {
                // save the joint values
                jointRef.SetRef(numberOfJoints, streamData.data);
                arm->PositionJoint.Position().Assign(jointRef);
                // trigger events based on joint values for masters
                if ((index == MTML) || (index == MTMR)) {
                    masterArm = reinterpret_cast<MasterArmData *>(arm);
                    // compute angle on last joint to trigger select event
                    eventCriterion = ((masterArm->PositionJoint.Position()[numberOfJoints - 1]) < MasterArmData::SelectAngle);
                    if (eventCriterion && !masterArm->Selected) {
                        atLeastOneMasterSelected = true;
                        buttonPayload = prmEventButton::PRESSED;
                        masterArm->Select(buttonPayload);
                        masterArm->Selected = true;
                    } else if (!eventCriterion && masterArm->Selected) {
                        buttonPayload = prmEventButton::RELEASED;
                        masterArm->Select(buttonPayload);
                        masterArm->Selected = false;
                    }
                    // clutch on masters makes sense only when master clutch is on
                    if (this->Console.Clutched) {
                        // update clutch rest angle if needed
                        if (masterArm->ClutchRestAngleNeedsUpdate) {
                            masterArm->ClutchRestAngle = masterArm->PositionJoint.Position()[numberOfJoints - 2];
                            masterArm->ClutchRestAngleNeedsUpdate = false;
                        }
                        // compute angle on before last joint to trigger select event
                        eventCriterion = (fabs((masterArm->PositionJoint.Position()[numberOfJoints - 2])
                                               - masterArm->ClutchRestAngle) > MasterArmData::ClutchAngle);
                        if (eventCriterion && !masterArm->Clutched) {
                            buttonPayload = prmEventButton::PRESSED;
                            masterArm->Clutch(buttonPayload);
                            masterArm->Clutched = true;
                        } else if (!eventCriterion && masterArm->Clutched) {
                            buttonPayload = prmEventButton::RELEASED;
                            masterArm->Clutch(buttonPayload);
                            masterArm->Clutched = false;
                        }
                    }
                }
            }
        }

        // based on joints, figure out if we are entering MaM
        if (this->Console.Clutched // needs to be clutched
            && atLeastOneMasterSelected // needs to have at least one arm clutched in this iteration
            && !this->Console.MastersAsMiced // needs not to be in MaM already
            && this->MasterArms[0]->Selected // needs both arms to be clutched
            && this->MasterArms[1]->Selected) {
            buttonPayload.SetType(prmEventButton::PRESSED);
            this->Console.MastersAsMice(buttonPayload);
            this->Console.MastersAsMiced = true;
        }

        // tip transform for all manipulators
        status = isi_get_stream_field(isiIndex, ISI_TIP_TRANSFORM, &streamData);
        if (status != ISI_SUCCESS) {
            CMN_LOG_CLASS_RUN_ERROR << "StreamCallback: get stream field failed for ISI_TIP_TRANSFORM, manipulator  \""
                                    << ManipulatorIndexToString(index) << "\", status: "
                                    << mtsISIAPIUtilities::StatusToString(status) << std::endl;
        } else {
            // check size of data, 4 vectors
            if (streamData.count != 12) {
                CMN_LOG_CLASS_RUN_ERROR << "StreamCallback: received wrong number of elements for ISI_TIP_TRANSFORM, manipulator \""
                                        << ManipulatorIndexToString(index) << "\", expected 12, received "
                                        << streamData.count << std::endl;
            } else {
                ISI_TRANSFORM * isiTransform = reinterpret_cast<ISI_TRANSFORM *>(streamData.data);
                mtsISIAPIUtilities::FrameFromISI(*isiTransform,
                                                   arm->PositionCartesian.Position());
            }
        }

        // advance state table for this arm
        arm->StateTable->Advance();
    }

}


bool mtsISIAPI::ConfigureEvents(void)
{
    // check if system is connected
    if (!this->Connected) {
        CMN_LOG_CLASS_RUN_ERROR << "ConfigureEvents: \"" << this->GetName()
                                << "\" not connected." << std::endl;
        return false;
    }
    // API calls
    ISI_STATUS status;
    status = isi_subscribe_all_events();
    if (status != ISI_SUCCESS) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureEvents: subscribe all events failed, status: "
                                 << mtsISIAPIUtilities::StatusToString(status) << std::endl;
        return false;
    }

    // register callback
    status = isi_set_event_callback(mtsISIAPIUtilities::EventCallback, this);
    if (status != ISI_SUCCESS) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureEvent: failed to set event callback, status: "
                                 << mtsISIAPIUtilities::StatusToString(status) << std::endl;
        return false;
    }

    return true;
}


void mtsISIAPI::EventCallback(ManipulatorIndexType manipulatorIndex, int eventId)
{
    prmEventButton buttonPayload;
    switch (eventId) {
        // console events
    case ISI_API_MASTER_CLUTCH_ON:
        buttonPayload.SetType(prmEventButton::PRESSED);
        this->Console.Clutch(buttonPayload);
        this->Console.Clutched = true;
        // reset clutch angles on masters
        this->MasterArms[0]->ClutchRestAngleNeedsUpdate = true;
        this->MasterArms[1]->ClutchRestAngleNeedsUpdate = true;
        break;
    case ISI_API_MASTER_CLUTCH_OFF:
        buttonPayload.SetType(prmEventButton::RELEASED);
        this->Console.Clutch(buttonPayload);
        this->Console.Clutched = false;
        // exit MaM if needed
        if (this->Console.MastersAsMiced) {
            buttonPayload.SetType(prmEventButton::RELEASED);
            this->Console.MastersAsMice(buttonPayload);
            this->Console.MastersAsMiced = false;
        }
        break;
    case ISI_API_CAMERA_CONTROL_ON:
        buttonPayload.SetType(prmEventButton::PRESSED);
        this->Console.Camera(buttonPayload);
        break;
    case ISI_API_CAMERA_CONTROL_OFF:
        buttonPayload.SetType(prmEventButton::RELEASED);
        this->Console.Camera(buttonPayload);
        break;
    case ISI_API_HEAD_IN:
        this->Console.HeadIn();
        break;
    case ISI_API_HEAD_OUT:
        this->Console.HeadOut();
        break;
    case ISI_API_ARM_SWAP:
        this->Console.ClutchQuickTap();
        break;
    case ISI_API_VIDEO_SWAP:
        this->Console.CameraQuickTap();
        break;
    default:
        CMN_LOG_CLASS_RUN_WARNING << "EventCallback: event type not handled: " << eventId << std::endl;
        break;
    }
}


bool mtsISIAPI::StartStream(void)
{
    // check if system is connected
    if (!this->Connected) {
        CMN_LOG_CLASS_RUN_ERROR << "StartStream: \"" << this->GetName()
                                << "\" not connected." << std::endl;
        return false;
    }
    // API calls
    ISI_STATUS status;
    status = isi_start_stream(this->RateInHz);
    if (status != ISI_SUCCESS) {
        CMN_LOG_CLASS_INIT_ERROR << "StartStream: start stream failed, status: "
                                 << mtsISIAPIUtilities::StatusToString(status) << std::endl;
        return false;
    }
    return true;
}


void mtsISIAPI::StopStream(void)
{
    // check if system is connected
    if (!this->Connected) {
        CMN_LOG_CLASS_RUN_ERROR << "StopStream: \"" << this->GetName()
                                << "\" not connected." << std::endl;
        return;
    }
    // API calls
    isi_stop_stream();
}


void mtsISIAPI::LogSystemConfiguration(cmnLogLevel logLevel) const
{
    // check if system is connected
    if (!this->Connected) {
        CMN_LOG_CLASS_RUN_ERROR << "LogSystemConfiguration: \"" << this->GetName()
                                << "\" not connected." << std::endl;
        return;
    }
    // API call
    ISI_STATUS status;
    ISI_SYSTEM_CONFIG configuration;
    status = isi_get_system_config(&configuration);
    // log results
    if (status == ISI_SUCCESS) {
        CMN_LOG_CLASS(logLevel) << "System configuration for \"" << this->GetName() << "\":" << std::endl
                                << mtsISIAPIUtilities::SystemConfigurationToString(configuration) << std::endl;
    } else {
        CMN_LOG_CLASS_RUN_ERROR << "LogSystemConfiguration: failed to obtain system configuration for \""
                                << this->GetName() << "\", status: "
                                << mtsISIAPIUtilities::StatusToString(status) << std::endl;
    }
}


void mtsISIAPI::LogManipulatorConfiguration(ManipulatorIndexType index,
                                              cmnLogLevel logLevel) const
{
    // check if system is connected
    if (!this->Connected) {
        CMN_LOG_CLASS_RUN_ERROR << "LogManipulatorConfiguration: \"" << this->GetName()
                                << "\" not connected." << std::endl;
        return;
    }
    // check the manipulator index
    if ((index < MTML) || (index > ECM1)) {
        CMN_LOG_CLASS_INIT_ERROR << "LogManipulatorConfiguration: called with incorrect manipulator index" << std::endl;
        return;
    }
}


void mtsISIAPI::LogToolConfiguration(ManipulatorIndexType index,
                                       cmnLogLevel logLevel) const
{
    // check if system is connected
    if (!this->Connected) {
        CMN_LOG_CLASS_RUN_ERROR << "LogToolConfiguration: \"" << this->GetName()
                                << "\" not connected." << std::endl;
        return;
    }
    // check the manipulator index
    if ((index < PSM1) || (index > PSM3)) {
        CMN_LOG_CLASS_INIT_ERROR << "LogToolConfiguration: called with incorrect manipulator index" << std::endl;
        return;
    }
}


void mtsISIAPI::LogManipulatorsAndToolsConfiguration(cmnLogLevel logLevel) const
{
    // check if system is connected
    if (!this->Connected) {
        CMN_LOG_CLASS_RUN_ERROR << "LogManipulatorsConfiguration: \"" << this->GetName()
                                << "\" not connected." << std::endl;
        return;
    }
    // log for each manipulator
    ManipulatorIndexType manipulatorIndex;
    for (manipulatorIndex = MTML;
         manipulatorIndex <= ECM1;
         manipulatorIndex = static_cast<ManipulatorIndexType>(manipulatorIndex + 1)) {
        this->LogManipulatorConfiguration(manipulatorIndex, logLevel);
        if ((manipulatorIndex == PSM1)
            || (manipulatorIndex == PSM2)
            || (manipulatorIndex == PSM3)) {
            this->LogToolConfiguration(manipulatorIndex, logLevel);
        }
    }
}


const std::string & mtsISIAPI::ManipulatorIndexToString(ManipulatorIndexType manipulatorIndex)
{
    static const std::string manipulatorStrings[]
        = {"MTML",
           "MTMR",
           "PSM1",
           "PSM2",
           "PSM3",
           "ECM1",
           "Console"};
    static const std::string errorString("InvalidIndex");
    if (manipulatorIndex < NUMBER_MANIPULATORS) {
        return manipulatorStrings[manipulatorIndex];
    }
    return errorString;
}


size_t mtsISIAPI::GetNumberOfJoints(ManipulatorIndexType manipulatorIndex)
{
    static const size_t numberOfJoints[]
        = {8, // MTML
           8, // MTMR
           7, // PSM1
           7, // PSM2
           7, // PSM3
           4  // ECM1
    };
    if (manipulatorIndex < (NUMBER_MANIPULATORS - 1)) {
        return numberOfJoints[manipulatorIndex];
    }
    return 0;
}


void mtsISIAPI::SetupArmsInterfaces(void)
{
    CMN_LOG_CLASS_INIT_DEBUG << "SetupArmInterfaces: adding interfaces and state tables" << std::endl;
    ManipulatorIndexType manipulatorIndex;
    ArmData * arm;

    size_t numberOfJoints;
    std::string manipulatorName;
    // create data for all arms
    for (manipulatorIndex = MTML;
         manipulatorIndex <= ECM1;
         manipulatorIndex = static_cast<ManipulatorIndexType>(manipulatorIndex + 1)) {
        manipulatorName = mtsISIAPI::ManipulatorIndexToString(manipulatorIndex);
        CMN_LOG_CLASS_INIT_DEBUG << "SetupMastersInterfaces: setting up arm \""
                                 << manipulatorName << "\"" << std::endl;
        // create the structure based on arm type
        switch (manipulatorIndex) {
        case MTML:
        case MTMR:
            arm = new MasterArmData;
            break;
        case PSM1:
        case PSM2:
        case PSM3:
        case ECM1:
            arm = new SlaveArmData;
            break;
        }
        CMN_ASSERT(arm);
        Arms[manipulatorIndex - MTML] = arm;
        // add a state table using the size of the default state table
        arm->StateTable = new mtsStateTable(this->StateTable.GetHistoryLength(), manipulatorName);
        CMN_ASSERT(arm->StateTable);
        this->AddStateTable(arm->StateTable, true);
        arm->StateTable->SetAutomaticAdvance(false);
        // add a provided interface
        arm->ProvidedInterface = this->AddInterfaceProvided(manipulatorName);
        CMN_ASSERT(arm->ProvidedInterface);
        // resize containers for joint information
        numberOfJoints = mtsISIAPI::GetNumberOfJoints(manipulatorIndex);
        arm->PositionJoint.SetSize(numberOfJoints);
        arm->VelocityJoint.SetSize(numberOfJoints);
        // add to state table and provided interface
        arm->StateTable->AddData(arm->PositionCartesian, "PositionCartesian");
        arm->ProvidedInterface->AddCommandReadState(*(arm->StateTable),
                                                    arm->PositionCartesian, "GetPositionCartesian");
        arm->StateTable->AddData(arm->VelocityCartesian, "VelocityCartesian");
        arm->ProvidedInterface->AddCommandReadState(*(arm->StateTable),
                                                    arm->VelocityCartesian, "GetVelocityCartesian");
        arm->StateTable->AddData(arm->PositionJoint, "PositionJoint");
        arm->ProvidedInterface->AddCommandReadState(*(arm->StateTable),
                                                    arm->PositionJoint, "GetPositionJoint");
        arm->StateTable->AddData(arm->VelocityJoint, "VelocityJoint");
        arm->ProvidedInterface->AddCommandReadState(*(arm->StateTable),
                                                    arm->VelocityJoint, "GetVelocityJoint");
    }
}


void mtsISIAPI::SetupMastersInterfaces(void)
{
    CMN_LOG_CLASS_INIT_DEBUG << "SetupMastersInterfaces: adding master specific events and commands" << std::endl;
    ManipulatorIndexType manipulatorIndex;
    MasterArmData * masterArm;
    std::string manipulatorName;
    for (manipulatorIndex = MTML;
         manipulatorIndex <= MTMR;
         manipulatorIndex = static_cast<ManipulatorIndexType>(manipulatorIndex + 1)) {
        // retrieve point to arm from Arms array
        masterArm = static_cast<MasterArmData *>(this->Arms[manipulatorIndex]);
        CMN_ASSERT(masterArm);
        MasterArms[manipulatorIndex - MTML] = masterArm;
        // events and commands specific to master arms
        manipulatorName = mtsISIAPI::ManipulatorIndexToString(manipulatorIndex);
        // select button interface and event
        masterArm->SelectEventProvidedInterface = this->AddInterfaceProvided(manipulatorName + "Select");
        CMN_ASSERT(masterArm->SelectEventProvidedInterface);
        masterArm->SelectEventProvidedInterface->AddEventWrite(masterArm->Select, "Button", prmEventButton());
        // clutch button interface and event
        masterArm->ClutchEventProvidedInterface = this->AddInterfaceProvided(manipulatorName + "Clutch");
        CMN_ASSERT(masterArm->ClutchEventProvidedInterface);
        masterArm->ClutchEventProvidedInterface->AddEventWrite(masterArm->Clutch, "Button", prmEventButton());
    }
}


void mtsISIAPI::SetupSlavesInterfaces(void)
{
    CMN_LOG_CLASS_INIT_DEBUG << "SetupSlavesInterfaces: adding slave specific events and commands" << std::endl;
    ManipulatorIndexType manipulatorIndex;
    SlaveArmData * slaveArm;
    std::string manipulatorName;
    for (manipulatorIndex = PSM1;
         manipulatorIndex <= ECM1;
         manipulatorIndex = static_cast<ManipulatorIndexType>(manipulatorIndex + 1)) {
        // retrieve point to arm from Arms array
        slaveArm = static_cast<SlaveArmData *>(this->Arms[manipulatorIndex]);
        CMN_ASSERT(slaveArm);
        SlaveArms[manipulatorIndex - MTML] = slaveArm;
        // events and commands specific to slave arms
        // RCM
        slaveArm->StateTable->AddData(slaveArm->PositionCartesianRCM, "PositionCartesianRCM");
        slaveArm->ProvidedInterface->AddCommandReadState(*(slaveArm->StateTable),
                                                         slaveArm->PositionCartesianRCM, "GetPositionCartesianRCM");
        // Setup joints
        slaveArm->StateTable->AddData(slaveArm->PositionCartesianSetup, "PositionCartesianSetup");
        slaveArm->ProvidedInterface->AddCommandReadState(*(slaveArm->StateTable),
                                                         slaveArm->PositionCartesianSetup, "GetPositionCartesianSetup");
        slaveArm->PositionJointSetup.SetSize(6);
        slaveArm->StateTable->AddData(slaveArm->PositionJointSetup, "PositionJointSetup");
        slaveArm->ProvidedInterface->AddCommandReadState(*(slaveArm->StateTable),
                                                         slaveArm->PositionJointSetup, "GetPositionJointSetup");
    }
}


void mtsISIAPI::SetupCameraInterfaces(void)
{
    CMN_LOG_CLASS_INIT_DEBUG << "SetupCamerasInterfaces: adding camera specific events and commands" << std::endl;
    ManipulatorIndexType manipulatorIndex;
    SlaveArmData * cameraArm;
    std::string manipulatorName;
    for (manipulatorIndex = ECM1;
         manipulatorIndex <= ECM1;
         manipulatorIndex = static_cast<ManipulatorIndexType>(manipulatorIndex + 1)) {
        // retrieve point to arm from Arms array
        cameraArm = static_cast<SlaveArmData *>(this->Arms[manipulatorIndex]);
        CMN_ASSERT(cameraArm);
        CameraArms[manipulatorIndex - MTML] = cameraArm;
        // events and commands specific to camera arms
        // this is a placeholder, I am not sure we will ever need it
    }
}


void mtsISIAPI::SetupConsoleInterfaces(void)
{
    Console.ProvidedInterface = this->AddInterfaceProvided("Console");
    CMN_ASSERT(Console.ProvidedInterface);
    Console.ProvidedInterface->AddEventVoid(Console.HeadIn, "HeadIn");
    Console.ProvidedInterface->AddEventVoid(Console.HeadOut, "HeadOut");
    Console.ProvidedInterface->AddEventVoid(Console.ClutchQuickTap, "ClutchQuickTap");
    Console.ProvidedInterface->AddEventVoid(Console.CameraQuickTap, "CameraQuickTap");

    Console.StandbyProvidedInterface = this->AddInterfaceProvided("Standby");
    CMN_ASSERT(Console.StandbyProvidedInterface);
    Console.StandbyProvidedInterface->AddEventWrite(Console.Standby, "Button", prmEventButton());

    Console.ReadyProvidedInterface = this->AddInterfaceProvided("Ready");
    CMN_ASSERT(Console.ReadyProvidedInterface);
    Console.ReadyProvidedInterface->AddEventWrite(Console.Ready, "Button", prmEventButton());

    Console.ClutchProvidedInterface = this->AddInterfaceProvided("Clutch");
    CMN_ASSERT(Console.ClutchProvidedInterface);
    Console.ClutchProvidedInterface->AddEventWrite(Console.Clutch, "Button", prmEventButton());

    Console.CameraProvidedInterface = this->AddInterfaceProvided("Camera");
    CMN_ASSERT(Console.CameraProvidedInterface);
    Console.CameraProvidedInterface->AddEventWrite(Console.Camera, "Button", prmEventButton());

    Console.MastersAsMiceProvidedInterface = this->AddInterfaceProvided("MastersAsMice");
    CMN_ASSERT(Console.MastersAsMiceProvidedInterface);
    Console.MastersAsMiceProvidedInterface->AddEventWrite(Console.MastersAsMice, "Button", prmEventButton());
}


void mtsISIAPI::SetupAllInterfaces(void)
{
    // this must be called first
    this->SetupArmsInterfaces();
    // arm specific extra data
    this->SetupMastersInterfaces();
    this->SetupSlavesInterfaces();
    this->SetupCameraInterfaces();
    // console interface
    this->SetupConsoleInterfaces();
}
