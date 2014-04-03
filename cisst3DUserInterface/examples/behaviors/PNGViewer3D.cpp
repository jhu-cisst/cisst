/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

Author(s):	Anton Deguet
Created on:	2011-04-25

(C) Copyright 2011 Johns Hopkins University (JHU), All Rights
Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <PNGViewer3D.h>

#include <vtkProperty.h>
#include <vtkImageData.h>
#include <vtkPNGReader.h>
#include <vtkTexture.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>

#include <cisst3DUserInterface/ui3Manager.h>

class PNGViewer3DVisibleObject: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
public:
    inline PNGViewer3DVisibleObject(const std::string & inputFile):
        ui3VisibleObject(),
        InputFile(inputFile),
        ImageSize(20.0),
        PNGReader(0),
		Texture(0),
        PlaneSource(0),
        Mapper(0),
        Actor(0)
    {}

    inline ~PNGViewer3DVisibleObject()
    {
    }

    inline bool CreateVTKObjects(void) {
        this->PNGReader = vtkPNGReader::New();
        CMN_ASSERT(this->PNGReader);
        this->PNGReader->SetFileName(this->InputFile.c_str());

        // Create a texture object for the button.
        this->Texture = vtkTexture::New();
        CMN_ASSERT(this->Texture);
        this->Texture->SetInput(this->PNGReader->GetOutput());
        this->Texture->InterpolateOn();

        // Create the button plane
        this->PlaneSource = vtkPlaneSource::New();
        CMN_ASSERT(this->PlaneSource);
        double halfSize = this->ImageSize / 2.0;
        this->PlaneSource->SetOrigin(-halfSize, -halfSize, 0.0);
        this->PlaneSource->SetPoint1(+halfSize, -halfSize, 0.0);
        this->PlaneSource->SetPoint2(-halfSize, +halfSize, 0.0);
        this->PlaneSource->SetCenter(0.0, 0.0, 0.0);
        this->PlaneSource->SetXResolution(1.0);
        this->PlaneSource->SetYResolution(1.0);

        // Create a polygon mapper for the button.
        this->Mapper = vtkPolyDataMapper::New();
        CMN_ASSERT(this->Mapper);
        this->Mapper->SetInput(this->PlaneSource->GetOutput());
        this->Mapper->ImmediateModeRenderingOn();

        // Create an actor for the button.
        this->Actor = vtkActor::New();
        CMN_ASSERT(this->Actor);
        this->Actor->SetMapper(this->Mapper);
        this->Actor->SetTexture(this->Texture);
        this->Actor->GetProperty()->SetOpacity(1.0);
        this->Actor->VisibilityOff();

        this->AddPart(this->Actor);
        return true;
	}

    inline bool UpdateVTKObjects(void) {
    return true;
    }

protected:
    std::string InputFile;
    double ImageSize;
    vtkPNGReader * PNGReader;
    vtkTexture * Texture;
    vtkPlaneSource * PlaneSource;
    vtkPolyDataMapper * Mapper;
    vtkActor * Actor;
};


CMN_DECLARE_SERVICES_INSTANTIATION(PNGViewer3DVisibleObject);
CMN_IMPLEMENT_SERVICES(PNGViewer3DVisibleObject);



PNGViewer3D::PNGViewer3D(const std::string & name, const std::string & fileName):
    ui3BehaviorBase(std::string("PNGViewer3D-") + name, 0),
    Widget3D(0),
    VisibleObject(0),
    Widget3DHandlesActive(true)
{
    this->Widget3D = new ui3Widget3D("PNGViewer3D");
    CMN_ASSERT(this->Widget3D);
    this->AddWidget3D(this->Widget3D);
    this->VisibleObject = new PNGViewer3DVisibleObject(fileName);
    this->Widget3D->Add(this->VisibleObject);
}


PNGViewer3D::~PNGViewer3D()
{
    if (this->Widget3D) {
        delete this->Widget3D;
    }
}


ui3VisibleObject * PNGViewer3D::GetVisibleObject(void)
{
    return this->Widget3D;
}


void PNGViewer3D::ToggleHandles(void)
{
    if (this->Widget3D->HandlesActive()) {
        this->Widget3DHandlesActive = false;	
    } else {
        this->Widget3DHandlesActive = true;
    }
    this->Widget3D->SetHandlesActive(this->Widget3DHandlesActive);
}


void PNGViewer3D::ConfigureMenuBar(void)
{
    this->MenuBar->AddClickButton("Move",
                                  1,
                                  "move.png",
                                  &PNGViewer3D::ToggleHandles,
                                  this);
}


bool PNGViewer3D::RunForeground(void)
{
    if (this->Manager->MastersAsMice() != this->PreviousMaM) {
        this->PreviousMaM = this->Manager->MastersAsMice();
        this->Widget3D->Show();
        this->Widget3D->SetHandlesActive(this->Widget3DHandlesActive);
    }

    // detect transition, should that be handled as an event?
    // State is used by multiple threads ...
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
        this->Widget3D->Show();
        this->Widget3D->SetHandlesActive(this->Widget3DHandlesActive);
    }
    return true;
}


bool PNGViewer3D::RunBackground(void)
{
    // detect transition
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
        this->Widget3D->Show();
        this->Widget3D->SetHandlesActive(false);
    }
    return true;
}

bool PNGViewer3D::RunNoInput(void)
{
    if (this->Manager->MastersAsMice() != this->PreviousMaM) {
        this->PreviousMaM = this->Manager->MastersAsMice();
        this->Widget3D->SetHandlesActive(false);
    }
    return true;
}


void PNGViewer3D::OnQuit(void)
{
    this->Widget3D->Hide();
}


void PNGViewer3D::OnStart(void)
{
    this->Position.X() = 0.0;
    this->Position.Y() = 0.0;
    this->Position.Z() = -50.0;
    this->Widget3D->SetPosition(this->Position);
    this->Widget3D->SetSize(12.0);
    this->Widget3D->Show();
}
