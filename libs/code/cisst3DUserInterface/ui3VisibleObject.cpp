/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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


#include <cisst3DUserInterface/ui3VisibleObject.h>

#include <vtkAssembly.h>
#include <vtkProperty.h>
#include <vtkMatrix4x4.h>


ui3VisibleObject::ui3VisibleObject(void):
    Assembly(0),
    Matrix(0),
    SceneManager(0),
    VTKHandle(0),
    Created(false),
    Visible(true)
{
    this->Assembly = vtkAssembly::New();
    CMN_ASSERT(this->Assembly);

    this->Matrix = vtkMatrix4x4::New();
    CMN_ASSERT(this->Matrix);
    this->Matrix->Identity();

    this->Assembly->SetUserMatrix(this->Matrix);
}


vtkProp3D * ui3VisibleObject::GetVTKProp(void)
{
    return this->Assembly;
}


void ui3VisibleObject::Show(void)
{
    this->Visible = true;
    this->ShowFromParent();
}


void ui3VisibleObject::Hide(void)
{
    this->Visible = false;
    this->HideFromParent();
}


void ui3VisibleObject::HideFromParent(void)
{
    unsigned int index;
    for (index = 0; index < Parts.size(); index++) {
        Parts[index]->VisibilityOff();
    }
}


void ui3VisibleObject::ShowFromParent(void)
{
    unsigned int index;
    for (index = 0; index < Parts.size(); index++) {
        if (this->Visible) {
            Parts[index]->VisibilityOn();
        } else {
            Parts[index]->VisibilityOff();
        }
    }
}


void ui3VisibleObject::SetPosition(const vctDouble3 & position)
{
    if (this->Created) {
        this->Lock();
        this->Matrix->SetElement(0, 3, position.X());
        this->Matrix->SetElement(1, 3, position.Y());
        this->Matrix->SetElement(2, 3, position.Z());
        this->Unlock();
    } else {
        CMN_LOG_CLASS_RUN_DEBUG << "SetPosition: called on object not yet created" << std::endl;
    }
}


void ui3VisibleObject::SetOrientation(const vctMatRot3 & rotationMatrix)
{
    if (this->Created) {
        unsigned int i, j;
        this->Lock();
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                this->Matrix->SetElement(i, j, rotationMatrix.Element(i, j));
            }
        }
        this->Unlock();
    } else {
        CMN_LOG_CLASS_RUN_DEBUG << "SetOrientation: called on object not yet created" << std::endl;
    }
}


void ui3VisibleObject::SetScale(const double & scale)
{
    if (this->Created) {
        this->Lock();
        this->Assembly->SetScale(scale);
        this->Unlock();
    } else {
        CMN_LOG_CLASS_RUN_DEBUG << "SetScale: called on object not yet created" << std::endl;
    }
}


vctDoubleFrm3 ui3VisibleObject::GetTransformation(void) const
{
    vctDoubleFrm3 result;
    unsigned int i, j;
    for (i = 0; i < 3; i++) {
        result.Translation().Element(i) = this->Matrix->GetElement(i, 3);
        for (j = 0; j < 3; j++) {
            result.Rotation().Element(i, j) = this->Matrix->GetElement(i, j);
        }
    }
    return result;
}


void ui3VisibleObject::SetVTKMatrix(vtkMatrix4x4* matrix)
{
    if (this->Created) {
        this->Matrix->DeepCopy(matrix);
    } else {
        CMN_LOG_CLASS_RUN_DEBUG << "SetVTKMatrix: called on object not yet created" << std::endl;
    }
}


void ui3VisibleObject::Lock(void)
{
    if (this->Created) {
        this->SceneManager->Lock(this->VTKHandle);
    } else {
        CMN_LOG_CLASS_RUN_DEBUG << "Lock: attempt to lock with an object not yet added to the scene" << std::endl;
    }
}


void ui3VisibleObject::Unlock(void)
{
    if (this->Created) {
        this->SceneManager->Unlock(this->VTKHandle);
    } else {
        CMN_LOG_CLASS_RUN_DEBUG << "Unlock: attempt to unlock with an object not yet added to the scene" << std::endl;
    }
}


void ui3VisibleObject::AddPart(vtkProp3D * part)
{
    this->Assembly->AddPart(part);
    this->Parts.push_back(part);
}
