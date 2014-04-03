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

#ifndef _mtsCollectorQtWidget_h
#define _mtsCollectorQtWidget_h

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

class CISST_EXPORT mtsCollectorQtWidget: public QWidget
{
    Q_OBJECT;

public:
    mtsCollectorQtWidget(void);
    ~mtsCollectorQtWidget(void) {};

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
    QLabel * StatsTotalCollectors;
    QLabel * StatsSamples;
    QLabel * StatsNbSamples;

public slots:
    // used internally for buttons
    void ManualStartStopSlot(bool checked);
    void ScheduledStartSlot(void);
    void FileDialogSlot(void);
    void FileNewSlot(void);

    // used to receive events from the QComponent
    void CollectorAdded(void);
    void CollectionStarted(void);
    void CollectionStopped(unsigned int count);
    void Progress(unsigned int count);

public:
signals:
    void StartCollection(void);
    void StopCollection(void);
    void StartCollectionIn(double delay);
    void StopCollectionIn(double delay);
    void SetWorkingDirectory(QString);
    void SetOutputToDefault(void);
};

#endif  // _mtsCollectorQtWidget_h
