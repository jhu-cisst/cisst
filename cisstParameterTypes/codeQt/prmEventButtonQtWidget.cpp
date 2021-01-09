/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2013-11-11

  (C) Copyright 2013-2021 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstParameterTypes/prmEventButtonQtWidget.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

#include <QCloseEvent>
#include <QCoreApplication>
#include <QMessageBox>
#include <QTime>

CMN_IMPLEMENT_SERVICES(prmEventButtonQtWidgetComponent);

prmEventButtonQtWidgetComponent_ButtonData::prmEventButtonQtWidgetComponent_ButtonData(const std::string & name,
                                                                                       mtsInterfaceRequired * interfaceRequired):
    Name(name),
    InterfaceRequired(interfaceRequired),
    Counter(0)
{
    InterfaceRequired->AddEventHandlerWrite(&prmEventButtonQtWidgetComponent_ButtonData::EventHandler, this, "Button");
    Widget = new QLabel((Name + ": none [0][--:--:--]").c_str()); // see format in EventHandler
    Widget->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    QObject::connect(this, SIGNAL(SetValueSignal(QString)),
                     Widget, SLOT(setText(QString)));
}

void prmEventButtonQtWidgetComponent_ButtonData::EventHandler(const prmEventButton & payload)
{
    Counter++;
    QString counterString;
    counterString.setNum(Counter);
    QString payloadString;
    switch (payload.Type()) {
    case prmEventButton::PRESSED:
        payloadString = "pressed";
        break;
    case prmEventButton::RELEASED:
        payloadString = "released";
        break;
    case prmEventButton::CLICKED:
        payloadString = "clicked";
        break;
    case prmEventButton::DOUBLE_CLICKED:
        payloadString = "double-clicked";
        break;
    default:
        break;
    }
    // see default format in ctor
    emit SetValueSignal(QString("%1: %2 [%3][%4]").arg(Name.c_str(), payloadString, counterString, QTime::currentTime().toString("hh:mm:ss")));
}

prmEventButtonQtWidgetComponent::prmEventButtonQtWidgetComponent(const std::string & name):
    QWidget(),
    mtsComponent(name)
{
    SetNumberOfColumns(4);
    GridLayout = new QGridLayout();
    GridLayout->setSpacing(1);
    this->setLayout(GridLayout);
}

void prmEventButtonQtWidgetComponent::closeEvent(QCloseEvent * event)
{
    int answer = QMessageBox::warning(this, tr("prmEventButtonQtWidgetComponent"),
                                      tr("Do you really want to quit this application?"),
                                      QMessageBox::No | QMessageBox::Yes);
    if (answer == QMessageBox::Yes) {
        event->accept();
        QCoreApplication::exit();
    } else {
        event->ignore();
    }
}

void prmEventButtonQtWidgetComponent::Startup(void)
{
    if (!parent()) {
        show();
    }
}

void prmEventButtonQtWidgetComponent::SetNumberOfColumns(const size_t nbColumns)
{
    if (this->ButtonsData.size() == 0) {
        this->NumberOfColumns = nbColumns;
        return;
    }
    CMN_LOG_CLASS_INIT_ERROR << "SetNumberOfColumns: can't change number of columns after elements have been added" << std::endl;
}

bool prmEventButtonQtWidgetComponent::AddEventButton(const std::string & buttonName)
{
    mtsInterfaceRequired * interfaceRequired = this->AddInterfaceRequired(buttonName);
    if (!interfaceRequired) {
        CMN_LOG_CLASS_INIT_ERROR << "AddEventButton: unable to add required interface named \""
                                 << buttonName << "\", make sure this event has not been added already"
                                 << std::endl;
        return false;
    }

    prmEventButtonQtWidgetComponent_ButtonData * buttonData =
        new prmEventButtonQtWidgetComponent_ButtonData(buttonName, interfaceRequired);
    GridLayout->addWidget(buttonData->Widget,
                          ButtonsData.size() / NumberOfColumns,
                          ButtonsData.size() % NumberOfColumns);
    ButtonsData.AddItem(buttonName, buttonData);
    return true;
}
