/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2009-10-22

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

/*!
  \todo Define the widgets inline (done)
  \todo Define the widgets in an external class
  \todo Define the widget using Qt Designer
*/

#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>

#include <QtGui>

#include "sineTask.h"
#include "proxyQt.h"


int main(int argc, char *argv[])
{
    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);

    // add a log per thread
    osaThreadedLogFile threadedLog("QtPeriodicTask-");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);

    // set the log level of detail on select tasks
    cmnClassRegister::SetLoD("sineTask", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("proxyQt", CMN_LOG_LOD_VERY_VERBOSE);

    // create the tasks
    const double PeriodSine = 1.0 * cmn_ms;  // in milliseconds
    sineTask * sineTaskObject = new sineTask("SIN", PeriodSine);
    proxyQt * proxyQtObject = new proxyQt("DISP");

    // add the tasks to the task manager
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    taskManager->AddTask(sineTaskObject);
    taskManager->AddDevice(proxyQtObject);

    // connect the tasks, e.g. RequiredInterface -> ProvidedInterface
    taskManager->Connect("DISP", "DataGenerator",
                         "SIN", "MainInterface");

    // generate a nice tasks diagram
    std::ofstream dotFile("QtPeriodicTask.dot");
    taskManager->ToStreamDot(dotFile);
    dotFile.close();

    // create the tasks
    taskManager->CreateAll();

    // start the tasks
    taskManager->StartAll();

    // user interface in Qt
    // ------------------------------------------------------------------------
    QApplication application(argc, argv);

    // create the widgets
    QMainWindow mainWindow;
    QWidget centralWidget(&mainWindow);  // central widget contains other widgets
    QTimer updateTimer(&centralWidget);
    QProgressBar dataBar(&centralWidget);
    QDial amplitudeDial(&centralWidget);
    QLabel amplitudeLabel("Amplitude:", &centralWidget);
    QLabel amplitudeValue("1", &centralWidget);
    QLabel dataLabel("Data:", &centralWidget);
    QLabel dataValue("0.0", &centralWidget);
    QPushButton quitButton("Quit", &centralWidget);
    QMenu viewMenu("&View", &mainWindow);  // menu item for viewing actions
    QAction normalSize("Normal Size", &mainWindow);  // viewing actions
    QAction fullscreen("Fullscreen", &mainWindow);

    // configure the widgets
    updateTimer.start(20);  // in milliseconds (=50Hz), tells the gui to read in the values from other tasks
    dataBar.setOrientation(Qt::Vertical);
    dataBar.setRange(-1000, 1000);  // initial range of sine, double(-1.0, 1.0) corresponds to int(-1000, 1000)
    amplitudeDial.setNotchesVisible(true);  // cause it looks cooler...
    amplitudeDial.setRange(1, 100);
    amplitudeLabel.setAlignment(Qt::AlignRight);
    dataLabel.setAlignment(Qt::AlignRight);
    mainWindow.menuBar()->addMenu(&viewMenu);  // add the view menu to the menubar
    viewMenu.addAction(&normalSize);  // add actions to the menu
    viewMenu.addAction(&fullscreen);

    // connect Qt signals to slots
    QObject::connect(&normalSize, SIGNAL(triggered()),
                     &mainWindow, SLOT(showNormal()));
    QObject::connect(&fullscreen, SIGNAL(triggered()),
                     &mainWindow, SLOT(showFullScreen()));
    QObject::connect(&updateTimer, SIGNAL(timeout(void)),
                     proxyQtObject, SLOT(UpdateUI(void)));
    QObject::connect(proxyQtObject, SIGNAL(GetData(int)),
                     &dataBar, SLOT(setValue(int)));
    QObject::connect(proxyQtObject, SIGNAL(GetRange(int, int)),
                     &dataBar, SLOT(setRange(int, int)));
    QObject::connect(&amplitudeDial, SIGNAL(valueChanged(int)),
                     proxyQtObject, SLOT(SetAmplitude(int)));
    QObject::connect(&amplitudeDial, SIGNAL(valueChanged(int)),
                     &amplitudeValue, SLOT(setNum(int)));
    QObject::connect(proxyQtObject, SIGNAL(GetData(double)),
                     &dataValue, SLOT(setNum(double)));
    QObject::connect(&quitButton, SIGNAL(clicked()),
                     &mainWindow, SLOT(close()));

    // create a layout for the widgets
    QGridLayout centralLayout(&centralWidget);
    centralLayout.setRowStretch(0, 1);  // stretch the first row to take up as much space as possible
    centralLayout.setColumnStretch(2, 1);  // stretch the second column
    centralLayout.addWidget(&dataBar, 0, 0, 3, 1);
    centralLayout.addWidget(&amplitudeDial, 0, 1, 1, 2);
    centralLayout.addWidget(&amplitudeLabel, 1, 1, 1, 1);
    centralLayout.addWidget(&amplitudeValue, 1, 2, 1, 1);
    centralLayout.addWidget(&dataLabel, 2, 1, 1, 1);
    centralLayout.addWidget(&dataValue, 2, 2, 1, 1);
    centralLayout.addWidget(&quitButton, 3, 0, 1, 3);

    // configure the main window
    mainWindow.setCentralWidget(&centralWidget);
    mainWindow.setGeometry(100, 100, 500, 500);
    mainWindow.setWindowTitle("MultiTask Example 1");
    mainWindow.show();

    application.exec();
    // ------------------------------------------------------------------------

    // perform cleanup
    taskManager->KillAll();
    taskManager->Cleanup();

    return 0;
}
