/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

#include <cisstCommon.h>
#include <cisstOSAbstraction.h>
#include <cisstMultiTask.h>

#include "appTask.h"
#include "monitorTask.h"
#include "robotLowLevel.h"

using namespace std;

int main(void)
{
    // Log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::GetMultiplexer()->AddChannel(cout, CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::HaltDefaultLog();
    cmnLogger::ResumeDefaultLog(CMN_LOG_LOD_VERY_VERBOSE);
    // add a log per thread
    osaThreadedLogFile threadedLog("example3-");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("appTask", CMN_LOG_LOD_VERY_VERBOSE);

    // create our tasks
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    robotLowLevel * robotTask = new robotLowLevel("RobotControl", 50 * cmn_ms);
    monitorTask * monitor = new monitorTask("Monitor", 20 * cmn_ms);
    appTask * appTaskControl1 = new appTask("ControlRobot1", "Robot1", "Robot2", 50 * cmn_ms);
    appTask * appTaskControl2 = new appTask("ControlRobot2", "Robot2", "Robot1", 100 * cmn_ms);

    // create and add Component Viewer
    mtsComponentViewer *componentViewer = new mtsComponentViewer("ComponentViewer", 1.0*cmn_s);
    if (!taskManager->AddComponentWithControlService(componentViewer)) {
        CMN_LOG_INIT_ERROR << "Failed to add ComponentViewer" << std::endl;
        exit(1);
    }

    // add all tasks
    taskManager->AddComponent(robotTask);
    taskManager->AddComponent(monitor);
    taskManager->AddComponent(appTaskControl1);
    taskManager->AddComponent(appTaskControl2);
    // connect: name of user, resource port, name of resource, resource interface
    taskManager->Connect("Monitor", "Robot1",
                         "RobotControl", "Robot1Observer");
    taskManager->Connect("Monitor", "Robot2",
                         "RobotControl", "Robot2Observer");
    taskManager->Connect("ControlRobot1", "ControlledRobot",
                         "RobotControl", "Robot1");
    taskManager->Connect("ControlRobot1", "ObservedRobot",
                         "RobotControl", "Robot2Observer");
    taskManager->Connect("ControlRobot2", "ControlledRobot",
                         "RobotControl", "Robot2");
    taskManager->Connect("ControlRobot2", "ObservedRobot",
                         "RobotControl", "Robot1Observer");

    // display a graph of connections
    std::ofstream dotFile("example3.dot"); 
    taskManager->ToStreamDot(dotFile);
    dotFile.close();

    // collect all state data in csv file
    mtsCollectorState * collector =
        new mtsCollectorState("RobotControl",
                              robotTask->GetDefaultStateTableName(),
                              mtsCollectorBase::COLLECTOR_FILE_FORMAT_CSV);
    collector->AddSignal(); // all signals
    taskManager->AddComponent(collector);
    collector->Connect(); // collector knows what to connect to

    taskManager->CreateAll();
    taskManager->StartAll();

    collector->StartCollection(0.0 * cmn_s); // start now

    // Loop until both tasks are closed
    while (!(appTaskControl1->IsTerminated() && appTaskControl2->IsTerminated())) {
        osaSleep(0.1 * cmn_s); // in seconds
    }
    taskManager->KillAll();

    /*
    while (!robotTask->IsTerminated()) osaTime::Sleep(PeriodRobot);
    while (!monitor->IsTerminated()) osaTime::Sleep(PeriodRobot);
    while (!appTaskControl1->IsTerminated()) osaTime::Sleep(PeriodRobot);
    while (!appTaskControl2->IsTerminated()) osaTime::Sleep(PeriodRobot);
    */
    taskManager->Cleanup();
    return 0;
}

/*
  Author(s):  Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/
