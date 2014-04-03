/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ali Uneri
  Created on: 2009-09-17

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "RegistrationBehavior.h"

#define NUM_FIDUCIALS 3


// registration model fiducials
// ----------------------------------------------------------------------------
class RegistrationModelFiducials: public ui3Selectable
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    inline RegistrationModelFiducials(unsigned int handleNumber):
        ui3Selectable("RegistrationModelFiducials"),
        HandleNumber(handleNumber),
        Source(0),
        Mapper(0),
        Actor(0) {}

    inline ~RegistrationModelFiducials(void) {
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

    inline bool CreateVTKObjects(void) {
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

    inline bool UpdateVTKObjects(void) {
        return true;
    }

    double GetIntention(const vctFrm3 & cursorPosition) const {
        vct3 difference;
        difference.DifferenceOf(cursorPosition.Translation(), this->GetAbsoluteTransformation().Translation());
        double distance = difference.Norm();
        const double threshold = 5.0;  // in mm
        if (distance > threshold) {
            return 0.0;
        } else {
            return (1.0 - (distance / threshold));  // normalized between 0 and 1
        }
    }

    inline void ShowIntention(void) {
        if (this->Created()) {
            this->Lock();
            if (this->Selected()) {
                this->Actor->GetProperty()->SetOpacity(1.0);
                this->Actor->GetProperty()->SetColor(0.0, 1.0, 0.0);
            } else {
                this->Actor->GetProperty()->SetOpacity(0.5 + (this->OverallIntention * 0.5));
                this->Actor->GetProperty()->SetColor(1.0, 1.0 - this->OverallIntention, 1.0 - this->OverallIntention);
            }
            this->Unlock();
        }
    }

    inline void OnSelect(void) {}
    inline void OnRelease(void) {}

protected:
    unsigned int HandleNumber;
    ui3Widget3D * Widget3D;
    vtkSphereSource * Source;
    vtkPolyDataMapper * Mapper;
    vtkActor * Actor;
};

CMN_DECLARE_SERVICES_INSTANTIATION(RegistrationModelFiducials);
CMN_IMPLEMENT_SERVICES(RegistrationModelFiducials);


// registration model
// ----------------------------------------------------------------------------
class RegistrationModel: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
    friend class RegistrationBehavior;

public:
    inline RegistrationModel(void):
        ui3VisibleObject(),
        Source(0),
        Mapper(0),
        Actor(0) {}

    inline ~RegistrationModel() {
        if (this->Actor) {
            this->Actor->Delete();
        }
        if (this->Mapper) {
            this->Mapper->Delete();
        }
        if (this->Source) {
            this->Source->Delete();
        }
    }

    inline bool CreateVTKObjects(void) {
        vtkVRMLImporter * vrml = vtkVRMLImporter::New();
        vrml->SetFileName("ui3Skull.wrl");
        vrml->Read();
        vtkActorCollection * actors = vrml->GetRenderer()->GetActors();
        actors->InitTraversal();
        for (int i = 0; i < actors->GetNumberOfItems(); i++) {
            this->Actor = actors->GetNextActor();
            this->AddPart(this->Actor);
        }
        return true;
    }

    inline bool UpdateVTKObjects(void) {
        return true;
    }

protected:
    vtkCubeSource * Source;
    vtkPolyDataMapper * Mapper;
    vtkActor * Actor;
    vct3 Position;  // initial position
};

CMN_DECLARE_SERVICES_INSTANTIATION(RegistrationModel);
CMN_IMPLEMENT_SERVICES(RegistrationModel);


// registration behavior
// ----------------------------------------------------------------------------
CMN_IMPLEMENT_SERVICES(RegistrationBehavior);

RegistrationBehavior::RegistrationBehavior(const std::string & name):
    ui3BehaviorBase(std::string("RegistrationBehavior::") + name, 0),
    Widget3D(0),
    VisibleObject1(0),
    ModelFiducials(0)
{
    this->Widget3D = new ui3Widget3D("RegistrationBehavior");
    CMN_ASSERT(this->Widget3D);
    AddWidget3D(this->Widget3D);

    this->VisibleObject1 = new RegistrationModel();
    CMN_ASSERT(this->VisibleObject1);
    this->Widget3D->Add(this->VisibleObject1);

    this->ModelFiducials = new ui3VisibleList("RegistrationModelFiducials");
    CMN_ASSERT(this->ModelFiducials);
    this->Widget3D->Add(this->ModelFiducials);
}


RegistrationBehavior::~RegistrationBehavior()
{
    if (this->Widget3D) {
        delete this->Widget3D;
    }
}


void RegistrationBehavior::ConfigureMenuBar(void)
{
    this->MenuBar->AddClickButton("Move",
                                  0,
                                  "move.png",
                                  &RegistrationBehavior::ToggleHandles,
                                  this);
    this->MenuBar->AddClickButton("Register",
                                  1,
                                  "map.png",
                                  &RegistrationBehavior::Register,
                                  this);
}


bool RegistrationBehavior::RunForeground(void)
{
    if (this->Manager->MastersAsMice() != this->PreviousMaM) {
        this->PreviousMaM = this->Manager->MastersAsMice();
        this->Widget3D->Show();
        this->Widget3D->SetHandlesActive(true);
    }

    // detect transition, should that be handled as an event?
    // State is used by multiple threads ...
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
        this->Widget3D->Show();
        this->Widget3D->SetHandlesActive(true);
    }
    return true;
}


