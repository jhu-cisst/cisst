/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ui3VisibleObject.cpp,v 1.2 2009/02/23 16:55:06 anton Exp $

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


#include <cisst3DUserInterface/ui3MenuBar.h>
#include <cisst3DUserInterface/ui3Manager.h>

#include <vtkAssembly.h>
#include <vtkProperty.h>



ui3VisibleObject::ui3VisibleObject(ui3Manager * manager):
    Assembly(0),
    Manager(manager),
    VTKHandle(0)
{
    this->Assembly = vtkAssembly::New();
    CMN_ASSERT(this->Assembly);
}


vtkProp3D * ui3VisibleObject::GetVTKProp(void)
{
    return this->Assembly;
}


void ui3VisibleObject::Show()
{
    this->Lock();
    this->Assembly->VisibilityOn();
    this->Unlock();
}


void ui3VisibleObject::Hide()
{
    this->Lock();
    this->Assembly->VisibilityOff();
    this->Unlock();
}


void ui3VisibleObject::SetPosition(vctDouble3 & position)
{
    this->Lock();
    this->Assembly->SetPosition(position.Pointer());
    this->Unlock();
}


void ui3VisibleObject::SetOrientation(vctMatRot3 & rotationMatrix)
{
    vctDoubleAxAnRot3 axisAngle(rotationMatrix, VCT_DO_NOT_NORMALIZE);
    this->Lock();
    this->Assembly->RotateWXYZ(axisAngle.Angle() * cmn180_PI,
                               axisAngle.Axis().X(),
                               axisAngle.Axis().Y(),
                               axisAngle.Axis().Z());
    this->Unlock();
}


void ui3VisibleObject::SetTransformation(vctDoubleFrm3 & frame)
{
    this->SetPosition(frame.Translation());
    this->SetOrientation(frame.Rotation());
}


void ui3VisibleObject::Lock(void)
{
    if (this->VTKHandle) {
        this->Manager->GetSceneManager()->Lock(this->VTKHandle);
    } else {
        CMN_LOG_CLASS(5) << "Lock: attempt to lock with an object not yet added to the scene" << std::endl;
    }
}


void ui3VisibleObject::Unlock(void)
{
    if (this->VTKHandle) {
        this->Manager->GetSceneManager()->Unlock(this->VTKHandle);
    } else {
        CMN_LOG_CLASS(5) << "Unlock: attempt to unlock with an object not yet added to the scene" << std::endl;
    }
}