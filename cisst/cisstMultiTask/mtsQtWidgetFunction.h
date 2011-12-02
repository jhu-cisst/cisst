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

#ifndef _mtsQtWidgetFunction_h
#define _mtsQtWidgetFunction_h

#include <map>

class mtsQtWidgetGenericObjectRead;
class mtsQtWidgetGenericObjectWrite;

#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsFunctionVoidReturn.h>
#include <cisstMultiTask/mtsFunctionWrite.h>
#include <cisstMultiTask/mtsFunctionWriteReturn.h>
#include <cisstMultiTask/mtsFunctionRead.h>
#include <cisstMultiTask/mtsFunctionQualifiedRead.h>

#include <cisstMultiTask/mtsGenericObject.h>

#include <QWidget>
#include <QTime>

class QLabel;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QFormLayout;
class QDoubleSpinBox;

class mtsQtWidgetFunction;

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

/* ! Output widget for all the information associated with an event.
   Currently, the only information which is displayed is the last
   time that the event was generated.
*/
class CISST_EXPORT mtsEventInformationWidget: public QWidget {

    Q_OBJECT;

private:
    QLabel* TimeSinceLastEvent;
    int SecondsSinceLastEvent;
    bool EventFired;
    QTime TimeOfLastEvent;
    int TimerID;

    void timerEvent(QTimerEvent* event);

public:
    mtsEventInformationWidget(void);
    void ResetTimeSinceLastEvent(void);
};

/* ! A widget that wraps a single mtsCommand, allowing the user to execute the command and view its output. */
class CISST_EXPORT mtsQtWidgetFunction : public QWidget, public cmnGenericObject
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

 private:
    QLabel * Label;
    QVBoxLayout * MainLayout;
    QHBoxLayout * ControlLayout;
    QFormLayout * ResultLayout;

    // periodic execution
    QDoubleSpinBox * PeriodSpinBox;
   
    // execution result
    QLabel * ExecutionResultLabel;
    mtsExecutionResult LastResult;
    void timerEvent(QTimerEvent * event);

 protected:

    QPushButton * ExecuteButton;

    void SetExecutionResult(mtsExecutionResult result);

    mtsQtWidgetGenericObjectRead * ReadWidget;
    mtsQtWidgetGenericObjectWrite * WriteWidget;
    mtsGenericObject * ReadValue;
    mtsGenericObject * WriteValue;
    mtsEventInformationWidget * EventInformation;

    int TimerID;
    bool HasTimer;
    bool Enabled;

    /* ! Add an editor widget to the mtsQtWidgetFunction. The editor widget can be used
       to provide the value for an argument for the mtsCommand.
       @param label The text to put in the QLabel associated with the widget.
       @param prototype An instance of the datatype that the widget should handle.
    */

    void SetWriteWidget(const std::string & label, const mtsGenericObject & prototype);
    /* ! Add a display widget to the mtsQtWidgetFunction. The display widget can be used
       to show the result of executing an mtsCommand.
       @param label The text to put in the QLabel associated with the widget.
       @param prototype An instance of the datatype that the widget should handle.
    */

    void SetReadWidget(const std::string & label, const mtsGenericObject & prototype);
    /* ! Add a set of widgets that allow the user to execute an mtsCommand to the
       mtsQtWidgetFunction.
       @param allowPeriodicExecution If true, add a PeriodicExecutionWidget.
    */

    /* ! Add an EventInformationWidget to the mtsQtWidgetFunction. */
    void AddEventWidgets(void);

    virtual void Execute(void) = 0;

public slots:
    void ExecuteIfEnabled(void);
    void StopPeriodicExecution(void);
    void HandleIntervalChanged(double newInterval);

public:
    mtsQtWidgetFunction(void);

    void SetTimer(int interval);
    void StopTimer(void);

    static mtsQtWidgetFunction * CreateCommandVoidWidget(mtsFunctionVoid & command);
    static mtsQtWidgetFunction * CreateCommandVoidReturnWidget(mtsFunctionVoidReturn & command);
    static mtsQtWidgetFunction * CreateCommandWriteWidget(mtsFunctionWrite & command);
    static mtsQtWidgetFunction * CreateCommandWriteReturnWidget(mtsFunctionWriteReturn & command);
    static mtsQtWidgetFunction * CreateCommandReadWidget(mtsFunctionRead & command);
    static mtsQtWidgetFunction * CreateCommandQualifiedReadWidget(mtsFunctionQualifiedRead & command);
    static mtsQtWidgetFunction * CreateEventVoidWidget(void);
    static mtsQtWidgetFunction * CreateEventWriteWidget(void);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsQtWidgetFunction)



class CISST_EXPORT mtsQtWidgetFunctionVoid: public mtsQtWidgetFunction
{
    Q_OBJECT;
 private:
    mtsFunctionVoid Function;
 public slots:
    virtual void Execute(void);
 public:
    explicit mtsQtWidgetFunctionVoid(mtsFunctionVoid & function);
};


class CISST_EXPORT mtsQtWidgetFunctionVoidReturn: public mtsQtWidgetFunction
{
    Q_OBJECT;
 private:
    mtsFunctionVoidReturn Function;
 public slots:
    virtual void Execute(void);
 public:
    explicit mtsQtWidgetFunctionVoidReturn(mtsFunctionVoidReturn & function);
};


class CISST_EXPORT mtsQtWidgetFunctionWrite: public mtsQtWidgetFunction
{
    Q_OBJECT;
 private:
    mtsFunctionWrite Function;
 public slots:
    virtual void Execute(void);
 public:
    explicit mtsQtWidgetFunctionWrite(mtsFunctionWrite & function);
};


class CISST_EXPORT mtsQtWidgetFunctionWriteReturn: public mtsQtWidgetFunction
{
    Q_OBJECT;
 private:
    mtsFunctionWriteReturn Function;
 public slots:
    virtual void Execute(void);
 public:
    explicit mtsQtWidgetFunctionWriteReturn(mtsFunctionWriteReturn & function);
};


class CISST_EXPORT mtsQtWidgetFunctionRead : public mtsQtWidgetFunction
{
    Q_OBJECT;
 private:
    mtsFunctionRead Function;
 public slots:
    virtual void Execute(void);
 public:
    explicit mtsQtWidgetFunctionRead(mtsFunctionRead & function);
};


class CISST_EXPORT mtsQtWidgetFunctionQualifiedRead: public mtsQtWidgetFunction
{
    Q_OBJECT;
 private:
    mtsFunctionQualifiedRead Function;
 public slots:
    virtual void Execute(void);
 public:
    explicit mtsQtWidgetFunctionQualifiedRead(mtsFunctionQualifiedRead & function);
};


class CISST_EXPORT CommandEventVoidWidget: public mtsQtWidgetFunction
{
    Q_OBJECT;
 public slots:
    virtual void Execute(void);
 public:
    CommandEventVoidWidget(void);
};


class CommandEventWriteWidget: public mtsQtWidgetFunction
{
    Q_OBJECT;
public slots:
    virtual void Execute(void);
public:
    CommandEventWriteWidget(void);
};


#endif // _mtsQtWidgetFunction_h
