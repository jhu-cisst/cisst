/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: appTaskUI.cpp,v 1.1 2008/02/07 20:33:28 pkaz Exp $ */

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
    Window = new Fl_Double_Window(300, 280, ControlledName.c_str());
    enum { row1 = 20, row2 = row1+40, row3 = row2+40, row4 = row3+40, row5 = row4+40, row6 = row5+40 };
    enum { col1 = 20, col2 = 100, col3 = 200 };
    // create labels to make the interface readable
    Fl_Output * controlledLabel = new Fl_Output(col2, row1, 80, 20);
    controlledLabel->box(FL_NO_BOX);
    controlledLabel->value(ControlledName.c_str());
    Fl_Output * observedLabel = new Fl_Output(col3, row1, 80, 20);
    observedLabel->box(FL_NO_BOX);
    observedLabel->value(ObservedName.c_str());
    Fl_Output * positionLabel = new Fl_Output(col1, row2, 80, 20);
    positionLabel->box(FL_NO_BOX);
    positionLabel->value("Position");
    // data members
    PositionControlled = new Fl_Value_Output(col2, row2, 80, 20);
    PositionControlled->value(0.0);
    PositionObserved = new Fl_Value_Output(col3, row2, 80, 20);
    PositionObserved->value(0.0);
    // create UI to set goal
    Fl_Output* goalLabel = new Fl_Output(col1, row3, 80, 20);
    goalLabel->box(FL_NO_BOX);
    goalLabel->value("Goal");
    GoalControlled = new Fl_Value_Input(col2, row3, 80, 20);
    GoalControlled->value(0.0);
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
    Mutex.Lock();
    Window->hide();
    Mutex.Unlock();
}

double AppTaskUI::GetGoalControlled(void) const
{
    double result;
    Mutex.Lock();
    result = GoalControlled->value();
    Mutex.Unlock();
    return result;
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

void AppTaskUI::Update(unsigned long ticks, double positionControlled, double positionObserved)
{
    Mutex.Lock();
    Ticks->value(ticks);
    PositionControlled->value(positionControlled);
    PositionObserved->value(positionObserved);
    Mutex.Unlock();
}

void AppTaskUI::cb_Close(Fl_Widget * CMN_UNUSED(widget), AppTaskUI * object)
{
    object->closeCB(object->Task);
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
