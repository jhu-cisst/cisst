/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: devNovintHDL.cpp 683 2009-08-14 21:40:14Z gsevinc1 $

  Author(s): Gorkem Sevinc, Anton Deguet
  Created on: 2009-09-04

  (C) Copyright 2008-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

// Sensable headers
#include <hdl/hdl.h>
#include <hdlu/hdlu.h>
#include <cisstDevices/devNovintHDL.h>

CMN_IMPLEMENT_SERVICES(devNovintHDL);

struct devNovintHDLDriverData {
    HDLOpHandle CallbackHandle;
    HDLServoOpExitCode CallbackReturnValue;
};

struct devNovintHDLHandle {
    HDLDeviceHandle DeviceHandle;
};


// The task Run method
void devNovintHDL::Run(void)
{
	ProcessQueuedCommands();
    ProcessQueuedEvents();
 
    int currentButtons;
    unsigned int index = 0;
    const unsigned int end = this->DevicesVector.size();
    DeviceData * deviceData;
    devNovintHDLHandle     * handle;
    HDLDeviceHandle hHD;

    for (index; index != end; index++) {
        currentButtons = 0;
        deviceData = DevicesVector(index);
        handle = DevicesHandleVector(index);
        // begin haptics frame
        hHD = handle->DeviceHandle;
        hdlMakeCurrent(hHD);
        
        hdlToolPosition(deviceData->PositionCartesian.Position().Translation().Pointer());
        deviceData->PositionCartesian.Position().Translation().Multiply(1000.0); //Convert from m to mm

        // hdlToolButton(&(deviceData->ButtonPressed));  // boolean indicating if any button is pressed, not used now.
        hdlToolButtons(&(currentButtons));

        // apply forces
        // deviceData->ForceCartesian.Force().Y() += 1.421;
        hdlSetToolForce(deviceData->ForceCartesian.Force().Pointer());

        // time stamp used to date data
        mtsStateIndex stateIndex = this->StateTable.GetIndexWriter();
        
        // compare to previous value to create events
        if (currentButtons != deviceData->Buttons) {
            int currentButtonState, previousButtonState;
            prmEventButton event;
            // test for button 1
            currentButtonState = currentButtons & HDL_BUTTON_1;
            previousButtonState = deviceData->Buttons & HDL_BUTTON_1;
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
            currentButtonState = currentButtons & HDL_BUTTON_2;
            previousButtonState = deviceData->Buttons & HDL_BUTTON_2;
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
            // test for button 3
            currentButtonState = currentButtons & HDL_BUTTON_3;
            previousButtonState = deviceData->Buttons & HDL_BUTTON_3;
            if (currentButtonState != previousButtonState) {
                if (currentButtonState == 0) {
                    deviceData->Clutch = false;
                    event.SetType(prmEventButton::RELEASED);
                } else {
                    deviceData->Clutch = true;
                    event.SetType(prmEventButton::PRESSED);
                }
                // throw the event
                deviceData->Button3Event(event);
            }
            // test for button 4
            currentButtonState = currentButtons & HDL_BUTTON_4;
            previousButtonState = deviceData->Buttons & HDL_BUTTON_4;
            if (currentButtonState != previousButtonState) {
                if (currentButtonState == 0) {
                    deviceData->Clutch = false;
                    event.SetType(prmEventButton::RELEASED);
                } else {
                    deviceData->Clutch = true;
                    event.SetType(prmEventButton::PRESSED);
                }
                // throw the event
                deviceData->Button4Event(event);
            }
            // save previous buttons state
            deviceData->Buttons = currentButtons;
        }
    }

    // check for errors and abort the callback if a scheduler error
    /*HDErrorInfo error;
    if (HD_DEVICE_ERROR(error = hdGetError())) {
        CMN_LOG_RUN_ERROR << "devNovintHDLCallback: Device error detected \""
                          << hdGetErrorString(error.errorCode) << "\"\n";
        if (hduIsSchedulerError(&error)) {
            CMN_LOG_RUN_ERROR << "devNovintHDLCallback: Scheduler error detected\n";
            this->Driver->CallbackReturnValue = HD_CALLBACK_DONE;
            return;
        }
    }
    */
    // call user defined control loop (if redefined from derived class)
    UserControl();

    // return flag to continue calling this function
    this->Driver->CallbackReturnValue = HDL_SERVOOP_CONTINUE;

}


