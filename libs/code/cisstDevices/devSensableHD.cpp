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

struct devSensableHDDriverData {
    HDSchedulerHandle CallbackHandle;
    HDCallbackCode CallbackReturnValue;
};

struct devSensableHDHandle {
    HHD DeviceHandle;
};


// The task Run method
void devSensableHD::Run(void)
{
	ProcessQueuedCommands();

    int currentButtons;
    unsigned int index = 0;
    const unsigned int end = this->DevicesVector.size();
    DeviceData * deviceData;
    devSensableHDHandle     * handle;
    HHD hHD;

    for (index; index != end; index++) {
        currentButtons = 0;
        deviceData = DevicesVector(index);
        handle = DevicesHandleVector(index);
        // begin haptics frame
        hHD = handle->DeviceHandle;
        hdMakeCurrentDevice(hHD);
        hdBeginFrame(hHD);

        // get the current cartesian position of the device
        hdGetDoublev(HD_CURRENT_TRANSFORM, deviceData->Frame4x4.Pointer());

        // retrieve cartesian velocities, write directly in state data
        hdGetDoublev(HD_CURRENT_VELOCITY, deviceData->VelocityCartesian.VelocityLinear().Pointer());
        hdGetDoublev(HD_CURRENT_ANGULAR_VELOCITY, deviceData->VelocityCartesian.VelocityAngular().Pointer());
        
        // retrieve joint positions, write directly in state data
        hdGetDoublev(HD_CURRENT_JOINT_ANGLES, deviceData->PositionJoint.Position().Pointer());
        hdGetDoublev(HD_CURRENT_GIMBAL_ANGLES, deviceData->GimbalJointsRef.Pointer());

        // retrieve the current button(s).
        hdGetIntegerv(HD_CURRENT_BUTTONS, &currentButtons);

        // apply forces
        if(deviceData->ForceOutputEnabled)
        {
            hdSetDoublev(HD_CURRENT_FORCE, deviceData->ForceCartesian.Force().Pointer());
        }
        // end haptics frame
        hdEndFrame(hHD);

        // time stamp used to date data
        mtsStateIndex stateIndex = this->StateTable.GetIndexWriter();

        // copy transformation to the state table
        deviceData->PositionCartesian.Position().Translation().Assign(deviceData->Frame4x4TranslationRef);
        deviceData->PositionCartesian.Position().Rotation().Assign(deviceData->Frame4x4RotationRef);
        
        // compare to previous value to create events
        if (currentButtons != deviceData->Buttons) {
            int currentButtonState, previousButtonState;
            prmEventButton event;
            // test for button 1
            currentButtonState = currentButtons & HD_DEVICE_BUTTON_1;
            previousButtonState = deviceData->Buttons & HD_DEVICE_BUTTON_1;
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
                    deviceData->Clutch = false;
                    event.SetType(prmEventButton::RELEASED);
                } else {
                    deviceData->Clutch = true;
                    event.SetType(prmEventButton::PRESSED);
                }
                // throw the event
                deviceData->Button2Event(event);
            }
            // save previous buttons state
            deviceData->Buttons = currentButtons;
        }
    }

    // check for errors and abort the callback if a scheduler error
    HDErrorInfo error;
    if (HD_DEVICE_ERROR(error = hdGetError())) {
        CMN_LOG_RUN_ERROR << "devSensableHDCallback: Device error detected \""
                          << hdGetErrorString(error.errorCode) << "\"\n";
        if (hduIsSchedulerError(&error)) {
            CMN_LOG_RUN_ERROR << "devSensableHDCallback: Scheduler error detected\n";
            this->Driver->CallbackReturnValue = HD_CALLBACK_DONE;
            return;
        }
    }
   
    // call user defined control loop (if redefined from derived class)
    UserControl();

    // always last, sync the state table
    this->StateTable.Advance();
    
    // return flag to continue calling this function
    this->Driver->CallbackReturnValue = HD_CALLBACK_CONTINUE;

}


devSensableHD::devSensableHD(const std::string & taskName):
    mtsTaskFromCallbackAdapter(taskName, 5000)
{
    CMN_LOG_CLASS_INIT_DEBUG << "constructor called, looking for \"DefaultArm\"" << std::endl;
    DevicesVector.SetSize(1);
    DevicesHandleVector.SetSize(1);
    DevicesVector(0) = new DeviceData;
    DevicesVector(0)->Name = "DefaultArm";
    DevicesVector(0)->ForceOutputEnabled = false;
    this->SetupInterfaces();
}


