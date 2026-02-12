/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2010-02-26

  (C) Copyright 2010-2025 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsCollectorFactoryQtWidget_h
#define _mtsCollectorFactoryQtWidget_h

#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsFunctionRead.h>
#include <cisstMultiTask/mtsFunctionWrite.h>

#include <QObject>
#include <QWidget>

class QGroupBox;
class QGridLayout;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QSpinBox;
class QLabel;
class QFileDialog;

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

class CISST_EXPORT mtsCollectorFactoryQtWidget: public QWidget, public mtsComponent
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

 public:
    mtsCollectorFactoryQtWidget(const std::string & component_name);
    ~mtsCollectorFactoryQtWidget(void) {};
    
    void Configure(const std::string & CMN_UNUSED(filename) = "") override {};
    
 protected:
    struct {
        mtsFunctionWrite Start;
        mtsFunctionWrite Stop;
        mtsFunctionWrite SetWorkingDirectory;
        mtsFunctionVoid SetOutputToDefault;
    } Collection;
    
    void CollectionStartedEventHandler(const bool & started);
    void ProgressEventHandler(const size_t & count);
                                              
 signals:
    void CollectorAddedQSignal(void) const;
    void CollectionStartedQSignal(bool);
    void ProgressQSignal(unsigned int);

 protected:
    unsigned int NumberOfCollectors;
    unsigned int NumberOfActiveCollectors;
    unsigned int NumberOfSamples;

    QVBoxLayout * CentralLayout;

    QGroupBox * StartStopBox;
    QGridLayout * StartStopLayout;
    QPushButton * ManualStartStop;
    QSpinBox * ScheduledBegin;
    QSpinBox * ScheduledDuration;
    QPushButton * ScheduledStart;

    QGroupBox * FileBox;
    QVBoxLayout * FileLayout;
    QPushButton * FileDialog;
    QLabel * FileDirectory;
    QPushButton * FileNew;

    QGroupBox * StatsBox;
    QGridLayout * StatsLayout;
    QLabel * StatsCollectors;
    QLabel * StatsNbCollectors;
    QLabel * StatsSamples;
    QLabel * StatsNbSamples;

public slots:
    // used internally for buttons
    void ManualStartStopSlot(bool checked);
    void ScheduledStartSlot(void);
    void FileDialogSlot(void);
    void FileNewSlot(void);

    // used to receive events from the QComponent
    void CollectionStarted(bool);
    void Progress(unsigned int count);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsCollectorFactoryQtWidget);

#endif  // _mtsCollectorFactoryQtWidget_h
