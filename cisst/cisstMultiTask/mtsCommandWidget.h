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

#include <cisstMultiTask/mtsArgumentWidget.h>

#include <map>

#include <cisstMultiTask/mtsCommandBase.h>
#include <cisstMultiTask/mtsFunctionBase.h>
#include <cisstMultiTask/mtsGenericObject.h>

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>
#include <QFormLayout>
#include <QTime>

class mtsCommandWidget;

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

class TypeInfoComparator {
public:
    inline bool operator()(const std::type_info* a, const std::type_info* b) const;
};


bool TypeInfoComparator::operator()(const std::type_info* a, const std::type_info* b) const
{
    return a->before(*b);
}

/* ! Output widget for the mtsExecutionResult returned by Command::Execute(). */
class CISST_EXPORT mtsExecutionResultWidget : public QWidget {

    Q_OBJECT;

private:
    QLabel* Label;

public slots:
    void SetValue(mtsExecutionResult result);

public:
    mtsExecutionResultWidget();
};


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
    QCheckBox* CheckBox;
    QSpinBox* SpinBox;
    QWidget* Container;
    mtsCommandWidget* CommandWidget;

private slots:
    void HandleStateChanged(int enabled);
    void HandleIntervalChanged(int newTime);

public:
    mtsPeriodicExecutionWidget(mtsCommandWidget* parent);
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
class CISST_EXPORT mtsCommandWidget : public QWidget {

    Q_OBJECT

	private:
    QLabel* Label;
    QFormLayout* Layout;

    void timerEvent(QTimerEvent * event);

    static bool Constructed;
    typedef std::map<const std::type_info*, ArgumentWidget* (*)(), TypeInfoComparator> TypeInfoMap;
    static TypeInfoMap InputWidgets;
    static TypeInfoMap OutputWidgets;

    static void CreateDefaultWidgetBindings();

protected:

    std::vector<ArgumentWidget*> Arguments;
    mtsExecutionResultWidget * ExecutionResult;
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

    void AddWriteArgument(const char* label, const mtsGenericObject& prototype);
    /* ! Add a display widget to the mtsCommandWidget. The display widget can be used
       to show the result of executing an mtsCommand.
       @param label The text to put in the QLabel associated with the widget.
       @param prototype An instance of the datatype that the widget should handle.
    */

    void AddReadArgument(const char* label, const mtsGenericObject& prototype);
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

    /* ! Set the widget used by mtsCommandWidget used to handle argument values of a certain type.
       @param type A pointer to the typeid of the type to be handled.
       @param createWidget A pointer to a function which creates widgets to handle the given type.
    */
    static void SetWidgetForType(const std::type_info* type, ArgumentWidget* (*createWidget)());

    static mtsCommandWidget* CreateCommandVoidWidget(mtsFunctionVoid& command);
    static mtsCommandWidget* CreateCommandVoidReturnWidget(mtsFunctionVoidReturn& command);
    static mtsCommandWidget* CreateCommandWriteWidget(mtsFunctionWrite& command);
    static mtsCommandWidget* CreateCommandWriteReturnWidget(mtsFunctionWriteReturn& command);
    static mtsCommandWidget* CreateCommandReadWidget(mtsFunctionRead& command);
    static mtsCommandWidget* CreateCommandQualifiedReadWidget(mtsFunctionQualifiedRead& command);
    static mtsCommandWidget* CreateEventVoidWidget();
    static mtsCommandWidget* CreateEventWriteWidget();
};

#endif //ifndef _mtsCommandWidget_h
