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

#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>

#include "ToyBehavior.h"

#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkConeSource.h>
#include <vtkSphereSource.h>
#include <vtkOutlineSource.h>

class ToyBehaviorSphere: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
    public:
    inline ToyBehaviorSphere(vctFrm3 position, const std::string & name = "sphere"):
        ui3VisibleObject(name),
        SphereSource(0),
        SphereMapper(0),
        SphereActor(0),
        Position(position)
        {}

        inline bool CreateVTKObjects(void) {

        this->SphereSource = vtkSphereSource::New();
        CMN_ASSERT(this->SphereSource);
        this->SphereSource->SetRadius(5.0);

        this->SphereMapper = vtkPolyDataMapper::New();
        CMN_ASSERT(this->SphereMapper);
        this->SphereMapper->SetInputConnection(this->SphereSource->GetOutputPort());
        this->SphereMapper->ImmediateModeRenderingOn();

        this->SphereActor = vtkActor::New();
        CMN_ASSERT(this->SphereActor);
        this->SphereActor->SetMapper(this->SphereMapper);
        this->SphereActor->GetProperty()->SetColor(1.0, 0.0, 0.0);

        this->AddPart(this->SphereActor);
        this->SetTransformation(this->Position);
        this->Hide();
        return true;
        }

    protected:

        vtkSphereSource * SphereSource;
        vtkPolyDataMapper * SphereMapper;
        vtkActor * SphereActor;

        vctFrm3 Position; // initial position
};


CMN_DECLARE_SERVICES_INSTANTIATION(ToyBehaviorSphere);
CMN_IMPLEMENT_SERVICES(ToyBehaviorSphere);

class Widget: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
    public:
    inline Widget(vctFrm3 position, const std::string & name = "3DWidgetAttempt"):
        ui3VisibleObject(name),
        SphereSource(0),
        SphereMapper(0),
        OutlineSource(0),
        OutlineMapper(0),
        OutlineActor(0),
        Position(position)
        {}

        inline bool CreateVTKObjects(void) {

        this->SphereSource = vtkSphereSource::New();
        CMN_ASSERT(this->SphereSource);
        this->SphereSource->SetRadius(3.0);

        this->SphereMapper = vtkPolyDataMapper::New();
        CMN_ASSERT(this->SphereMapper);
        this->SphereMapper->SetInputConnection(this->SphereSource->GetOutputPort());
        this->SphereMapper->ImmediateModeRenderingOn();

        //create the top control point
        this->sphere1 = vtkActor::New();
        CMN_ASSERT(this->sphere1);
        this->sphere1->SetMapper(this->SphereMapper);
        this->sphere1->GetProperty()->SetColor(1.0, 0.0, 0.0);
        this->sphere1->SetPosition(20.0, 0.0, 0.0);
        
        //create the bottom control point
        this->sphere2 = vtkActor::New();
        CMN_ASSERT(this->sphere2);
        this->sphere2->SetMapper(this->SphereMapper);
        this->sphere2->GetProperty()->SetColor(0.0, 1.0, 0.0);
        this->sphere2->SetPosition(-20.0, 0.0, 0.0);
        
        //create the right ctl point
        this->sphere3 = vtkActor::New();
        CMN_ASSERT(this->sphere3);
        this->sphere3->SetMapper(this->SphereMapper);
        this->sphere3->GetProperty()->SetColor(0.0, 0.0, 1.0);
        this->sphere3->SetPosition(0.0, 20.0, 0.0);
        
        //create the left ctl point
        this->sphere4 = vtkActor::New();
        CMN_ASSERT(this->sphere4);
        this->sphere4->SetMapper(this->SphereMapper);
        this->sphere4->GetProperty()->SetColor(1.0, 1.0, 1.0);
        this->sphere4->SetPosition(0.0, -20.0, 0.0);

        this->AddPart(this->sphere1);
        this->AddPart(this->sphere2);
        this->AddPart(this->sphere3);
        this->AddPart(this->sphere4);

        this->OutlineSource= vtkOutlineSource::New();
        this->OutlineSource->SetBounds(-15,15,-15,15,-15,15);

        this->OutlineMapper = vtkPolyDataMapper::New();
        this->OutlineMapper->SetInputConnection(OutlineSource->GetOutputPort());
        this->OutlineMapper->ImmediateModeRenderingOn();

        this->OutlineActor = vtkActor::New();
        this->OutlineActor->SetMapper(this->OutlineMapper);
        this->OutlineActor->GetProperty()->SetColor(1,1,1);
        this->OutlineActor->SetPosition(0.0,0.0,0.0);

        this->AddPart(this->OutlineActor);

        this->SetTransformation(this->Position);

        this->CreateControlPoints();
        return true;
        }

        void CreateControlPoints(void)
        {
            vctFrm3 ctl1, ctl2, ctl3, ctl4;
            ctl1.Translation().Assign(20.0, 0.0, 0.0);
            ctl2.Translation().Assign(-20.0, 0.0, 0.0);
            ctl3.Translation().Assign(0.0, 20.0, 0.0);
            ctl4.Translation().Assign(0.0, -20.0, 0.0);
            Rel_ControlPoints.push_back(ctl1);
            Rel_ControlPoints.push_back(ctl2);
            Rel_ControlPoints.push_back(ctl3);
            Rel_ControlPoints.push_back(ctl4);
        }
        
        std::list<vctFrm3> GetRelativeControlPoints(void)
        {
            return Rel_ControlPoints;
        }

    protected:

        vtkSphereSource * SphereSource;
        vtkPolyDataMapper * SphereMapper;
        vtkActor * sphere1, *sphere2, *sphere3, *sphere4;
        vtkOutlineSource * OutlineSource;
        vtkPolyDataMapper * OutlineMapper;
        vtkActor * OutlineActor;

        std::list<vctFrm3> Rel_ControlPoints;

        vctFrm3 Position; // initial position
};


