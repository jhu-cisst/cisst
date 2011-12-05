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

#include <cisstMultiTask/mtsQtWidgetInterfaceRequired.h>

#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsFunctionVoidReturn.h>
#include <cisstMultiTask/mtsFunctionWrite.h>
#include <cisstMultiTask/mtsFunctionWriteReturn.h>
#include <cisstMultiTask/mtsFunctionRead.h>
#include <cisstMultiTask/mtsFunctionQualifiedRead.h>

#include <QVBoxLayout>
#include <QToolBox>


mtsQtWidgetInterfaceRequired::mtsQtWidgetInterfaceRequired(mtsInterfaceProvided * interface,
                                                           mtsInterfaceRequired * executionInterface):
    QWidget(),
    ExecutionInterface(executionInterface)
{
    FunctionsWidget = new QToolBox();
    QLayout* layout = new QVBoxLayout();
    layout->addWidget(FunctionsWidget);
    setLayout(layout);
    CreateWidgets(*interface, *ExecutionInterface);
}


void mtsQtWidgetInterfaceRequired::CreateWidgets(mtsInterfaceProvided & interface, mtsInterfaceRequired & executionInterface)
{
    typedef std::vector<std::string> NamesVector;
    NamesVector functionNames;
    NamesVector::const_iterator functionName;
    NamesVector::const_iterator end;
    std::string name;
    mtsQtWidgetFunction * functionWidget;
    typedef std::list<mtsQtWidgetFunction *> FunctionWidgetsList;
    FunctionWidgetsList functionWidgets;

    // void functions
    mtsFunctionVoid * functionVoid;
    functionNames = interface.GetNamesOfCommandsVoid();
    end = functionNames.end();
    for (functionName = functionNames.begin(); functionName != end; functionName++) {
        name = *functionName;
        functionVoid = new mtsFunctionVoid;
        executionInterface.AddFunction(name, *functionVoid, MTS_REQUIRED);
        functionWidget = new mtsQtWidgetFunctionVoid(functionVoid);
        FunctionsWidget->addItem(functionWidget, name.c_str());
        functionWidgets.push_back(functionWidget);
    }

    // void return functions
    mtsFunctionVoidReturn * functionVoidReturn;
    functionNames = interface.GetNamesOfCommandsVoidReturn();
    end = functionNames.end();
    for (functionName = functionNames.begin(); functionName != end; functionName++) {
        name = *functionName;
        functionVoidReturn = new mtsFunctionVoidReturn;
        executionInterface.AddFunction(name, *functionVoidReturn, MTS_REQUIRED);
        functionWidget = new mtsQtWidgetFunctionVoidReturn(functionVoidReturn);
        FunctionsWidget->addItem(functionWidget, name.c_str());
        functionWidgets.push_back(functionWidget);
    }

    // write functions
    mtsFunctionWrite * functionWrite;
    functionNames = interface.GetNamesOfCommandsWrite();
    end = functionNames.end();
    for (functionName = functionNames.begin(); functionName != end; functionName++) {
        name = *functionName;
        functionWrite = new mtsFunctionWrite;
        executionInterface.AddFunction(name, *functionWrite, MTS_REQUIRED);
        functionWidget = new mtsQtWidgetFunctionWrite(functionWrite);
        FunctionsWidget->addItem(functionWidget, name.c_str());
        functionWidgets.push_back(functionWidget);
    }

    // write return functions
    mtsFunctionWriteReturn * functionWriteReturn;
    functionNames = interface.GetNamesOfCommandsWriteReturn();
    end = functionNames.end();
    for (functionName = functionNames.begin(); functionName != end; functionName++) {
        name = *functionName;
        functionWriteReturn = new mtsFunctionWriteReturn;
        executionInterface.AddFunction(name, *functionWriteReturn, MTS_REQUIRED);
        functionWidget = new mtsQtWidgetFunctionWriteReturn(functionWriteReturn);
        FunctionsWidget->addItem(functionWidget, name.c_str());
        functionWidgets.push_back(functionWidget);
    }

    // read functions
    mtsFunctionRead * functionRead;
    functionNames = interface.GetNamesOfCommandsRead();
    end = functionNames.end();
    for (functionName = functionNames.begin(); functionName != end; functionName++) {
        name = *functionName;
        functionRead = new mtsFunctionRead;
        executionInterface.AddFunction(name, *functionRead, MTS_REQUIRED);
        functionWidget = new mtsQtWidgetFunctionRead(functionRead);
        FunctionsWidget->addItem(functionWidget, name.c_str());
        functionWidgets.push_back(functionWidget);
    }

    // qualified read functions
    mtsFunctionQualifiedRead * functionQualifiedRead;
    functionNames = interface.GetNamesOfCommandsQualifiedRead();
    end = functionNames.end();
    for (functionName = functionNames.begin(); functionName != end; functionName++) {
        name = *functionName;
        functionQualifiedRead = new mtsFunctionQualifiedRead;
        executionInterface.AddFunction(name, *functionQualifiedRead, MTS_REQUIRED);
        functionWidget = new mtsQtWidgetFunctionQualifiedRead(functionQualifiedRead);
        FunctionsWidget->addItem(functionWidget, name.c_str());
        functionWidgets.push_back(functionWidget);
    }


    // connect all
    executionInterface.ConnectTo(&interface);

    // refresh all widgets based on argument prototypes
    FunctionWidgetsList::iterator widgetIterator = functionWidgets.begin();
    FunctionWidgetsList::iterator widgetIteratorEnd = functionWidgets.end();
    for (; widgetIterator != widgetIteratorEnd; ++widgetIterator) {
        (*widgetIterator)->CreateArgumentsWidgets();
    }
}
