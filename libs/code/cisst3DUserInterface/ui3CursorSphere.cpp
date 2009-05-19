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

#include <cisst3DUserInterface/ui3CursorSphere.h>
#include <cisst3DUserInterface/ui3VisibleList.h>

#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSphereSource.h>
#include <vtkLineSource.h>

CMN_IMPLEMENT_SERVICES(ui3CursorSphere);


class CursorTip: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);

protected:
    vtkSphereSource * Source;
    vtkPolyDataMapper * Mapper;
    vtkActor * Actor;

public:
    CursorTip(ui3Manager * manager):
        ui3VisibleObject(manager),
        Source(0),
        Mapper(0),
        Actor(0)
    {}
    
    
    ~CursorTip()
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


    bool CreateVTKObjects(void)
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

        return true;
    }


    void UpdateColor(bool isClutched, bool isPressed, bool is2D)
    {
        this->Lock();
        if (isClutched) {
            this->Actor->GetProperty()->SetOpacity(0.5);
        } else {
            this->Actor->GetProperty()->SetOpacity(1.0);
        }
        if (is2D) {
            this->Source->SetRadius(1.0);
            if (isPressed) {
                this->Actor->GetProperty()->SetColor(0.5, 0.8, 0.8);
            } else {
                this->Actor->GetProperty()->SetColor(0.5, 1.0, 1.0);
            }
        } else {
            this->Source->SetRadius(2.0);
            if (isPressed) {
                this->Actor->GetProperty()->SetColor(1.0, 0.8, 0.8);
            } else {
                this->Actor->GetProperty()->SetColor(1.0, 1.0, 1.0);
            }
        }
        this->Unlock();
    }
};


CMN_DECLARE_SERVICES_INSTANTIATION(CursorTip);
CMN_IMPLEMENT_SERVICES(CursorTip);


ui3CursorSphere::ui3CursorSphere(ui3Manager * manager):
    ui3CursorBase(manager),
    IsPressed(false),
    Is2D(false),
    IsClutched(false),
    VisibleTip(0),
    VisibleAnchor(0),
    VisibleList()
{
    this->VisibleList = new ui3VisibleList(manager);
    this->VisibleTip = new CursorTip(manager);
    this->VisibleList->Add(this->VisibleTip);
}


ui3CursorSphere::~ui3CursorSphere()
{
    if (this->VisibleTip) {
        delete this->VisibleTip;
    }
    if (this->VisibleList) {
        delete this->VisibleList;
    }
}


void ui3CursorSphere::UpdateColor(void)
{
    dynamic_cast<CursorTip *>(this->VisibleTip)->UpdateColor(this->IsClutched, this->IsPressed, this->Is2D);
}


void ui3CursorSphere::SetPressed(bool pressed)
{
    this->IsPressed = pressed;
    this->UpdateColor();
}


void ui3CursorSphere::Set2D(bool is2D)
{
    this->Is2D = is2D;
    this->UpdateColor();
}

void ui3CursorSphere::SetClutched(bool clutched)
{
    this->IsClutched = clutched;
    this->UpdateColor();
}

ui3VisibleObject * ui3CursorSphere::GetVisibleObject(void)
{
    return this->VisibleTip;
}

void ui3CursorSphere::SetTransformation(vctDoubleFrm3 & frame)
{
    this->VisibleTip->SetTransformation(frame);
}
