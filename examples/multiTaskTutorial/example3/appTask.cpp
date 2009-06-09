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
    mtsRequiredInterface *req = AddRequiredInterface("ControlledRobot");
    if (req) {
        req->AddFunction("GetPositionJoint", GetPositionJointControlled);
        req->AddFunction("MovePositionJoint", MovePositionJointControlled);
        req->AddEventHandlerWrite(&appTask::HandleMotionFinishedControlled, this,
                                  "MotionFinished", PositionJointType(NB_JOINTS));
        req->AddEventHandlerVoid(&appTask::HandleMotionStartedControlled, this,
                                  "MotionStarted");
    }
    req = AddRequiredInterface("ObservedRobot");
    if (req) {
        req->AddFunction("GetPositionJoint", GetPositionJointObserved);
    }
}

// Mutex for UI
static osaMutex myMutex;

void appTask::HandleMotionFinishedControlled(const PositionJointType &)
{
    myMutex.Lock();
    ui.ShowMoving(false);
    myMutex.Unlock();
}

void appTask::HandleMotionStartedControlled(void)
{
    myMutex.Lock();
    ui.ShowMoving(true);
    myMutex.Unlock();
}

void appTask::Startup(void) 
{
    Ticks = 0;
    ui.SetCloseHandler(Close);

    myMutex.Lock();
    ui.Show();
    myMutex.Unlock();
}

void appTask::Run(void)
{
    Ticks++;
    ProcessQueuedEvents();

    PositionJointType posControlled;
    posControlled.SetSize(NB_JOINTS);
    PositionJointType posObserved;
    posObserved.SetSize(NB_JOINTS);
    PositionJointType posDesired;
    posDesired.SetSize(NB_JOINTS);
    GetPositionJointControlled(posControlled);
    GetPositionJointObserved(posObserved);
    myMutex.Lock();
    if (ui.MoveControlledPressed) {
        ui.MoveControlledPressed = false;
        ui.GetGoalControlled(posDesired[0], posDesired[1]);
        MovePositionJointControlled(posDesired);
    }
    ui.Update(Ticks,
              posControlled[0], posControlled[1],
              posObserved[0], posObserved[1]);
    Fl::check();
    myMutex.Unlock();
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