devSensableHD::devSensableHD(const std::string & taskName, 
                             const std::string & firstDeviceName):
    mtsTaskFromCallbackAdapter(taskName, 5000)
{
    CMN_LOG_CLASS_INIT_DEBUG << "constructor called, looking for \"" << firstDeviceName << "\"" << std::endl;
    DevicesVector.SetSize(1);
    DevicesHandleVector.SetSize(1);
    DevicesVector(0) = new DeviceData;
    DevicesVector(0)->Name = firstDeviceName;
    DevicesVector(0)->ForceOutputEnabled = false;
    
    this->SetupInterfaces();
}

devSensableHD::devSensableHD(const std::string & taskName, 
                             const std::string & firstDeviceName, 
                             bool firstDeviceForcesEnabled):
    mtsTaskFromCallbackAdapter(taskName, 5000)
{
    CMN_LOG_CLASS_INIT_DEBUG << "constructor called, looking for \"" << firstDeviceName << "\"" << std::endl;
    DevicesVector.SetSize(1);
    DevicesHandleVector.SetSize(1);
    DevicesVector(0) = new DeviceData;
    DevicesVector(0)->Name = firstDeviceName;
    DevicesVector(0)->ForceOutputEnabled = firstDeviceForcesEnabled;
    
    this->SetupInterfaces();
}


devSensableHD::devSensableHD(const std::string & taskName,
                             const std::string & firstDeviceName,
                             const std::string & secondDeviceName):
    mtsTaskFromCallbackAdapter(taskName, 5000)
{
    if (firstDeviceName == secondDeviceName) {
        CMN_LOG_CLASS_INIT_ERROR << "In constructor: name of devices provided are identical, \""
                                 << firstDeviceName << "\" and \""
                                 << secondDeviceName << "\"" << std::endl;
    }
    CMN_LOG_CLASS_INIT_DEBUG << "constructor called, looking for \"" << firstDeviceName
                             << "\" and \"" << secondDeviceName << "\"" << std::endl;
    DevicesVector.SetSize(2);
    DevicesHandleVector.SetSize(2);
    DevicesVector(0) = new DeviceData;
    DevicesVector(1) = new DeviceData;
    DevicesVector(0)->Name = firstDeviceName;
    DevicesVector(1)->Name = secondDeviceName;
    DevicesVector(0)->ForceOutputEnabled = false;
    DevicesVector(1)->ForceOutputEnabled = false;
    this->SetupInterfaces();
}

devSensableHD::devSensableHD(const std::string & taskName,
                             const std::string & firstDeviceName,
                             const std::string & secondDeviceName,
                             bool firstDeviceForcesEnabled,
                             bool secondDeviceForcesEnabled):
    mtsTaskFromCallbackAdapter(taskName, 5000)
{
    if (firstDeviceName == secondDeviceName) {
        CMN_LOG_CLASS_INIT_ERROR << "In constructor: name of devices provided are identical, \""
                                 << firstDeviceName << "\" and \""
                                 << secondDeviceName << "\"" << std::endl;
    }
    CMN_LOG_CLASS_INIT_DEBUG << "constructor called, looking for \"" << firstDeviceName
                             << "\" and \"" << secondDeviceName << "\"" << std::endl;
    DevicesVector.SetSize(2);
    DevicesHandleVector.SetSize(2);
    DevicesVector(0) = new DeviceData;
    DevicesVector(1) = new DeviceData;
    DevicesVector(0)->Name = firstDeviceName;
    DevicesVector(1)->Name = secondDeviceName;
    DevicesVector(0)->ForceOutputEnabled = firstDeviceForcesEnabled;
    DevicesVector(1)->ForceOutputEnabled = secondDeviceForcesEnabled;
    this->SetupInterfaces();
}

