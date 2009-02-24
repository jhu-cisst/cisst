/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ui3MenuButton.cpp,v 1.3 2009/02/17 06:02:52 anton Exp $

  Author(s):	Balazs Vagvolgyi, Simon DiMaio, Anton Deguet
  Created on:	2008-06-10

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisst3DUserInterface/ui3MenuButton.h>

#include <vtkBMPReader.h>
#include <vtkTexture.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkImageData.h>

ui3MenuButton::~ui3MenuButton()
{
}


bool ui3MenuButton::CreateVTKObjects(void)
{
    this->BMPReader = vtkBMPReader::New();
    CMN_ASSERT(this->BMPReader);
    this->BMPReader->SetFileName(this->IconFile.c_str());
    
    // Create a texture object for the button.
    this->Texture = vtkTexture::New();
    CMN_ASSERT(this->Texture);
    this->Texture->SetInput(this->BMPReader->GetOutput());
    this->Texture->InterpolateOn();

    // Create the button plane
    this->PlaneSource = vtkPlaneSource::New();
    CMN_ASSERT(this->PlaneSource);
    double halfSize = this->ButtonSize / 2.0;
    this->PlaneSource->SetOrigin(-halfSize, -halfSize, 0.0);
    this->PlaneSource->SetPoint1(+halfSize, -halfSize, 0.0);
    this->PlaneSource->SetPoint2(-halfSize, +halfSize, 0.0);
    this->PlaneSource->SetCenter(0.0, 0.0, 0.0);
    this->PlaneSource->SetXResolution(1.0);
    this->PlaneSource->SetYResolution(1.0);

    // Create a polygon mapper for the button.
    this->Mapper = vtkPolyDataMapper::New();
    CMN_ASSERT(this->Mapper);
    this->Mapper->SetInput(this->PlaneSource->GetOutput());

    // Create an actor for the button.
    this->Actor = vtkActor::New();
    CMN_ASSERT(this->Actor);
    this->Actor->SetMapper(this->Mapper);
    this->Actor->SetTexture(this->Texture);
    this->Actor->GetProperty()->SetOpacity(0.5);
    this->Actor->VisibilityOn();
	return true;
}


vtkProp3D * ui3MenuButton::GetVTKProp(void)
{
    return this->Actor;
}


ui3Handle ui3MenuButton::GetHandle()
{
    return Handle;
}

ui3Handle ui3MenuButton::GetActorHandle()
{
    return ActorHandle;
}

void ui3MenuButton::SetCheck(bool state)
{
}

void ui3MenuButton::SetPush(bool state)
{
}

void ui3MenuButton::SetHighlight(bool state)
{
    if (state) {
        this->Actor->GetProperty()->SetOpacity(1.0);
    } else {
        this->Actor->GetProperty()->SetOpacity(0.5);
    }
}

void ui3MenuButton::SetEnable(bool state)
{
}

bool ui3MenuButton::GetCheck()
{
    return CheckState;
}

bool ui3MenuButton::GetPush()
{
    return PushState;
}

bool ui3MenuButton::GetHighlight()
{
    return HighlightState;
}

bool ui3MenuButton::GetEnable()
{
    return EnableState;
}

vtkActor * ui3MenuButton::VTKPointer(void)
{
    return this->Actor;
}

void ui3MenuButton::SetPosition(vct3 & position)
{
    // compute bounding box
    double halfSize = this->ButtonSize / 2.0;
    this->BottomLeft2D.X() = position.X() - halfSize ;
    this->BottomLeft2D.Y() = -halfSize;
    this->TopRight2D.X() = position.X() + halfSize;
    this->TopRight2D.Y() = +halfSize;
    // position actor wrt menu reference frame
    this->Actor->SetPosition(position.Pointer());
}

bool ui3MenuButton::IsCursorOver(const vct2 & cursor2D)
{
    return (cursor2D.GreaterOrEqual(this->BottomLeft2D)
            && cursor2D.LesserOrEqual(this->TopRight2D));
}