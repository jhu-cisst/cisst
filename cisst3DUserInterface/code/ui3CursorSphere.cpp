/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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


#include <cisstVector/vctRandom.h>

#include <cisst3DUserInterface/ui3CursorSphere.h>
#include <cisst3DUserInterface/ui3VisibleList.h>

#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSphereSource.h>
#include <vtkLineSource.h>

CMN_IMPLEMENT_SERVICES(ui3CursorSphere);


class ui3CursorSphereTip: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    vtkSphereSource * Source;
    vtkPolyDataMapper * Mapper;
    vtkActor * Actor;

public:
    ui3CursorSphereTip():
        ui3VisibleObject("ui3CursorSphereTip"),
        Source(0),
        Mapper(0),
        Actor(0)
    {}
    
    
    ~ui3CursorSphereTip()
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
        this->Source->SetRadius(1.0);
        
        this->Mapper = vtkPolyDataMapper::New();
        CMN_ASSERT(this->Mapper);
        this->Mapper->SetInputConnection(this->Source->GetOutputPort());
        this->Mapper->ImmediateModeRenderingOn();
        
        this->Actor = vtkActor::New();
        CMN_ASSERT(this->Actor);
        this->Actor->SetMapper(this->Mapper);

        this->AddPart(this->Actor);
        return true;
    }


    bool UpdateVTKObjects(void)
    {
        return true;
    }


    void UpdateColor(bool isClutched, bool isPressed, bool is2D)
    {
        if (this->Created()) {
            this->Lock();
            if (isClutched) {
                this->Actor->GetProperty()->SetOpacity(0.5);
            } else {
                this->Actor->GetProperty()->SetOpacity(1.0);
            }
            if (is2D) {
                this->Source->SetRadius(0.9);
                if (isPressed) {
                    this->Actor->GetProperty()->SetColor(0.5, 0.8, 0.8);
                } else {
                    this->Actor->GetProperty()->SetColor(0.5, 1.0, 1.0);
                }
            } else {
                this->Source->SetRadius(1.0);
                if (isPressed) {
                    this->Actor->GetProperty()->SetColor(1.0, 0.8, 0.8);
                } else {
                    this->Actor->GetProperty()->SetColor(1.0, 1.0, 1.0);
                }
            }
            this->Unlock();
        }
    }
};


CMN_DECLARE_SERVICES_INSTANTIATION(ui3CursorSphereTip);
CMN_IMPLEMENT_SERVICES(ui3CursorSphereTip);


class ui3CursorSphereAnchor: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    vtkLineSource * Source;
    vtkPolyDataMapper * Mapper;
    vtkActor * Actor;

public:
    ui3CursorSphereAnchor(void):
        ui3VisibleObject("ui3CursorSphereAnchor"),
        Source(0),
        Mapper(0),
        Actor(0)
    {}
    
    
    ~ui3CursorSphereAnchor()
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
        this->Source = vtkLineSource::New();
        CMN_ASSERT(this->Source);
        // this->Source->SetRadius(2.0);
        
        this->Mapper = vtkPolyDataMapper::New();
        CMN_ASSERT(this->Mapper);
        this->Mapper->SetInputConnection(this->Source->GetOutputPort());
        this->Mapper->ImmediateModeRenderingOn();
        
        this->Actor = vtkActor::New();
        CMN_ASSERT(this->Actor);
        this->Actor->SetMapper(this->Mapper);
        
        this->AddPart(this->Actor);
        
        return true;
    }


    bool UpdateVTKObjects(void)
    {
        return true;
    }


    void SetCursorPosition(vct3 & position)
    {
        this->Lock();
        if (this->Created()) {
        	this->Source->SetPoint2(position.Pointer());
        }
        this->Unlock();
    }

    void SetAnchorPosition(vct3 & position)
    {
        this->Lock();
        if (this->Created()) {
        	this->Source->SetPoint1(position.Pointer());
        }
        this->Unlock();
    }
};

CMN_DECLARE_SERVICES_INSTANTIATION(ui3CursorSphereAnchor);
CMN_IMPLEMENT_SERVICES(ui3CursorSphereAnchor);



ui3CursorSphere::ui3CursorSphere(void):
    ui3CursorBase(),
    IsPressed(false),
    Is2D(false),
    IsClutched(false),
    VisibleTip(0),
    VisibleAnchor(0),
    VisibleList(0)
{
    this->VisibleList = new ui3VisibleList("CursorSphere");
    this->VisibleTip = new ui3CursorSphereTip();
    this->VisibleList->Add(this->VisibleTip);
    this->VisibleAnchor = new ui3CursorSphereAnchor();
    this->VisibleList->Add(this->VisibleAnchor);
    this->VisibleList->Hide();
}


ui3CursorSphere::~ui3CursorSphere()
{
    if (this->VisibleTip) {
        delete this->VisibleTip;
    }
    if (this->VisibleAnchor) {
        delete this->VisibleAnchor;
    }
    if (this->VisibleList) {
        delete this->VisibleList;
    }
}


void ui3CursorSphere::UpdateColor(void)
{
    this->VisibleTip->UpdateColor(this->IsClutched, this->IsPressed, this->Is2D);
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
    return this->VisibleList;
}

void ui3CursorSphere::SetTransformation(vctDoubleFrm3 & frame)
{
    vctDouble3 anchor;
    switch (this->Anchor) {
        case ui3CursorBase::CENTER_RIGHT:
            anchor.Assign(200.0, 0.0, -50.0);
            this->VisibleAnchor->SetAnchorPosition(anchor);
            break;
        case ui3CursorBase::CENTER_LEFT:
            anchor.Assign(-200.0, 0.0, -50.0);
            this->VisibleAnchor->SetAnchorPosition(anchor);
            break;
        default:
            std::cout << "----------------- oops: " << anchor << std::endl;
            break;
    }
    

    this->VisibleTip->SetTransformation(frame);
    this->VisibleAnchor->SetCursorPosition(frame.Translation());

}
