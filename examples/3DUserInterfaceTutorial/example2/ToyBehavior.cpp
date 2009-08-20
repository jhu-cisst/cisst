/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ToyBehavior.cpp 612 2009-07-29 22:38:37Z adeguet1 $

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

#include "ToyBehavior.h"

#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkConeSource.h>
#include <vtkSphereSource.h>
#include <vtkOutlineSource.h>

class ToyBehaviorVisibleObject: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
    public:
    inline ToyBehaviorVisibleObject(vctFrm3 position, const std::string & name = "sphere"):
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


CMN_DECLARE_SERVICES_INSTANTIATION(ToyBehaviorVisibleObject);
CMN_IMPLEMENT_SERVICES(ToyBehaviorVisibleObject);

class Widget: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
    public:
    inline Widget(vctFrm3 position, const std::string & name = "3DWidgetAttempt"):
        ui3VisibleObject(name),
        SphereSource(0),
        SphereMapper(0),
        ctl1(0),
        ctl2(0),
        ctl3(0),
        ctl4(0),
        OutlineSource(0),
        OutlineMapper(0),
        OutlineActor(0),
        Position(position)
        {}

        inline bool CreateVTKObjects(void) {

        this->SphereSource = vtkSphereSource::New();
        CMN_ASSERT(this->SphereSource);
        this->SphereSource->SetRadius(5.0);

        this->SphereMapper = vtkPolyDataMapper::New();
        CMN_ASSERT(this->SphereMapper);
        this->SphereMapper->SetInputConnection(this->SphereSource->GetOutputPort());

        //create the top control point
        this->ctl1 = vtkActor::New();
        CMN_ASSERT(this->ctl1);
        this->ctl1->SetMapper(this->SphereMapper);
        this->ctl1->GetProperty()->SetColor(1.0, 0.0, 0.0);
        this->ctl1->SetPosition(15.0, 0.0, 0.0);
        
        //create the bottom control point
        this->ctl2 = vtkActor::New();
        CMN_ASSERT(this->ctl2);
        this->ctl2->SetMapper(this->SphereMapper);
        this->ctl2->GetProperty()->SetColor(1.0, 0.0, 0.0);
        this->ctl2->SetPosition(-15.0, 0.0, 0.0);
        
        //create the right ctl point
        this->ctl3 = vtkActor::New();
        CMN_ASSERT(this->ctl3);
        this->ctl3->SetMapper(this->SphereMapper);
        this->ctl3->GetProperty()->SetColor(1.0, 0.0, 0.0);
        this->ctl3->SetPosition(0.0, 15.0, 0.0);
        
        //create the left ctl point
        this->ctl4 = vtkActor::New();
        CMN_ASSERT(this->ctl4);
        this->ctl4->SetMapper(this->SphereMapper);
        this->ctl4->GetProperty()->SetColor(1.0, 0.0, 0.0);
        this->ctl3->SetPosition(0.0, -15.0, 0.0);

        this->AddPart(this->ctl1);
        this->AddPart(this->ctl2);
        this->AddPart(this->ctl3);
        this->AddPart(this->ctl4);
        
        this->OutlineSource= vtkOutlineSource::New();
        OutlineSource->SetBounds(-15,15,-15,15,-15,15);
        
        vtkPolyDataMapper *OutlineMapper = vtkPolyDataMapper::New();
        OutlineMapper->SetInputConnection(OutlineSource->GetOutputPort());
        
        OutlineActor = vtkActor::New();;
        OutlineActor->SetMapper(this->OutlineMapper);
        OutlineActor->GetProperty()->SetColor(1,1,1);
        
        this->AddPart(this->OutlineActor);

        this->SetTransformation(this->Position);
        this->Hide();
        return true;
        }
        

    protected:

        vtkSphereSource * SphereSource;
        vtkPolyDataMapper * SphereMapper;
        vtkActor * ctl1, *ctl2, *ctl3, *ctl4;
        vtkOutlineSource * OutlineSource;
        vtkPolyDataMapper * OutlineMapper;
        vtkActor * OutlineActor;



        vctFrm3 Position; // initial position
};


CMN_DECLARE_SERVICES_INSTANTIATION(Widget);
CMN_IMPLEMENT_SERVICES(Widget);


ToyBehavior::ToyBehavior(const std::string & name):
        ui3BehaviorBase(std::string("ToyBehavior::") + name, 0),
        Ticker(0),
        Following(false),
        VisibleList(0)
{
    this->VisibleList = new ui3VisibleList("ToyList");
    CMN_ASSERT(this->VisibleList);
    
    this->WidgetObject = new Widget(this->Position);

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

    // compute offset
    this->GetPrimaryMasterPosition(position);
    if (this->Following) {
        vctDouble3 deltaCursor;
        deltaCursor.DifferenceOf(position.Position().Translation(),
                                 this->PreviousCursorPosition);
        this->Offset.Add(deltaCursor);
    }
    this->PreviousCursorPosition.Assign(position.Position().Translation());

    // apply to object
    this->Slave1->GetCartesianPosition(this->Slave1Position);
    this->Slave1Position.Position().Translation().Add(this->Offset);
    this->VisibleList->SetTransformation(this->Slave1Position.Position());

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

    this->Slave1->GetCartesianPosition(this->Slave1Position);
    this->Slave1Position.Position().Translation().Add(this->Offset);
    this->VisibleList->SetTransformation(this->Slave1Position.Position());

    return true;
}

bool ToyBehavior::RunNoInput()
{
    this->Ticker++;
    if (this->Manager->MastersAsMice() != this->PreviousMaM) {
        this->PreviousMaM = this->Manager->MastersAsMice();
        this->VisibleList->Show();
    }

    this->Slave1->GetCartesianPosition(this->Slave1Position);
    this->Slave1Position.Position().Translation().Add(this->Offset);
    this->VisibleList->SetTransformation(this->Slave1Position.Position());

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

void ToyBehavior::FirstButtonCallback()
{
    CMN_LOG_RUN_VERBOSE << "Behavior \"" << this->GetName() << "\" Button 1 pressed" << std::endl;
}

void ToyBehavior::PrimaryMasterButtonCallback(const prmEventButton & event)
{
    if (event.Type() == prmEventButton::PRESSED) {
        this->RightMTMOpen = true;
    } else if (event.Type() == prmEventButton::RELEASED) {
        this->RightMTMOpen = false;
    }
}

void ToyBehavior::AddSphereCallback(void)
{
    
}

void ToyBehavior::AddCubeCallback(void)
{
    
}

void ToyBehavior::AddCylinderCallback(void)
{
    
}

ui3VisibleObject * ToyBehavior::FindClosestShape(void)
{
    
}


