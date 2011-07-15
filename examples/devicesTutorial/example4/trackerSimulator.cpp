/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ali Uneri
  Created on: 2009-08-11

  (C) Copyright 2007-2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "trackerSimulator.h"

CMN_IMPLEMENT_SERVICES(trackerSimulator);


trackerSimulator::trackerSimulator(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 500),
    ExitFlag(false)
{
    // required interfaces
    mtsInterfaceRequired * requiredInterface;
    requiredInterface = AddInterfaceRequired("RequiresPositionCartesian");
    if (requiredInterface) {
        requiredInterface->AddFunction("GetPositionCartesian", GetPositionCartesian);
    }

    // provided interfaces
    mtsInterfaceProvided * providedInterface;
    providedInterface = AddInterfaceProvided("ProvidesPositionCartesian");
    if (providedInterface) {
        StateTable.AddData(FrameSend, "FrameSend");
        providedInterface->AddCommandReadState(StateTable, FrameSend, "GetPositionCartesian");
    }
}


void trackerSimulator::Startup()
{
    UI.DisplayWindow->show();
}


void trackerSimulator::Run()
{
    ProcessQueuedCommands();

    Fl::check();
    if (Fl::thread_message() != 0) {
        CMN_LOG_CLASS_RUN_ERROR << "Run: GUI error " << Fl::thread_message() << std::endl;
        return;
    }

    // get frame to be sent from the UI
    for (unsigned int i = 0; i < 3; i++) {
        FrameSend.Position().Rotation().Element(i,0) = UI.FrameSend[i]->value();
        FrameSend.Position().Rotation().Element(i,1) = UI.FrameSend[i+3]->value();
        FrameSend.Position().Rotation().Element(i,2) = UI.FrameSend[i+6]->value();
        FrameSend.Position().Translation().Element(i) = UI.FrameSend[i+9]->value();
    }

    GetPositionCartesian(FrameRecv);

    // set received frame on the UI
    for (unsigned int i = 0; i < 3; i++) {
        UI.FrameRecv[i]->value(FrameRecv.Position().Rotation().Element(i,0));
        UI.FrameRecv[i+3]->value(FrameRecv.Position().Rotation().Element(i,1));
        UI.FrameRecv[i+6]->value(FrameRecv.Position().Rotation().Element(i,2));
        UI.FrameRecv[i+9]->value(FrameRecv.Position().Translation().Element(i));
    }

    if (UI.QuitClicked) {
        ExitFlag = true;
    }
}
