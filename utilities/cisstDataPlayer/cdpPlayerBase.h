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

#ifndef _cdpPlayerBase_h
#define _cdpPlayerBase_h


#include <QObject>
#include <QDockWidget>
#include <QTimerEvent>
#include <QErrorMessage>

#include "cdpSaveParameters.h"
#include "cdpPlayerDataInfo.h"

#include <cisstMultiTask/mtsTaskPeriodic.h>

//The update rate is a QT timer that calls Update() implemented by the derived classes
//can also use other ways...
//User can use the QT timer or their own thread, as in case of SVL stream.

//Time refers to the absolute timestamp in the data stream
//CPUTime refers to the computer clock.
//The



// Always include last
#include "cdpExport.h"

class CISST_EXPORT cdpPlayerBase: public QObject, public mtsTaskPeriodic
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:

    QWidget * GetWidget(void) {
        return &Widget;
    }

    virtual bool Connect(void) { return false; };

    // all four methods are pure virtual in mtsTask
    virtual void Startup(void) {};    // set some initial values
    virtual void Run(void) {};        // performed periodically
    virtual void Cleanup(void) {}; // user defined cleanup

    virtual void SetSynced(bool isSynced) { Sync = isSynced; };

    //States of operation
    enum {STOP, PLAY, SEEK};

protected:
    //
    cdpPlayerBase(const std::string & name, double period);
    ~cdpPlayerBase() {};


    //The instance of the widget representing this player.
    QWidget Widget;

    bool Sync;
    mtsInt State;

    //Current time (aka position) in the data playback process
    mtsDouble Time;
    //the start and end timestamps of the data in our system. min/max.
    cdpPlayerDataInfo PlayerDataInfo;
    //the time when current play was started, timestamp is the cpu clock,
    //the .Data is the corresponding time in the Data stream
    mtsDouble PlayStartTime;
    //the Data stream time when the stream playback should stop
    mtsDouble PlayUntilTime;

    //Paramters used to trigger saving: path,prefix,time range.
    cdpSaveParameters SaveParameters;

    //The following commands are sent
    //Payload : mtsDouble "Data's absolute timestamp where playing should start"
    mtsFunctionWrite PlayRequest;
    //Payload : mtsDouble "Data's absolute timestamp when playing should stop"
    mtsFunctionWrite StopRequest;
    //Payload : mtsDouble "Data's absolute timestamp to seek to"
    mtsFunctionWrite SeekRequest;
    //the start and end of the data period in our system.
    mtsFunctionWrite UpdatePlayerInfo;

    struct {
        mtsFunctionRead IsSyncing;
        mtsFunctionRead GetState;
        mtsFunctionRead GetTime;
        mtsFunctionWrite WriteTime;
        mtsFunctionRead GetPlayStartTime;
        mtsFunctionRead GetPlayerDataInfo;
        mtsFunctionRead GetPlayUntilTime;
    } BaseAccess;

    //The following functions are callbacks are called by the playerManager
    //need to be implemented by each player.
    virtual void Stop(const mtsDouble & time) = 0;
    virtual void Seek(const mtsDouble & time) = 0;
    virtual void Play(const mtsDouble & time) = 0;
    virtual void Save(const cdpSaveParameters & saveParameters) = 0;
    virtual void Quit(void) = 0;
    //by calling "emit QSignalUpdateQT" this function will be called.
    //used this to udpate qt widgets in a thread safe way.
    virtual void UpdateQT(void) {};

    //! Check if timestamp is within the timestamp range of the data
    bool IsInRange(const double & time);
    //! Adjust timestamp so it is within the range of the data
    //! return true if time is in the range and no change is applied.
    bool SetInRange(double & time);
    // Set Time
    void SetTime(const mtsDouble &time);

    osaTimeServer TimeServer;

    QErrorMessage * ErrorMessageDialog;
    void ErrorMessage(const std::string & message);

    // void closeEvent(QCloseEvent *event) ;

    bool HasRunOnce;

private:
    //These are event handlers that call their cooresponding virtual functions.
    void PlayEventHandler(const mtsDouble & time) {
        Play(time);
    }

    //Stop the player at time
    void StopEventHandler(const mtsDouble & time) {
        Stop(time);
    }

    //Seek to time
    void SaveEventHandler(const cdpSaveParameters & saveParameters) {
        Save(saveParameters);
    }

    //Seek to time
    void SeekEventHandler(const mtsDouble & time) {
        Seek(time);
    }

    void QuitEventHandler(void){
        Quit();
    }

public slots:


    void QSlotUpdateQT(void) {
        UpdateQT();
    };


signals:

    //emit this signal in order to
    void QSignalUpdateQT(void);

};

CMN_DECLARE_SERVICES_INSTANTIATION(cdpPlayerBase)

#endif  //_cdpPlayerBase_h
