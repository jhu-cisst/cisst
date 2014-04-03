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

#ifndef _prmQtWidgetEventButtonsComponent_h
#define _prmQtWidgetEventButtonsComponent_h

#include <QWidget>
#include <QGridLayout>
#include <QLabel>

#include <cisstMultiTask/mtsComponent.h>
#include <cisstParameterTypes/prmEventButton.h>

// Always include last
#include <cisstParameterTypes/prmExportQt.h>

class prmQtWidgetEventButtonsComponent_ButtonData: public QObject {
    Q_OBJECT;
protected:
    std::string Name;
    mtsInterfaceRequired * InterfaceRequired;
    unsigned int Counter;
public:
    prmQtWidgetEventButtonsComponent_ButtonData(const std::string & name, mtsInterfaceRequired * interfaceRequired);
    void EventHandler(const prmEventButton & payload);
    QLabel * Widget;
signals:
    void SetValueSignal(QString);
};

class CISST_EXPORT prmQtWidgetEventButtonsComponent: public QWidget, public mtsComponent
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    virtual void closeEvent(QCloseEvent *event);
    cmnNamedMap<prmQtWidgetEventButtonsComponent_ButtonData> ButtonsData;
    QGridLayout * GridLayout;
    unsigned int NumberOfColumns;

public:
    prmQtWidgetEventButtonsComponent(const std::string & name);

    void Startup(void);
    void SetNumberOfColumns(const size_t nbColumns);
    bool AddEventButton(const std::string & buttonName);
};

CMN_DECLARE_SERVICES_INSTANTIATION(prmQtWidgetEventButtonsComponent);

#endif // _prmQtWidgetEventButtonsComponent_h
