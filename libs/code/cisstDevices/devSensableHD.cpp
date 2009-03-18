/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Anton Deguet
  Created on: 2008-04-04

  (C) Copyright 2008-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

// Sensable headers
#include <HD/hd.h>
#include <HDU/hduError.h>

// Define this before including mtsTaskFromCallback.h (which is included by
// devSensableHD.h).
#define MTS_TASK_CALLBACK_CONVENTION HDCALLBACK
#include <cisstDevices/devSensableHD.h>

CMN_IMPLEMENT_SERVICES(devSensableHD);

struct devSensableHDDeviceData {
    HHD DeviceHandle;
    bool DeviceEnabled;
    
    // local copy of the buttons state as defined by Sensable
    cmnInt Buttons;
    
    // local copy of the position and velocities
    prmPositionCartesianGet PositionCartesian;
    prmVelocityCartesianGet VelocityCartesian;
    prmPositionJointGet PositionJoint;
    vctDynamicVectorRef<double> GimbalJointsRef;
    
    // mtsFunction called to broadcast the event
    mtsFunctionWrite Button1Event, Button2Event;
	
    // local buffer used to store the position as provided
    // by Sensable
    typedef vctFixedSizeMatrix<double, 4, 4, VCT_COL_MAJOR> Frame4x4Type;
    Frame4x4Type Frame4x4;
    vctFixedSizeConstVectorRef<double, 3, Frame4x4Type::ROWSTRIDE> Frame4x4TranslationRef;
    vctFixedSizeConstMatrixRef<double, 3, 3,
                               Frame4x4Type::ROWSTRIDE, Frame4x4Type::COLSTRIDE> Frame4x4RotationRef;
    
    // added to provide tip position to the frame manager
    mtsFunctionRead PositionFunctionForTransformationManager;
};


struct devSensableHDDriverData {
    HDSchedulerHandle CallbackHandle;
    HDCallbackCode CallbackReturnValue;
};


// The task Run method
void devSensableHD::Run(void)
{
    int currentButtons;
    DevicesMapType::iterator iterator = this->Devices.begin();
    const DevicesMapType::iterator end = this->Devices.end();
    devSensableHDDeviceData * deviceData;
    HHD hHD;

    for (; iterator != end; iterator++) {
        currentButtons = 0;
        deviceData = iterator->second;

        // begin haptics frame
        hHD = deviceData->DeviceHandle;
        hdMakeCurrentDevice(hHD);
        hdBeginFrame(hHD);

        // get the current cartesian position of the device
        hdGetDoublev(HD_CURRENT_TRANSFORM, deviceData->Frame4x4.Pointer());

        // retrieve cartesian velocities, write directly in state data
        hdGetDoublev(HD_CURRENT_VELOCITY, deviceData->VelocityCartesian.VelocityLinear().Pointer());
        hdGetDoublev(HD_CURRENT_ANGULAR_VELOCITY, deviceData->VelocityCartesian.VelocityAngular().Pointer());
        
        // retrive joint positions, write directly in state data
        hdGetDoublev(HD_CURRENT_JOINT_ANGLES, deviceData->PositionJoint.Position().Pointer());
        hdGetDoublev(HD_CURRENT_GIMBAL_ANGLES, deviceData->GimbalJointsRef.Pointer());

        // retrieve the current button(s).
        hdGetIntegerv(HD_CURRENT_BUTTONS, &currentButtons);

        // end haptics frame
        hdEndFrame(hHD);

        // time stamp used to date data
        mtsStateIndex stateIndex = this->StateTable.GetIndexWriter();

        // copy transformation to the state table
        deviceData->PositionCartesian.SetStateIndex(stateIndex);
        deviceData->PositionCartesian.Position().Translation().Assign(deviceData->Frame4x4TranslationRef);
        deviceData->PositionCartesian.Position().Rotation().Assign(deviceData->Frame4x4RotationRef);
        
        // time stamp velocity
        deviceData->VelocityCartesian.SetStateIndex(stateIndex);

        // compare to previous value to create events
        if (currentButtons != deviceData->Buttons) {
            int currentButtonState, previousButtonState;
            prmEventButton event;
            event.SetStateIndex(stateIndex); 
            // test for button 1
            currentButtonState = currentButtons & HD_DEVICE_BUTTON_1;
            previousButtonState = deviceData->Buttons.Data & HD_DEVICE_BUTTON_1;
            if (currentButtonState != previousButtonState) {
                if (currentButtonState == 0) {
                    event.SetType(prmEventButton::RELEASED);
                } else {
                    event.SetType(prmEventButton::PRESSED);
                }
                // throw the event
                deviceData->Button1Event(event);
            }
            // test for button 2
            currentButtonState = currentButtons & HD_DEVICE_BUTTON_2;
            previousButtonState = deviceData->Buttons & HD_DEVICE_BUTTON_2;
            if (currentButtonState != previousButtonState) {
                if (currentButtonState == 0) {
                    event.SetType(prmEventButton::RELEASED);
                } else {
                    event.SetType(prmEventButton::PRESSED);
                }
                // throw the event
                deviceData->Button2Event(event);
            }
            // save previous buttons state
            deviceData->Buttons.Data = currentButtons;
        }
    }

    // check for errors and abort the callback if a scheduler error
    HDErrorInfo error;
    if (HD_DEVICE_ERROR(error = hdGetError())) {
        CMN_LOG(5) << "devSensableHDCallback: Device error detected";
        if (hduIsSchedulerError(&error)) {
            CMN_LOG(5) << "devSensableHDCallback: Scheduler error detected";
            this->Driver->CallbackReturnValue = HD_CALLBACK_DONE;
            return;
        }
    }

    // always last, sync the state table
    this->StateTable.Advance();
    
    // return flag to continue calling this function
    this->Driver->CallbackReturnValue = HD_CALLBACK_CONTINUE;
}


