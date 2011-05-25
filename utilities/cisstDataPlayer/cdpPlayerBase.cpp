/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Marcin Balicki
  Created on: 2011-02-10

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "cdpPlayerBase.h"
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <QMessageBox>

CMN_IMPLEMENT_SERVICES(cdpPlayerBase);

cdpPlayerBase::cdpPlayerBase(const std::string & name, double period):
    mtsTaskPeriodic(name,period),
    Sync(false),
    State(STOP),
    Time(0),
    PlayStartTime(0),
    HasRunOnce(false)
{
    ErrorMessageDialog = new QErrorMessage();
    PlayerDataInfo.Name() = name;

    StateTable.AddData(Sync,       "Sync");
    StateTable.AddData(State,      "State");
    StateTable.AddData(Time,       "Time");
    StateTable.AddData(PlayStartTime,  "PlayStartTime");
    StateTable.AddData(PlayerDataInfo,  "PlayerDataInfo");
    StateTable.AddData(PlayUntilTime,  "PlayUntilTime");

    Time.SetAutomaticTimestamp(false);
    PlayStartTime.SetAutomaticTimestamp(false);
    PlayUntilTime.SetAutomaticTimestamp(false);

    mtsInterfaceProvided * provided = AddInterfaceProvided("ProvidesStatus");
    if (provided) {
        provided->AddCommandReadState(StateTable, Sync,         "IsSyncing");
        provided->AddCommandReadState(StateTable, State,        "GetState");
        provided->AddCommandReadState(StateTable, Time,         "GetTime");
        provided->AddCommandReadState(StateTable, PlayStartTime,   "GetPlayStartTime");
        provided->AddCommandReadState(StateTable, PlayerDataInfo,   "GetPlayerDataInfo");
        provided->AddCommandReadState(StateTable, PlayUntilTime,   "GetPlayUntilTime");

    }

    mtsInterfaceRequired *reqMan = AddInterfaceRequired("RequiresPlayerManager",MTS_OPTIONAL);

    if (reqMan) {
        reqMan->AddFunction("PlayRequest",        PlayRequest);
        reqMan->AddFunction("SeekRequest",        SeekRequest);
        reqMan->AddFunction("StopRequest",        StopRequest);
        reqMan->AddFunction("UpdatePlayerInfo",   UpdatePlayerInfo);

        reqMan->AddEventHandlerWrite(&cdpPlayerBase::PlayEventHandler,    this, "Play");
        reqMan->AddEventHandlerWrite(&cdpPlayerBase::StopEventHandler,    this, "Stop");
        reqMan->AddEventHandlerWrite(&cdpPlayerBase::SeekEventHandler,    this, "Seek");
        reqMan->AddEventHandlerWrite(&cdpPlayerBase::SaveEventHandler,    this, "Save");
        reqMan->AddEventHandlerVoid(&cdpPlayerBase::QuitEventHandler,     this, "Quit");

    }

    TimeServer = mtsTaskManager::GetInstance()->GetTimeServer();


    QObject::connect(this, SIGNAL(QSignalUpdateQT()),
                     this, SLOT( QSlotUpdateQT()) );


}

void cdpPlayerBase::ErrorMessage(const std::string &msg){

    //    ErrorMessageDialog->showMessage(tr(msg.c_str()));

    QMessageBox::critical(this->GetWidget(), tr(GetName().c_str()),tr(msg.c_str()));
}


//! Check if timestamp is within the timestamp range of the data
bool cdpPlayerBase::IsInRange(const double &time) {

    if (time > PlayerDataInfo.DataEnd())
        return false;
    if (time < PlayerDataInfo.DataStart())
       return false;

    return true;

}

//! Adjust timestamp so it is within the range of the data
bool cdpPlayerBase::SetInRange(double &time) {

    double oldTime = time;

    if (time >  PlayerDataInfo.DataEnd())
        time =  PlayerDataInfo.DataEnd();
    if (time < PlayerDataInfo.DataStart())
        time = PlayerDataInfo.DataStart();

    if (oldTime == time)
        return true;
    else
        return false;
}
