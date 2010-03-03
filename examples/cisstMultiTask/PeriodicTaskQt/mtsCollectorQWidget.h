/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2010-02-26

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsCollectorQWidget_h
#define _mtsCollectorQWidget_h

#include <QGridLayout>
#include <QPushButton>
#include <QWidget>


class mtsCollectorQWidget: public QWidget
{
    Q_OBJECT;

public:
    mtsCollectorQWidget(void);
    ~mtsCollectorQWidget(void) {};

protected:
    QGridLayout * CentralLayout;
    QPushButton * ButtonRecord;

public slots:
    void ToggleRecordSlot(bool checked);

public:
signals:
    void StartCollection(void);
    void StopCollection(void);

};

#endif  // _mtsCollectorQWidget_h
