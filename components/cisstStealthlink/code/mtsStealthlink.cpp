/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Peter Kazanzides, Anton Deguet
  Created on: 2006

  (C) Copyright 2007-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


// Stealthlink definitions
#include <GRI_Protocol/GRI.h>

#include <cisstStealthlink/mtsStealthlink.h>

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnXMLPath.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

#ifdef CISST_HAS_STEALTHLINK
#if (CISST_OS == CISST_WINDOWS)
// Prevent inclusion of <winsock.h> from <windows.h>.
#define _WINSOCKAPI_
#endif
#endif

// Stealthlink include files
#include <AsCL/AsCL_Client.h>
#include "mtsStealthlink_AsCL_Stuff.h"

CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(mtsStealthlink, mtsTaskPeriodic, mtsTaskPeriodicConstructorArg)

typedef float floatArray44[4][4];

void mtsStealthlinkFrameToVctFrame(vctFrm3 & result, const floatArray44 & input) {
    size_t row, col;
    for (row = 0; row < 3; row++) {
        for (col = 0; col < 3; col++) {
            result.Rotation().at(row, col) =  input[row][col];
        }
        result.Translation().at(row) = input[row][3];
    }
}


void mtsStealthlink::Init(void)
{
    // create Stealthlink objects
    this->Client = new AsCL_Client;
    this->Utils = new mtsStealthlink_AsCL_Utils;

    SurgicalPlan.SetSize(6);

    // Stealth Tool -- the position of the tracked tool, as a frame
    StateTable.AddData(ToolData, "ToolData");
    // Stealth Frame -- the position of the base frame, as a frame
    StateTable.AddData(FrameData, "FrameData");
    // Stealth Registration
    StateTable.AddData(RegistrationData, "RegistrationData");
    // Stealth Tool Calibration
    StateTable.AddData(ProbeCal, "ProbeCalibration");

    mtsInterfaceProvided * provided = AddInterfaceProvided("Controller");
    if (provided) {
        provided->AddCommandReadState(StateTable, ToolData, "GetTool");
        provided->AddCommandReadState(StateTable, FrameData, "GetFrame");
        provided->AddCommandReadState(StateTable, RegistrationData, "GetRegistration");
        provided->AddCommandReadState(StateTable, ProbeCal, "GetProbeCalibration");
        provided->AddCommandVoid(&mtsStealthlink::RequestSurgicalPlan, this, "RequestSurgicalPlan");
        provided->AddCommandRead(&mtsStealthlink::GetSurgicalPlan, this, "GetSurgicalPlan", SurgicalPlan);
    }

    // Add interface for registration, ideally we should standardize such interface commands/payloads
    // maybe we should create a separate state table for registration?  Would only advance if changed.
    StateTable.AddData(RegistrationMember.Transformation, "RegistrationTransformation");
    StateTable.AddData(RegistrationMember.PredictedAccuracy, "RegistrationPredictedAccuracy");
    provided = AddInterfaceProvided("Registration");
    if (provided) {
        provided->AddCommandReadState(StateTable, RegistrationMember.Transformation, "GetTransformation");
        provided->AddCommandReadState(StateTable, RegistrationMember.PredictedAccuracy, "GetPredictedAccuracy");
    }
}

mtsStealthlink::mtsStealthlink(const std::string & taskName, const double & periodInSeconds) :
    mtsTaskPeriodic(taskName, periodInSeconds, false, 1000),
    StealthlinkPresent(false),
    CurrentTool(0)
{
    Init();
}


mtsStealthlink::mtsStealthlink(const mtsTaskPeriodicConstructorArg &arg) :
    mtsTaskPeriodic(arg),
    StealthlinkPresent(false),
    CurrentTool(0)
{
    Init();
}


mtsStealthlink::~mtsStealthlink()
{
    Cleanup();
}


// Windows defines a SetPort macro
#ifdef SetPort
#undef SetPort
#endif

