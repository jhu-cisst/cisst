/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ui3BehaviorBase.cpp,v 1.11 2009/02/24 02:43:13 anton Exp $

  Author(s):	Balazs Vagvolgyi, Simon DiMaio, Anton Deguet
  Created on:	2008-06-10

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaSleep.h>

#include <cisst3DUserInterface/ui3BehaviorBase.h>
#include <cisst3DUserInterface/ui3SceneManager.h>
#include <cisst3DUserInterface/ui3VTKRenderer.h>
#include <cisst3DUserInterface/ui3VideoInterfaceFilter.h>
#include <cisst3DUserInterface/ui3Manager.h>


CMN_IMPLEMENT_SERVICES(ui3BehaviorBase)


ui3BehaviorBase::ui3BehaviorBase(const std::string & name,
                                 ui3Manager * manager):
    mtsTaskContinuous(name, 500), /* all behaviors have a state table of 500, ui3Manager needs at least 3 */
    MenuBar(0),
    Manager(manager),
    State(Background)
{
}


ui3BehaviorBase::~ui3BehaviorBase()
{
    for (unsigned int i = 0; i < Streams.size(); i ++) {
        if (Streams[i]) delete Streams[i];
    }
}


void ui3BehaviorBase::AddMenuBar(bool isManager)
{
    CMN_ASSERT(this->Manager);

    // create the menu bar
    this->MenuBar = new ui3MenuBar(this->Manager);
    CMN_ASSERT(this->MenuBar);

    // add a button in the main menu bar with callback
    if (!isManager) {
        this->MenuBar->AddClickButton("Hide",
                                      0,
                                      "",
                                      &ui3BehaviorBase::SetStateBackground,
                                      this);
    }
}


ui3BehaviorBase::StateType ui3BehaviorBase::GetState(void) const
{
    return this->State;
}

ui3Manager * ui3BehaviorBase::GetManager(void)
{
    return this->Manager;
}

ui3SceneManager * ui3BehaviorBase::GetSceneManager(void)
{
    return this->GetManager()->GetSceneManager();
}

void ui3BehaviorBase::Configure(const std::string & configFile)
{
    CMN_LOG_CLASS(2) << "Configure: use default configure from base class, doing nothing" << std::endl;
}

bool ui3BehaviorBase::SaveConfiguration(const std::string& configFile)
{
    CMN_LOG_CLASS(2) << "SaveConfiguration: use default save configuration from base class, doing nothing" << std::endl;
    return true;
}

void ui3BehaviorBase::RegisterCallback(ui3Handle ctrlhandle, ControlCallbackType method)
{
}

void ui3BehaviorBase::UnregisterCallback(ui3Handle ctrlhandle)
{
}

void ui3BehaviorBase::SubscribeInputCallback(unsigned int inputid)
{
}

void ui3BehaviorBase::UnsubscribeInputCallback(unsigned int inputid)
{
}

void ui3BehaviorBase::Run(void)
{
    this->ProcessQueuedEvents();
    switch(this->State) {
    case Foreground:
        this->RunForeground();
        break;
    case Background:
        this->RunBackground();
        break;
    case Idle:
        this->RunNoInput();
        break;
    default:
        break;
    }
    osaSleep(10.0 * cmn_ms); // bad, needs to be a variable/data member
}

void ui3BehaviorBase::OnInputAction(unsigned int inputid, ui3InputDeviceBase::InputAction action)
{
}

void ui3BehaviorBase::DispatchGUIEvents(void)
{
}

void ui3BehaviorBase::SetStateForeground(void)
{
    this->SetState(Foreground);
}

void ui3BehaviorBase::SetStateBackground(void)
{
    this->SetState(Background);
}

void ui3BehaviorBase::SetState(const StateType & newState)
{
    if (newState != this->State) {
        this->State = newState;
        switch (this->State) {
            case Foreground:
                if (this->Manager->ActiveBehavior) {
                    this->Manager->ActiveBehavior->MenuBar->Hide();
                    this->Manager->ActiveBehavior->State = Background;
                }
                this->MenuBar->Show();
                this->Manager->ActiveBehavior = this;
                break;
            case Background:
                this->MenuBar->Hide();
                this->Manager->ActiveBehavior = this->Manager;
                this->Manager->State = Foreground;
                this->Manager->ActiveBehavior->MenuBar->Show();
                break;
            case Idle:
                this->MenuBar->Hide();
                break;
        }
    }
}

void ui3BehaviorBase::OnStreamSample(svlSample* sample, int streamindex)
{
    // Default implementation does nothing
}

int ui3BehaviorBase::AddStream(svlStreamType type, const std::string & streamname)
{
    int streamindex = Streams.size();
    ui3VideoInterfaceFilter* filter = new ui3VideoInterfaceFilter(type, streamindex, this);
    CMN_ASSERT(filter);

    Streams.resize(streamindex + 1);
    StreamNames.resize(streamindex + 1);

    Streams[streamindex] = filter;
    StreamNames[streamindex] = streamname;

    return streamindex;
}

unsigned int ui3BehaviorBase::GetStreamWidth(const int streamindex, unsigned int channel)
{
    if (streamindex < 0 || streamindex >= static_cast<int>(Streams.size())) return 0;
    return Streams[streamindex]->GetWidth(channel);
}

unsigned int ui3BehaviorBase::GetStreamWidth(const std::string & streamname, unsigned int channel)
{
    return GetStreamWidth(GetStreamIndexFromName(streamname), channel);
}

unsigned int ui3BehaviorBase::GetStreamHeight(const int streamindex, unsigned int channel)
{
    if (streamindex < 0 || streamindex >= static_cast<int>(Streams.size())) return 0;
    return Streams[streamindex]->GetHeight(channel);
}

unsigned int ui3BehaviorBase::GetStreamHeight(const std::string & streamname, unsigned int channel)
{
    return GetStreamHeight(GetStreamIndexFromName(streamname), channel);
}

int ui3BehaviorBase::GetStreamIndexFromName(const std::string & streamname)
{
    unsigned int i;
    for (i = 0; i < StreamNames.size(); i ++) {
        if (StreamNames[i] == streamname) break;
    }
    if (i < StreamNames.size()) return i;
    return -1;
}

svlFilterBase* ui3BehaviorBase::GetStreamSamplerFilter(const std::string & streamname)
{
    int streamindex = GetStreamIndexFromName(streamname);
    if (streamindex < 0 || streamindex >= static_cast<int>(Streams.size())) return 0;
    return dynamic_cast<svlFilterBase*>(Streams[streamindex]);
}

void ui3BehaviorBase::RightMasterButtonCallback(const prmEventButton & event)
{
    CMN_LOG_CLASS(6) << "RightMasterButtonCallback not overloaded for \""
                     << this->GetName() << "\"" << std::endl;
}

void ui3BehaviorBase::LeftMasterButtonCallback(const prmEventButton & event)
{
    CMN_LOG_CLASS(6) << "LeftMasterButtonCallback not overloaded for \""
                     << this->GetName() << "\"" << std::endl;
}
