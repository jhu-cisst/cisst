/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2009-10-26

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _clientQDevice_h
#define _clientQDevice_h

#include <cisstMultiTask.h>

#include <QMainWindow>
#include <QObject>

#include "ui_client.h"


class clientQDevice : public QObject, public mtsDevice
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

 public:
    clientQDevice(const std::string & taskName);
    ~clientQDevice(void) {};

    void Configure(const std::string & CMN_UNUSED(filename)) {};

 protected:
    void EventVoidHandler(void);
    void EventWriteHandler(const mtsInt & value);

    Ui::ClientWidget ClientWidget;
    QWidget CentralWidget;
    QMainWindow MainWindow;

    mtsFunctionVoid Void;
    mtsFunctionWrite Write;
    mtsFunctionRead Read;
    mtsFunctionQualifiedRead QualifiedRead;

    int EventCounter;

 signals:
    void ReadQSignal(int newValue);
    void QualifiedReadQSignal(int newValue);
    void EventQSignal(int newValue);

 public slots:
    void VoidQSlot(void);
    void WriteQSlot(int newValue);
    void ReadQSlot(void);
    void QualifiedReadQSlot(int newValue);
};

CMN_DECLARE_SERVICES_INSTANTIATION(clientQDevice);

#endif  // _clientQDevice_h
