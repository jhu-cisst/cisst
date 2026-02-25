/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet, Ali Uneri, Peter Kazanzides
  Created on: 2009-10-22

  (C) Copyright 2009-2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <QApplication>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QWidget>

#include <cisstMultiTask/mtsInterfaceRequired.h>

#include "mainQtComponent.h"
#include "displayQtComponent.h"

CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(mainQtComponent, mtsComponent, std::string);

mainQtComponent::mainQtComponent(const std::string &name) : mtsComponent(name)
{
    // create a vertical widget for quit button and tabs
    mainWidget = new QWidget();
    mainWidget->setWindowTitle("Periodic Task Example");
    QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // create a widget with all the sine wave controllers
    QWidget * displayWidget = new QWidget();
    QGridLayout * displayLayout = new QGridLayout(displayWidget);
    displayLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(displayWidget);
    for (unsigned int i = 0; i < NumSineTasks; i++) {
        std::ostringstream index;
        index << i;
        std::string displayName("DISP" + index.str());
        display[i] = new displayQtComponent;
        displayLayout->addWidget(display[i]->GetWidget(), 1, i);

        // create the cisstMultiTask interface with commands and events
        mtsInterfaceRequired * interfaceRequired = AddInterfaceRequired(displayName);
        if (interfaceRequired) {
           interfaceRequired->AddFunction("GetData", display[i]->Generator.GetData);
           interfaceRequired->AddFunction("SetAmplitude", display[i]->Generator.SetAmplitude);
        }
    }

    // one large quit button under all tabs
    QPushButton * buttonQuit = new QPushButton("Quit", mainWidget);
    mainLayout->addWidget(buttonQuit);
    QObject::connect(buttonQuit, SIGNAL(clicked()),
                     QApplication::instance(), SLOT(quit()));
}

// should destroy dynamically allocated objects
mainQtComponent::~mainQtComponent()
{
}

void mainQtComponent::Startup(void)
{
    // run Qt user interface
    mainWidget->resize(NumSineTasks * 220, 360);
    mainWidget->show();
}