devSensableHD::devSensableHD(const std::string & taskName):
    mtsTaskFromCallbackAdapter(taskName, 5000)
{
    CMN_LOG_CLASS(4) << "constructor called, looking for \"DefaultArm\"" << std::endl;
    Devices["DefaultArm"] = 0;
    this->SetupInterfaces();
}


devSensableHD::devSensableHD(const std::string & taskName, const std::string & firstDeviceName):
    mtsTaskFromCallbackAdapter(taskName, 5000)
{
    CMN_LOG_CLASS(4) << "constructor called, looking for \"" << firstDeviceName << "\"" << std::endl;
    Devices[firstDeviceName] = 0;
    this->SetupInterfaces();
}


devSensableHD::devSensableHD(const std::string & taskName,
                             const std::string & firstDeviceName,
                             const std::string & secondDeviceName):
    mtsTaskFromCallbackAdapter(taskName, 5000)
{
    if (firstDeviceName == secondDeviceName) {
        CMN_LOG_CLASS(1) << "In constructor: name of devices provided are identical, \""
                         << firstDeviceName << "\" and \""
                         << secondDeviceName << "\"" << std::endl;
    }
    CMN_LOG_CLASS(4) << "constructor called, looking for \"" << firstDeviceName
                     << "\" and \"" << secondDeviceName << "\"" << std::endl;
    Devices[firstDeviceName] = 0;
    Devices[secondDeviceName] = 0;
    this->SetupInterfaces();
}


void devSensableHD::SetupInterfaces(void)
{
    this->Driver = new devSensableHDDriverData;
    CMN_ASSERT(this->Driver);

    DevicesMapType::iterator iterator = this->Devices.begin();
    const DevicesMapType::iterator end = this->Devices.end();
    devSensableHDDeviceData * deviceData;
    std::string interfaceName;
    mtsProvidedInterface * providedInterface;

    for (; iterator != end; iterator++) {
        // allocate memory
        iterator->second = new devSensableHDDeviceData;
        // use local data pointer to make code more readable
        deviceData = iterator->second;
        CMN_ASSERT(deviceData);
        interfaceName = iterator->first;

        deviceData->Frame4x4TranslationRef.SetRef(deviceData->Frame4x4.Column(3), 0);
        deviceData->Frame4x4RotationRef.SetRef(deviceData->Frame4x4, 0, 0);
        deviceData->PositionJoint.Position().SetSize(NB_JOINTS);
        deviceData->GimbalJointsRef.SetRef(deviceData->PositionJoint.Position(), 3, 3);

        // set to zero
        deviceData->PositionJoint.Position().SetAll(0.0);
        deviceData->VelocityCartesian.VelocityLinear().SetAll(0.0);
        deviceData->VelocityCartesian.VelocityAngular().SetAll(0.0);

        // create interface with the device name, i.e. the map key
        CMN_LOG_CLASS(4) << "SetupInterfaces: creating interface \"" << interfaceName << "\"" << std::endl;
        providedInterface = this->AddProvidedInterface(interfaceName);

        // add the state data to the table
        this->StateTable.AddData(deviceData->PositionCartesian, interfaceName + "PositionCartesian");
        this->StateTable.AddData(deviceData->VelocityCartesian, interfaceName + "VelocityCartesian");
        this->StateTable.AddData(deviceData->PositionJoint, interfaceName + "PositionJoint");
        this->StateTable.AddData(deviceData->Buttons, interfaceName + "Buttons");

        // provide read methods for state data
        providedInterface->AddCommandReadState(this->StateTable,
                                               deviceData->PositionCartesian,
                                               "GetPositionCartesian");
        providedInterface->AddCommandReadState(this->StateTable,
                                               deviceData->VelocityCartesian,
                                               "GetVelocityCartesian");
        providedInterface->AddCommandReadState(this->StateTable,
                                               deviceData->PositionJoint,
                                               "GetPositionJoint");

        // add a method to read the current state index
        providedInterface->AddCommandRead(&mtsStateTable::GetIndexReader, &StateTable,
                                          "GetStateIndex");
        
        // adds frames to transformation manager
        deviceData->PositionCartesian.ReferenceFrame() =
            new prmTransformationFixed(interfaceName + "Base",
                                       vctFrm3::Identity(),
                                       &prmTransformationManager::TheWorld);
        deviceData->PositionFunctionForTransformationManager.Bind(providedInterface
                                                                      ->GetCommandRead("GetPositionCartesian"));
        deviceData->PositionCartesian.MovingFrame() =
            new prmTransformationDynamic(interfaceName + "Tip",
                                         deviceData->PositionFunctionForTransformationManager,
                                         deviceData->PositionCartesian.ReferenceFrame());
        
        // Add interfaces for button with events
        providedInterface = this->AddProvidedInterface(interfaceName + "Button1");
        deviceData->Button1Event.Bind(providedInterface->AddEventWrite("Button",
                                                                       prmEventButton()));
        providedInterface = this->AddProvidedInterface(interfaceName + "Button2");
        deviceData->Button2Event.Bind(providedInterface->AddEventWrite("Button",
                                                                       prmEventButton()));

        // This allows us to return Data->RetValue from the Run method.
        this->Driver->CallbackReturnValue = HD_CALLBACK_CONTINUE;
        this->SetThreadReturnValue(static_cast<void *>(&this->Driver->CallbackReturnValue));
    }
    CMN_LOG_CLASS(4) << "SetupInterfaces: interfaces created: " << std::endl
                     << *this << std::endl;
}


