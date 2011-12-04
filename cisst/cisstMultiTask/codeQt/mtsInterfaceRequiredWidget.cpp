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

#include <cisstMultiTask/mtsInterfaceRequiredWidget.h>

#include <cisstMultiTask/mtsInterfaceRequired.h>

#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsFunctionVoidReturn.h>
#include <cisstMultiTask/mtsFunctionWrite.h>
#include <cisstMultiTask/mtsFunctionWriteReturn.h>
#include <cisstMultiTask/mtsFunctionRead.h>
#include <cisstMultiTask/mtsFunctionQualifiedRead.h>

#include <QVBoxLayout>
// #include <QTabWidget>
#include <QToolBox>
 
mtsEventVoidHandler::mtsEventVoidHandler(mtsQtWidgetFunction * widget)
    : Widget(widget)
{

}

void mtsEventVoidHandler::HandleEvent(void)
{
    Widget->ExecuteIfEnabled();
}

mtsEventVoidHandler* mtsEventVoidHandler::CreateEventVoidHandler(mtsQtWidgetFunction * widget)
{
    return new mtsEventVoidHandler(widget);
}

mtsEventWriteHandler::mtsEventWriteHandler(mtsQtWidgetFunction * widget)
    : Widget(widget)
{

}

void mtsEventWriteHandler::HandleEvent(const mtsGenericObject & data)
{
    Widget->ExecuteIfEnabled();
}

mtsEventWriteHandler* mtsEventWriteHandler::CreateEventWriteHandler(mtsQtWidgetFunction * widget)
{
    return new mtsEventWriteHandler(widget);
}

mtsInterfaceRequiredWidget::mtsInterfaceRequiredWidget(mtsInterfaceProvided * interface,
                                                       mtsInterfaceRequired * executionInterface):
    QWidget(),
    Interface(interface),
    ExecutionInterface(executionInterface)
{
    // TabWidget = new QTabWidget();
    TabWidget = new QToolBox();
    // TabWidget->setMovable(true);
    QLayout* layout = new QVBoxLayout();
    layout->addWidget(TabWidget);
    setLayout(layout);
    UpdateUI(*Interface, *ExecutionInterface);
}


void mtsInterfaceRequiredWidget::UpdateUI(mtsInterfaceProvided & interface, mtsInterfaceRequired & executionInterface)
{
    mtsInterfaceProvided & userInterface = interface;
    //TODO: swap layout
    bool (mtsInterfaceRequired::*createFunctionVoidCallback) (const std::string&, mtsFunctionVoid&, mtsRequiredType) = &mtsInterfaceRequired::AddFunction;
    CreateFunctions<mtsFunctionVoid>(
                                     executionInterface,
                                     userInterface.GetNamesOfCommandsVoid(),
                                     &CreateFunctionVoid,
                                     createFunctionVoidCallback
                                     );
    bool (mtsInterfaceRequired::*createFunctionVoidReturnCallback) (const std::string&, mtsFunctionVoidReturn&, mtsRequiredType) = &mtsInterfaceRequired::AddFunction;
    CreateFunctions<mtsFunctionVoidReturn>(
                                           executionInterface,
                                           userInterface.GetNamesOfCommandsVoidReturn(),
                                           &CreateFunctionVoidReturn,
                                           createFunctionVoidReturnCallback
                                           );
    bool (mtsInterfaceRequired::*createFunctionWriteCallback) (const std::string&, mtsFunctionWrite&, mtsRequiredType) = &mtsInterfaceRequired::AddFunction;
    CreateFunctions<mtsFunctionWrite>(
                                      executionInterface,
                                      userInterface.GetNamesOfCommandsWrite(),
                                      &CreateFunctionWrite,
                                      createFunctionWriteCallback
                                      );
    bool (mtsInterfaceRequired::*createFunctionWriteReturnCallback) (const std::string&, mtsFunctionWriteReturn&, mtsRequiredType) = &mtsInterfaceRequired::AddFunction;
    CreateFunctions<mtsFunctionWriteReturn>(
                                            executionInterface,
                                            userInterface.GetNamesOfCommandsWriteReturn(),
                                            &CreateFunctionWriteReturn,
                                            createFunctionWriteReturnCallback
                                            );
    bool (mtsInterfaceRequired::*createFunctionReadCallback) (const std::string&, mtsFunctionRead&, mtsRequiredType) = &mtsInterfaceRequired::AddFunction;
    CreateFunctions<mtsFunctionRead>(
                                     executionInterface,
                                     userInterface.GetNamesOfCommandsRead(),
                                     &CreateFunctionRead,
                                     createFunctionReadCallback
                                     );
    bool (mtsInterfaceRequired::*createFunctionQualifiedReadCallback) (const std::string&, mtsFunctionQualifiedRead&, mtsRequiredType) = &mtsInterfaceRequired::AddFunction;
    CreateFunctions<mtsFunctionQualifiedRead>(
                                              executionInterface,
                                              userInterface.GetNamesOfCommandsQualifiedRead(),
                                              &CreateFunctionQualifiedRead,
                                              createFunctionQualifiedReadCallback
                                              );

    {
        std::vector< std::string > eventNames = userInterface.GetNamesOfEventsVoid();
        std::vector< std::string >::const_iterator i = eventNames.begin();
        std::vector< std::string >::const_iterator e = eventNames.end();
        mtsEventVoidHandler * eventHandler;
        mtsCommandVoid * command;
        for ( ; i < e; ++i) {
            eventHandler = mtsEventVoidHandler::CreateEventVoidHandler(mtsQtWidgetFunction::CreateEventVoidWidget());
            command = executionInterface.AddEventHandlerVoid(mtsEventVoidHandler::HandleFunction(), eventHandler, *i, MTS_INTERFACE_EVENT_POLICY);
        }
    }

    {
        std::vector< std::string > eventNames = userInterface.GetNamesOfEventsVoid();
        std::vector< std::string >::const_iterator i = eventNames.begin();
        std::vector< std::string >::const_iterator e = eventNames.end();
        mtsEventWriteHandler * eventHandler;
        mtsCommandWriteBase * command;
        for ( ; i < e; ++i) {
            eventHandler = mtsEventWriteHandler::CreateEventWriteHandler(mtsQtWidgetFunction::CreateEventWriteWidget());
            command = executionInterface.AddEventHandlerWriteGeneric(mtsEventWriteHandler::HandleFunction(), eventHandler, *i, MTS_INTERFACE_EVENT_POLICY);
        }
    }

    executionInterface.ConnectTo(&interface);

    CreateWidgets<mtsFunctionVoid>(
                                   executionInterface,
                                   executionInterface.GetNamesOfFunctionsVoid(),
                                   &mtsInterfaceRequired::GetFunctionVoid,
                                   &mtsQtWidgetFunction::CreateCommandVoidWidget
                                   );
    CreateWidgets<mtsFunctionVoidReturn>(
                                         executionInterface,
                                         executionInterface.GetNamesOfFunctionsVoidReturn(),
                                         &mtsInterfaceRequired::GetFunctionVoidReturn,
                                         &mtsQtWidgetFunction::CreateCommandVoidReturnWidget
                                         );
    CreateWidgets<mtsFunctionWrite>(
                                    executionInterface,
                                    executionInterface.GetNamesOfFunctionsWrite(),
                                    &mtsInterfaceRequired::GetFunctionWrite,
                                    &mtsQtWidgetFunction::CreateCommandWriteWidget
                                    );
    CreateWidgets<mtsFunctionWriteReturn>(
                                          executionInterface,
                                          executionInterface.GetNamesOfFunctionsWriteReturn(),
                                          &mtsInterfaceRequired::GetFunctionWriteReturn,
                                          &mtsQtWidgetFunction::CreateCommandWriteReturnWidget
                                          );
    CreateWidgets<mtsFunctionRead>(
                                   executionInterface,
                                   executionInterface.GetNamesOfFunctionsRead(),
                                   &mtsInterfaceRequired::GetFunctionRead,
                                   &mtsQtWidgetFunction::CreateCommandReadWidget
                                   );
    CreateWidgets<mtsFunctionQualifiedRead>(
                                            executionInterface,
                                            executionInterface.GetNamesOfFunctionsQualifiedRead(),
                                            &mtsInterfaceRequired::GetFunctionQualifiedRead,
                                            &mtsQtWidgetFunction::CreateCommandQualifiedReadWidget
                                            );
}

