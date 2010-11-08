/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

#include <math.h>
#include "appTask.h"
#include "robotLowLevel.h"

CMN_IMPLEMENT_SERVICES(appTask);

appTask::appTask(const std::string & taskName,
                 const std::string & controlledRobot,
                 const std::string & observedRobot,
                 double period):
    mtsTaskPeriodic(taskName, period, false, 5000),
    ExitFlag(false),
    ui(controlledRobot, observedRobot, this)
{
}

// should use osaSemaphore instead.
static bool mySemaphoreLocked = false;

void appTask::Startup(void) 
{
    Ticks = 0;
    ui.SetCloseHandler(Close);

    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    controlledTask = dynamic_cast<robotLowLevel*>(taskManager->GetComponent(ui.GetControlledName()));
    if (!controlledTask)
        CMN_LOG_INIT_ERROR << "Controlled task not a robot!" << std::endl;
    observedTask = dynamic_cast<robotLowLevel*>(taskManager->GetComponent(ui.GetObservedName()));
    if (!observedTask)
        CMN_LOG_INIT_ERROR << "Observed task not a robot!" << std::endl;

    ui.Show();
}

void appTask::Run(void)
{
    Ticks++;

    vct6 posControlled, posObserved;

    if (controlledTask && observedTask) {    
       posControlled = controlledTask->GetPositionJoint();
       posObserved = observedTask->GetPositionJoint();
       if (ui.MoveControlledPressed) {
           ui.MoveControlledPressed = false;
           controlledTask->MovePositionJoint(vct6(ui.GetGoalControlled(),0,0,0,0,0));
       }
       if (!mySemaphoreLocked) {
           mySemaphoreLocked = true;
           ui.Update(Ticks, posControlled[0], posObserved[0]);
           Fl::check();
           mySemaphoreLocked = false;
       }
    }
}

void appTask::Close(mtsTask * task)
{
    appTask* myTask = dynamic_cast<appTask*>(task);
    CMN_LOG_RUN_ERROR << "Closing task " << myTask->GetName() << std::endl;
    myTask->ui.Hide();
    myTask->ExitFlag = true;
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
