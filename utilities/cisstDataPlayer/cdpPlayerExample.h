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

#ifndef _cdpPlayerExample_h
#define _cdpPlayerExample_h

#include <QObject>
#include <QtGui/QCloseEvent>
#include <cisstMultiTask.h>

#include "ui_cdpPlayerExampleWidget.h"
#include "cdpPlayerBase.h"

//! todo Think about the thread safety issue. (At the moment, mtscommands and qslots can manipulte same data at the same time)
//! If the data processing is not heavy we can use the main QT thread via QTimer and mtsTaskFromCallback, or add more mtsFunction calls.

class cdpPlayerExample: public cdpPlayerBase
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_DEBUG);

public:
    cdpPlayerExample(const std::string & name, double period);
    ~cdpPlayerExample();

    //call this after tasks connections are made
    void Configure(const std::string & filename = "");

    void Startup(void);
    void Run(void);
    void Cleanup(void) {};

private:

    Ui::cdpPlayerExampleWidget ExWidget;
    void MakeQTConnections(void);
    //by calling "emit QSignalQTUpdate" this function will be called.
    //used this to udpate qt widgets in a thread safe way.
    void UpdateQT(void);

    void Stop(const mtsDouble & time);
    void Seek(const mtsDouble & time);
    void Play(const mtsDouble & time);
    void Save(const cdpSaveParameters & saveParameters);
    void Quit(void);
    void LoadData(void);
    void UpdateLimits(void);


private slots:

    void QSlotSeekSliderMoved(int c);

    void QSlotSyncCheck(bool checked);

    void QSlotPlayClicked(void);

    void QSlotStopClicked(void);

    void QSlotSetSaveStartClicked(void);

    void QSlotSetSaveEndClicked(void);

};

CMN_DECLARE_SERVICES_INSTANTIATION(cdpPlayerExample);

#endif // _cdpPlayerExample_h
