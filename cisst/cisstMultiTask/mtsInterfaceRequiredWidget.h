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

#ifndef _mtsInterfaceRequiredWidget_h
#define _mtsInterfaceRequiredWidget_h

#include <cisstMultiTask/mtsQtWidgetFunction.h>

#include <QWidget>
class QToolBox;

#include <cisstMultiTask/mtsInterfaceProvided.h>

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

class CISST_EXPORT mtsEventVoidHandler {
private:
    mtsQtWidgetFunction * Widget;

    mtsEventVoidHandler(mtsQtWidgetFunction * widget);

public:
    typedef void (mtsEventVoidHandler::*HandleFunctionType)();

    void HandleEvent(void);
    inline static HandleFunctionType HandleFunction(void);
    static mtsEventVoidHandler * CreateEventVoidHandler(mtsQtWidgetFunction * widget);
};


inline mtsEventVoidHandler::HandleFunctionType mtsEventVoidHandler::HandleFunction()
{
    return 0; // &HandleEvent;
}


class mtsEventWriteHandler {
private:
    mtsQtWidgetFunction * Widget;

    mtsEventWriteHandler(mtsQtWidgetFunction * widget);

public:
    typedef void (mtsEventWriteHandler::*HandleFunctionType)(const mtsGenericObject& data);

    void HandleEvent(const mtsGenericObject & data);
    inline static HandleFunctionType HandleFunction(void);
    static mtsEventWriteHandler* CreateEventWriteHandler(mtsQtWidgetFunction * widget);
};


inline mtsEventWriteHandler::HandleFunctionType mtsEventWriteHandler::HandleFunction()
{
    return 0; // &HandleEvent;
}


class mtsInterfaceRequiredWidget: public QWidget
{

    Q_OBJECT;

public:
    mtsInterfaceRequiredWidget(mtsInterfaceProvided * interface,
                               mtsInterfaceRequired * executionInterface = 0);
    inline const mtsInterfaceProvided * GetInterfaceProvided(void) const;
    inline void Update(void);

private:
    mtsInterfaceProvided * Interface;
    mtsInterfaceRequired * ExecutionInterface;
    // QTabWidget * TabWidget;
    QToolBox * TabWidget;

    void UpdateUI(mtsInterfaceProvided & interface, mtsInterfaceRequired & executionInterface);

    template<typename functionType>
    void CreateFunctions(mtsInterfaceRequired & executionInterface,
                         const std::vector< std::string > & namesOfCommands,
                         functionType* (*createFunctionObjectCallback) (),
                         bool (mtsInterfaceRequired::*createFunctionCallback) (const std::string &, functionType &, mtsRequiredType)
                         );
    template<typename functionType>
    void CreateWidgets(mtsInterfaceRequired & executionInterface,
                       const std::vector< std::string > & namesOfFunctions,
                       functionType * (mtsInterfaceRequired::*getFunctionCallback) (const std::string&) const,
                       mtsQtWidgetFunction * (*createWidgetsCallback) (functionType&)
                       );

    static mtsFunctionVoid * CreateFunctionVoid(void);
    static mtsFunctionVoidReturn * CreateFunctionVoidReturn(void);
    static mtsFunctionWrite * CreateFunctionWrite(void);
    static mtsFunctionWriteReturn * CreateFunctionWriteReturn(void);
    static mtsFunctionRead * CreateFunctionRead(void);
    static mtsFunctionQualifiedRead * CreateFunctionQualifiedRead(void);
};


inline const mtsInterfaceProvided * mtsInterfaceRequiredWidget::GetInterfaceProvided(void) const
{
    return Interface;
}


inline void mtsInterfaceRequiredWidget::Update(void)
{
    UpdateUI(*Interface, *ExecutionInterface);
}


#endif // _mtsInterfaceRequiredWidget_h
