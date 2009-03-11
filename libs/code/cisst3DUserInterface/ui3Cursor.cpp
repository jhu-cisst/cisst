/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):	Anton Deguet
  Created on:	2009-02-04

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisst3DUserInterface/ui3Cursor.h>

#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSphereSource.h>

CMN_IMPLEMENT_SERVICES(ui3Cursor);

ui3Cursor::ui3Cursor(ui3Manager * manager):
    ui3VisibleObject(manager),
    Source(0),
    Mapper(0),
    Actor(0),
    IsPressed(false),
    Is2D(false),
    IsClutched(false)
{}


ui3Cursor::~ui3Cursor()
{
    if (this->Source) {
        this->Source->Delete();
    }

    if (this->Mapper) {
        this->Mapper->Delete();
    }

    if (this->Actor) {
        this->Actor->Delete();
    }
}


bool ui3Cursor::CreateVTKObjects()
{
    this->Source = vtkSphereSource::New();
    CMN_ASSERT(this->Source);
    this->Source->SetRadius(2.0);

    this->Mapper = vtkPolyDataMapper::New();
    CMN_ASSERT(this->Mapper);
    this->Mapper->SetInputConnection(this->Source->GetOutputPort());

    this->Actor = vtkActor::New();
    CMN_ASSERT(this->Actor);
    this->Actor->SetMapper(this->Mapper);

    this->Assembly->AddPart(this->Actor);

    this->SetPressed(false);
    return true;
}


void ui3Cursor::UpdateColor(void)
{
    this->Lock();
    if (this->IsClutched) {
        this->Actor->GetProperty()->SetOpacity(0.5);
    } else {
        this->Actor->GetProperty()->SetOpacity(1.0);
    }
    if (this->Is2D) {
        this->Source->SetRadius(1.0);
        if (this->IsPressed) {
            this->Actor->GetProperty()->SetColor(0.5, 0.8, 0.8);
        } else {
            this->Actor->GetProperty()->SetColor(0.5, 1.0, 1.0);
        }
    } else {
        this->Source->SetRadius(2.0);
        if (this->IsPressed) {
            this->Actor->GetProperty()->SetColor(1.0, 0.8, 0.8);
        } else {
            this->Actor->GetProperty()->SetColor(1.0, 1.0, 1.0);
        }
    }
    this->Unlock();
}


void ui3Cursor::SetPressed(bool pressed)
{
    this->IsPressed = pressed;
    this->UpdateColor();
}


void ui3Cursor::Set2D(bool is2D)
{
    this->Is2D = is2D;
    this->UpdateColor();
}

void ui3Cursor::SetClutched(bool clutched)
{
    this->IsClutched = clutched;
    this->UpdateColor();
}

