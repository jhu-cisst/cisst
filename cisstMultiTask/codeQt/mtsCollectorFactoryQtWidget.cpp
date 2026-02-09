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

#include <cisstMultiTask/mtsCollectorFactoryQtWidget.h>

#include <cisstCommon/cmnPath.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

#include <QGroupBox>
#include <QGridLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <QFileDialog>

mtsCollectorFactoryQtWidget::mtsCollectorFactoryQtWidget(const std::string & component_name):
    mtsComponent(component_name),
    NumberOfCollectors(0),
    NumberOfActiveCollectors(0),
    NumberOfSamples(0)
{
    mCategory = mtsComponentCategory::UI;
    // create the cisstMultiTask interface with commands and events
    mtsInterfaceRequired * interfaceRequired = AddInterfaceRequired("Collector");
    if (interfaceRequired) {
       interfaceRequired->AddFunction("StartCollection", Collection.Start);
       interfaceRequired->AddFunction("StopCollection", Collection.Stop);
       interfaceRequired->AddFunction("SetWorkingDirectory", Collection.SetWorkingDirectory);
       interfaceRequired->AddFunction("SetOutputToDefault", Collection.SetOutputToDefault);
       interfaceRequired->AddEventHandlerWrite(&mtsCollectorFactoryQtWidget::CollectionStartedEventHandler, this,
                                               "CollectionStarted");
       interfaceRequired->AddEventHandlerWrite(&mtsCollectorFactoryQtWidget::ProgressEventHandler, this,
                                               "Progress");
    }

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
    QObject::connect(this, SIGNAL(CollectionStartedQSignal(bool)),
                     this, SLOT(CollectionStarted(bool)));
    QObject::connect(this, SIGNAL(ProgressQSignal(unsigned int)),
                     this, SLOT(Progress(unsigned int)));
}


void mtsCollectorFactoryQtWidget::ManualStartStopSlot(bool checked)
{
    if (checked) {
        ManualStartStop->setText("Stop now");
        Collection.Start(0.0);
    } else {
        ManualStartStop->setText("Start now");
        Collection.Stop(0.0);
    }
}


void mtsCollectorFactoryQtWidget::ScheduledStartSlot(void)
{
    const double begin = ScheduledBegin->value();
    const double duration = ScheduledDuration->value();
    Collection.Start(begin);
    Collection.Stop(begin + duration);
}


void mtsCollectorFactoryQtWidget::FileDialogSlot(void)
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
        Collection.SetWorkingDirectory(result.toStdString());
    }
}


void mtsCollectorFactoryQtWidget::FileNewSlot(void)
{
    this->NumberOfSamples = 0;
    StatsNbSamples->setNum(0);
    Collection.SetOutputToDefault();
}


void mtsCollectorFactoryQtWidget::CollectionStarted(bool started)
{
    if (started) {
        this->NumberOfActiveCollectors++;
    } else {
        this->NumberOfActiveCollectors--;
    }
    this->StatsNbCollectors->setNum(static_cast<int>(this->NumberOfActiveCollectors));
}


void mtsCollectorFactoryQtWidget::Progress(unsigned int count)
{
    this->NumberOfSamples += count;
    this->StatsNbSamples->setNum(static_cast<int>(this->NumberOfSamples));
}


void mtsCollectorFactoryQtWidget::CollectionStartedEventHandler(const bool & started)
{
    emit CollectionStartedQSignal(started);
}


void mtsCollectorFactoryQtWidget::ProgressEventHandler(const size_t & count)
{
    emit ProgressQSignal(count);
}
