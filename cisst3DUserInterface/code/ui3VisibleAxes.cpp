/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id:

  Author(s):	Gorkem Sevinc, Anton Deguet
  Created on:	2009-06-19

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisst3DUserInterface/ui3VisibleAxes.h>

#include <vtkAssembly.h>
#include <vtkProperty.h>
#include <vtkAxesActor.h>
#include <vtkMatrix4x4.h>


CMN_IMPLEMENT_SERVICES(ui3VisibleAxes);

ui3VisibleAxes::~ui3VisibleAxes()
{
    if (this->AxesActor) {
        this->AxesActor->Delete();
    }
}


bool ui3VisibleAxes::CreateVTKObjects(void) 
{

    this->AxesActor = vtkAxesActor::New();
    CMN_ASSERT(this->AxesActor);

    this->AxesActor->SetShaftTypeToCylinder();

    this->AddPart(this->AxesActor);
    this->SetSize(10.0);
    this->ShowLabels();

    this->Matrix->SetElement(0, 3, 0.0);
    this->Matrix->SetElement(1, 3, 0.0);
    this->Matrix->SetElement(2, 3, -200.0);

    return true;
}


void ui3VisibleAxes::SetSize(double size)
{
    this->Lock();
    this->AxesActor->SetTotalLength(size, size, size);
    this->Unlock();
}


void ui3VisibleAxes::ShowLabels(void)
{
    this->Lock();
    this->AxesActor->SetXAxisLabelText("x");
    this->AxesActor->SetYAxisLabelText("y");
    this->AxesActor->SetZAxisLabelText("z");
    this->AxesActor->AxisLabelsOn();
    this->Unlock();
}    


void ui3VisibleAxes::HideLabels(void)
{
    this->Lock();
    this->AxesActor->AxisLabelsOff();
    this->Unlock();
}

