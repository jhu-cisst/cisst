/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: example1.cpp,v 1.13 2009/02/24 14:58:26 anton Exp $

  Author(s):	Balazs Vagvolgyi, Simon DiMaio, Anton Deguet
  Created on:	2008-05-23

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstDevices/devSensableHD.h>

#include "example1.h"


CExampleBehavior::CExampleBehavior(const std::string & name):
    ui3BehaviorBase(std::string("CExampleBehavior:") + name, 0)
{
}

CExampleBehavior::~CExampleBehavior()
{
}

void CExampleBehavior::ConfigureMenuBar()
{
    this->MenuBar->AddClickButton("FirstButton",
                                  1,
                                  "",
                                  &CExampleBehavior::FirstButtonCallback,
                                  this);
}

void CExampleBehavior::FirstButtonCallback()
{
    std::cerr << "Behavior \"" << this->GetName() << "\" Button 1 pressed" << std::endl;
}

void CExampleBehavior::Startup(void)
{

#if 0
    // Construct a new VTK actor object
    h3DModel = GetSceneManager()->CreateActor();

    // Request actor object
    vtkActor* actor = GetSceneManager()->GetActor(h3DModel);

        // Initialize actor
        // TO DO something like:
        //      actor->Load("3dmodel.vtk");

    // Release actor object
    GetSceneManager()->ReleaseActor(h3DModel);
#endif
}

void CExampleBehavior::Cleanup(void)
{
    // menu bar will release itself upon destruction
}

bool CExampleBehavior::RunForeground()
{
    // running in foreground GUI mode
    // menu bar is visible

    prmPositionCartesianGet position;
    this->RightMasterPositionFunction(position);
    std::cout << "\"Behavior \"" << this->GetName()
              << "\" : Position: " << position.Position().Translation() << std::endl;

#if 0
    // Request actor object
    vtkActor* actor = GetSceneManager()->GetActor(h3DModel);

        // Manipulate actor in a thread safe manner
        // TO DO something like:
        //      actor->Move(ptrpos);

    // Release actor object
    GetSceneManager()->ReleaseActor(h3DModel);
#endif
    return true;
}

bool CExampleBehavior::RunBackground()
{
    // running in background GUI mode
    // menu bar is hidden

    // do whatever processing...
    // TO DO

    return true;
}

bool CExampleBehavior::RunNoInput()
{
    // running in tele-operated mode
    // menu bar is hidden

    // do whatever processing...
    // TO DO

    return true;
}

void CExampleBehavior::Configure(const std::string & configFile)
{
    // load settings
}

bool CExampleBehavior::SaveConfiguration(const std::string & configFile)
{
    // save settings
    return true;
}