devNovintHDL::devNovintHDL(const std::string & taskName):
    mtsTaskFromCallbackAdapter(taskName, 5000)
{
    CMN_LOG_CLASS_INIT_DEBUG << "constructor called, looking for \"DefaultArm\"" << std::endl;
    DevicesVector.SetSize(1);
    DevicesHandleVector.SetSize(1);
    DevicesVector(0) = new DeviceData;
    DevicesVector(0)->DeviceNumber = 0;
    DevicesVector(0)->Name = "DefaultArm";
    this->SetupInterfaces();
}


devNovintHDL::devNovintHDL(const std::string & taskName, 
                             const std::string & firstDeviceName):
    mtsTaskFromCallbackAdapter(taskName, 5000)
{
    CMN_LOG_CLASS_INIT_DEBUG << "constructor called, looking for \"" << firstDeviceName << "\"" << std::endl;
    DevicesVector.SetSize(1);
    DevicesHandleVector.SetSize(1);
    DevicesVector(0) = new DeviceData;
    DevicesVector(0)->DeviceNumber = 0;
    DevicesVector(0)->Name = firstDeviceName;
    
    this->SetupInterfaces();
}


devNovintHDL::devNovintHDL(const char * taskName,
                           const char * firstDeviceName,
                           const char * secondDeviceName):
    mtsTaskFromCallbackAdapter(taskName, 5000)
{
    this->SetInterfaces(std::string(firstDeviceName), std::string(secondDeviceName));
    this->SetupInterfaces();
}


devNovintHDL::devNovintHDL(const std::string & taskName,
                           const std::string & firstDeviceName,
                           const std::string & secondDeviceName):
    mtsTaskFromCallbackAdapter(taskName, 5000)
{
    this->SetInterfaces(firstDeviceName, secondDeviceName);
    this->SetupInterfaces();
}


devNovintHDL::devNovintHDL(const std::string & taskName,
                           const std::string & firstDeviceName,
                           const std::string & secondDeviceName,
                           const std::string & thirdDeviceName,
                           const std::string & fourthDeviceName):
    mtsTaskFromCallbackAdapter(taskName, 5000)
{
    this->SetInterfaces(firstDeviceName, secondDeviceName, thirdDeviceName, fourthDeviceName);
    this->SetupInterfaces();
}


devNovintHDL::devNovintHDL(const char * taskName,
                           const char * firstDeviceName,
                           const char * secondDeviceName,
                           const char * thirdDeviceName,
                           const char * fourthDeviceName):
    mtsTaskFromCallbackAdapter(taskName, 5000)
{
    this->SetInterfaces(std::string(firstDeviceName), std::string(secondDeviceName), 
                        std::string(thirdDeviceName), std::string(fourthDeviceName));
    this->SetupInterfaces();
}


void devNovintHDL::SetInterfaces(const std::string & firstDeviceName,
                                  const std::string & secondDeviceName)
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
    int index = 0;
    for(index; index < 2; index++)
    {
        DevicesVector(index) = new DeviceData;
        DevicesVector(index)->DeviceNumber = index;
    }
    DevicesVector(0)->Name = firstDeviceName;
    DevicesVector(1)->Name = secondDeviceName;
}

void devNovintHDL::SetInterfaces(const std::string & firstDeviceName,
                                 const std::string & secondDeviceName,
                                 const std::string & thirdDeviceName,
                                 const std::string & fourthDeviceName)
{
    DevicesVector.SetSize(4);
    DevicesHandleVector.SetSize(4);
    int index = 0;
    for(index; index < 4; index++)
    {
        DevicesVector(index) = new DeviceData;
        DevicesVector(index)->DeviceNumber = index;
    }
    DevicesVector(0)->Name = firstDeviceName;
    DevicesVector(1)->Name = secondDeviceName;
    DevicesVector(2)->Name = thirdDeviceName;
    DevicesVector(3)->Name = fourthDeviceName;
}
   

