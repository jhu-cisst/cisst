/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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

#ifndef _mtsCommandWidget_h
#define _mtsCommandWidget_h

#include <map>

class mtsQtWidgetGenericObjectRead;
class mtsQtWidgetGenericObjectWrite;

#include <cisstMultiTask/mtsCommandBase.h>
#include <cisstMultiTask/mtsFunctionBase.h>
#include <cisstMultiTask/mtsGenericObject.h>

/// \todo replace by forward declarations for all Qt types
#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QFormLayout>
#include <QTime>

class mtsCommandWidget;

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

/* ! A button that allows the user to invoke Command::Execute(). */
class CISST_EXPORT mtsExecuteButton : public QPushButton {

    Q_OBJECT;

public:
    mtsExecuteButton();

public slots:
    void Disable();
    void Enable();
};

/* ! A set of controls that allow the user to toggle and configure periodic execution (execution of the command at a regular interval). */
class CISST_EXPORT mtsPeriodicExecutionWidget : public QWidget {

    Q_OBJECT;

private:
    QCheckBox * CheckBox;
    QDoubleSpinBox * SpinBox;
    QWidget * Container;
    mtsCommandWidget * CommandWidget;

private slots:
    void HandleStateChanged(int enabled);
    void HandleIntervalChanged(double newTime);

public:
    mtsPeriodicExecutionWidget(mtsCommandWidget * parent);
    bool ExecutePeriodically(void) const;
    double ExecutionInterval(void) const;
};

/* ! Output widget for all the information associated with an event.
   Currently, the only information which is displayed is the last
   time that the event was generated.
*/
class CISST_EXPORT mtsEventInformationWidget : public QWidget {

    Q_OBJECT;

private:
    QLabel* TimeSinceLastEvent;
    int SecondsSinceLastEvent;
    bool EventFired;
    QTime TimeOfLastEvent;
    int TimerID;

    void timerEvent(QTimerEvent* event);

public:
    mtsEventInformationWidget();
    void ResetTimeSinceLastEvent();
};

/* ! A widget that wraps a single mtsCommand, allowing the user to execute the command and view its output. */
class CISST_EXPORT mtsCommandWidget : public QWidget, public cmnGenericObject
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

 private:
    QLabel * Label;
    QFormLayout * Layout;

    // execution result
    QLabel * ExecutionResultLabel;
    mtsExecutionResult LastResult;
    void timerEvent(QTimerEvent * event);

protected:

    void SetExecutionResult(mtsExecutionResult result);

    mtsQtWidgetGenericObjectRead * ReadWidget;
    mtsQtWidgetGenericObjectWrite * WriteWidget;
    mtsGenericObject * ReadValue;
    mtsGenericObject* WriteValue;
    mtsExecuteButton * ExecuteButton;
    mtsPeriodicExecutionWidget * PeriodicExecution;
    mtsEventInformationWidget * EventInformation;

    int TimerID;
    bool HasTimer;

    /* ! Add an editor widget to the mtsCommandWidget. The editor widget can be used
       to provide the value for an argument for the mtsCommand.
       @param label The text to put in the QLabel associated with the widget.
       @param prototype An instance of the datatype that the widget should handle.
    */

    void SetWriteWidget(const std::string & label, const mtsGenericObject & prototype);
    /* ! Add a display widget to the mtsCommandWidget. The display widget can be used
       to show the result of executing an mtsCommand.
       @param label The text to put in the QLabel associated with the widget.
       @param prototype An instance of the datatype that the widget should handle.
    */

    void SetReadWidget(const std::string & label, const mtsGenericObject & prototype);
    /* ! Add a set of widgets that allow the user to execute an mtsCommand to the
       mtsCommandWidget.
       @param allowPeriodicExecution If true, add a PeriodicExecutionWidget.
    */

    void AddExecutionWidgets(bool allowPeriodicExecution = false);

    /* ! Add an EventInformationWidget to the mtsCommandWidget. */
    void AddEventWidgets();

public slots:
    virtual void Execute() = 0;

public:
    mtsCommandWidget();

    void SetTimer(int interval);
    void StopTimer(void);

    static mtsCommandWidget* CreateCommandVoidWidget(mtsFunctionVoid& command);
    static mtsCommandWidget* CreateCommandVoidReturnWidget(mtsFunctionVoidReturn& command);
    static mtsCommandWidget* CreateCommandWriteWidget(mtsFunctionWrite& command);
    static mtsCommandWidget* CreateCommandWriteReturnWidget(mtsFunctionWriteReturn& command);
    static mtsCommandWidget* CreateCommandReadWidget(mtsFunctionRead& command);
    static mtsCommandWidget* CreateCommandQualifiedReadWidget(mtsFunctionQualifiedRead& command);
    static mtsCommandWidget* CreateEventVoidWidget();
    static mtsCommandWidget* CreateEventWriteWidget();
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsCommandWidget)

#endif //ifndef _mtsCommandWidget_h
