/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Praneeth Sadda, Anton Deguet
  Created on: 2011-11-11

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstMultiTask/mtsQtWidgetEvent.h>

#include <cisstCommon/cmnUnits.h>
#include <cisstMultiTask/mtsQtWidgetFactory.h>
#include <cisstMultiTask/mtsQtWidgetGenericObject.h>

#include <QVBoxLayout>
#include <QObject>
#include <QLabel>


mtsQtWidgetEvent::mtsQtWidgetEvent(void):
    QWidget(),
    Counter(0)
{
    MainLayout = new QVBoxLayout();
    MainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(MainLayout);

    Info = new QLabel("No event so far");
    MainLayout->addWidget(Info);
}


void mtsQtWidgetEvent::UpdateInfo(void)
{
    QString info;
    Counter++;
    if (Counter == 1) {
        info = "Only 1 event so far, received at ";
    } else {
        info.setNum(Counter);
        info += " events, last received at ";
    }
    info += QTime::currentTime().toString();
    Info->setText(info);
}


mtsQtWidgetEventVoid::mtsQtWidgetEventVoid(void):
    mtsQtWidgetEvent()
{
}

void mtsQtWidgetEventVoid::EventHandler(void)
{
    UpdateInfo();
}


mtsQtWidgetEventWrite::mtsQtWidgetEventWrite(void):
    mtsQtWidgetEvent(),
    PayloadWidget(0),
    Command(0)
{
}


void mtsQtWidgetEventWrite::EventHandler(const mtsGenericObject & payload)
{
    UpdateInfo();
    mtsQtWidgetGenericObjectRead * widget = dynamic_cast<mtsQtWidgetGenericObjectRead *>(PayloadWidget);
    if (widget) {
        widget->SetValue(payload);
    }
}


void mtsQtWidgetEventWrite::SetCommand(const mtsCommandWriteBase * command)
{
    Command = command;
}


void mtsQtWidgetEventWrite::CreateArgumentsWidgets(void)
{
    if (Command) {
        mtsQtWidgetGenericObjectRead * widget = mtsQtWidgetFactory::CreateWidgetRead(&typeid(*(Command->GetArgumentPrototype())));
        if (!widget) {
            std::string message = Command->GetArgumentPrototype()->Services()->GetName();
            CMN_LOG_CLASS_INIT_WARNING << "EventHandler: can't create widget for \"" << message << "\"" << std::endl;
            message.append("Payload type not supported");
            PayloadWidget = new QLabel(message.c_str());
        } else {
            PayloadWidget = widget;
            widget->SetValue(*(Command->GetArgumentPrototype()));
        }
        MainLayout->addWidget(PayloadWidget);
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "CreateArgumentsWidgets: Command not set" << std::endl;
    }
}
