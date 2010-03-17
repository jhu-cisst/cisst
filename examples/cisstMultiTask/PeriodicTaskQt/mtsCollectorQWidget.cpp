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


mtsCollectorQWidget::mtsCollectorQWidget(void)
{
    // create the widgets
    ButtonRecord = new QPushButton("Record", this);
    ButtonRecord->setCheckable(true);
    ButtonNewFile = new QPushButton("New file", this);

    // create a layout for the widgets
    CentralLayout = new QGridLayout(this);
    CentralLayout->addWidget(ButtonRecord);
    CentralLayout->addWidget(ButtonNewFile);

    QObject::connect(this->ButtonRecord, SIGNAL(toggled(bool)),
                     this, SLOT(ToggleRecordSlot(bool)));
    QObject::connect(this->ButtonNewFile, SIGNAL(clicked()),
                     this, SLOT(ButtonNewFileSlot()));
}


void mtsCollectorQWidget::ToggleRecordSlot(bool checked)
{
    if (checked) {
        emit StartCollection();
    } else {
        emit StopCollection();
    }
}

#include <iostream>

void mtsCollectorQWidget::ButtonNewFileSlot(void)
{
    emit SetOutputToDefault();
}
