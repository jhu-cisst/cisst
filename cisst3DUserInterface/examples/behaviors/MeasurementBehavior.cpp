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

#include "MeasurementBehavior.h"

#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisst3DUserInterface/ui3Manager.h>
#include <cisst3DUserInterface/ui3SlaveArm.h> // bad, ui3 should not have slave arm to start with (adeguet1)

#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include "vtkVectorText.h"
#include "vtkFollower.h"


class MeasurementBehaviorVisibleObject: public ui3VisibleObject
{
	CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
public:
    inline MeasurementBehaviorVisibleObject(vctFrm3 position, const std::string & name = "Text"):
        ui3VisibleObject(name),
        Text(0),
        TextMapper(0),
        TextActor(0),
        Position(position)
    {}

    inline ~MeasurementBehaviorVisibleObject()
    {}

    inline bool CreateVTKObjects(void) {

        Text = vtkVectorText::New();
        CMN_ASSERT(Text);
        Text->SetText(" ");

        TextMapper = vtkPolyDataMapper::New();
        CMN_ASSERT(TextMapper);
        TextMapper->SetInputConnection(Text->GetOutputPort());
        TextMapper->ImmediateModeRenderingOn();

        TextActor = vtkFollower::New();
        CMN_ASSERT(TextActor);
        TextActor->SetMapper(TextMapper);
        TextActor->GetProperty()->SetColor(0.0, 0.0, 200.0/255.0 );
        TextActor-> SetScale(2.5);

        this->AddPart(this->TextActor);

        return true;
    }

	inline bool UpdateVTKObjects(void) {
        return true;
    }

    inline void SetText(const std::string & text)
    {
        if (this->Text) {
            this->Text->SetText(text.c_str());
        }
    }

    inline void SetColor(double r, double g, double b)
    {
        if (this->TextActor && (r+g+b)<= 3) {
            this->TextActor->GetProperty()->SetColor(r,g,b);
        }
    }


protected:
    vtkVectorText * Text;
    vtkPolyDataMapper * TextMapper;
    vtkFollower * TextActor;
    vctFrm3 Position;  // initial position
};


CMN_DECLARE_SERVICES_INSTANTIATION(MeasurementBehaviorVisibleObject);
CMN_IMPLEMENT_SERVICES(MeasurementBehaviorVisibleObject);


MeasurementBehavior::MeasurementBehavior(const std::string & name):
    ui3BehaviorBase(std::string("MeasurementBehavior::") + name, 0),
    Ticker(0),
    Measuring(false),
    ClutchMeasureStarted(false),
    VisibleList(0),
    VisibleObject(0)
{
    this->Offset.Assign(0.0, 0.0, 0.0);
    this->VisibleList = new ui3VisibleList("TextList");
    this->VisibleObject = new MeasurementBehaviorVisibleObject(this->Position);

    this->VisibleList->Add(this->VisibleObject);

    // interface to trigger start/end of measurement
    mtsInterfaceRequired * interfaceRequired;
    interfaceRequired = this->AddInterfaceRequired("StartStopMeasure");
    interfaceRequired->AddEventHandlerWrite(&MeasurementBehavior::StartStopMeasure,
                                            this,
                                            "Button");
}


MeasurementBehavior::~MeasurementBehavior()
{
}


void MeasurementBehavior::ConfigureMenuBar()
{
}


void MeasurementBehavior::Startup(void)
{
    this->Slave1 = this->Manager->GetSlaveArm("Slave1");
    if (!this->Slave1) {
        CMN_LOG_CLASS_INIT_ERROR << "This behavior requires a slave arm ..." << std::endl;
    }
    RightMTMOpen = true;
    MeasurePoint1.Assign(0.0,0.0,0.0);
}


void MeasurementBehavior::Cleanup(void)
{
    // menu bar will release itself upon destruction
}


