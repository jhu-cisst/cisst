/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ali Uneri
  Created on: 2009-10-27

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstNDISerial/mtsNDISerialToolQtComponent.h>

#include <QDir>
#include <QString>

CMN_IMPLEMENT_SERVICES(mtsNDISerialToolQtComponent);


mtsNDISerialToolQtComponent::mtsNDISerialToolQtComponent(const std::string & taskName) :
    mtsComponent(taskName)
{
    ToolWidget.setupUi(&CentralWidget);
    ToolWidget.ToolGroup->setTitle(QString::fromStdString(taskName));
    CentralWidget.setWindowTitle(QString::fromStdString(taskName));

    mtsInterfaceRequired * required = AddInterfaceRequired(taskName);
    if (required) {
       required->AddFunction("GetPositionCartesian", NDI.GetPositionCartesian);
    }

    // connect Qt signals to slots
    QObject::connect(ToolWidget.ButtonRecord, SIGNAL(clicked()),
                     this, SLOT(RecordQSlot()));

    startTimer(20);
}


void mtsNDISerialToolQtComponent::timerEvent(QTimerEvent * CMN_UNUSED(event))
{
    NDI.GetPositionCartesian(NDI.PositionCartesian);
    if (NDI.PositionCartesian.Valid()) {
        ToolWidget.PositionX->setNum(NDI.PositionCartesian.Position().Translation().X());
        ToolWidget.PositionY->setNum(NDI.PositionCartesian.Position().Translation().Y());
        ToolWidget.PositionZ->setNum(NDI.PositionCartesian.Position().Translation().Z());
    } else {
        ToolWidget.PositionX->setNum(0.0);
        ToolWidget.PositionY->setNum(0.0);
        ToolWidget.PositionZ->setNum(0.0);
    }
}


void mtsNDISerialToolQtComponent::RecordQSlot(void)
{
    QString path = QDir::currentPath() + "/CollectedPoints.csv";
    std::ofstream file;
    file.open(path.toAscii(), std::ios::app);
    file << NDI.PositionCartesian.Timestamp() << ", "
         << NDI.PositionCartesian.Position().Translation().X() << ", "
         << NDI.PositionCartesian.Position().Translation().Y() << ", "
         << NDI.PositionCartesian.Position().Translation().Z() << std::endl;
    file.close();

    CMN_LOG_CLASS_RUN_VERBOSE << "RecordQSlot: point collected" << std::endl;
    qApp->beep();
}