void devNovintHDL::SetupInterfaces(void)
{
    this->Driver = new devNovintHDLDriverData;
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
        DevicesHandleVector(index) = new devNovintHDLHandle;

        // set to zero
        deviceData->Clutch = false;

        // create interface with the device name, i.e. the map key
        CMN_LOG_CLASS_INIT_DEBUG << "SetupInterfaces: creating interface \"" << interfaceName << "\"" << std::endl;
        providedInterface = this->AddProvidedInterface(interfaceName);

        // add the state data to the table
        this->StateTable.AddData(deviceData->PositionCartesian, interfaceName + "PositionCartesian");
        this->StateTable.AddData(deviceData->Buttons, interfaceName + "Buttons");

        this->StateTable.AddData(deviceData->ForceCartesian, interfaceName + "ForceCartesian");
        providedInterface->AddCommandWriteState(this->StateTable,
                                                deviceData->ForceCartesian,
                                                 "SetForceCartesian");
        // provide read methods for state data
        providedInterface->AddCommandReadState(this->StateTable,
                                               deviceData->PositionCartesian,
                                               "GetPositionCartesian");

        // add a method to read the current state index
        providedInterface->AddCommandRead(&mtsStateTable::GetIndexReader, &StateTable,
                                          "GetStateIndex");
        
        // Add interfaces for button with events
        providedInterface = this->AddProvidedInterface(interfaceName + "Button1");
        deviceData->Button1Event.Bind(providedInterface->AddEventWrite("Button",
                                                                       prmEventButton()));
        providedInterface = this->AddProvidedInterface(interfaceName + "Button2");
        deviceData->Button2Event.Bind(providedInterface->AddEventWrite("Button",
                                                                       prmEventButton()));

        // This allows us to return Data->RetValue from the Run method.
        this->Driver->CallbackReturnValue = HDL_SERVOOP_CONTINUE;
        this->SetThreadReturnValue(static_cast<void *>(&this->Driver->CallbackReturnValue));
    }
    CMN_LOG_CLASS_INIT_DEBUG << "SetupInterfaces: interfaces created: " << std::endl
                             << *this << std::endl;
}


devNovintHDL::~devNovintHDL()
{
    // free data object created using new
    if (this->Driver) {
        delete this->Driver;
        this->Driver = 0;
    }
    unsigned int index = 0;
    const unsigned int end = this->DevicesVector.size();
    
    // Delete structs created
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

void devNovintHDL::Create(void * data)
{ 
    unsigned int index = 0;
    const unsigned int end = this->DevicesVector.size();
    
    DeviceData * deviceData;
    devNovintHDLHandle     * handle;
    std::string interfaceName;
    HDLError error;

    CMN_ASSERT(this->Driver);

    // Get the number of devices connected and display to the user
    this->DeviceCount = hdlCountDevices();
    CMN_LOG_CLASS_INIT_VERBOSE << "Number of haptic devices connected: " << this->DeviceCount << std::endl;

    for (index; index != end; index++) {
        deviceData = DevicesVector(index);
        interfaceName = DevicesVector(index)->Name;
        handle = DevicesHandleVector(index);
        CMN_ASSERT(deviceData);

        // Initialize device(s) based on name provided
        handle->DeviceHandle = hdlInitNamedDevice(interfaceName.c_str());
        
        error = hdlGetError();
        if (error != HDL_NO_ERROR)
        {
            CMN_LOG_CLASS_INIT_ERROR << "Create: Failed to initialize haptic device number "
                                     << index << " (" << interfaceName << ")" << std::endl;
            deviceData->DeviceEnabled = false;
            return;
        }

        deviceData->DeviceEnabled = true;
        // Get the device model and display to the user
        const char * model = hdlDeviceModel();
        if (model) {
            CMN_LOG_CLASS_INIT_VERBOSE << "Create: Found device model: "
                                       << model << " for device \""
                                       << interfaceName << "\"" << std::endl;
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "Create: Can not find model name for device \""
                                     << interfaceName << "\"" << std::endl;
        }
    }

    // Call base class Create function
    mtsTaskFromCallback::Create();
}


void devNovintHDL::Start(void)
{
    // Start the device
    hdlStart();
    // Check for errors
    HDLError error = hdlGetError();
    if (error != HDL_NO_ERROR) {
        CMN_LOG_CLASS_INIT_ERROR << "Start: Failed to start scheduler" << std::endl;
    }
    
    // Schedule the main callback that will communicate with the device
    this->Driver->CallbackHandle = hdlCreateServoOp(mtsTaskFromCallbackAdapter::CallbackAdapter<HDLServoOpExitCode>,
                                                    this->GetCallbackParameter(),
                                                    false);

    if(this->Driver->CallbackHandle != HDL_NO_ERROR) {
        CMN_LOG_CLASS_INIT_ERROR << "Invalid Servo op handle" << std::endl;
    }

    // Call base class Start function
    mtsTaskFromCallback::Start();
}


void devNovintHDL::Kill(void)
{
    // For cleanup, unschedule callback and stop the scheduler
    hdlStop();
    hdlDestroyServoOp(this->Driver->CallbackHandle);

    // Disable the devices
    unsigned int index = 0;
    const unsigned int end = this->DevicesVector.size();
    DeviceData * deviceData;
    devNovintHDLHandle     * handle;
    for (index; index != end; index++) {
        deviceData = DevicesVector(index);
        handle = DevicesHandleVector(index);
        if (deviceData->DeviceEnabled) {
            hdlUninitDevice(handle->DeviceHandle);
        }
    }

    // Call base class Kill function
    mtsTaskFromCallback::Kill();
}