void mtsStealthlink::Configure(const std::string &filename)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Configure: using " << filename << std::endl;

    cmnXMLPath config;
    config.SetInputSource(filename);

    // initialize serial port
    std::string ipAddress;
    if (!config.GetXMLValue("/tracker/controller", "@ip", ipAddress, "192.168.0.1"))
        CMN_LOG_CLASS_INIT_WARNING << "IP address not found, using default: " << ipAddress << std::endl;

    // Configure Stealthlink interface
    AsCL_SetVerboseLevel(0);
    this->Client->SetPort(GRI_PORT_NUMBER);

    // Set StealthLink server IP address
    CMN_LOG_CLASS_INIT_VERBOSE << "Setting Stealthink IP address = " << ipAddress << std::endl;
    this->Client->SetHostName(const_cast<char *>(ipAddress.c_str()));

    // add pre-defined tools (up to 100)
    for (unsigned int i = 0; i < 100; i++) {
        std::stringstream context;
        std::string stealthName, name;
        context << "/tracker/tools/tool[" << i+1 << "]";
        config.GetXMLValue(context.str().c_str(), "@stealthName", stealthName, "");
        config.GetXMLValue(context.str().c_str(), "@name", name, "");
        if (stealthName.empty() && name.empty()) {
            break;
        }
        if (stealthName.empty()) {
            AddTool(name, name);
        } else if (name.empty()) {
            AddTool(stealthName, stealthName);
        } else {
            AddTool(stealthName, name);
        }
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "Initializing Stealthlink" << std::endl;
    StealthlinkPresent = this->Client->Initialize(*(this->Utils)) ? true : false;
    if (!StealthlinkPresent) {
        CMN_LOG_CLASS_RUN_WARNING << "Configure: could not Initialize StealthLink" << std::endl;
    }
}


void mtsStealthlink::ResetAllTools(void)
{
    ToolsContainer::iterator it;
    const ToolsContainer::iterator end = Tools.end();
    for (it = Tools.begin(); it != end; it++) {
        (*it)->MarkerPosition.SetValid(false);
        (*it)->TooltipPosition.SetValid(false);
    }
}


mtsStealthlink::Tool * mtsStealthlink::FindTool(const std::string & stealthName) const
{
    ToolsContainer::const_iterator it;
    const ToolsContainer::const_iterator end = Tools.end();
    for (it = Tools.begin(); it != end; it++) {
        if ((*it)->GetStealthName() == stealthName) {
            return *it;
        }
    }
    return 0;
}


mtsStealthlink::Tool * mtsStealthlink::AddTool(const std::string & stealthName, const std::string & interfaceName)
{
    // First, check if tool has already been added
    Tool * tool = FindTool(stealthName);
    if (tool) {
        if (tool->GetInterfaceName() == interfaceName) {
            CMN_LOG_CLASS_RUN_WARNING << "AddTool: tool " << stealthName << " already exists with interface "
                                      << interfaceName << std::endl;
            return tool;
        }
        // We could support having the same tool in multiple interfaces, but we would need to maintain
        // an array of CurrentTools, or loop through the entire Tools list in the Run method (to assign the
        // MarkerPosition and TooltipPosition).
        CMN_LOG_CLASS_RUN_ERROR << "AddTool: tool " << stealthName << " already exists in interface "
                                << tool->GetInterfaceName() << ", could not create new interface "
                                << interfaceName << std::endl;
        return 0;
    }

    // Next, check if interface has already been added
    mtsInterfaceProvided * provided = GetInterfaceProvided(interfaceName);
    if (provided) {
        CMN_LOG_CLASS_RUN_ERROR << "AddTool: interface " << interfaceName << " already exists." << std::endl;
        return 0;
    }

    // Create the tool and add it to the list
    tool = new Tool(stealthName, interfaceName);
    Tools.push_back(tool);
    CMN_LOG_CLASS_RUN_VERBOSE << "AddTool: adding " << stealthName << " to interface " << interfaceName << std::endl;
    provided = AddInterfaceProvided(interfaceName);
    if (provided) {
        StateTable.AddData(tool->TooltipPosition, interfaceName + "Position");
        provided->AddCommandReadState(StateTable, tool->TooltipPosition, "GetPositionCartesian");
        StateTable.AddData(tool->MarkerPosition, interfaceName + "Marker");
        provided->AddCommandReadState(StateTable, tool->MarkerPosition, "GetMarkerCartesian");
    }
    return tool;
}


