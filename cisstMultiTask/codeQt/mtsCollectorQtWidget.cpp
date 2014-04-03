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

#include <cisstMultiTask/mtsCollectorQtWidget.h>
#include <cisstCommon/cmnPath.h>

#include <QGroupBox>
#include <QGridLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <QFileDialog>

mtsCollectorQtWidget::mtsCollectorQtWidget(void):
    NumberOfCollectors(0),
    NumberOfActiveCollectors(0),
    NumberOfSamples(0)
{
    // create main layout
    CentralLayout = new QVBoxLayout(this);

    // create group for manual trigger
    StartStopBox = new QGroupBox("Start stop");
    StartStopLayout = new QGridLayout(this->StartStopBox);
    StartStopBox->setLayout(StartStopLayout);
    CentralLayout->addWidget(StartStopBox);
    // manual start
    ManualStartStop = new QPushButton("Start now", this);
    ManualStartStop->setCheckable(true);
    StartStopLayout->addWidget(ManualStartStop, 0, 0);
    // scheduled start
    ScheduledBegin = new QSpinBox(this);
    ScheduledBegin->setRange(0, 1000000);
    ScheduledBegin->setPrefix("Start in ");
    ScheduledBegin->setSuffix(" s");
    StartStopLayout->addWidget(ScheduledBegin, 1, 0);
    ScheduledDuration = new QSpinBox(this);
    ScheduledDuration->setRange(1, 1000000);
    ScheduledDuration->setPrefix("for ");
    ScheduledDuration->setSuffix(" s");
    StartStopLayout->addWidget(ScheduledDuration, 1, 1);
    ScheduledStart = new QPushButton("Start", this);
    StartStopLayout->addWidget(ScheduledStart, 1, 2);

    // create group for file setting
    FileBox = new QGroupBox("Output files");
    FileLayout = new QVBoxLayout(this->FileBox);
    FileBox->setLayout(FileLayout);
    CentralLayout->addWidget(FileBox);
    // add buttons
    FileDialog = new QPushButton("Change directory", this);
    FileLayout->addWidget(FileDialog);
    FileDirectory = new QLabel(this);
    FileDirectory->setText(cmnPath::GetWorkingDirectory().c_str());
    FileLayout->addWidget(FileDirectory);
    FileNew = new QPushButton("Start new file(s)", this);
    FileLayout->addWidget(FileNew);

    // create group for stats
    StatsBox = new QGroupBox("Statistics");
    StatsLayout = new QGridLayout(this->StatsBox);
    StatsBox->setLayout(StatsLayout);
    CentralLayout->addWidget(StatsBox);
    // add labels
    StatsCollectors = new QLabel(this);
    StatsCollectors->setText("Active collectors:");
    StatsLayout->addWidget(StatsCollectors, 0, 0);
    StatsNbCollectors = new QLabel(this);
    StatsNbCollectors->setNum(0);
    StatsLayout->addWidget(StatsNbCollectors, 0, 1);
    StatsTotalCollectors = new QLabel(this);
    StatsTotalCollectors->setText(" out of 0");
    StatsLayout->addWidget(StatsTotalCollectors, 0, 2);
    StatsSamples = new QLabel(this);
    StatsSamples->setText("Samples collected:");
    StatsLayout->addWidget(StatsSamples, 1, 0);
    StatsNbSamples = new QLabel(this);
    StatsNbSamples->setNum(0);
    StatsLayout->addWidget(StatsNbSamples, 1, 1);

    // make sure widgets to spread
    CentralLayout->addStretch();

    // connect all
    QObject::connect(this->ManualStartStop, SIGNAL(toggled(bool)),
                     this, SLOT(ManualStartStopSlot(bool)));
    QObject::connect(this->ScheduledStart, SIGNAL(clicked()),
                     this, SLOT(ScheduledStartSlot()));
    QObject::connect(this->FileDialog, SIGNAL(clicked()),
                     this, SLOT(FileDialogSlot()));
    QObject::connect(this->FileNew, SIGNAL(clicked()),
                     this, SLOT(FileNewSlot()));
}


void mtsCollectorQtWidget::ManualStartStopSlot(bool checked)
{
    if (checked) {
        ManualStartStop->setText("Stop now");
        emit StartCollection();
    } else {
        ManualStartStop->setText("Start now");
        emit StopCollection();
    }
}


void mtsCollectorQtWidget::ScheduledStartSlot(void)
{
    const double begin = ScheduledBegin->value();
    const double duration = ScheduledDuration->value();
    emit StartCollectionIn(begin);
    emit StopCollectionIn(begin + duration);
}


void mtsCollectorQtWidget::FileDialogSlot(void)
{
    QString result;
    result = QFileDialog::getExistingDirectory(this, QString("Select directory"),
                                               FileDirectory->text(),
                                               QFileDialog::ShowDirsOnly
                                               | QFileDialog::DontResolveSymlinks);
    if (!result.isNull()) {
        FileDirectory->setText(result);
        this->NumberOfSamples = 0;
        StatsNbSamples->setNum(0);
        emit SetWorkingDirectory(result);
    }
}


void mtsCollectorQtWidget::FileNewSlot(void)
{
    this->NumberOfSamples = 0;
    StatsNbSamples->setNum(0);
    emit SetOutputToDefault();
}


void mtsCollectorQtWidget::CollectorAdded(void)
{
    this->NumberOfCollectors++;
    QString numberOfCollectors;
    numberOfCollectors.setNum(this->NumberOfCollectors);
    StatsTotalCollectors->setText(QString("(out of ") + numberOfCollectors + QString(")"));
}


void mtsCollectorQtWidget::CollectionStarted(void)
{
    this->NumberOfActiveCollectors++;
    this->StatsNbCollectors->setNum(static_cast<int>(this->NumberOfActiveCollectors));
}


void mtsCollectorQtWidget::CollectionStopped(unsigned int count)
{
    this->NumberOfActiveCollectors--;
    this->StatsNbCollectors->setNum(static_cast<int>(this->NumberOfActiveCollectors));
    this->NumberOfSamples += count;
    this->StatsNbSamples->setNum(static_cast<int>(this->NumberOfSamples));
}


void mtsCollectorQtWidget::Progress(unsigned int count)
{
    this->NumberOfSamples += count;
    this->StatsNbSamples->setNum(static_cast<int>(this->NumberOfSamples));
}
