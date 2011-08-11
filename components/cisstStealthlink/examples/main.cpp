/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2011-07-14

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstStealthlink/mtsStealthlink.h>


class RequiredForStealthlink : public mtsComponent
{
public:

    struct ControllerStruct {
        mtsFunctionRead GetTool;
        mtsFunctionRead GetFrame;
    };

    struct ToolStruct {
        mtsFunctionRead GetMarkerCartesian;
        mtsFunctionRead GetPositionCartesian;
    };

    RequiredForStealthlink(const std::string &name);
    ~RequiredForStealthlink() {}

    void AddToolInterface(const std::string &toolName,
                          ToolStruct &functionSet);

    ControllerStruct Stealthlink;
    ToolStruct Pointer;
    ToolStruct Frame;
};

RequiredForStealthlink::RequiredForStealthlink(const std::string &name) :
    mtsComponent(name)
{
    mtsInterfaceRequired *required = AddInterfaceRequired("Stealthlink");
    if (required) {
        required->AddFunction("GetTool", Stealthlink.GetTool);
        required->AddFunction("GetFrame", Stealthlink.GetFrame);
    }
    AddToolInterface("Pointer", Pointer);
    AddToolInterface("Frame", Frame);
}

void RequiredForStealthlink::AddToolInterface(const std::string &toolName,
                                              RequiredForStealthlink::ToolStruct &functionSet)
{
    mtsInterfaceRequired *required = AddInterfaceRequired(toolName, MTS_OPTIONAL);
    if (required) {
        required->AddFunction("GetPositionCartesian", functionSet.GetPositionCartesian);
        required->AddFunction("GetMarkerCartesian", functionSet.GetMarkerCartesian);
    }
}

int main(int argc, char *argv[])
{
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ALL);

    // set the log level of detail on select components
    cmnLogger::SetMaskClass("cmnXMLPath", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("mtsStealthlink", CMN_LOG_ALLOW_ALL);

    // create the components
    mtsStealthlink * componentStealthlink = new mtsStealthlink("Stealthlink", 50.0 * cmn_ms);
    componentStealthlink->Configure("config.xml");

    RequiredForStealthlink *componentTest = new RequiredForStealthlink("Test");

    // add the components to the component manager
    mtsManagerLocal * componentManager = mtsComponentManager::GetInstance();
    componentManager->AddComponent(componentStealthlink);
    componentManager->AddComponent(componentTest);

    // Connect the test component to the Stealthlink component
    if (!componentManager->Connect(componentTest->GetName(), "Stealthlink",
                                   componentStealthlink->GetName(), "Controller")) {
        CMN_LOG_INIT_ERROR << "Could not connect test component to Stealthlink component." << std::endl;
        return 0;
    }

    // Now, connect to the tools (we assume these are pre-defined in the XML file)
    if (!componentManager->Connect(componentTest->GetName(), "Pointer",
                                   componentStealthlink->GetName(), "Pointer")) {
        CMN_LOG_INIT_WARNING << "Could not connect test component to Pointer tool." << std::endl;
    }
    if (!componentManager->Connect(componentTest->GetName(), "Frame",
                                   componentStealthlink->GetName(), "Frame")) {
        CMN_LOG_INIT_WARNING << "Could not connect test component to Frame tool." << std::endl;
    }

 
    // create and start all components
    componentManager->CreateAll();
    componentManager->WaitForStateAll(mtsComponentState::READY, 2.0 * cmn_s);
    componentManager->StartAll();
    componentManager->WaitForStateAll(mtsComponentState::ACTIVE, 2.0 * cmn_s);

    mtsStealthTool StealthTool;
    mtsStealthFrame StealthFrame;
    prmPositionCartesianGet prmPos;

    while (1) {
        bool didOutput = false;
        componentTest->Stealthlink.GetTool(StealthTool);
        componentTest->Stealthlink.GetFrame(StealthFrame);
        if (StealthTool.Valid()) {
            std::cout << "Tool " << StealthTool.GetName() << ": " << StealthTool.GetFrame().Translation() << "; ";
            didOutput = true;
        }
        if (StealthFrame.Valid()) {
            std::cout << "Frame " << StealthFrame.GetName() << ": " << StealthFrame.GetFrame().Translation() << "; ";
            didOutput = true;
        }
        if (componentTest->Pointer.GetPositionCartesian.IsValid()) {
            componentTest->Pointer.GetPositionCartesian(prmPos);
            if (prmPos.Valid()) {
                std::cout << "Interface Pointer: " << prmPos.Position().Translation() << "; ";
                didOutput = true;
            }
        }
        else if (componentTest->Pointer.GetMarkerCartesian.IsValid()) {
            componentTest->Pointer.GetMarkerCartesian(prmPos);
            if (prmPos.Valid()) {
                std::cout << "Interface PointerM: " << prmPos.Position().Translation() << "; ";
                didOutput = true;
            }
        }
        if (componentTest->Frame.GetPositionCartesian.IsValid()) {
            componentTest->Frame.GetPositionCartesian(prmPos);
            if (prmPos.Valid()) {
                std::cout << "Interface Frame: " << prmPos.Position().Translation();
                didOutput = true;
            }
        }
        else if (componentTest->Frame.GetMarkerCartesian.IsValid()) {
            componentTest->Frame.GetMarkerCartesian(prmPos);
            if (prmPos.Valid()) {
                std::cout << "Interface FrameM: " << prmPos.Position().Translation();
                didOutput = true;
            }
        }
        if (didOutput)
            std::cout << std::endl;
        osaSleep(0.1);
    
    }

    // kill all components and perform cleanup
    componentManager->KillAll();
    componentManager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);
    componentManager->Cleanup();
    delete componentStealthlink;

    return 0;
}