bool RegistrationBehavior::RunBackground(void)
{
    // detect transition
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
        this->Widget3D->Show();
        this->Widget3D->SetHandlesActive(false);
    }
    return true;
}


bool RegistrationBehavior::RunNoInput(void)
{
    if (this->Manager->MastersAsMice() != this->PreviousMaM) {
        this->PreviousMaM = this->Manager->MastersAsMice();
        this->Widget3D->Hide();
    }
    return true;
}


void RegistrationBehavior::OnQuit(void)
{
    this->Widget3D->Hide();
}

void RegistrationBehavior::OnStart(void)
{
    // adjust scaling
    //this->VisibleObject1->Actor->SetScale(0.25);

    // adjust position offsets
    vct6 bounds;
    this->VisibleObject1->Lock();
    bounds.Assign(this->VisibleObject1->Actor->GetBounds());
    this->VisibleObject1->Unlock();

    vct3 offset;
    for (unsigned int i = 0; i < offset.size(); i++) {
        offset[i] = (bounds[2*i] + bounds[(2*i)+1]) / -2.0;
    }
    this->VisibleObject1->Actor->AddPosition(offset.Pointer());

    // adjust orientation offsets
    double thetaX = -60.0 * cmnPI_180;
    double thetaY = 0.0 * cmnPI_180;
    double thetaZ = 0.0 * cmnPI_180;
    vctRot3 rotateX(
        1.0, 0.0, 0.0,
        0.0, cos(thetaX), -sin(thetaX),
        0.0, sin(thetaX), cos(thetaX));
    vctRot3 rotateY(
        cos(thetaY), 0.0, sin(thetaY),
        0.0, 1.0, 0.0,
        -sin(thetaY), 0.0, cos(thetaY));
    vctRot3 rotateZ(
        cos(thetaZ), -sin(thetaZ), 0.0,
        sin(thetaZ), cos(thetaZ), 0.0,
        0.0, 0.0, 1.0);
    vctRot3 rotateZYX;
    rotateZYX = rotateZ * rotateY * rotateX;
    this->VisibleObject1->SetOrientation(rotateZYX);

    this->ModelFiducials->SetPosition(vct3(0.0));
    this->ModelFiducials->Show();

    this->Position.Assign(0.0, 0.0, -500.0);
    this->Widget3D->SetPosition(this->Position);
    this->Widget3D->SetSize(150.0);
    this->Widget3D->Show();
}


void RegistrationBehavior::PrimaryMasterButtonCallback(const prmEventButton & event)
{
    if (this->Manager->MastersAsMice() &&
        event.Type() == prmEventButton::RELEASED) {

        prmPositionCartesianGet position;
        this->GetPrimaryMasterPosition(position);
        size_t fiducialID;

        if (VirtualFiducials.size() < NUM_FIDUCIALS) {
            fiducialID = VirtualFiducials.size();
            VirtualFiducials.resize(fiducialID + 1);
            VirtualFiducials[fiducialID] = this->Widget3D->GetAbsoluteTransformation().Inverse() *
                                           position.Position().Translation();

            //RegistrationModelFiducials * modelFiducial = new RegistrationModelFiducials(fiducialID);
            //this->ModelFiducials->Add(modelFiducial);
            //this->ModelFiducials->SetPosition(vct3(0.0));
            //this->ModelFiducials->GetLast()->SetPosition(VirtualFiducials[fiducialID]);
            //this->ModelFiducials->GetLast()->Show();

            CMN_LOG_CLASS_RUN_VERBOSE << "PrimaryMasterButtonCallback: added virtual fiducial: "
                                      << VirtualFiducials[fiducialID] << std::endl;
        } else if (RealFiducials.size() < NUM_FIDUCIALS) {
            fiducialID = RealFiducials.size();
            RealFiducials.resize(fiducialID + 1);
            RealFiducials[fiducialID] = this->Widget3D->GetAbsoluteTransformation().Inverse() *
                                        position.Position().Translation();

            CMN_LOG_CLASS_RUN_VERBOSE << "PrimaryMasterButtonCallback: added real fiducial: "
                                      << RealFiducials[fiducialID] << std::endl;
        }
    }
}


void RegistrationBehavior::ToggleHandles(void)
{
    if (this->Widget3D->HandlesActive()) {
        this->Widget3D->SetHandlesActive(false);
    } else {
        this->Widget3D->SetHandlesActive(true);
    }
}


void RegistrationBehavior::Register(void)
{
    if (VirtualFiducials.size() < NUM_FIDUCIALS) {
        CMN_LOG_CLASS_RUN_VERBOSE << "Register: select more virtual fiducials" << std::endl;
    } else if (RealFiducials.size() < NUM_FIDUCIALS) {
        CMN_LOG_CLASS_RUN_VERBOSE << "Register: collect more real fiducials" << std::endl;
    } else {
        nmrRegistrationRigid(VirtualFiducials, RealFiducials, Registration);
        this->VisibleObject1->SetTransformation(Registration);
    }
}