bool MeasurementBehavior::RunForeground()
{
    this->Ticker++;

    if (this->Manager->MastersAsMice() != this->PreviousMaM) {
        this->PreviousMaM = this->Manager->MastersAsMice();
        this->VisibleList->Show();
        this->DelayToGrab = 20;
    }

    // detect transition, should that be handled as an event?
    // State is used by multiple threads ...
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
        this->VisibleList->Show();
        this->DelayToGrab = 20;
    }
    if (this->DelayToGrab > 0) {
        this->DelayToGrab--;
    }

    // running in foreground GUI mode
    prmPositionCartesianGet position;

    // compute offset
    if (this->DelayToGrab == 0) {
        this->GetPrimaryMasterPosition(position);
        if (this->Measuring) {
            if (this->Transition) {
                this->PreviousCursorPosition.Assign(position.Position().Translation());
                this->Transition = false;
            }
            vctDouble3 deltaCursor;
            deltaCursor.DifferenceOf(position.Position().Translation(),
                                     this->PreviousCursorPosition);
            this->Offset.Add(deltaCursor);
        }
    }
    this->PreviousCursorPosition.Assign(position.Position().Translation());

    // apply to object
    this->Slave1->GetCartesianPosition(this->Slave1Position);
    this->Slave1Position.Position().Translation().Add(this->Offset);
    this->VisibleList->SetPosition(this->Slave1Position.Position().Translation());

    return true;
}

bool MeasurementBehavior::RunBackground()
{
    this->Ticker++;

    // detect transition
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
        this->VisibleList->Show();
    }
    this->Transition = true;
    this->Slave1->GetCartesianPosition(this->Slave1Position);
    this->Slave1Position.Position().Translation().Add(this->Offset);
    this->VisibleList->SetPosition(this->Slave1Position.Position().Translation());

    return true;
}

bool MeasurementBehavior::RunNoInput()
{
    this->Ticker++;
    if (this->Manager->MastersAsMice() != this->PreviousMaM) {
        this->PreviousMaM = this->Manager->MastersAsMice();
        this->VisibleList->Show();
    }

    this->Transition = true;
    this->Slave1->GetCartesianPosition(this->Slave1Position);
    this->Slave1Position.Position().Translation().Add(this->Offset);
    this->VisibleList->SetPosition(this->Slave1Position.Position().Translation());
    this->VisibleList->Show();

    if (this->Measuring) {
        this->GetMeasurement();
    }

    return true;
}


void MeasurementBehavior::Configure(const std::string & CMN_UNUSED(configFile))
{
    // load settings
}

bool MeasurementBehavior::SaveConfiguration(const std::string & CMN_UNUSED(configFile))
{
    // save settings
    return true;
}



void MeasurementBehavior::PrimaryMasterButtonCallback(const prmEventButton & event)
{
    if (event.Type() == prmEventButton::PRESSED) {
        this->SetMeasurePoint1();
        this->Measuring = true;
    } else if (event.Type() == prmEventButton::RELEASED) {
        this->Measuring = false;
    }
}


void MeasurementBehavior::StartStopMeasure(const prmEventButton & event)
{
    if (event.Type() == prmEventButton::RELEASED) {
        this->ClutchMeasureStarted = !this->ClutchMeasureStarted;
        if (this->ClutchMeasureStarted) {
            this->SetMeasurePoint1();
            this->Measuring = true;
        } else {
            this->Measuring = false;
        }
    }
}


void MeasurementBehavior::SetMeasurePoint1(void)
{
    vctFrm3 frame;
    frame = Slave1Position.Position(); //Moves the point from the control point of the probe to the center

    //saves the first point
    MeasurePoint1.Assign(frame.Translation());
}


void MeasurementBehavior::GetMeasurement(void)
{
    char    measure_string[100];
    vctFrm3 frame;
    frame = Slave1Position.Position(); //Moves the point from the control point of the probe to the center

    //calculates the distance moved
    vctDouble3 diff;
    diff.DifferenceOf(MeasurePoint1, frame.Translation());

    double AbsVal = diff.Norm();

    //displays the distance in mm
    sprintf(measure_string,"%4.1fmm", AbsVal);
    this->VisibleObject->SetText(measure_string);
}

