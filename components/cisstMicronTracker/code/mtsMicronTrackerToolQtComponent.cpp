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

#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMicronTracker/mtsMicronTrackerToolQtComponent.h>

#include <QDir>
#include <QString>

CMN_IMPLEMENT_SERVICES(mtsMicronTrackerToolQtComponent);


mtsMicronTrackerToolQtComponent::mtsMicronTrackerToolQtComponent(const std::string & taskName) :
    mtsComponent(taskName)
{
    MTC.MarkerProjectionLeft.SetSize(2);
    MTC.MarkerProjectionRight.SetSize(2);

    ToolWidget.setupUi(&CentralWidget);
    ToolWidget.ToolGroup->setTitle(QString::fromStdString(taskName));
    CentralWidget.setWindowTitle(QString::fromStdString(taskName));

    mtsInterfaceRequired * required = AddInterfaceRequired(taskName);
    if (required) {
       required->AddFunction("GetPositionCartesian", MTC.GetPositionCartesian);
       required->AddFunction("GetMarkerProjectionLeft", MTC.GetMarkerProjectionLeft);
       required->AddFunction("GetMarkerProjectionRight", MTC.GetMarkerProjectionRight);
    }

    // connect Qt signals to slots
    QObject::connect(ToolWidget.ButtonRecord, SIGNAL(clicked()),
                     this, SLOT(RecordQSlot()));

    startTimer(20);
}


void mtsMicronTrackerToolQtComponent::timerEvent(QTimerEvent * event)
{
    MTC.GetPositionCartesian(MTC.PositionCartesian);
    MTC.GetMarkerProjectionLeft(MTC.MarkerProjectionLeft);
    MTC.GetMarkerProjectionRight(MTC.MarkerProjectionRight);

    if (MTC.PositionCartesian.Valid()) {
        ToolWidget.PositionX->setNum(MTC.PositionCartesian.Position().Translation().X());
        ToolWidget.PositionY->setNum(MTC.PositionCartesian.Position().Translation().Y());
        ToolWidget.PositionZ->setNum(MTC.PositionCartesian.Position().Translation().Z());
        MarkerProjectionLeft.setX(MTC.MarkerProjectionLeft.X());
        MarkerProjectionLeft.setY(MTC.MarkerProjectionLeft.Y());
        MarkerProjectionRight.setX(MTC.MarkerProjectionRight.X());
        MarkerProjectionRight.setY(MTC.MarkerProjectionRight.Y());
    } else {
        ToolWidget.PositionX->setNum(0.0);
        ToolWidget.PositionY->setNum(0.0);
        ToolWidget.PositionZ->setNum(0.0);
        MarkerProjectionLeft.setX(0.0);
        MarkerProjectionLeft.setY(0.0);
        MarkerProjectionRight.setX(0.0);
        MarkerProjectionRight.setY(0.0);
    }
}


void mtsMicronTrackerToolQtComponent::RecordQSlot(void)
{
    QString path = QDir::currentPath() + "/CollectedPoints.csv";
    std::ofstream file;
    file.open(path.toAscii(), std::ios::app);
    file << MTC.PositionCartesian.Timestamp() << ", "
         << MTC.PositionCartesian.Position().Translation().X() << ", "
         << MTC.PositionCartesian.Position().Translation().Y() << ", "
         << MTC.PositionCartesian.Position().Translation().Z() << std::endl;
    file.close();
}
