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

#ifndef _serverTask_h
#define _serverTask_h

#include <cisstMultiTask.h>

#include "ui_server.h"
#include <QObject>
#include <QMainWindow>


class serverTask: public QObject, public mtsDevice
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    serverTask(const std::string & taskName);
    ~serverTask(void) {};

    void Configure(const std::string & CMN_UNUSED(filename)) {};

protected:
    void Void(void);
    void Write(const mtsInt & data);
    void Read(mtsInt & placeHolder) const;
    void QualifiedRead(const mtsInt & data, mtsInt & placeHolder) const;
    void SendButtonClickEvent() {
        EventVoid();
    }

    mtsFunctionVoid EventVoid;
    mtsFunctionWrite EventWrite;

    int VoidCounter;
    int ReadValue;
    int QualifiedReadValue;

    QMainWindow MainWindow;
    Ui::ServerWindow ServerWindow;

signals:
    void VoidQtSignal(int voidCounter);
    void WriteQtSignal(int newValue);

public slots:
    void ReadQtSlot(int);
    void QualifiedReadQtSlot(int);
    void EventVoidQtSlot(void);
    void EventWriteQtSlot(int);
};

CMN_DECLARE_SERVICES_INSTANTIATION(serverTask);

#endif  // _serverTask_h
