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

#ifndef _mtsCollectorQComponent_h
#define _mtsCollectorQComponent_h

#include <cisstMultiTask/mtsDevice.h>
#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsFunctionReadOrWrite.h>

#include <QObject>
#include <QWidget>

class mtsCollectorQComponent: public QObject, public mtsDevice
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

 public:
    mtsCollectorQComponent(const std::string & taskName);
    ~mtsCollectorQComponent(void);

    void Configure(const std::string & CMN_UNUSED(filename) = "") {};

    void ConnectToWidget(QWidget * widget);

 protected:
    struct {
        mtsFunctionVoid StartCollection;
        mtsFunctionVoid StopCollection;
        mtsFunctionWrite StartCollectionIn;
        mtsFunctionWrite StopCollectionIn;
        mtsFunctionVoid SetOutputToDefault;
    } Collection;

 public slots:
    void StartCollectionQSlot(void);
    void StopCollectionQSlot(void);
    void StartCollectionInQSlot(double);
    void StopCollectionInQSlot(double);
    void SetOutputToDefaultQSlot(void);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsCollectorQComponent);

#endif  // _mtsCollectorQComponent_h
