/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: MeasurementBehavior.cpp 612 2009-07-29 22:38:37Z adeguet1 $

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

#include "MeasurementBehavior.h"

#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include "vtkVectorText.h"
#include "vtkFollower.h"


class MeasurementText: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
public:
    inline MeasurementText(vctFrm3 position, const std::string & name = "Text"):
        ui3VisibleObject(name),
        warningtextActor(0),
        warning_text(0),
        warningtextMapper(0),
        Position(position)
    {}
    
        inline ~MeasurementText()
    {}
        
    inline bool CreateVTKObjects(void) {

        warning_text = vtkVectorText::New();
        CMN_ASSERT(warning_text);
        warning_text->SetText(" ");

        warningtextMapper = vtkPolyDataMapper::New();
        CMN_ASSERT(warningtextMapper);
        warningtextMapper->SetInputConnection( warning_text->GetOutputPort() );

        warningtextActor = vtkFollower::New();
        CMN_ASSERT(warningtextActor);
        warningtextActor->SetMapper( warningtextMapper );
        warningtextActor->GetProperty()->SetColor(1, 165.0/255, 79.0/255 );
        //warningtextActor-> VisibilityOff();
        warningtextActor-> SetScale(2.5);

        this->AddPart(this->warningtextActor);

        this->SetTransformation(this->Position);
        return true;
    }

    inline void SetText(const std::string & text)
    {
        if (this->warning_text) {
            this->warning_text->SetText(text.c_str());
        }
    }
    

    inline void SetColor(double r, double g, double b)
    {
        if (this->warningtextActor && (r+g+b)<= 3) {
            this->warningtextActor->GetProperty()->SetColor(r,g,b);
        }
    }
    
    
protected:
    vtkFollower             *warningtextActor;
    vtkVectorText           *warning_text;
    vtkPolyDataMapper       *warningtextMapper;
    vtkProperty             *property;
    vctFrm3 Position;  // initial position
    //vctMatrix4x4            *textXform;
};

CMN_DECLARE_SERVICES_INSTANTIATION(MeasurementText);
CMN_IMPLEMENT_SERVICES(MeasurementText);


MeasurementBehavior::MeasurementBehavior(const std::string & name):
        ui3BehaviorBase(std::string("MeasurementBehavior::") + name, 0),
        Ticker(0),
        Following(false),
        VisibleList(0),
        TEXT(0)
{
    this->VisibleList = new ui3VisibleList("TextList");
    this->TEXT = new MeasurementText(this->Position);
    
    this->VisibleList->Add(TEXT);
}


MeasurementBehavior::~MeasurementBehavior()
{
}


void MeasurementBehavior::ConfigureMenuBar()
{
    this->MenuBar->AddClickButton("FirstButton",
                                  1,
                                  "empty.png",
                                  &MeasurementBehavior::FirstButtonCallback,
                                  this);
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
        if (this->Transition)
        {
            this->PreviousCursorPosition.Assign(position.Position().Translation());
            this->Transition = false;
        }
        vctDouble3 deltaCursor;
        deltaCursor.DifferenceOf(position.Position().Translation(),
                                 this->PreviousCursorPosition);
        this->Offset.Add(deltaCursor);
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
    
    if (!RightMTMOpen)
{
    this-> GetMeasurement();
}
    else {
        MeasurementActive = false;
        //can either leave the last measurement on the screen or let it dispear
        //this->SetText(MeasureText, " ");
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

void MeasurementBehavior::FirstButtonCallback()
{
    CMN_LOG_RUN_VERBOSE << "Behavior \"" << this->GetName() << "\" Button 1 pressed" << std::endl;
}

void MeasurementBehavior::PrimaryMasterButtonCallback(const prmEventButton & event)
{
    if (event.Type() == prmEventButton::PRESSED) {
        this->RightMTMOpen = false;
        this->Following = true;
    } else if (event.Type() == prmEventButton::RELEASED) {
        this->RightMTMOpen = true;
        this->Following = false;
    }
}

/*!

Displays the absolute 3D distance the center of the probe has moved since being activated

 */

void MeasurementBehavior::GetMeasurement()
{

    char    measure_string[100];
    vctFrm3 frame;
    frame = Slave1Position.Position(); //Moves the point from the control point of the probe to the center

    if (!MeasurementActive)
    {
        MeasurementActive = true;
        //saves the first point
        MeasurePoint1.Assign(frame.Translation());
        std::cout<< "MeasurePoint1 from if statement: " << MeasurePoint1<< std::endl;
    } else {
        //calculates the distance maoved
        vctDouble3 diff;
        diff.DifferenceOf(MeasurePoint1, frame.Translation());

        double AbsVal = diff.Norm();

        //displays the distance in mm 
        sprintf(measure_string,"%4.1fmm", AbsVal);
        this->SetText(TEXT, measure_string);
    }
}

/*!

Sets the text of the text objects
@param obj the text object in which the text is being set or changed
@param text the text

 */

void MeasurementBehavior::SetText(MeasurementText * obj, const std::string & text)
{
    obj->SetText(text);
}

