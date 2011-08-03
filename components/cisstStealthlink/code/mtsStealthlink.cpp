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


#include <cisstStealthlink/mtsStealthlink.h>

#include <cisstCommon/cmnPortability.h>
#include <cisstOSAbstraction/osaGetTime.h>
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

CMN_IMPLEMENT_SERVICES(mtsStealthlink);

// For classes defined in stealthTypes.h
CMN_IMPLEMENT_SERVICES(mtsStealthTool);
CMN_IMPLEMENT_SERVICES(mtsStealthFrame);
CMN_IMPLEMENT_SERVICES(mtsStealthRegistration);
CMN_IMPLEMENT_SERVICES(mtsStealthProbeCal);

mtsStealthlink::mtsStealthlink(const std::string & taskName, const double & periodInSeconds):
	mtsTaskPeriodic(taskName, periodInSeconds, false, 1000),
    StealthlinkPresent(false)
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

    mtsInterfaceProvided * provided = AddInterfaceProvided("MainInterface");
    if (provided) {
        provided->AddCommandReadState(StateTable, ToolData, "GetToolData");
        provided->AddCommandReadState(StateTable, FrameData, "GetFrameData");
        provided->AddCommandReadState(StateTable, RegistrationData, "GetRegistrationData");
        provided->AddCommandReadState(StateTable, ProbeCal, "GetProbeCalibration");
        provided->AddCommandVoid(&mtsStealthlink::RequestSurgicalPlan, this, "RequestSurgicalPlan");
        provided->AddCommandRead(&mtsStealthlink::GetSurgicalPlan, this, "GetSurgicalPlan", SurgicalPlan);
	}
}

// Windows defines a SetPort macro
#ifdef SetPort
#undef SetPort
#endif

void mtsStealthlink::Configure(const std::string &filename)
{
	// Configure Stealthlink interface
	AsCL_SetVerboseLevel(0);
	this->Client->SetPort(GRI_PORT_NUMBER);
	// Set StealthLink server IP address
	this->Client->SetHostName("192.168.0.1");
	CMN_LOG_CLASS_INIT_VERBOSE << "Initializing Stealthlink" << std::endl;
	StealthlinkPresent = this->Client->Initialize(*(this->Utils)) ? true : false;
	if (!StealthlinkPresent) {
		CMN_LOG_CLASS_RUN_WARNING << "Configure: could not Initialize StealthLink" << std::endl;
    }
}


void mtsStealthlink::RequestSurgicalPlan(void)
{
	if (StealthlinkPresent) {
		surg_plan the_surg_plan;
		this->Client->GetDataForCode(GET_SURGICAL_PLAN, (void*)&the_surg_plan);
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
	if (StealthlinkPresent) {
		// Get the data from Stealthlink.
		all_info info;

		/*
		// T: timing analisys
		// Get timing information
		mtsTaskManager * manager = mtsTaskManager::GetInstance();
		double measTime1 = manager->GetTimeServer().GetRelativeTime();
		*/
		this->Client->GetDataForCode(GET_ALL, (void*)&info);

		/*
		//T: timing analisys cont.
		double measTime2 = manager->GetTimeServer().GetRelativeTime(); //T: timing
		double avgPeriod = GetAveragePeriod();

		if (countt%50 == 0)
		CMN_LOG_RUN_VERBOSE << "StartPoll" << measTime1 << " STOP Poll" << measTime2 << " Average cycle" <<  avgPeriod << std::endl;
		countt++;
		*/

		ToolData = info.Tool;
		FrameData = info.Frame;
		RegistrationData = info.Reg;

		if (ToolData.Valid() && FrameData.Valid()) {
			//get tool tip calibration if it is invalid or has changed
			if ((strcmp(ToolData.GetName(), ProbeCal.GetName()) != 0) || !ProbeCal.Valid()) {
				probe_calibration probe_cal;
				this->Client->GetDataForCode(GET_PROBE_CALIBRATION, (void*)&probe_cal);
				ProbeCal = probe_cal;
			}
		}
	}
	ProcessQueuedCommands();
    this->Utils->CheckCallbacks();
}


void mtsStealthlink::Cleanup(void)
{
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