devSensableHD::~devSensableHD()
{
    // free data object created using new
    if (this->Driver) {
        delete this->Driver;
        this->Driver = 0;
    }
    DevicesMapType::iterator iterator = this->Devices.begin();
    const DevicesMapType::iterator end = this->Devices.end();
    for (; iterator != end; iterator++) {
        if (iterator->second) {
            delete iterator->second;
            iterator->second = 0;
        }
    }
}


void devSensableHD::Create(void * data)
{ 
    DevicesMapType::iterator iterator = this->Devices.begin();
    const DevicesMapType::iterator end = this->Devices.end();
    devSensableHDDeviceData * deviceData;
    std::string interfaceName;
    HDErrorInfo error;

    CMN_ASSERT(this->Driver);

    for (; iterator != end; iterator++) {
        deviceData = iterator->second;
        interfaceName = iterator->first;
        CMN_ASSERT(deviceData);
        deviceData->DeviceHandle = hdInitDevice(interfaceName.c_str());
        if (HD_DEVICE_ERROR(error = hdGetError())) {
            CMN_LOG_CLASS(1) << "Create: Failed to initialize haptic device \""
                             << interfaceName << "\"" << std::endl;
            deviceData->DeviceEnabled = false;
            return;
        }
        deviceData->DeviceEnabled = true;
        CMN_LOG_CLASS(3) << "Create: Found device model: "
                         << hdGetString(HD_DEVICE_MODEL_TYPE) << " for device \""
                         << interfaceName << "\"" << std::endl;
        
    }

    // Schedule the main callback that will communicate with the device
    this->Driver->CallbackHandle = hdScheduleAsynchronous(mtsTaskFromCallbackAdapter::CallbackAdapter<HDCallbackCode>,
                                                          this->GetCallbackParameter(),
                                                          HD_MAX_SCHEDULER_PRIORITY);
    // Forces should be disabled by default but make sure they are
    hdDisable(HD_FORCE_OUTPUT);

    // Call base class Create function
    mtsTaskFromCallback::Create();
}


void devSensableHD::Start(void)
{
    HDErrorInfo error;
    hdStartScheduler();
    // Check for errors
    if (HD_DEVICE_ERROR(error = hdGetError())) {
        CMN_LOG_CLASS(1) << "Start: Failed to start scheduler" << std::endl;
    }
    
    // Call base class Start function
    mtsTaskFromCallback::Start();
}


void devSensableHD::Kill(void)
{
    // For cleanup, unschedule callback and stop the scheduler
    hdStopScheduler();
    hdUnschedule(this->Driver->CallbackHandle);

    // Disable the devices
    DevicesMapType::iterator iterator = this->Devices.begin();
    const DevicesMapType::iterator end = this->Devices.end();
    devSensableHDDeviceData * deviceData;
    for (; iterator != end; iterator++) {
        deviceData = iterator->second;
        if (deviceData->DeviceEnabled) {
            hdDisableDevice(deviceData->DeviceHandle);
        }
    }

    // Call base class Kill function
    mtsTaskFromCallback::Kill();
}

