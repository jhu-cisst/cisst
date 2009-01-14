/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: userInterface.cpp,v 1.6 2009/01/10 05:02:51 pkaz Exp $ */

#include "userInterface.h"

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Value_Output.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Check_Button.H>

osaMutex userInterface::Mutex;

userInterface::userInterface(const std::string & robotName,
                             mtsDeviceInterface * interfacePointer):
    Ticks(0),
    Name(robotName),
    CloseRequested(false)
{
    // -1- Initialize the required interface. In this implementation, we are not
    // using a mailbox for events, so the events aren't queued. As a result, we
    // need to use a mutex in the event handlers (CallBackStarted and CallBackFinished).
    mtsRequiredInterface Robot("Robot");
    Robot.AddFunction("GetPositionJoint", GetPositionJoint);
    Robot.AddFunction("MovePositionJoint", MovePositionJoint);
    // false --> Event handlers are not queued
    Robot.AddEventHandlerVoid(&userInterface::CallBackStarted, this,
                              "MotionStarted", false);
    Robot.AddEventHandlerWrite(&userInterface::CallBackFinished, this,
                  "MotionFinished", PositionJointType(NB_JOINTS), false);

    // tell task or device that this thread will use it, will create a
    // mailbox if needed.  if this method is not called, call to
    // GetCommandXyz will likely fail
    interfacePointer->AllocateResourcesForCurrentThread();

    // -2- Connect to the device/task that provides the required resources
    Robot.ConnectTo(interfacePointer);
    Robot.BindCommandsAndEvents();

    // -3- Setup the user interface
    Window = new Fl_Double_Window(500, 400, Name.c_str());
    enum {row1 = 20, row2 = 60, row3 = 100, row4 = 140, row5 = 180, row6 = 220};
    enum {col1 = 20, col2 = 100, col3 = 200, col4 = 300, col5 = 400};
    // create labels to make the interface readable
    Fl_Output * label = new Fl_Output(col2, row1, 80, 20);
    label->box(FL_NO_BOX);
    label->value(Name.c_str());
    Fl_Output * positionLabel = new Fl_Output(col1, row2, 80, 20);
    positionLabel->box(FL_NO_BOX);
    positionLabel->value("Position");
    // data members
    Position1Window = new Fl_Value_Output(col2, row2, 80, 20);
    Position1Window->value(0.0);
    Position2Window = new Fl_Value_Output(col3, row2, 80, 20);
    Position2Window->value(0.0);
    // create UI to set goal
    Fl_Output* goalLabel = new Fl_Output(col1, row3, 80, 20);
    goalLabel->box(FL_NO_BOX);
    goalLabel->value("Goal");
    Goal1Window = new Fl_Value_Input(col2, row3, 80, 20);
    Goal1Window->value(0.0);
    Goal2Window = new Fl_Value_Input(col3, row3, 80, 20);
    Goal2Window->value(0.0);
    Fl_Button * move = new Fl_Button(col2, row4, 80, 20, "Move");
    move->callback((Fl_Callback*)CallBackMove, this);
    Moving = new Fl_Output(col3, row4, 20, 20);
    Moving->box(FL_NO_BOX);
    Moving->label("@||");
    // show time flying
    TicksWindow = new Fl_Value_Output(col2, row6, 80, 20, "Timestamp  ");
    TicksWindow->value(0);
    // close button
    Fl_Return_Button* close = new Fl_Return_Button(col3, row6, 75, 25, "Close");
    close->box(FL_THIN_UP_BOX);
    close->callback((Fl_Callback*)CallBackClose, this);
    Window->end();
    Window->show();
}

userInterface::~userInterface()
{}

void userInterface::Hide(void)
{
    Mutex.Lock();
    Window->hide();
    Mutex.Unlock();
}

void userInterface::CallBackStarted(void)
{
    // mutex is important as the callback method will be called from
    // the low level task thread
    Mutex.Lock();
    Moving->label("@>");
    Mutex.Unlock();
}

void userInterface::CallBackFinished(const PositionJointType & finalPosition)
{
    // mutex is important as the callback method will be called from
    // the low level task thread
    Mutex.Lock();
    Moving->label("@||");
    Position1Window->value(finalPosition[0]);
    Position2Window->value(finalPosition[1]);
    Mutex.Unlock();
}

void userInterface::Update(void)
{
    // mutex is used as the interface (as well as any data member of
    // this class) can be used by the current thread as well as the
    // low level task thread
    Mutex.Lock();
    GetPositionJoint(Position);
    Position1Window->value(Position[0]);
    Position2Window->value(Position[1]);
    TicksWindow->value(Ticks++);
    Fl::check(); // all the FTLK events
    Mutex.Unlock();
}

void userInterface::CallBackClose(Fl_Widget * CMN_UNUSED(widget),
                                  userInterface * object)
{
    // do not use mutex here, this is called by Fl::check which is
    // already mutex protected
    object->Window->hide();
    object->CloseRequested = true;
}

void userInterface::CallBackMove(Fl_Widget * CMN_UNUSED(widget),
                                 userInterface * object)
{
    // do not use mutex here, this is called by Fl::check which is
    // already mutex protected
    object->Position[0] = object->Goal1Window->value();
    object->Position[1] = object->Goal2Window->value();
    object->MovePositionJoint(object->Position);
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
