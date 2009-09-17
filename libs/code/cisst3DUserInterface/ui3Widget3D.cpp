/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ui3Widget3D.cpp 137 2009-03-11 18:51:15Z adeguet1 $

  Author(s):	Anton Deguet
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
#include <cisst3DUserInterface/ui3MasterArm.h>

#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSphereSource.h>

CMN_IMPLEMENT_SERVICES(ui3Widget3DHandle);

ui3Widget3DHandle::ui3Widget3DHandle(unsigned int handleNumber,
                                     ui3Widget3D * widget3D):
    ui3Selectable("ui3Widget3DHandle"),
    HandleNumber(handleNumber),
    Widget3D(widget3D),
    Source(0),
    Mapper(0),
    Actor(0)
{}
    
    
ui3Widget3DHandle::~ui3Widget3DHandle()
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


bool ui3Widget3DHandle::CreateVTKObjects(void)
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


void ui3Widget3DHandle::UpdateColor(bool selected)
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


void ui3Widget3DHandle::ShowIntention(void)
{
    if (this->Created()) {
        // this->Lock();
        if (this->Selected()) {
            this->Actor->GetProperty()->SetOpacity(1.0);
            this->Actor->GetProperty()->SetColor(0.0, 1.0, 0.0);
        } else {
            this->Actor->GetProperty()->SetOpacity(0.5 + (this->OverallIntention * 0.5));
            this->Actor->GetProperty()->SetColor(1.0, 1.0 - this->OverallIntention, 1.0 - this->OverallIntention);
        }
        // this->Unlock();
    }
}


double ui3Widget3DHandle::GetIntention(const vctFrm3 & cursorPosition) const
{
    vctDouble3 difference;
    difference.DifferenceOf(cursorPosition.Translation(), this->GetAbsoluteTransformation().Translation());
    double distance = difference.Norm();
    const double threshold = 5.0; // in mm
    if (distance > threshold) {
        return 0.0;
    } else {
        return (1.0 - (distance / threshold)); // normalized between 0 and 1
    } 
}




CMN_IMPLEMENT_SERVICES(ui3Widget3D);


ui3Widget3D::ui3Widget3D(const std::string & name):
    BaseType(name + "-3DWidget"),
    UserObjects(0),
    Handles(0),
    PreviousFirstSideHandle(-1),
    PreviousSecondSideHandle(-1),
    PreviousCornerHandle(-1)
{
    this->UserObjects = new ui3VisibleList(this->Name() + "[user objects]");
    CMN_ASSERT(this->UserObjects);
    BaseType::Add(this->UserObjects);

    this->Handles = new ui3VisibleList(this->Name() + "[handles]");
    CMN_ASSERT(this->Handles);
    BaseType::Add(this->Handles);

    // create handles to be used and add them to base class list
    unsigned int handleCounter;
    ui3Widget3DHandle * handle;
    for (handleCounter = 0;
         handleCounter < 4;
         handleCounter++) {
        handle = new ui3Widget3DHandle(handleCounter, this);
        CMN_ASSERT(handle);
        this->Handles->Add(handle);
        this->SideHandles[handleCounter] = handle;
    }
    for (handleCounter = 0;
         handleCounter < 4;
         handleCounter++) {
        handle = new ui3Widget3DHandle(handleCounter, this);
        CMN_ASSERT(handle);
        this->Handles->Add(handle);
        this->CornerHandles[handleCounter] = handle;
    }

    // set handles as active
    this->SetHandlesActive(true);
}


bool ui3Widget3D::Add(ui3VisibleObject * object)
{
    CMN_ASSERT(this->UserObjects);
    return this->UserObjects->Add(object);
}


void ui3Widget3D::SetSize(double halfSize)
{
    this->SetHandlesSpacing(halfSize);
    unsigned int handleCounter;
    vctDouble3 position;

    const short sides[5] = {-1, 0, 1, 0, -1};
    const short corners[5] = {-1, -1, 1, 1, -1};
    for (handleCounter = 0;
         handleCounter < 4;
         handleCounter++) {
        position.Assign(sides[handleCounter] * halfSize, sides[handleCounter + 1] * halfSize, 0.0);
        CMN_LOG_CLASS_VERY_VERBOSE << "SetSize: set side handle at: " << position << std::endl;
        CMN_ASSERT(this->SideHandles[handleCounter]);
        this->SideHandles[handleCounter]->SetPosition(position);
        position.Assign(corners[handleCounter] * halfSize, corners[handleCounter + 1] * halfSize, 0.0);
        CMN_LOG_CLASS_VERY_VERBOSE << "SetSize: set corner handle at: " << position << std::endl;
        CMN_ASSERT(this->CornerHandles[handleCounter]);
        this->CornerHandles[handleCounter]->SetPosition(position);
    }
}


void ui3Widget3D::SetHandlesActive(bool handlesActive)
{
    this->HandlesActiveMember = handlesActive;
    unsigned int handleCounter;

    if (this->HandlesActive()) {
        for (handleCounter = 0;
             handleCounter < 4;
             handleCounter++) {
            this->SideHandles[handleCounter]->SetActivated(true);
            this->CornerHandles[handleCounter]->SetActivated(true);
        }        
    } else {
        for (handleCounter = 0;
             handleCounter < 4;
             handleCounter++) {
            this->SideHandles[handleCounter]->SetActivated(false);
            this->CornerHandles[handleCounter]->SetActivated(false);
        }        
    }
}


