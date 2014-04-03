/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2008-06-10

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
    this->Source->SetRadius(0.5);

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


void ui3Widget3DHandle::UpdateColor(bool selected)
{
#if 0
    if (this->Created()) {
        this->Lock();
        if (selected) {
            this->Actor->GetProperty()->SetOpacity(1.0);
        } else {
            this->Actor->GetProperty()->SetOpacity(1.0);
        }
        this->Unlock();
    }
#endif
}


void ui3Widget3DHandle::ShowIntention(void)
{
    if (this->Created()) {
        // this->Lock();
        if (this->Selected()) {
            this->Actor->GetProperty()->SetOpacity(1.0);
            this->Actor->GetProperty()->SetColor(0.0, 1.0, 0.0);
        } else {
            //            this->Actor->GetProperty()->SetOpacity(0.5 + (this->OverallIntention * 0.5));
            this->Actor->GetProperty()->SetOpacity(1.0);
            this->Actor->GetProperty()->SetColor(1.0, 1.0 - this->OverallIntention, 1.0 - this->OverallIntention);
        }
        // this->Unlock();
    }
}


double ui3Widget3DHandle::GetIntention(const vctFrm3 & cursorPosition) const
{
    vctDouble3 difference;
    difference.DifferenceOf(cursorPosition.Translation(), this->GetAbsoluteTransformation().Translation());
    difference.Z() /= 3.0;
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
        CMN_LOG_CLASS_RUN_DEBUG << "SetSize: set side handle at: " << position << std::endl;
        CMN_ASSERT(this->SideHandles[handleCounter]);
        this->SideHandles[handleCounter]->SetPosition(position);
        position.Assign(corners[handleCounter] * halfSize, corners[handleCounter + 1] * halfSize, 0.0);
        CMN_LOG_CLASS_RUN_DEBUG << "SetSize: set corner handle at: " << position << std::endl;
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
            //vctFrm3 newPosition;
            //newPosition.Assign(this->GetTransformation());
            //this->SetTransformation(newPosition, false);
            //this->UserObjects->SetTransformation(vctFrm3::Identity(), false);
            //this->Handles->SetPosition(vct3(0.0));
            //this->Handles->SetOrientation(newPosition.Rotation().InverseSelf(), false);
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
        translation.DifferenceOf(this->CornerHandles[cornerHandle]->CurrentPosition.Translation(),
                                 this->CornerHandles[cornerHandle]->PreviousPosition.Translation());
        this->GetTransformation().Rotation().ApplyInverseTo(translation, translationInWorld);
        CurrentTransformation.Translation().Assign(translationInWorld);
        CurrentTransformation.Rotation().Assign(vctFrm3::RotationType::Identity());

        // apply last known transformation to whole widget but keep the handles in place
        vctFrm3 newPosition;
        this->GetTransformation().ApplyTo(this->CurrentTransformation, newPosition);
        this->SetTransformation(newPosition, false);
        // this->UserObjects->SetTransformation(vctFrm3::Identity(), false);
        this->Handles->SetPosition(vct3(0.0));
        this->Handles->SetOrientation(newPosition.Rotation().InverseSelf(), false);
    }
    if (firstSideHandle != -1) {
        vctDouble3 axis;
        double angle;
        axis.Assign(1.0,0.0,0.0);
        angle = 0.0;

        if (secondSideHandle == -1) {
            /*
            // one handed manipulation

            // get handle displacement
            vctDouble3 center, initial, current;
            center.Assign(this->GetAbsoluteTransformation().Translation());
            initial.DifferenceOf(this->SideHandles[firstSideHandle]->PreviousPosition.Translation(),
            center);
            initial.NormalizedSelf();
            current.DifferenceOf(this->SideHandles[firstSideHandle]->CurrentPosition.Translation(),
            center);
            current.NormalizedSelf();

            double initialAngle, currentAngle;
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
            */
        } else {
            // two handed manipulation
            //std::cerr << "Two handles manipulation not yet implemented. Why" << std::endl;
            double object_displacement[3], object_rotation[4];
            ComputeTransform(this->SideHandles[firstSideHandle]->PreviousPosition.Translation().Pointer(),
                             this->SideHandles[secondSideHandle]->PreviousPosition.Translation().Pointer(),
                             this->SideHandles[firstSideHandle]->CurrentPosition.Translation().Pointer(),
                             this->SideHandles[secondSideHandle]->CurrentPosition.Translation().Pointer(),
                             object_displacement, object_rotation);

            // Set the Translation.
            vctDouble3 translation, translationInWorld;
            translation.Assign(object_displacement);
            this->GetTransformation().Rotation().ApplyInverseTo(translation, translationInWorld);
            CurrentTransformation.Translation().Assign(translationInWorld);

            // Set the Rotation.
            angle = object_rotation[0];
            axis.Assign(object_rotation+1);
            vctDouble3 axisInWorld;
            this->GetTransformation().Rotation().ApplyInverseTo(axis, axisInWorld);
            CurrentTransformation.Rotation().From(vctAxAnRot3(axisInWorld, angle));

            // apply last known transformation to whole widget but keep the handles in place
            vctFrm3 newPosition;
            this->GetTransformation().ApplyTo(this->CurrentTransformation, newPosition);
            this->SetTransformation(newPosition, false);
            // this->UserObjects->SetTransformation(vctFrm3::Identity(), false);
            this->Handles->SetPosition(vct3(0.0));
            this->Handles->SetOrientation(newPosition.Rotation().InverseSelf(), false);
        }
    }

    this->PreviousFirstSideHandle = firstSideHandle;
    this->PreviousSecondSideHandle = secondSideHandle;
    this->PreviousCornerHandle = cornerHandle;
}