CMN_DECLARE_SERVICES_INSTANTIATION(Widget);
CMN_IMPLEMENT_SERVICES(Widget);


ToyBehavior::ToyBehavior(const std::string & name):
        ui3BehaviorBase(std::string("ToyBehavior::") + name, 0),
        Ticker(0),
        Following(false),
        VisibleList(0),
        RightMTMOpen(true),
        Transition(true)
{
    this->VisibleList = new ui3VisibleList("ToyList");
    CMN_ASSERT(this->VisibleList);
    
    this->WidgetObject = new Widget(this->Position); //NOTE: I would like to use the 3DObjectViewer instead, should work, some error about virtual function....

    this->VisibleList->Add(this->WidgetObject);
    this->Offset.SetAll(0.0);
}


ToyBehavior::~ToyBehavior()
{
}


void ToyBehavior::ConfigureMenuBar()
{
    this->MenuBar->AddClickButton("AddSphereButton",
                                  1,
                                  "sphere.png",
                                  &ToyBehavior::AddSphereCallback,
                                  this);

    this->MenuBar->AddClickButton("AddCubeButton",
                                  1,
                                  "cube.png",
                                  &ToyBehavior::AddCubeCallback,
                                  this);

    this->MenuBar->AddClickButton("AddCylinderButton",
                                  1,
                                  "cylinder.png",
                                  &ToyBehavior::AddCylinderCallback,
                                  this);
    
}


void ToyBehavior::Startup(void)
{
    this->Slave1 = this->Manager->GetSlaveArm("Slave1");
    if (!this->Slave1) {
        CMN_LOG_CLASS_INIT_ERROR << "This behavior requires a slave arm ..." << std::endl;
    }
}


void ToyBehavior::Cleanup(void)
{
    // menu bar will release itself upon destruction
}


bool ToyBehavior::RunForeground()
{
    this->Ticker++;
    // running in foreground GUI mode
    prmPositionCartesianGet position;

    // compute offset
    this->GetPrimaryMasterPosition(position);

    if (this->Manager->MastersAsMice() != this->PreviousMaM) {
        this->PreviousMaM = this->Manager->MastersAsMice();
        this->VisibleList->Show();
        this->OnStart();
    }

    // detect transition, should that be handled as an event?
    // State is used by multiple threads ...
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
        this->VisibleList->Show();
        this->PreviousCursorPosition.Assign(position.Position().Translation());
    }

    if (!this->RightMTMOpen) {
        if(this->Transition)
        {
            this->PreviousCursorPosition.Assign(position.Position().Translation());
            this->Transition = false;
        }
        vctDouble3 deltaCursor;
        deltaCursor.DifferenceOf(position.Position().Translation(),
                                 this->PreviousCursorPosition);
        this->Position.Translation().Add(deltaCursor);
        this->VisibleList->SetPosition(this->Position.Translation());
    }
    this->PreviousCursorPosition.Assign(position.Position().Translation());
    return true;
}

bool ToyBehavior::RunBackground()
{
    this->Ticker++;

    // detect transition
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
        this->VisibleList->Show();
    }
    this->Transition = true;
    return true;
}

bool ToyBehavior::RunNoInput()
{
    this->Ticker++;
    if (this->Manager->MastersAsMice() != this->PreviousMaM) {
        this->PreviousMaM = this->Manager->MastersAsMice();
        this->VisibleList->Show();
    }
    
    this->Transition = true;
    return true;
}

void ToyBehavior::Configure(const std::string & CMN_UNUSED(configFile))
{
    // load settings
}

bool ToyBehavior::SaveConfiguration(const std::string & CMN_UNUSED(configFile))
{
    // save settings
    return true;
}

void ToyBehavior::OnStart()
{
    this->Position.Translation().Assign(vctDouble3(0.0,0.0,-150.0));
    this->VisibleList->SetPosition(this->Position.Translation());
    this->VisibleList->Show();
}

void ToyBehavior::FirstButtonCallback()
{
    CMN_LOG_RUN_VERBOSE << "Behavior \"" << this->GetName() << "\" Button 1 pressed" << std::endl;
}

void ToyBehavior::PrimaryMasterButtonCallback(const prmEventButton & event)
{
    if (event.Type() == prmEventButton::PRESSED) {
        this->RightMTMOpen = false;
    } else if (event.Type() == prmEventButton::RELEASED) {
        this->RightMTMOpen = true;
    }
}

void ToyBehavior::AddSphereCallback(void)
{
//	ToyBehaviorSphere * newSphere = new ToyBehaviorSphere("sphere");
}

void ToyBehavior::AddCubeCallback(void)
{
    //need to write this
}

void ToyBehavior::AddCylinderCallback(void)
{
    //need to write this
}

ui3VisibleObject * ToyBehavior::FindClosestShape(void)
{
    // this should find the closet shape the cursor is near, and highlight it.  this will be the one that is manitulated
    return 0;
}


