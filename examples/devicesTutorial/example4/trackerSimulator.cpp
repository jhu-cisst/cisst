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
    mtsProvidedInterface * providedInterface;
    providedInterface = AddProvidedInterface("PositionCartesian");

    StateTable.AddData(PositionCartesian, "PositionCartesian");
    providedInterface->AddCommandReadState(StateTable, PositionCartesian, "GetPositionCartesian");
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

    for (unsigned int r = 0; r < 3; r++) {
        PositionCartesian.Position().Translation().Element(r) = UI.Translation[r]->value();
        for (unsigned int c = 0; c < 3; c++) {
            PositionCartesian.Position().Rotation().Element(r,c) = UI.Rotation[(r*3)+c]->value();
        }
    }

    if (UI.QuitClicked) {
        ExitFlag = true;
    }
}
