/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2009-11-16

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnLogger.h>
#include <cisstCommon/cmnLoggerQtWidget.h>

#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>

int main(int argc, char *argv[])
{
    CMN_LOG_INIT_ERROR << "Test" << std::endl;

    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ALL);

    // create a Qt user interface
    QApplication application(argc, argv);

    // create a widget to control the logger
    QWidget * mainWidget = new QWidget();
    mainWidget->setWindowTitle("commonTutorial Logget Qt");
    QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);

    cmnLoggerQtWidget * loggerWidget = new cmnLoggerQtWidget(mainWidget);
    mainLayout->addWidget(loggerWidget->GetWidget());

    // add a quit buttom
    QPushButton * buttonQuit = new QPushButton("Quit", mainWidget);
    mainLayout->addWidget(buttonQuit);
    QObject::connect(buttonQuit, SIGNAL(clicked()),
                     QApplication::instance(), SLOT(quit()));

    // run Qt user interface
    mainWidget->show();
    application.exec();

    return 0;
}
