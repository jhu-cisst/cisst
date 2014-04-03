/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2010-02-26

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsCollectorQtComponent_h
#define _mtsCollectorQtComponent_h

#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsFunctionRead.h>
#include <cisstMultiTask/mtsFunctionWrite.h>

#include <QObject>
#include <QWidget>

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

/*! Class used to translate cisstMultiTask commands and events to Qt
  slots and signals for components derived from mtsCollectorBase
  (i.e. mtsCollectorState and mtsCollectorEvent).  One can then use
  the Qt Widget mtsCollectorQtWidget to control one or more data
  collection components.  It is also possible to use a custom widget
  as long as it implements some or all slots and signals found in
  mtsCollectorQtWidget. */
class CISST_EXPORT mtsCollectorQtComponent: public QObject, public mtsComponent
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    mtsCollectorQtComponent(const std::string & componentName);
    ~mtsCollectorQtComponent(void);

    void Configure(const std::string & CMN_UNUSED(filename) = "") {};

    /*! Connect to a QtWidget using slots and signals.  One can use
      the widget mtsCollectorQtWidget or any custom widget with the
      same slots and signals. */
    void ConnectToWidget(QWidget * widget) const;

protected:
    struct {
        mtsFunctionVoid StartCollection;
        mtsFunctionVoid StopCollection;
        mtsFunctionWrite StartCollectionIn;
        mtsFunctionWrite StopCollectionIn;
        mtsFunctionWrite SetWorkingDirectory;
        mtsFunctionVoid SetOutputToDefault;
    } Collection;

    void CollectionStartedHandler(void);
    void CollectionStoppedHandler(const mtsUInt & count);
    void ProgressHandler(const mtsUInt & count);

public slots:
    void StartCollectionQSlot(void);
    void StopCollectionQSlot(void);
    void StartCollectionInQSlot(double);
    void StopCollectionInQSlot(double);
    void SetWorkingDirectoryQSlot(QString);
    void SetOutputToDefaultQSlot(void);

public:
signals:
    void CollectorAddedQSignal(void) const;
    void CollectionStartedQSignal(void);
    void CollectionStoppedQSignal(unsigned int);
    void ProgressQSignal(unsigned int);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsCollectorQtComponent);

#endif  // _mtsCollectorQtComponent_h
