/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

#include <math.h>
#include "appTask.h"

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
                                  "MotionFinished");
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

void appTask::HandleMotionFinishedControlled(const PositionJointType & position)
{ 
    myMutex.Lock();
    ui.ShowMoving(false);
    CMN_LOG_RUN_VERBOSE << "Event motion finished at position: " << position << std::endl;
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
    PositionControlled.ReconstructFrom(*(GetPositionJointControlled.GetArgumentPrototype()));
    PositionObserved.ReconstructFrom(*(GetPositionJointObserved.GetArgumentPrototype()));
    PositionDesired.ReconstructFrom(*(MovePositionJointControlled.GetArgumentPrototype()));
    myMutex.Lock();
    ui.Show();
    myMutex.Unlock();
}

void appTask::Run(void)
{
    Ticks++;
    ProcessQueuedEvents();

    GetPositionJointControlled(PositionControlled);
    GetPositionJointObserved(PositionObserved);

    myMutex.Lock();
    if (ui.MoveControlledPressed) {
        ui.MoveControlledPressed = false;
        ui.GetGoalControlled(PositionDesired[0], PositionDesired[1]);
        MovePositionJointControlled(PositionDesired);
    }
    ui.Update(Ticks,
              PositionControlled[0], PositionControlled[1],
              PositionObserved[0], PositionObserved[1]);
    Fl::check();
    myMutex.Unlock();
}

void appTask::Close(mtsTask * task)
{
    appTask* myTask = dynamic_cast<appTask*>(task);
    CMN_LOG_INIT_VERBOSE << "Closing task " << myTask->GetName() << std::endl;
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