void devSensableHD::SetupInterfaces(void)
{
    this->Driver = new devSensableHDDriverData;
    CMN_ASSERT(this->Driver);

    unsigned int index = 0;
    const unsigned int end = this->DevicesVector.size();
    DeviceData * deviceData;
    std::string interfaceName;
    mtsProvidedInterface * providedInterface;

    for (index; index != end; index++) {
        // use local data pointer to make code more readable
        deviceData = DevicesVector(index);
        CMN_ASSERT(deviceData);
        interfaceName = DevicesVector(index)->Name;
        DevicesHandleVector(index) = new devSensableHDHandle;

        deviceData->Frame4x4TranslationRef.SetRef(deviceData->Frame4x4.Column(3), 0);
        deviceData->Frame4x4RotationRef.SetRef(deviceData->Frame4x4, 0, 0);
        deviceData->PositionJoint.Position().SetSize(NB_JOINTS);
        deviceData->GimbalJointsRef.SetRef(deviceData->PositionJoint.Position(), 3, 3);

        // set to zero
        deviceData->PositionJoint.Position().SetAll(0.0);
        deviceData->VelocityCartesian.VelocityLinear().SetAll(0.0);
        deviceData->VelocityCartesian.VelocityAngular().SetAll(0.0);
        deviceData->Clutch = false;

        // create interface with the device name, i.e. the map key
        CMN_LOG_CLASS_INIT_DEBUG << "SetupInterfaces: creating interface \"" << interfaceName << "\"" << std::endl;
        providedInterface = this->AddProvidedInterface(interfaceName);

        // add the state data to the table
        this->StateTable.AddData(deviceData->PositionCartesian, interfaceName + "PositionCartesian");
        this->StateTable.AddData(deviceData->VelocityCartesian, interfaceName + "VelocityCartesian");
        this->StateTable.AddData(deviceData->PositionJoint, interfaceName + "PositionJoint");
        this->StateTable.AddData(deviceData->Buttons, interfaceName + "Buttons");

        if(deviceData->ForceOutputEnabled)
        {
            this->StateTable.AddData(deviceData->ForceCartesian, interfaceName + "ForceCartesian");
            providedInterface->AddCommandWriteState(this->StateTable,
                                                    deviceData->ForceCartesian,
                                                    "SetForceCartesian");
        }
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
    CMN_LOG_CLASS_INIT_DEBUG << "SetupInterfaces: interfaces created: " << std::endl
                             << *this << std::endl;
}


devSensableHD::~devSensableHD()
{
    // free data object created using new
    if (this->Driver) {
        delete this->Driver;
        this->Driver = 0;
    }
    unsigned int index = 0;
    const unsigned int end = this->DevicesVector.size();
    
    for (index; index != end; index++) {
        if (DevicesVector(index)) {
            delete DevicesVector(index);
            DevicesVector(index) = 0;
        }
        if (DevicesHandleVector(index)) {
            delete DevicesHandleVector(index);
            DevicesHandleVector(index) = 0;
        }
    }
}


void devSensableHD::Create(void * data)
{ 
    unsigned int index = 0;
    const unsigned int end = this->DevicesVector.size();
    
    DeviceData * deviceData;
    devSensableHDHandle     * handle;
    std::string interfaceName;
    HDErrorInfo error;

    CMN_ASSERT(this->Driver);

    for (index; index != end; index++) {
        deviceData = DevicesVector(index);
        interfaceName = DevicesVector(index)->Name;
        handle = DevicesHandleVector(index);
        CMN_ASSERT(deviceData);
        handle->DeviceHandle = hdInitDevice(interfaceName.c_str());
        if (HD_DEVICE_ERROR(error = hdGetError())) {
            CMN_LOG_CLASS_INIT_ERROR << "Create: Failed to initialize haptic device \""
                                     << interfaceName << "\"" << std::endl;
            deviceData->DeviceEnabled = false;
            return;
        }
        deviceData->DeviceEnabled = true;
        CMN_LOG_CLASS_INIT_VERBOSE << "Create: Found device model: "
                                   << hdGetString(HD_DEVICE_MODEL_TYPE) << " for device \""
                                   << interfaceName << "\"" << std::endl;
        if(deviceData->ForceOutputEnabled) {
            hdEnable(HD_FORCE_OUTPUT);
        } else {
            hdDisable(HD_FORCE_OUTPUT);
        }
    }

    // Schedule the main callback that will communicate with the device
    this->Driver->CallbackHandle = hdScheduleAsynchronous(mtsTaskFromCallbackAdapter::CallbackAdapter<HDCallbackCode>,
                                                          this->GetCallbackParameter(),
                                                          HD_MAX_SCHEDULER_PRIORITY);
    // Call base class Create function
    mtsTaskFromCallback::Create();
}


void devSensableHD::Start(void)
{
    HDErrorInfo error;
    //hdSetSchedulerRate(500);
    hdStartScheduler();
    // Check for errors
    if (HD_DEVICE_ERROR(error = hdGetError())) {
        CMN_LOG_CLASS_INIT_ERROR << "Start: Failed to start scheduler" << std::endl;
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
    unsigned int index = 0;
    const unsigned int end = this->DevicesVector.size();
    DeviceData * deviceData;
    devSensableHDHandle     * handle;
    for (index; index != end; index++) {
        deviceData = DevicesVector(index);
        handle = DevicesHandleVector(index);
        if (deviceData->DeviceEnabled) {
            hdDisableDevice(handle->DeviceHandle);
        }
    }

    // Call base class Kill function
    mtsTaskFromCallback::Kill();
}