void mtsStealthlink::RequestSurgicalPlan(void)
{
    if (StealthlinkPresent) {
        surg_plan the_surg_plan;
        this->Client->GetDataForCode(GET_SURGICAL_PLAN,
                                     reinterpret_cast<void*>(&the_surg_plan));
        SurgicalPlan[0] = the_surg_plan.entry[0];
        SurgicalPlan[1] = the_surg_plan.entry[1];
        SurgicalPlan[2] = the_surg_plan.entry[2];
        SurgicalPlan[3] = the_surg_plan.target[0];
        SurgicalPlan[4] = the_surg_plan.target[1];
        SurgicalPlan[5] = the_surg_plan.target[2];
    }
}


void mtsStealthlink::GetSurgicalPlan(mtsDoubleVec & plan) const
{
    plan = SurgicalPlan;
}


void mtsStealthlink::Run(void)
{
    ResetAllTools();  // Set all tools invalid
    if (StealthlinkPresent) {
        // Get the data from Stealthlink.
        all_info info;
        this->Client->GetDataForCode(GET_ALL, reinterpret_cast<void*>(&info));

        // set data for "controller" interface (note: this uses non
        // standard cisst types and should probably be removed later (adeguet1)
        ToolData = info.Tool;
        FrameData = info.Frame;
        RegistrationData = info.Reg;

        // update tool interfaces data
        if (ToolData.Valid()) {
            if (!CurrentTool || (CurrentTool->GetStealthName() != ToolData.GetName())) {
                CurrentTool = FindTool(ToolData.GetName());
                if (!CurrentTool) {
                    CMN_LOG_CLASS_INIT_VERBOSE << "Adding new tool: " << ToolData.GetName() << std::endl;
                    CurrentTool = AddTool(ToolData.GetName(), ToolData.GetName());
                }
            }
            // rely on older interface to retrieve tool information
            if (CurrentTool) {
                CurrentTool->MarkerPosition.Position() = ToolData.GetFrame();
                CurrentTool->MarkerPosition.SetValid(true);
            }
            // Get tool tip calibration if it is invalid or has changed
            if ((strcmp(ToolData.GetName(), ProbeCal.GetName()) != 0) || !ProbeCal.Valid()) {
                probe_calibration probe_cal;
                this->Client->GetDataForCode(GET_PROBE_CALIBRATION,
                                             reinterpret_cast<void*>(&probe_cal));
                ProbeCal = probe_cal;
            }
            // If we have valid data, then store the result
            if (CurrentTool && ProbeCal.Valid() &&
                (strcmp(ToolData.GetName(), ProbeCal.GetName()) == 0)) {
                CurrentTool->TooltipPosition.Position() = vctFrm3(ToolData.GetFrame().Rotation(),
                                                                  ToolData.GetFrame() * ProbeCal.GetTip());
                CurrentTool->TooltipPosition.SetValid(true);
            }
        }
        
        // update registration interface data
        mtsStealthlinkFrameToVctFrame(this->RegistrationMember.Transformation,
                                      info.Reg.xform);
        this->RegistrationMember.Transformation.SetValid(info.Reg.valid);
        this->RegistrationMember.PredictedAccuracy = info.Reg.predicted_accuracy;
        this->RegistrationMember.PredictedAccuracy.SetValid(info.Reg.valid);
    }
    ProcessQueuedCommands();
    this->Utils->CheckCallbacks();
}


void mtsStealthlink::Cleanup(void)
{
    ToolsContainer::iterator it;
    const ToolsContainer::iterator end = Tools.end();
    for (it = Tools.begin(); it != end; it++) {
        delete (*it);
        *it = 0;
    }
    Tools.clear();
    if (this->Client) {
        delete this->Client;
        this->Client = 0;
    }
    if (this->Utils) {
        delete this->Utils;
        this->Utils = 0;
    }
    CMN_LOG_RUN_VERBOSE << CMN_LOG_DETAILS << "...Finished." << std::endl;
}
