/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ImageViewer.cpp 848 2009-09-13 17:59:39Z adeguet1 $

  Author(s):	Anton Deguet, Simon DiMaio
  Created on:	2009-09-13

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>

#include "ImageViewer.h"

#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSphereSource.h>

class ImageViewerVisibleObject: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
public:
    inline ImageViewerVisibleObject(void):
        ui3VisibleObject(),
        Source(0),
        Mapper(0),
        Actor(0),
        Red(true)
    {}

    inline ~ImageViewerVisibleObject()
    {
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
        this->Source = vtkSphereSource::New();
        CMN_ASSERT(this->Source);
        this->Source->SetRadius(5.0);

        this->Mapper = vtkPolyDataMapper::New();
        CMN_ASSERT(this->Mapper);
        this->Mapper->SetInputConnection(this->Source->GetOutputPort());

        this->Actor = vtkActor::New();
        CMN_ASSERT(this->Actor);
        this->Actor->SetMapper(this->Mapper);
        this->Actor->GetProperty()->SetColor(1.0, 0.0, 0.0);

        this->Actor->VisibilityOff();

        this->AddPart(this->Actor);
        return true;
    }

    void ToggleColor(void) {
        if (this->Red) {
            this->Actor->GetProperty()->SetColor(0.1, 0.9, 0.2);
            this->Red = false;
        } else {
            this->Actor->GetProperty()->SetColor(0.9, 0.1, 0.2);
            this->Red = true;
        }
    }

protected:
    vtkSphereSource * Source;
    vtkPolyDataMapper * Mapper;
    vtkActor * Actor;
    vctDouble3 Position; // initial position
    bool Red;
};

CMN_DECLARE_SERVICES_INSTANTIATION(ImageViewerVisibleObject);
CMN_IMPLEMENT_SERVICES(ImageViewerVisibleObject);


ImageViewer::ImageViewer(const std::string & name):
    ui3BehaviorBase(std::string("ImageViewer::") + name, 0),
    Following(false),
    Widget3D(0),
    VisibleObject1(0),
    VisibleObject2(0),
    Counter(0.0)
{
    this->Widget3D = new ui3Widget3D("ImageViewer");

    this->VisibleObject1 = new ImageViewerVisibleObject();
    //    this->Widget3D->Add(this->VisibleObject1);

    this->VisibleObject2 = new ImageViewerVisibleObject();
    // this->Widget3D->Add(this->VisibleObject2);
    
    CMN_ASSERT(this->Widget3D);
}


ImageViewer::~ImageViewer()
{
    if (this->Widget3D) {
        delete this->Widget3D;
    }
}


void ImageViewer::ToggleColor()
{
    if (this->VisibleObject1) {
        this->VisibleObject1->ToggleColor();
    }
    if (this->VisibleObject2) {
        this->VisibleObject2->ToggleColor();
    }
}


void ImageViewer::ConfigureMenuBar()
{
    this->MenuBar->AddClickButton("ToggleColor",
                                  1,
                                  "redo.png",
                                  &ImageViewer::ToggleColor,
                                  this);
}


bool ImageViewer::RunForeground()
{
    if (this->Manager->MastersAsMice() != this->PreviousMaM) {
        this->PreviousMaM = this->Manager->MastersAsMice();
        this->Widget3D->Show();
    }

    // detect transition, should that be handled as an event?
    // State is used by multiple threads ...
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
        this->Widget3D->Show();
    }
    // running in foreground GUI mode
    prmPositionCartesianGet position;
    this->GetPrimaryMasterPosition(position);
    if (this->Following) {
        vctDouble3 deltaCursor;
        deltaCursor.DifferenceOf(position.Position().Translation(),
                                 this->PreviousCursorPosition);
        this->Position.Add(deltaCursor);
        this->Widget3D->SetPosition(this->Position);
        this->Widget3D->SetOrientation(position.Position().Rotation());
    }
    this->PreviousCursorPosition.Assign(position.Position().Translation());
    this->UpdateRelativePosition();
    return true;
}

bool ImageViewer::RunBackground()
{
    // detect transition
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
        this->Widget3D->Hide();
    }
    this->UpdateRelativePosition();
    return true;
}

bool ImageViewer::RunNoInput()
{
    if (this->Manager->MastersAsMice() != this->PreviousMaM) {
        this->PreviousMaM = this->Manager->MastersAsMice();
        this->Widget3D->Hide();
    }
    return true;
}


void ImageViewer::OnQuit()
{
    this->Widget3D->Hide();
}


void ImageViewer::OnStart()
{
    this->Position.X() = 0.0;
    this->Position.Y() = 0.0;
    this->Position.Z() = -100.0;
    this->Widget3D->SetPosition(this->Position);
    this->Widget3D->SetSize(20.0);
    this->Widget3D->Show();
}


void ImageViewer::PrimaryMasterButtonCallback(const prmEventButton & event)
{
    if (event.Type() == prmEventButton::PRESSED) {
        this->Following = true;
        CMN_LOG_CLASS_VERY_VERBOSE << "Primary master button pressed, following started" << std::endl;
    } else if (event.Type() == prmEventButton::RELEASED) {
        this->Following = false;
        CMN_LOG_CLASS_VERY_VERBOSE << "Primary master button pressed, following ended" << std::endl;
    }
}


void ImageViewer::UpdateRelativePosition(void)
{
    this->Counter += 0.01;
    this->VisibleObject2->SetPosition(vctDouble3(0.0, 20.0 * sin(Counter), 0.0));
}
