/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

#include "appTaskUI.h"

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Value_Output.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Check_Button.H>

osaMutex AppTaskUI::Mutex;

AppTaskUI::AppTaskUI(const std::string & controlledRobot,
                     const std::string & observedRobot,
                     mtsTask * task):
    Task(task),
    ControlledName(controlledRobot),
    ObservedName(observedRobot),
    MoveControlledPressed(false)
{
    Mutex.Lock();
    Window = new Fl_Double_Window(500, 400, ControlledName.c_str());
    enum {row1 = 20, row2 = 60, row3 = 100, row4 = 140, row5 = 180, row6 = 220};
    enum {col1 = 20, col2 = 100, col3 = 200, col4 = 300, col5 = 400};
    // create labels to make the interface readable
    Fl_Output * controlledLabel = new Fl_Output(col2, row1, 80, 20);
    controlledLabel->box(FL_NO_BOX);
    controlledLabel->value(ControlledName.c_str());
    Fl_Output * observedLabel = new Fl_Output(col4, row1, 80, 20);
    observedLabel->box(FL_NO_BOX);
    observedLabel->value(ObservedName.c_str());
    Fl_Output * positionLabel = new Fl_Output(col1, row2, 80, 20);
    positionLabel->box(FL_NO_BOX);
    positionLabel->value("Position");
    // data members
    PositionControlled1 = new Fl_Value_Output(col2, row2, 80, 20);
    PositionControlled1->value(0.0);
    PositionControlled2 = new Fl_Value_Output(col3, row2, 80, 20);
    PositionControlled2->value(0.0);
    PositionObserved1 = new Fl_Value_Output(col4, row2, 80, 20);
    PositionObserved1->value(0.0);
    PositionObserved2 = new Fl_Value_Output(col5, row2, 80, 20);
    PositionObserved2->value(0.0);
    // create UI to set goal
    Fl_Output* goalLabel = new Fl_Output(col1, row3, 80, 20);
    goalLabel->box(FL_NO_BOX);
    goalLabel->value("Goal");
    GoalControlled1 = new Fl_Value_Input(col2, row3, 80, 20);
    GoalControlled1->value(0.0);
    GoalControlled2 = new Fl_Value_Input(col3, row3, 80, 20);
    GoalControlled2->value(0.0);
    Fl_Button * move = new Fl_Button(col2, row4, 80, 20, "Move");
    move->callback((Fl_Callback*)cb_MoveControlled, this);
    Moving = new Fl_Output(col3, row4, 20, 20);
    Moving->box(FL_NO_BOX);
    Moving->label("@||");
    // show time flying
    Ticks = new Fl_Value_Output(col2, row6, 80, 20, "Timestamp  ");
    Ticks->value(0);
    // close button
    Fl_Return_Button* close = new Fl_Return_Button(col3, row6, 75, 25, "Close");
    close->box(FL_THIN_UP_BOX);
    close->callback((Fl_Callback*)cb_Close, this);
    Window->end();
    Mutex.Unlock();
}

AppTaskUI::~AppTaskUI()
{}

void AppTaskUI::Show(void)
{
    Mutex.Lock();
    Window->show();
    Mutex.Unlock();
}

void AppTaskUI::Hide(void)
{
    Window->hide();
}

void AppTaskUI::GetGoalControlled(double & joint1, double & joint2) const
{
    joint1 = GoalControlled1->value();
    joint2 = GoalControlled2->value();
}

void AppTaskUI::ShowMoving(bool isMoving) 
{
    Mutex.Lock();
    if (isMoving) {
        Moving->label("@>");
    } else {
        Moving->label("@||");
    }
    Mutex.Unlock();
}

void AppTaskUI::Update(unsigned long ticks,
                       double positionControlled1, double positionControlled2,
                       double positionObserved1, double positionObserved2)
{
    Mutex.Lock();
    Ticks->value(ticks);
    PositionControlled1->value(positionControlled1);
    PositionControlled2->value(positionControlled2);
    PositionObserved1->value(positionObserved1);
    PositionObserved2->value(positionObserved2);
    Mutex.Unlock();
}

void AppTaskUI::cb_Close(Fl_Widget * CMN_UNUSED(widget), AppTaskUI * object)
{
    Mutex.Lock();
    object->closeCB(object->Task);
    Mutex.Unlock();
}

void AppTaskUI::cb_MoveControlled(Fl_Widget * CMN_UNUSED(widget), AppTaskUI * object)
{
    Mutex.Lock();
    object->MoveControlledPressed = true;
    Mutex.Unlock();
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