/*--------------------------------------------------------------------------------------*/


/*!
  Compute the object transform from the motion of two grabbed control points.
  @param pointa               Right control position.
  @param pointb               Left control position.
  @param point1               Right cursor pos.
  @param point2               Left cursor pos.
  @param object_displacement  [dx, dy, dz]
  @param object_rotation      [angle, axis_x, axis_y, axis_z]
*/
void ui3Widget3D::ComputeTransform(double pointa[3], double pointb[3],
                                   double point1[3], double point2[3],
                                   double object_displacement[3],
                                   double object_rotation[4])
{
    double v1[3], v2[3], v1norm, v2norm, wnorm;
    double w[3], angle, dotarg;

    //cout << "pointa: " << pointa[0] << " " << pointa[1] << " " << pointa[2] << endl;
    //cout << "pointb: " << pointb[0] << " " << pointb[1] << " " << pointb[2] << endl;

    // v1 = ((pb-pa)/norm(pb-pa))
    v1[0] = pointb[0]-pointa[0];
    v1[1] = pointb[1]-pointa[1];
    v1[2] = pointb[2]-pointa[2];
    v1norm = sqrt(v1[0]*v1[0]+v1[1]*v1[1]+v1[2]*v1[2]);
    if(v1norm>cmnTypeTraits<double>::Tolerance())
        {
            v1[0] /= v1norm;
            v1[1] /= v1norm;
            v1[2] /= v1norm;
        }

    // v2 = ((p2-p1)/norm(p2-p1))
    v2[0] = point2[0]-point1[0];
    v2[1] = point2[1]-point1[1];
    v2[2] = point2[2]-point1[2];
    v2norm = sqrt(v2[0]*v2[0]+v2[1]*v2[1]+v2[2]*v2[2]);
    if(v2norm>cmnTypeTraits<double>::Tolerance())
        {
            v2[0] /= v2norm;
            v2[1] /= v2norm;
            v2[2] /= v2norm;
        }

    // w = (v1 x v2)/norm(v1 x v2)
    w[0] = v1[1]*v2[2] - v1[2]*v2[1];
    w[1] = v1[2]*v2[0] - v1[0]*v2[2];
    w[2] = v1[0]*v2[1] - v1[1]*v2[0];
    wnorm = sqrt(w[0]*w[0]+w[1]*w[1]+w[2]*w[2]);
    if(wnorm> cmnTypeTraits<double>::Tolerance())
        {
            w[0] /= wnorm;
            w[1] /= wnorm;
            w[2] /= wnorm;
        }
    else {
        w[0] = 1.0;
        w[1] = w[2] = 0.0;
    }

    // theta = arccos(v1.v2/(norm(v1)*norm(v2))
    dotarg = v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2];
    if(dotarg>-1.0 && dotarg<1.0)
        angle = acos(dotarg);
    else
        angle = 0.0;
    //if(CMN_ISNAN(angle)) angle=0.0;

    cout << "v1: " << v1[0] << " " << v1[1] << " " << v1[2] << endl;
    cout << "v2: " << v2[0] << " " << v2[1] << " " << v2[2] << endl;
    cout << "w: " << w[0] << " " << w[1] << " " << w[2] << " angle: " << angle*180.0/cmnPI << endl;

    // Set object pose updates.
    object_displacement[0] = (point1[0]+point2[0])/2 - (pointa[0]+pointb[0])/2;
    object_displacement[1] = (point1[1]+point2[1])/2 - (pointa[1]+pointb[1])/2;
    object_displacement[2] = (point1[2]+point2[2])/2 - (pointa[2]+pointb[2])/2;

    object_rotation[0] = angle;
    object_rotation[1] = w[0];
    object_rotation[2] = w[1];
    object_rotation[3] = w[2];
}

/*--------------------------------------------------------------------------------------*/

