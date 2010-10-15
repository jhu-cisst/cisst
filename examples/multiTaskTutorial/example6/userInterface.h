/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

#ifndef _userInterface_h
#define _userInterface_h

#include <FL/Fl.H>
#include <cisstMultiTask.h>
#include <cisstOSAbstraction.h>

class Fl_Double_Window;
class Fl_Value_Output;
class Fl_Value_Input;
class Fl_Check_Button;
class Fl_Output;

class userInterface {

public:
    enum {NB_JOINTS = 2};
    typedef mtsDoubleVec PositionJointType;

protected:
    static osaMutex Mutex;
    unsigned long Ticks;

    // Use cisstMultiTask function objects
    mtsFunctionRead GetPositionJoint;
    mtsFunctionWrite MovePositionJoint;

    // mts events callbacks, in this example started event is void,
    // end event is write
    void CallBackStarted(void);
    mtsCommandVoid * CallBackStartedCommand;
    void CallBackFinished(const PositionJointType &);
    mtsCommandWriteBase * CallBackFinishedCommand;

    // Data member to store joint values
    PositionJointType Position;

    // Widgets
    Fl_Double_Window * Window;
    Fl_Value_Output * Position1Window;
    Fl_Value_Output * Position2Window;
    Fl_Value_Input * Goal1Window;
    Fl_Value_Input * Goal2Window;
    Fl_Value_Output * TicksWindow;
    Fl_Output * Moving;

    std::string Name;

    // UI callbacks
    static void CallBackClose(Fl_Widget * CMN_UNUSED(widget), userInterface * object);
    static void CallBackMove(Fl_Widget * CMN_UNUSED(widget), userInterface * object);

public:
    userInterface(const std::string & robotName,
                  mtsInterfaceProvided * interfacePointer);
    ~userInterface();
    void Hide(void);
    bool CloseRequested;
    void Update(void);
    void ShowMoving(bool isMoving);
};

#endif // _userInterface_h

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
