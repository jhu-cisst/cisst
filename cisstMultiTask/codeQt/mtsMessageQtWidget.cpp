/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2013-05-17

  (C) Copyright 2013-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

// system include
#include <iostream>

// cisst
#include <cisstMultiTask/mtsMessageQtWidget.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

// Qt
#include <QScrollBar>
#include <QTime>
#include <QColor>
#include <QKeyEvent>

mtsMessageQtWidget::mtsMessageQtWidget(void)
{
    Tag = 0;
}

void mtsMessageQtWidget::SetInterfaceRequired(mtsInterfaceRequired * interfaceRequired)
{
    if (interfaceRequired) {
        interfaceRequired->AddEventHandlerWrite(&mtsMessageQtWidget::ErrorEventHandler,
                                                this, "error");
        interfaceRequired->AddEventHandlerWrite(&mtsMessageQtWidget::WarningEventHandler,
                                                this, "warning");
        interfaceRequired->AddEventHandlerWrite(&mtsMessageQtWidget::StatusEventHandler,
                                                this, "status");
    }
}

void mtsMessageQtWidget::keyPressEvent(QKeyEvent * event)
{
    switch(event->key()) {
    case Qt::Key_C:
        emit SignalClear();
        break;
    case Qt::Key_T:
        Tag++;
        emit SignalSetColor(QColor("green"));
        emit SignalAppendMessage(QTime::currentTime().toString("hh:mm:ss") + QString(" <----- Tag ")
                                 + QString::number(Tag) + QString(" -----> "));
        break;
    default:
        break;
    }
}

void mtsMessageQtWidget::SlotTextChanged(void)
{
    this->verticalScrollBar()->setSliderPosition(this->verticalScrollBar()->maximum());
}

void mtsMessageQtWidget::setupUi(void)
{
    this->setReadOnly(true);
    this->ensureCursorVisible();
    this->resize(this->width(), 600);
    this->setToolTip(QString("'c' to clear, 't' to tag"));

    // messages
    connect(this, SIGNAL(SignalAppendMessage(QString)),
            this, SLOT(append(QString)));
    connect(this, SIGNAL(SignalSetColor(QColor)),
            this, SLOT(setTextColor(QColor)));
    connect(this, SIGNAL(SignalClear()),
            this, SLOT(clear()));
    connect(this, SIGNAL(textChanged()),
            this, SLOT(SlotTextChanged()));
}

void mtsMessageQtWidget::ErrorEventHandler(const mtsMessage & message)
{
    emit SignalSetColor(QColor("red"));
    emit SignalAppendMessage(QTime::currentTime().toString("hh:mm:ss") + QString(" error #")
                             + QString::number(message.Counter) + QString(": ")
                             + QString(message.Message.c_str()));
}

void mtsMessageQtWidget::WarningEventHandler(const mtsMessage & message)
{
    emit SignalSetColor(QColor("darkOrange"));
    emit SignalAppendMessage(QTime::currentTime().toString("hh:mm:ss") + QString(" warning #")
                             + QString::number(message.Counter) + QString(": ")
                             + QString(message.Message.c_str()));
}

void mtsMessageQtWidget::StatusEventHandler(const mtsMessage & message)
{
    emit SignalSetColor(palette().color(QPalette::Text));
    emit SignalAppendMessage(QTime::currentTime().toString("hh:mm:ss") + QString(" status #")
                             + QString::number(message.Counter) + QString(": ")
                             + QString(message.Message.c_str()));
}



CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(mtsMessageQtWidgetComponent, mtsComponent, std::string);

mtsMessageQtWidgetComponent::mtsMessageQtWidgetComponent(const std::string & componentName):
    mtsComponent(componentName)
{
    // Setup CISST Interface
    mtsInterfaceRequired * interfaceRequired = AddInterfaceRequired("Component");
    if (interfaceRequired) {
        this->SetInterfaceRequired(interfaceRequired);
    }
    setupUi();
}
