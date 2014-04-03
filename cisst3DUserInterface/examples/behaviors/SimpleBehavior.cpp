/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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

#include <SimpleBehavior.h>

#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisst3DUserInterface/ui3Manager.h>

#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSphereSource.h>

class SimpleBehaviorVisibleObject: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
public:
    inline SimpleBehaviorVisibleObject(void):
        ui3VisibleObject(),
        Source(0),
        Mapper(0),
        Actor(0),
        Red(true)
    {}

    inline ~SimpleBehaviorVisibleObject()
    {
        if (this->Actor) {
            this->Actor->Delete();
        }
        if (this->Mapper) {
            this->Mapper->Delete();
        }
        if (this->Source) {
            this->Source->Delete();
        }
    }

    inline bool CreateVTKObjects(void) {
        this->Source = vtkSphereSource::New();
        CMN_ASSERT(this->Source);
        this->Source->SetRadius(5.0);

        this->Mapper = vtkPolyDataMapper::New();
        CMN_ASSERT(this->Mapper);
        this->Mapper->SetInputConnection(this->Source->GetOutputPort());
        this->Mapper->ImmediateModeRenderingOn();

        this->Actor = vtkActor::New();
        CMN_ASSERT(this->Actor);
        this->Actor->SetMapper(this->Mapper);
        this->Actor->GetProperty()->SetColor(1.0, 0.0, 0.0);

        this->Actor->VisibilityOff();

        this->AddPart(this->Actor);
        return true;
    }

    inline bool UpdateVTKObjects(void) {
        return true;
    }

    void ToggleColor(void) {
        if (this->Red) {
            this->Actor->GetProperty()->SetColor(0.1, 0.9, 0.2);
            this->Red = false;
        } else {
            this->Actor->GetProperty()->SetColor(0.9, 0.1, 0.2);
            this->Red = true;
        }
    }

protected:
    vtkSphereSource * Source;
    vtkPolyDataMapper * Mapper;
    vtkActor * Actor;
    vctDouble3 Position; // initial position
    bool Red;
};

CMN_DECLARE_SERVICES_INSTANTIATION(SimpleBehaviorVisibleObject);
CMN_IMPLEMENT_SERVICES(SimpleBehaviorVisibleObject);


SimpleBehavior::SimpleBehavior(const std::string & name):
    ui3BehaviorBase(std::string("SimpleBehavior::") + name, 0),
    Following(false),
    VisibleList(0),
    VisibleObject1(0),
    VisibleObject2(0),
    Counter(0.0)
{
    this->VisibleList = new ui3VisibleList("SimpleBehavior");

    this->VisibleObject1 = new SimpleBehaviorVisibleObject();
    this->VisibleList->Add(this->VisibleObject1);


    this->VisibleObject2 = new SimpleBehaviorVisibleObject();
    this->VisibleList->Add(this->VisibleObject2);
    
    CMN_ASSERT(this->VisibleList);
}


SimpleBehavior::~SimpleBehavior()
{
    if (this->VisibleList) {
        delete this->VisibleList;
    }
}


void SimpleBehavior::ToggleColor()
{
    if (this->VisibleObject1) {
        this->VisibleObject1->ToggleColor();
    }
    if (this->VisibleObject2) {
        this->VisibleObject2->ToggleColor();
    }
}


void SimpleBehavior::ConfigureMenuBar()
{
    this->MenuBar->AddClickButton("ToggleColor",
                                  1,
                                  "redo.png",
                                  &SimpleBehavior::ToggleColor,
                                  this);
}


bool SimpleBehavior::RunForeground()
{
    if (this->Manager->MastersAsMice() != this->PreviousMaM) {
        this->PreviousMaM = this->Manager->MastersAsMice();
        this->VisibleList->Show();
    }

    // detect transition, should that be handled as an event?
    // State is used by multiple threads ...
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
        this->VisibleList->Show();
    }
    // running in foreground GUI mode
    prmPositionCartesianGet position;
    this->GetPrimaryMasterPosition(position);
    this->VisibleList->Lock();
    if (this->Following) {
        vctDouble3 deltaCursor;
        deltaCursor.DifferenceOf(position.Position().Translation(),
                                 this->PreviousCursorPosition);
        this->Position.Add(deltaCursor);
        this->VisibleList->SetPosition(this->Position, false);
        this->VisibleList->SetOrientation(position.Position().Rotation(), false);
    }
    this->PreviousCursorPosition.Assign(position.Position().Translation());
    this->UpdateRelativePosition();
    this->VisibleList->Unlock();
    return true;
}

bool SimpleBehavior::RunBackground()
{
    // detect transition
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
        this->VisibleList->Hide();
    }
    this->VisibleList->Lock();
    this->UpdateRelativePosition();
    this->VisibleList->Unlock();
    return true;
}

bool SimpleBehavior::RunNoInput()
{
    if (this->Manager->MastersAsMice() != this->PreviousMaM) {
        this->PreviousMaM = this->Manager->MastersAsMice();
        this->VisibleList->Hide();
    }
    return true;
}


void SimpleBehavior::OnQuit()
{
    this->VisibleList->Hide();
}


void SimpleBehavior::OnStart()
{

    this->Position.X() = 0.0;
    this->Position.Y() = 0.0;
    this->Position.Z() = -100.0;
    this->VisibleList->SetPosition(this->Position);
    this->VisibleList->Show();
}


void SimpleBehavior::PrimaryMasterButtonCallback(const prmEventButton & event)
{
    if (event.Type() == prmEventButton::PRESSED) {
        this->Following = true;
        CMN_LOG_CLASS_RUN_DEBUG << "Primary master button pressed, following started" << std::endl;
    } else if (event.Type() == prmEventButton::RELEASED) {
        this->Following = false;
        CMN_LOG_CLASS_RUN_DEBUG << "Primary master button pressed, following ended" << std::endl;
    }
}


void SimpleBehavior::UpdateRelativePosition(void)
{
    this->Counter += 0.01;
    this->VisibleObject2->SetPosition(vctDouble3(0.0, 20.0 * sin(Counter), 0.0), false);
}
