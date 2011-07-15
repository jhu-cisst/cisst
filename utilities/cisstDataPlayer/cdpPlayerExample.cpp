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

#include "cdpPlayerExample.h"
#include <math.h>
#include <QMenu>
#include <cisstOSAbstraction/osaGetTime.h>

#include <iostream>
#include <sstream>

CMN_IMPLEMENT_SERVICES(cdpPlayerExample);

cdpPlayerExample::cdpPlayerExample(const std::string & name, double period):
    cdpPlayerBase(name,period)
{
    // create the user interface

    ExWidget.setupUi(&Widget);
}


cdpPlayerExample::~cdpPlayerExample()
{
}


void cdpPlayerExample::MakeQTConnections(void)
{
    QObject::connect(ExWidget.PlayButton, SIGNAL(clicked()),
                     this, SLOT( QSlotPlayClicked()) );

    QObject::connect(ExWidget.TimeSlider, SIGNAL(sliderMoved(int)),
                     this, SLOT( QSlotSeekSliderMoved(int)) );

    QObject::connect(ExWidget.SyncCheck, SIGNAL(clicked(bool)),
                     this, SLOT( QSlotSyncCheck(bool)) );

    QObject::connect(ExWidget.StopButton, SIGNAL(clicked()),
                     this, SLOT( QSlotStopClicked()) );

    QObject::connect(ExWidget.SetSaveStartButton, SIGNAL(clicked()),
                     this, SLOT( QSlotSetSaveStartClicked()) );

    QObject::connect(ExWidget.SetSaveEndButton, SIGNAL(clicked()),
                     this, SLOT( QSlotSetSaveEndClicked()) );
}


void cdpPlayerExample::Configure(const std::string & CMN_UNUSED(filename))
{
    MakeQTConnections();
    LoadData();
    UpdateLimits();

    Widget.setWindowTitle(QString::fromStdString(GetName()));
    Widget.show();
}


void cdpPlayerExample::Startup(void)
{

}


void cdpPlayerExample::Run(void)
{
    ProcessQueuedEvents();
    ProcessQueuedCommands();

    //update the model (load data) etc.
    if (State == PLAY) {

        double currentTime = TimeServer.GetAbsoluteTimeInSeconds();
        Time = currentTime - PlayStartTime.Timestamp() + PlayStartTime.Data;

        if (Time > PlayUntilTime)  {
            Time = PlayUntilTime;
            State = STOP;
        }
        else {
            //Load and Prep current data
        }
    }
    //make sure we are at the correct seek position.
    else if (State == SEEK) {
        //Load and Prep current data
    }

    else if (State == STOP) {
        //do Nothing
    }

    //now display updated data in the qt thread space.
    if (Widget.isVisible()) {
        emit QSignalUpdateQT();
    }
}


//in QT thread space
void cdpPlayerExample::UpdateQT(void)
{
    if (State == PLAY) {
        //Display the last datasample before Time.
        ExWidget.TimeSlider->setValue((int)Time.Data);
    }
    //Make sure we are at the correct seek location.
    else if (State == STOP) {
        //Optional: Test if the data needs to be updated:
        ExWidget.TimeSlider->setValue((int)Time.Data);
    }

    else if (State == SEEK) {
        //Optional: Test if the data needs to be updated:
        ExWidget.TimeSlider->setValue((int)Time.Data);
    }

    ExWidget.TimeLabel->setText(QString::number(Time.Data,'f', 3));
}


void cdpPlayerExample::Play(const mtsDouble & time)
{
    if (Sync) {
        CMN_LOG_CLASS_RUN_DEBUG << "Play " << PlayStartTime << std::endl;
        State = PLAY;
        PlayUntilTime = PlayerDataInfo.DataEnd();
        PlayStartTime = time;
    }
}


void cdpPlayerExample::Stop(const mtsDouble & time)
{
    if (Sync) {
        CMN_LOG_CLASS_RUN_DEBUG << "Stop " << time << std::endl;
        PlayUntilTime = time;
    }
}


void cdpPlayerExample::Seek(const mtsDouble & time)
{
    if (Sync) {
        CMN_LOG_CLASS_RUN_DEBUG << "Seek " << time << std::endl;

        State = SEEK;
        PlayUntilTime = PlayerDataInfo.DataEnd();
        Time = time;
    }
}


void cdpPlayerExample::Save(const cdpSaveParameters & saveParameters)
{
    if (Sync) {
        CMN_LOG_CLASS_RUN_DEBUG << "Save " << saveParameters << std::endl;
    }
}


void cdpPlayerExample::Quit(void)
{
    CMN_LOG_CLASS_RUN_DEBUG << "Quit" << std::endl;
    this->Kill();
}


void cdpPlayerExample::QSlotPlayClicked(void)
{
    mtsDouble playTime = Time; //this should be read from the state table!!!
    playTime.Timestamp() = TimeServer.GetAbsoluteTimeInSeconds();

    if (Sync) {
        PlayRequest(playTime);
    } else {
        //not quite thread safe, if there is mts play call this can be corrupt.
        State = PLAY;
        PlayUntilTime = PlayerDataInfo.DataEnd();
        PlayStartTime = playTime;
    }
}


void cdpPlayerExample::QSlotSeekSliderMoved(int c)
{
    mtsDouble t = c;

    if (Sync) {
        SeekRequest(t);
    } else {
        State = SEEK;
        Time = t;
    }
    PlayUntilTime = PlayerDataInfo.DataEnd();
}


void cdpPlayerExample::QSlotSyncCheck(bool checked)
{
    Sync = checked;
}


void cdpPlayerExample::QSlotStopClicked(void)
{
    mtsDouble now = Time;

    if (Sync) {
        StopRequest(now);
    } else {
        PlayUntilTime = now;
    }
}


void cdpPlayerExample::LoadData(void)
{
    //PlayerDataInfo.DataStart() = 1297723451.415;
    //PlayerDataInfo.DataEnd() = 1297723900.022;

    PlayerDataInfo.DataStart() = 4;
    PlayerDataInfo.DataEnd() = 100;


    if (Time < PlayerDataInfo.DataStart()) {
        Time = PlayerDataInfo.DataStart();
    }

    if (Time > PlayerDataInfo.DataEnd()) {
        Time = PlayerDataInfo.DataEnd();
    }

    //This is the standard.
    PlayUntilTime = PlayerDataInfo.DataEnd();

    UpdatePlayerInfo(PlayerDataInfo);
    UpdateLimits();
}


void cdpPlayerExample::QSlotSetSaveStartClicked(void)
{
    ExWidget.SaveStartSpin->setValue(Time.Data);
}
 

void cdpPlayerExample::QSlotSetSaveEndClicked(void)
{
    ExWidget.SaveEndSpin->setValue(Time.Data);
}


void cdpPlayerExample::UpdateLimits()
{
    ExWidget.TimeSlider->setRange((int)PlayerDataInfo.DataStart(), (int)PlayerDataInfo.DataEnd());

    ExWidget.TimeStartLabel->setText( QString::number(PlayerDataInfo.DataStart(),'f', 3));
    ExWidget.TimeEndLabel->setText( QString::number( PlayerDataInfo.DataEnd(),'f', 3));

    ExWidget.SaveStartSpin->setRange( PlayerDataInfo.DataStart(), PlayerDataInfo.DataEnd());
    ExWidget.SaveEndSpin->setRange( PlayerDataInfo.DataStart(), PlayerDataInfo.DataEnd());
}
