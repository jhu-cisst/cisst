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

#include "BehaviorWithSlave.h"

#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkConeSource.h>
#include <vtkSphereSource.h>

class BehaviorWithSlaveVisibleObject: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
public:
    inline BehaviorWithSlaveVisibleObject(vctFrm3 position):
        ui3VisibleObject(),
        ConeSource(0),
        ConeMapper(0),
        ConeActor(0),
        SphereSource(0),
        SphereMapper(0),
        SphereActor(0),
        Position(position)
    {}

    inline bool CreateVTKObjects(void) {
        this->ConeSource = vtkConeSource::New();
        CMN_ASSERT(this->ConeSource);
        this->ConeSource->SetRadius(1.0);
        this->ConeSource->SetHeight(3.0);

        this->ConeMapper = vtkPolyDataMapper::New();
        CMN_ASSERT(this->ConeMapper);
        this->ConeMapper->SetInputConnection(this->ConeSource->GetOutputPort());
        this->ConeMapper->ImmediateModeRenderingOn();

        this->ConeActor = vtkActor::New();
        CMN_ASSERT(this->ConeActor);
        this->ConeActor->SetMapper(this->ConeMapper);

        this->AddPart(this->ConeActor);

        this->SphereSource = vtkSphereSource::New();
        CMN_ASSERT(this->SphereSource);
        this->SphereSource->SetRadius(1.0);

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
        this->Show();
        return true;
    }

protected:
    vtkConeSource * ConeSource;
    vtkPolyDataMapper * ConeMapper;
    vtkActor * ConeActor;

    vtkSphereSource * SphereSource;
    vtkPolyDataMapper * SphereMapper;
    vtkActor * SphereActor;

    vctFrm3 Position; // initial position
};


CMN_DECLARE_SERVICES_INSTANTIATION(BehaviorWithSlaveVisibleObject);
CMN_IMPLEMENT_SERVICES(BehaviorWithSlaveVisibleObject);


BehaviorWithSlave::BehaviorWithSlave(const std::string & name):
    ui3BehaviorBase(std::string("BehaviorWithSlave::") + name, 0),
    Ticker(0),
    Following(false),
    VisibleObject(0)
{
    this->VisibleObject = new BehaviorWithSlaveVisibleObject(this->Position);
    CMN_ASSERT(this->VisibleObject);

    this->Offset.SetAll(0.0);
}


BehaviorWithSlave::~BehaviorWithSlave()
{
}


void BehaviorWithSlave::ConfigureMenuBar()
{
    this->MenuBar->AddClickButton("FirstButton",
                                  1,
                                  "empty.png",
                                  &BehaviorWithSlave::FirstButtonCallback,
                                  this);
}


void BehaviorWithSlave::Startup(void)
{
    this->Slave1 = this->Manager->GetSlaveArm("Slave1");
    if (!this->Slave1) {
        CMN_LOG_CLASS_INIT_ERROR << "This behavior requires a slave arm ..." << std::endl;
    }
}


void BehaviorWithSlave::Cleanup(void)
{
    // menu bar will release itself upon destruction
}


bool BehaviorWithSlave::RunForeground()
{
    this->Ticker++;

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
    this->VisibleObject->SetTransformation(this->Slave1Position.Position());

    return true;
}

bool BehaviorWithSlave::RunBackground()
{
    this->Ticker++;

    // detect transition
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
        this->VisibleObject->Show();
    }

    this->Slave1->GetCartesianPosition(this->Slave1Position);
    this->Slave1Position.Position().Translation().Add(this->Offset);
    this->VisibleObject->SetTransformation(this->Slave1Position.Position());

    return true;
}

bool BehaviorWithSlave::RunNoInput()
{
    this->Ticker++;
    if (this->Manager->MastersAsMice() != this->PreviousMaM) {
        this->PreviousMaM = this->Manager->MastersAsMice();
        this->VisibleObject->Show();
    }

    this->Slave1->GetCartesianPosition(this->Slave1Position);
    this->Slave1Position.Position().Translation().Add(this->Offset);
    this->VisibleObject->SetTransformation(this->Slave1Position.Position());

    return true;
}

void BehaviorWithSlave::Configure(const std::string & CMN_UNUSED(configFile))
{
    // load settings
}

bool BehaviorWithSlave::SaveConfiguration(const std::string & CMN_UNUSED(configFile))
{
    // save settings
    return true;
}

void BehaviorWithSlave::FirstButtonCallback()
{
    CMN_LOG_RUN_VERBOSE << "Behavior \"" << this->GetName() << "\" Button 1 pressed" << std::endl;
}

void BehaviorWithSlave::PrimaryMasterButtonCallback(const prmEventButton & event)
{
    if (event.Type() == prmEventButton::PRESSED) {
        this->Following = true;
    } else if (event.Type() == prmEventButton::RELEASED) {
        this->Following = false;
    }
}
