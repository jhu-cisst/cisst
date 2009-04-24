/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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

#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSphereSource.h>

class BehaviorVisibleObject: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
public:
    inline BehaviorVisibleObject(ui3Manager * manager, vctDouble3 position):
        ui3VisibleObject(manager),
        Source(0),
        Mapper(0),
        Actor(0),
        Position(position)
    {}

    inline bool CreateVTKObjects(void) {
        this->Source = vtkSphereSource::New();
        CMN_ASSERT(this->Source);
        this->Source->SetRadius(10.0);

        this->Mapper = vtkPolyDataMapper::New();
        CMN_ASSERT(this->Mapper);
        this->Mapper->SetInputConnection(this->Source->GetOutputPort());

        this->Actor = vtkActor::New();
        CMN_ASSERT(this->Actor);
        this->Actor->SetMapper(this->Mapper);

        this->Assembly->AddPart(this->Actor);
        this->SetPosition(this->Position);
        this->Hide();
        return true;
    }

protected:
    vtkSphereSource * Source;
    vtkPolyDataMapper * Mapper;
    vtkActor * Actor;
    vctDouble3 Position; // initial position
};

CMN_DECLARE_SERVICES_INSTANTIATION(BehaviorVisibleObject);
CMN_IMPLEMENT_SERVICES(BehaviorVisibleObject);


CExampleBehavior::CExampleBehavior(const std::string & name, ui3Manager * manager):
    ui3BehaviorBase(std::string("CExampleBehavior::") + name, 0),
    Following(false),
    VisibleObject(0)
{
    this->Position.X() = 0.0;
    this->Position.Y() = 0.0;
    this->Position.Z() = -100.0;
    this->VisibleObject = new BehaviorVisibleObject(manager, this->Position);
    CMN_ASSERT(this->VisibleObject);
}


CExampleBehavior::~CExampleBehavior()
{
}


void CExampleBehavior::ConfigureMenuBar()
{
    this->MenuBar->AddClickButton("FirstButton",
                                  1,
                                  "empty.png",
                                  &CExampleBehavior::FirstButtonCallback,
                                  this);
}


void CExampleBehavior::Startup(void)
{

}


void CExampleBehavior::Cleanup(void)
{
    // menu bar will release itself upon destruction
}


bool CExampleBehavior::RunForeground()
{
    if (this->Manager->MastersAsMice() != this->PreviousMaM) {
        this->PreviousMaM = this->Manager->MastersAsMice();
        this->VisibleObject->Show();
    }

    // detect transition, should that be handled as an event?
    // State is used by multiple threads ...
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
        this->VisibleObject->Show();
    }
    // running in foreground GUI mode
    prmPositionCartesianGet position;

    this->GetPrimaryMasterPosition(position);

    if (this->Following) {
        vctDouble3 deltaCursor;
        deltaCursor.DifferenceOf(position.Position().Translation(),
                                 this->PreviousCursorPosition);
        this->Position.Add(deltaCursor);
        this->VisibleObject->SetPosition(this->Position);
    }
    this->PreviousCursorPosition.Assign(position.Position().Translation());
    return true;
}

bool CExampleBehavior::RunBackground()
{
    // detect transition
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
        this->VisibleObject->Hide();
    }
    return true;
}

bool CExampleBehavior::RunNoInput()
{
    if (this->Manager->MastersAsMice() != this->PreviousMaM) {
        this->PreviousMaM = this->Manager->MastersAsMice();
        this->VisibleObject->Hide();
    }
    return true;
}

void CExampleBehavior::Configure(const std::string & CMN_UNUSED(configFile))
{
    // load settings
}

bool CExampleBehavior::SaveConfiguration(const std::string & CMN_UNUSED(configFile))
{
    // save settings
    return true;
}

void CExampleBehavior::FirstButtonCallback()
{
    CMN_LOG_CLASS(6) << "Behavior \"" << this->GetName() << "\" Button 1 pressed" << std::endl;
}

void CExampleBehavior::PrimaryMasterButtonCallback(const prmEventButton & event)
{
    if (event.Type() == prmEventButton::PRESSED) {
        this->Following = true;
    } else if (event.Type() == prmEventButton::RELEASED) {
        this->Following = false;
    }
}
