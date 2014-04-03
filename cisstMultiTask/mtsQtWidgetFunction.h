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
#include <QFrame>
#include <QTime>

class QLabel;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QFormLayout;
class QDoubleSpinBox;
class QToolButton;

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

/* ! A widget that wraps a single mtsCommand, allowing the user to execute the command and view its output. */
class CISST_EXPORT mtsQtWidgetFunction: public QWidget, public cmnGenericObject
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
    void DisableExecute(void);

    mtsQtWidgetGenericObjectRead * ReadWidget;
    mtsQtWidgetGenericObjectWrite * WriteWidget;
    mtsGenericObject * ReadValue;
    mtsGenericObject * WriteValue;

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

    /* ! Add an EventInformationWidget to the mtsQtWidgetFunction. */
    void AddEventWidgets(void);

    virtual void Execute(void) = 0;

 public slots:
    void ExecuteIfEnabled(void);
    void StopPeriodicExecution(void);
    void HandleIntervalChanged(double newInterval);

 public:
    mtsQtWidgetFunction(void);
    virtual void CreateArgumentsWidgets(void) = 0;
    void SetTimer(int interval);
    void StopTimer(void);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsQtWidgetFunction)



class CISST_EXPORT mtsQtWidgetFunctionVoid: public mtsQtWidgetFunction
{
    Q_OBJECT;
 private:
    mtsFunctionVoid * Function;
 public slots:
    virtual void Execute(void);
 public:
    explicit mtsQtWidgetFunctionVoid(mtsFunctionVoid * function);
    void CreateArgumentsWidgets(void);
};


class CISST_EXPORT mtsQtWidgetFunctionVoidReturn: public mtsQtWidgetFunction
{
    Q_OBJECT;
 private:
    mtsFunctionVoidReturn * Function;
 public slots:
    virtual void Execute(void);
 public:
    explicit mtsQtWidgetFunctionVoidReturn(mtsFunctionVoidReturn * function);
    void CreateArgumentsWidgets(void);
};


class CISST_EXPORT mtsQtWidgetFunctionWrite: public mtsQtWidgetFunction
{
    Q_OBJECT;
 private:
    mtsFunctionWrite * Function;
 public slots:
    virtual void Execute(void);
 public:
    explicit mtsQtWidgetFunctionWrite(mtsFunctionWrite * function);
    void CreateArgumentsWidgets(void);
};


class CISST_EXPORT mtsQtWidgetFunctionWriteReturn: public mtsQtWidgetFunction
{
    Q_OBJECT;
 private:
    mtsFunctionWriteReturn * Function;
 public slots:
    virtual void Execute(void);
 public:
    explicit mtsQtWidgetFunctionWriteReturn(mtsFunctionWriteReturn * function);
    void CreateArgumentsWidgets(void);
};


class CISST_EXPORT mtsQtWidgetFunctionRead : public mtsQtWidgetFunction
{
    Q_OBJECT;
 private:
    mtsFunctionRead * Function;
 public slots:
    virtual void Execute(void);
 public:
    explicit mtsQtWidgetFunctionRead(mtsFunctionRead * function);
    void CreateArgumentsWidgets(void);
};


class CISST_EXPORT mtsQtWidgetFunctionQualifiedRead: public mtsQtWidgetFunction
{
    Q_OBJECT;
 private:
    mtsFunctionQualifiedRead * Function;
 public slots:
    virtual void Execute(void);
 public:
    explicit mtsQtWidgetFunctionQualifiedRead(mtsFunctionQualifiedRead * function);
    void CreateArgumentsWidgets(void);
};


class CISST_EXPORT mtsQtFunctionContainerWidget: public QFrame
{
    Q_OBJECT;
private:
    QWidget * TitleBar;
    QLabel * TitleLabel;
    QToolButton * ToggleButton;
    QWidget * FunctionWidgetContainer;

private slots:
    void ToggleCollapsed();

public:
    mtsQtFunctionContainerWidget();
    void SetFunctionWidget(QWidget * widget, const QString & name = "");
};


class CISST_EXPORT mtsQtFunctionListContainerWidget: public QWidget
{
    Q_OBJECT;
public:
    mtsQtFunctionListContainerWidget();
    void addItem(QWidget * widget, const QString & name);
};


#endif // _mtsQtWidgetFunction_h