template<typename functionType>
void mtsInterfaceRequiredWidget::CreateFunctions(mtsInterfaceRequired& executionInterface,
                                                 const std::vector< std::string >& namesOfCommands,
                                                 functionType* (*createFunctionObjectCallback) (),
                                                 bool (mtsInterfaceRequired::*createFunctionCallback) (const std::string&, functionType&, mtsRequiredType)
                                                 )
{
    std::vector< std::string >::const_iterator i = namesOfCommands.begin();
    std::vector< std::string >::const_iterator e = namesOfCommands.end();
    functionType* function;
    for( ; i < e; ++i) {
        function = (*createFunctionObjectCallback)();
        (executionInterface.*createFunctionCallback)(*i, *function, MTS_REQUIRED);
    }
}

template<typename functionType>
void mtsInterfaceRequiredWidget::CreateWidgets(mtsInterfaceRequired& executionInterface,
                                               const std::vector< std::string >& namesOfFunctions,
                                               functionType* (mtsInterfaceRequired::*getFunctionCallback) (const std::string&) const,
                                               mtsQtWidgetFunction * (*createWidgetsCallback) (functionType&)
                                               )
{
    std::vector< std::string >::const_iterator i = namesOfFunctions.begin();
    std::vector< std::string >::const_iterator e = namesOfFunctions.end();
    functionType* function;
    for( ; i < e; ++i) {
        function = (executionInterface.*getFunctionCallback)(*i);
        TabWidget->addItem((*createWidgetsCallback)(*function), i->c_str());
    }
}

mtsFunctionVoid* mtsInterfaceRequiredWidget::CreateFunctionVoid()
{
    return new mtsFunctionVoid();
}

mtsFunctionVoidReturn* mtsInterfaceRequiredWidget::CreateFunctionVoidReturn()
{
    return new mtsFunctionVoidReturn();
}

mtsFunctionWrite* mtsInterfaceRequiredWidget::CreateFunctionWrite()
{
    return new mtsFunctionWrite();
}

mtsFunctionWriteReturn* mtsInterfaceRequiredWidget::CreateFunctionWriteReturn()
{
    return new mtsFunctionWriteReturn();
}

mtsFunctionRead* mtsInterfaceRequiredWidget::CreateFunctionRead()
{
    return new mtsFunctionRead();
}

mtsFunctionQualifiedRead* mtsInterfaceRequiredWidget::CreateFunctionQualifiedRead()
{
    return new mtsFunctionQualifiedRead();
}