void ui3Widget3D::UpdatePosition(void)
{
    unsigned int handleCounter;
    int firstSideHandle = -1; // invalid number
    int secondSideHandle = -1; // invalid number
    int cornerHandle = -1; // invalid number

    // figure out which handles are used
    for (handleCounter = 0;
         handleCounter < 4;
         handleCounter++) {
        if (this->SideHandles[handleCounter]->Selected()) {
            // save the handle index
            if (firstSideHandle == -1) {
                firstSideHandle = handleCounter;
            } else {
                secondSideHandle = handleCounter;
            }
        }
        if (this->CornerHandles[handleCounter]->Selected()) {
            // save the handle index
            cornerHandle = handleCounter;
        }
    }
    // sanity check, corner always prevails
    if (cornerHandle != -1) {
        firstSideHandle = -1;
        secondSideHandle = -1;
    }

    // test for transition, should re-activate all handles
    if ((firstSideHandle != this->PreviousFirstSideHandle)
        || (secondSideHandle != this->PreviousSecondSideHandle)
        || (cornerHandle != this->PreviousCornerHandle)) {
        // no handle selected, re-activate all
        if ((firstSideHandle == -1) && (cornerHandle == -1)) {
            this->SetHandlesActive(true);
            // apply last known transformation to whole widget but keep the handles in place
            vctFrm3 newPosition;
            this->GetTransformation().ApplyTo(this->CurrentTransformation, newPosition);
            this->SetTransformation(newPosition, false);
            this->UserObjects->SetTransformation(vctFrm3::Identity(), false);
            this->Handles->SetPosition(vct3(0.0));
            this->Handles->SetOrientation(newPosition.Rotation().InverseSelf(), false);
            // this->Handles->SetTransformation(vctFrm3::Identity(), false);
            // this->PositionBeforeManipulation.Assign(this->GetAbsoluteTransformation());
        } else if ((secondSideHandle == -1) && (cornerHandle == -1)){
            // de-activate all but the one opposite direction of selected side handle
            this->SetHandlesActive(false);
            this->SideHandles[(firstSideHandle + 2) % 4]->Show();
            this->SideHandles[(firstSideHandle + 2) % 4]->SetActivated(true);
        } else {
            // de-activate all
            this->SetHandlesActive(false);
        }
    }

    // do the actual computation if needed
    if (cornerHandle != -1) {
        vctDouble3 translation, translationInWorld;
        translation.DifferenceOf(this->CornerHandles[cornerHandle]->FinalPosition.Translation(),
                                 this->CornerHandles[cornerHandle]->InitialPosition.Translation());
        this->GetTransformation().Rotation().ApplyInverseTo(translation, translationInWorld);
        CurrentTransformation.Translation().Assign(translationInWorld);
        CurrentTransformation.Rotation().Assign(vctFrm3::RotationType::Identity());
        this->UserObjects->SetTransformation(CurrentTransformation);
        this->Handles->SetTransformation(CurrentTransformation);
    }
    if (firstSideHandle != -1) {
        if (secondSideHandle == -1) {
            // one handed manipulation

            // get handle displacement
            vctDouble3 center, initial, current;
            center.Assign(this->GetAbsoluteTransformation().Translation());
            initial.DifferenceOf(this->SideHandles[firstSideHandle]->InitialPosition.Translation(),
                                 center);
            initial.NormalizedSelf();
            current.DifferenceOf(this->SideHandles[firstSideHandle]->FinalPosition.Translation(),
                                 center);
            current.NormalizedSelf();

            vctDouble3 axis;
            double initialAngle, currentAngle, angle;
            // rotation along Y
            if ((firstSideHandle == 0) || (firstSideHandle == 2)) {
                initialAngle = atan2(initial.X(), initial.Z());
                currentAngle = atan2(current.X(), current.Z());
                angle = currentAngle - initialAngle;
                axis.Assign(0.0, 1.0, 0.0);
            } else {
                // rotation along X
                initialAngle = atan2(initial.Y(), initial.Z());
                currentAngle = atan2(current.Y(), current.Z());
                angle = initialAngle - currentAngle;
                axis.Assign(1.0, 0.0, 0.0);
            }
            vctDouble3 axisInWorld;
            this->GetTransformation().Rotation().ApplyInverseTo(axis, axisInWorld);
            CurrentTransformation.Translation().SetAll(0.0);
            CurrentTransformation.Rotation().From(vctAxAnRot3(axisInWorld, angle));
            this->UserObjects->SetTransformation(CurrentTransformation);
            //            vctFrm3 handlesTransformation;
            // this->GetTransformation().Rotation().ApplyInverseTo(CurrentTransformation.Rotation(), handlesTransformation.Rotation());
            this->Handles->SetTransformation(CurrentTransformation);
        } else {
            // two handed manipulation
            std::cerr << "two handles manipulation not yet implemented" << std::endl;
        }
    }

    this->PreviousFirstSideHandle = firstSideHandle;
    this->PreviousSecondSideHandle = secondSideHandle;
    this->PreviousCornerHandle = cornerHandle;
}
