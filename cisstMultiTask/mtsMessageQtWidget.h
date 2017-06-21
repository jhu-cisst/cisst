/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2013-05-17

  (C) Copyright 2013-2017 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _mtsMessageQtWidget_h
#define _mtsMessageQtWidget_h

#include <cisstMultiTask/mtsComponent.h>

#include <QTextEdit>

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

// Widget without the component, can be included in another widget
class CISST_EXPORT mtsMessageQtWidget: public QTextEdit
{
    Q_OBJECT;

public:
    mtsMessageQtWidget(void);
    inline virtual ~mtsMessageQtWidget() {}

    void SetInterfaceRequired(mtsInterfaceRequired * interfaceRequired);
    void setupUi(void);

signals:
    void SignalAppendMessage(QString);
    void SignalSetColor(QColor);
    void SignalClear();

private slots:
    void SlotTextChanged(void);

protected:
    virtual void keyPressEvent(QKeyEvent * event);
    void ErrorEventHandler(const mtsMessage & message);
    void WarningEventHandler(const mtsMessage & message);
    void StatusEventHandler(const mtsMessage & message);

    int Tag;
};

// Widget with a component, can be used directly with cisstMultiTask component manager
class CISST_EXPORT mtsMessageQtWidgetComponent: public mtsMessageQtWidget, public mtsComponent
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION_ONEARG, CMN_LOG_ALLOW_DEFAULT);

public:
    mtsMessageQtWidgetComponent(const std::string & componentName);
    ~mtsMessageQtWidgetComponent() {}

    inline void Configure(const std::string & CMN_UNUSED(filename) = "") {};
    inline void Startup(void) {};
    inline void Cleanup(void) {};
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsMessageQtWidgetComponent);

#endif // _mtsMessageQtWidget_h
