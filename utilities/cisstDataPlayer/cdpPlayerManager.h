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

#ifndef _cdpPlayerManager_h
#define _cdpPlayerManager_h

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <QObject>
#include <QDockWidget>
#include <QTimerEvent>
#include <QErrorMessage>
#include "cdpSaveParameters.h"
#include "cdpPlayerDataInfo.h"
#include "cdpPlayerBase.h"

#include "ui_cdpPlayerManagerWidget.h"


//! /todo How to manage start and stop of the
//! PlayerManager should show the start and end of the data??

//! /todo Decide how to deal with path/prefix
//! /todo Centrilize quit button.

// Always include last
#include "cdpExport.h"

class CISST_EXPORT cdpPlayerManager: public QObject, public mtsTaskPeriodic
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:

    QDockWidget * GetWidget(void) {
        return &Widget;
    }

    //
    cdpPlayerManager(const std::string & name, double updatePeriod);
    ~cdpPlayerManager() {};

    void Configure(const std::string & filename = "");
    void Startup(void);    // set some initial values
    void Run(void);        // performed periodically
    void Cleanup(void) {}; // user defined cleanup

    //States of operation
    enum {STOP, PLAY, SEEK};

    // connect interfaces
    void AddPlayer(cdpPlayerBase *player);

private:
    //The instance of the widget representing this behavior.
    QDockWidget         Widget;

    Ui::cdpPlayerManagerWidget MgrWidget;
    void MakeQTConnections(void);
    //by calling "emit QSignalQTUpdate" this function will be called.
    //used this to udpate qt widgets in a thread safe way.
    void UpdateQT(void);

    mtsInt  State;

    //Current time (aka position) in the data playback process
    mtsDouble Time;

    //the start and end timestamps of the data in our system. min/max.
    mtsDouble DataStartTime;
    mtsDouble DataEndTime;
    //the time when current play was started, timestamp is the cpu clock,
    //the .Data is the corresponding time in the Data stream
    mtsDouble PlayStartTime;
    //the Data stream time when the stream playback should stop
    mtsDouble PlayUntilTime;


    //Paramters used to trigger saving: path,prefix,time range.
    cdpSaveParameters SaveParameters;

    //The following commands are sent
    //Payload : mtsDouble "Data's absolute timestamp where playing should start"
    mtsFunctionWrite Play;
    //Payload : mtsDouble "Data's absolute timestamp when playing should stop"
    mtsFunctionWrite Stop;
    //Payload : mtsDouble "Data's absolute timestamp to seek to"
    mtsFunctionWrite Seek;
    //Payload : prmSaveParamaters
    mtsFunctionWrite Save;

    //Payload : prmSaveParamaters
    mtsFunctionVoid Quit;

    void StopRequestHandler(const mtsDouble & time);
    void SeekRequestHandler(const mtsDouble & time);
    void PlayRequestHandler(const mtsDouble & time);
    void SaveRequestHandler(const cdpSaveParameters & saveParameters);
    void QuitRequestHandler(void);
    void UpdatePlayerInfoHandler(const cdpPlayerDataInfo & info);

    void UpdateLimits(void);
    //! /todo how to update the player who has just come online???
    //    void SyncRequestHandler(const mtsDouble &time);

    osaTimeServer TimeServer;
    QErrorMessage * ErrorMessageDialog;
    void ErrorMessage(const std::string & message);

    std::vector<cdpPlayerDataInfo> PlayerList;

public slots:

    void QSlotPlayClicked();
    void QSlotStopClicked();
    void QSlotSaveClicked();
    void QSlotSeekSliderMoved(int c);
    void QSlotSetSaveStartClicked(void);
    void QSlotSetSaveEndClicked(void);
    void QSlotPathClicked(void);

    void QSlotQuitClicked(void) {
        QuitRequestHandler();
    }

    void QSlotUpdateQT(void) {
        UpdateQT();
    };

signals:
    //emit this signal in order to
    void QSignalUpdateQT(void);

};

CMN_DECLARE_SERVICES_INSTANTIATION(cdpPlayerManager)

#endif  //_cdpPlayerManager_h
