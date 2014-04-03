/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2013-11-11

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstParameterTypes/prmQtWidgetEventButtonsComponent.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

#include <QCloseEvent>
#include <QCoreApplication>
#include <QMessageBox>
#include <QTime>

CMN_IMPLEMENT_SERVICES(prmQtWidgetEventButtonsComponent);

prmQtWidgetEventButtonsComponent_ButtonData::prmQtWidgetEventButtonsComponent_ButtonData(const std::string & name,
                                                                                         mtsInterfaceRequired * interfaceRequired):
    Name(name),
    InterfaceRequired(interfaceRequired),
    Counter(0)
{
    InterfaceRequired->AddEventHandlerWrite(&prmQtWidgetEventButtonsComponent_ButtonData::EventHandler, this, "Button");
    Widget = new QLabel((Name + ": none [0][--:--:--]").c_str()); // see format in EventHandler
    Widget->setFrameStyle(QFrame::StyledPanel || QFrame::Sunken);
    QObject::connect(this, SIGNAL(SetValueSignal(QString)),
                     Widget, SLOT(setText(QString)));
}

void prmQtWidgetEventButtonsComponent_ButtonData::EventHandler(const prmEventButton & payload)
{
    Counter++;
    QString counterString;
    counterString.setNum(Counter);
    QString payloadString;
    if (payload.Type() == prmEventButton::PRESSED) {
        payloadString = "pressed";
    } else {
        payloadString = "released";
    }
    // see default format in ctor
    emit SetValueSignal(QString("%1: %2 [%3][%4]").arg(Name.c_str(), payloadString, counterString, QTime::currentTime().toString("hh:mm:ss")));
}

prmQtWidgetEventButtonsComponent::prmQtWidgetEventButtonsComponent(const std::string & name):
    QWidget(),
    mtsComponent(name)
{
    SetNumberOfColumns(4);
    GridLayout = new QGridLayout();
    GridLayout->setSpacing(1);
    this->setLayout(GridLayout);
}

void prmQtWidgetEventButtonsComponent::closeEvent(QCloseEvent * event)
{
    int answer = QMessageBox::warning(this, tr("prmQtWidgetEventButtonsComponent"),
                                      tr("Do you really want to quit this application?"),
                                      QMessageBox::No | QMessageBox::Yes);
    if (answer == QMessageBox::Yes) {
        event->accept();
        QCoreApplication::exit();
    } else {
        event->ignore();
    }
}

void prmQtWidgetEventButtonsComponent::Startup(void)
{
    if (!parent()) {
        show();
    }
}

void prmQtWidgetEventButtonsComponent::SetNumberOfColumns(const size_t nbColumns)
{
    if (this->ButtonsData.size() == 0) {
        this->NumberOfColumns = nbColumns;
        return;
    }
    CMN_LOG_CLASS_INIT_ERROR << "SetNumberOfColumns: can't change number of columns after elements have been added" << std::endl;
}

bool prmQtWidgetEventButtonsComponent::AddEventButton(const std::string & buttonName)
{
    mtsInterfaceRequired * interfaceRequired = this->AddInterfaceRequired(buttonName);
    if (!interfaceRequired) {
        CMN_LOG_CLASS_INIT_ERROR << "AddEventButton: unable to add required interface named \""
                                 << buttonName << "\", make sure this event has not been added already"
                                 << std::endl;
        return false;
    }

    prmQtWidgetEventButtonsComponent_ButtonData * buttonData =
            new prmQtWidgetEventButtonsComponent_ButtonData(buttonName, interfaceRequired);
    GridLayout->addWidget(buttonData->Widget,
                          ButtonsData.size() / NumberOfColumns,
                          ButtonsData.size() % NumberOfColumns);
    ButtonsData.AddItem(buttonName, buttonData);
    return true;
}
