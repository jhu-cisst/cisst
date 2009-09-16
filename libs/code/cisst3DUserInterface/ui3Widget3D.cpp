/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ui3Widget3D.cpp 137 2009-03-11 18:51:15Z adeguet1 $

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


#include <cisst3DUserInterface/ui3Widget3D.h>
#include <cisst3DUserInterface/ui3SceneManager.h>
#include <cisst3DUserInterface/ui3VisibleList.h>

#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSphereSource.h>


CMN_IMPLEMENT_SERVICES(ui3Widget3DRotationHandle);


ui3Widget3DRotationHandle::ui3Widget3DRotationHandle():
    ui3Selectable("ui3Widget3DRotationHandle"),
    Source(0),
    Mapper(0),
    Actor(0)
{}
    
    
ui3Widget3DRotationHandle::~ui3Widget3DRotationHandle()
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


bool ui3Widget3DRotationHandle::CreateVTKObjects(void)
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
    
    this->AddPart(this->Actor);
    return true;
}


void ui3Widget3DRotationHandle::UpdateColor(bool selected)
{
    if (this->Created()) {
        this->Lock();
        if (selected) {
            this->Actor->GetProperty()->SetOpacity(1.0);
        } else {
            this->Actor->GetProperty()->SetOpacity(0.5);
        }
        this->Unlock();
    }
}


void ui3Widget3DRotationHandle::ShowIntention(double intention)
{
    if (this->Created()) {
        // this->Lock();
        if (this->Selected()) {
            this->Actor->GetProperty()->SetOpacity(1.0);
            this->Actor->GetProperty()->SetColor(0.0, 1.0, 0.0);
        } else {
            this->Actor->GetProperty()->SetOpacity(0.5 + (intention * 0.5));
            this->Actor->GetProperty()->SetColor(1.0, 1.0 - intention, 1.0 - intention);
        }
        // this->Unlock();
    }
}


double ui3Widget3DRotationHandle::GetIntention(const vctFrm3 & cursorPosition) const
{
    vctDouble3 difference;
    difference.DifferenceOf(cursorPosition.Translation(), this->GetAbsoluteTransformation().Translation());
    double distance = difference.Norm();
    const double threshold = 20;
    if (distance > threshold) {
        return 0.0;
    } else {
        return (1.0 - (distance / threshold)); // normalized between 0 and 1
    } 
}


CMN_IMPLEMENT_SERVICES(ui3Widget3D);


ui3Widget3D::ui3Widget3D(const std::string & name):
    BaseType(name + "-3DWidget"),
    UserObjects(0)
{
    this->UserObjects = new ui3VisibleList(this->Name() + "[user objects]");
    CMN_ASSERT(this->UserObjects);
    BaseType::Add(this->UserObjects);

    this->Handles = new ui3VisibleList(this->Name() + "[handles]");
    CMN_ASSERT(this->Handles);
    BaseType::Add(this->Handles);

    // create handles to be used and add them to base class list
    unsigned int handleCounter;
    ui3Widget3DRotationHandle * handle;
    for (handleCounter = 0;
         handleCounter < 4;
         handleCounter++) {
        handle = new ui3Widget3DRotationHandle();
        CMN_ASSERT(handle);
        this->Handles->Add(handle);
        this->RotationHandles[handleCounter] = handle;
    }
}


bool ui3Widget3D::Add(ui3VisibleObject * object)
{
    CMN_ASSERT(this->UserObjects);
    return this->UserObjects->Add(object);
}


void ui3Widget3D::SetSize(double size)
{
    this->SetRotationHandlesSpacing(size);
    unsigned int handleCounter;
    vctDouble3 position;

    const short orientation[5] = {-1, 0, 1, 0, -1};
    
    for (handleCounter = 0;
         handleCounter < 4;
         handleCounter++) {
        position.Assign(orientation[handleCounter] * size, orientation[handleCounter + 1] * size, 0.0);
        CMN_LOG_CLASS_VERY_VERBOSE << "SetSize: position rotation handle at: " << position << std::endl;
        CMN_ASSERT(this->RotationHandles[handleCounter]);
        this->RotationHandles[handleCounter]->SetPosition(position);
    }
}
