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

#include "mtsCollectorQWidget.h"
#include <cisstCommon/cmnPath.h>

#include <QGroupBox>
#include <QGridLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <QFileDialog>

mtsCollectorQWidget::mtsCollectorQWidget(void)
{
    // create main layout
    CentralLayout = new QVBoxLayout(this);

    // create group for manual trigger
    ManualBox = new QGroupBox("Manual start/stop");
    ManualLayout = new QVBoxLayout(this);
    ManualStartStop = new QPushButton("Start collection", this);
    ManualStartStop->setCheckable(true);
    ManualLayout->addWidget(ManualStartStop);
    ManualBox->setLayout(ManualLayout);
    CentralLayout->addWidget(ManualBox);

    // create group for scheduled collection
    ScheduledBox = new QGroupBox("Scheduled start/stop");
    ScheduledLayout = new QHBoxLayout(this);
    ScheduledBegin = new QSpinBox(this);
    ScheduledBegin->setRange(0, 1000000);
    ScheduledBegin->setPrefix("Start in ");
    ScheduledBegin->setSuffix(" s");
    ScheduledLayout->addWidget(ScheduledBegin);
    ScheduledDuration = new QSpinBox(this);
    ScheduledDuration->setRange(1, 1000000);
    ScheduledDuration->setPrefix("for ");
    ScheduledDuration->setSuffix(" s");
    ScheduledLayout->addWidget(ScheduledDuration);
    ScheduledStart = new QPushButton("Start", this);
    ScheduledLayout->addWidget(ScheduledStart);
    ScheduledBox->setLayout(ScheduledLayout);
    CentralLayout->addWidget(ScheduledBox);

    // create group for file setting
    FileBox = new QGroupBox("Output files");
    FileLayout = new QVBoxLayout(this);
    FileDialog = new QPushButton("Change directory", this);
    FileLayout->addWidget(FileDialog);
    FileDirectory = new QLabel(this);
    FileDirectory->setText(cmnPath::GetWorkingDirectory().c_str());
    FileLayout->addWidget(FileDirectory);
    FileNew = new QPushButton("Start new file(s)", this);
    FileLayout->addWidget(FileNew);
    FileBox->setLayout(FileLayout);
    CentralLayout->addWidget(FileBox);

    QObject::connect(this->ManualStartStop, SIGNAL(toggled(bool)),
                     this, SLOT(ManualStartStopSlot(bool)));
    QObject::connect(this->ScheduledStart, SIGNAL(clicked()),
                     this, SLOT(ScheduledStartSlot()));
    QObject::connect(this->FileDialog, SIGNAL(clicked()),
                     this, SLOT(FileDialogSlot()));
    QObject::connect(this->FileNew, SIGNAL(clicked()),
                     this, SLOT(FileNewSlot()));
}


void mtsCollectorQWidget::ManualStartStopSlot(bool checked)
{
    if (checked) {
        ManualStartStop->setText("&Stop collection");
        emit StartCollection();
    } else {
        ManualStartStop->setText("&Start collection");
        emit StopCollection();
    }
}


void mtsCollectorQWidget::ScheduledStartSlot(void)
{
    const double begin = ScheduledBegin->value();
    const double duration = ScheduledDuration->value();
    emit StartCollectionIn(begin);
    emit StopCollectionIn(begin + duration);
}


void mtsCollectorQWidget::FileDialogSlot(void)
{
    QString result;
    result = QFileDialog::getExistingDirectory(this, QString("Select directory"),
                                               FileDirectory->text(),
                                               QFileDialog::ShowDirsOnly
                                               | QFileDialog::DontResolveSymlinks);
    if (!result.isNull()) {
        FileDirectory->setText(result);
        emit SetWorkingDirectory(result);
    }
}


void mtsCollectorQWidget::FileNewSlot(void)
{
    emit SetOutputToDefault();
}
