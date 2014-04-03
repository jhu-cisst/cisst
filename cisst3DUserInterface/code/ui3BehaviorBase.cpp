/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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
#include <cisst3DUserInterface/ui3Widget3D.h>


CMN_IMPLEMENT_SERVICES(ui3BehaviorBase)


ui3BehaviorBase::ui3BehaviorBase(const std::string & name,
                                 ui3Manager * manager):
    mtsTaskContinuous(name, 500), /* all behaviors have a state table of 500, ui3Manager needs at least 3 */
    MenuBar(0),
    State(Background),
    Manager(manager)
{
}


ui3BehaviorBase::~ui3BehaviorBase()
{
    for (unsigned int i = 0; i < Streams.size(); i ++) {
        if (Streams[i]) delete Streams[i];
    }
}


void ui3BehaviorBase::AddWidget3D(ui3Widget3D * widget3D)
{
    this->Widget3Ds.push_back(widget3D);

    unsigned int handleCounter;
    for (handleCounter = 0;
         handleCounter < 4;
         handleCounter++) {
        this->AddSelectable(widget3D->SideHandles[handleCounter]);
        this->AddSelectable(widget3D->CornerHandles[handleCounter]);
    }

}


void ui3BehaviorBase::AddSelectable(ui3Selectable * selectable)
{
    this->Selectables.push_back(selectable);
}


void ui3BehaviorBase::AddMenuBar(bool isManager)
{
    CMN_ASSERT(this->Manager);

    // create the menu bar
    this->MenuBar = new ui3MenuBar(this->GetName());
    CMN_ASSERT(this->MenuBar);

    // add a button in the main menu bar with callback
    if (!isManager) {
        this->MenuBar->AddClickButton("Hide",
                                      0,
                                      "iconify-left.png",
                                      &ui3BehaviorBase::SetStateBackground,
                                      this);
        this->MenuBar->AddClickButton("Close",
                                      99,
                                      "close.png",
                                      &ui3BehaviorBase::SetStateIdle,
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

void ui3BehaviorBase::Configure(const std::string & CMN_UNUSED(configFile))
{
    CMN_LOG_CLASS_INIT_WARNING << "Configure: use default configure from base class, doing nothing" << std::endl;
}

bool ui3BehaviorBase::SaveConfiguration(const std::string & CMN_UNUSED(configFile))
{
    CMN_LOG_CLASS_INIT_WARNING << "SaveConfiguration: use default save configuration from base class, doing nothing" << std::endl;
    return true;
}

void ui3BehaviorBase::Run(void)
{
    this->ProcessQueuedEvents();
    if (this->Manager->MastersAsMice()) {
        switch(this->State) {
        case Foreground:
            this->MenuBar->Show();
            this->RunForeground();
            break;
        case Background:
            this->RunBackground();
            break;
        case Idle:
        default:
            break;
        }
    } else {
        switch(this->State) {
        case Foreground:
        case Background:
            this->RunNoInput();
            break;
        case Idle:
        default:
            break;
        }
    }
    osaSleep(50.0 * cmn_ms); // bad, needs to be a variable/data member
}


void ui3BehaviorBase::SetStateForeground(void)
{
    this->SetState(Foreground);
    this->SetStateForegroundCallback();
}


void ui3BehaviorBase::SetStateBackground(void)
{
    this->SetState(Background);
    this->SetStateBackgroundCallback();
}


void ui3BehaviorBase::SetStateIdle(void)
{
    this->SetState(Idle);
    this->SetStateIdleCallback();
}


void ui3BehaviorBase::SetState(const StateType & newState)
{
    if (newState != this->State) {
        switch (newState) {
            case Foreground:
                if (this->Manager->ActiveBehavior) {
                    this->Manager->ActiveBehavior->MenuBar->Hide();
                    this->Manager->ActiveBehavior->State = Background;
                }
                this->MenuBar->Show();
                this->Manager->ActiveBehavior = this;
                if (this->State == Idle) {
                    this->OnStart();
                }
                break;
            case Background:
                this->MenuBar->Hide();
                this->Manager->ActiveBehavior = this->Manager;
                this->Manager->State = Foreground;
                this->Manager->ActiveBehavior->MenuBar->Show();
                break;
            case Idle:
                this->MenuBar->Hide();
                if (this->GetVisibleObject()) {
                    this->GetVisibleObject()->Hide();
                }
                this->Manager->ActiveBehavior = this->Manager;
                this->Manager->State = Foreground;
                this->Manager->ActiveBehavior->MenuBar->Show();
                this->OnQuit();
                break;
        }
        this->State = newState;
    }
}


void ui3BehaviorBase::OnStreamSample(svlSample * CMN_UNUSED(sample), size_t CMN_UNUSED(streamindex))
{
    // Default implementation does nothing
}


size_t ui3BehaviorBase::AddStream(svlStreamType type, const std::string & streamname)
{
    size_t streamindex = Streams.size();
    ui3VideoInterfaceFilter* filter = new ui3VideoInterfaceFilter(type, streamindex, this);
    CMN_ASSERT(filter);

    Streams.resize(streamindex + 1);
    StreamNames.resize(streamindex + 1);

    Streams[streamindex] = filter;
    StreamNames[streamindex] = streamname;

    return streamindex;
}


unsigned int ui3BehaviorBase::GetStreamWidth(const size_t streamindex, unsigned int channel)
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


svlFilterBase * ui3BehaviorBase::GetStreamSamplerFilter(const std::string & streamname)
{
    int streamindex = GetStreamIndexFromName(streamname);
    if (streamindex < 0 || streamindex >= static_cast<int>(Streams.size())) return 0;
    return dynamic_cast<svlFilterBase*>(Streams[streamindex]);
}


void ui3BehaviorBase::PrimaryMasterButtonCallback(const prmEventButton & CMN_UNUSED(event))
{
    CMN_LOG_RUN_VERBOSE << "PrimaryMasterButtonCallback not overloaded for \""
                        << this->GetName() << "\"" << std::endl;
}


void ui3BehaviorBase::SecondaryMasterButtonCallback(const prmEventButton & CMN_UNUSED(event))
{
    CMN_LOG_RUN_VERBOSE << "SecondaryMasterButtonCallback not overloaded for \""
                        << this->GetName() << "\"" << std::endl;
}


void ui3BehaviorBase::SetStateIdleCallback(void)
{
    CMN_LOG_RUN_VERBOSE << "SetStateIdleCallback not overloaded for \""
                        << this->GetName() << "\"" << std::endl;
}


void ui3BehaviorBase::SetStateForegroundCallback(void)
{
    CMN_LOG_RUN_VERBOSE << "SetStateForegroundCallback not overloaded for \""
                        << this->GetName() << "\"" << std::endl;
}


void ui3BehaviorBase::SetStateBackgroundCallback(void)
{
    CMN_LOG_RUN_VERBOSE << "SetStateBackgroundCallback not overloaded for \""
                        << this->GetName() << "\"" << std::endl;
}

