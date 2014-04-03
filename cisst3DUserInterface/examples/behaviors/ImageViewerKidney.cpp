/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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
#include <cisst3DUserInterface/ui3Widget3D.h>
#include <cisst3DUserInterface/ui3Manager.h>

#include "ImageViewerKidney.h"

#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSphereSource.h>
#include <vtkCubeSource.h>
#include <vtkContourFilter.h>
#include <vtkStripper.h>
#include <vtkVolumeReader.h>
#include <vtkPolyDataNormals.h>
#include <vtkVolume16Reader.h>
#include <vtkOutlineFilter.h>
#include <vtkImageActor.h>
#include <vtkLookupTable.h>
#include <vtkImageMapToColors.h>
#include <vtkPolyDataReader.h>

class ImageViewerKidneySurfaceVisibleObject: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
public:
    inline ImageViewerKidneySurfaceVisibleObject(const std::string & inputFile, bool hasOutline = false):
        ui3VisibleObject(),
        InputFile(inputFile),
        SurfaceReader(0),
        SurfaceMapper(0),
        SurfaceActor(0),
        HasOutline(hasOutline),
        OutlineData(0),
        OutlineMapper(0),
        OutlineActor(0)
    {}

    inline ~ImageViewerKidneySurfaceVisibleObject()
    {
        if (this->SurfaceActor) {
            this->SurfaceActor->Delete();
            this->SurfaceActor = 0;
        }
        if (this->SurfaceMapper) {
            this->SurfaceMapper->Delete();
            this->SurfaceMapper = 0;
        }
        if (this->SurfaceReader) {
            this->SurfaceReader->Delete();
            this->SurfaceReader = 0;
        }
        if (this->OutlineData) {
            this->OutlineData->Delete();
            this->OutlineData = 0;
        }
        if (this->OutlineMapper) {
            this->OutlineMapper->Delete();
            this->OutlineMapper = 0;
        }
        if (this->OutlineActor) {
            this->OutlineActor->Delete();
            this->OutlineActor = 0;
        }
    }

    inline bool CreateVTKObjects(void) {
        SurfaceReader = vtkPolyDataReader::New();
        CMN_ASSERT(SurfaceReader);
        CMN_LOG_CLASS_INIT_VERBOSE << "Loading file \"" << InputFile << "\"" << std::endl;
        SurfaceReader->SetFileName(InputFile.c_str());
        CMN_LOG_CLASS_INIT_VERBOSE << "File \"" << InputFile << "\" loaded" << std::endl;
        SurfaceReader->Update();

        SurfaceMapper = vtkPolyDataMapper::New();
        CMN_ASSERT(SurfaceMapper);
        SurfaceMapper->SetInputConnection(SurfaceReader->GetOutputPort());
        SurfaceMapper->ScalarVisibilityOff();
        SurfaceMapper->ImmediateModeRenderingOn();

        SurfaceActor = vtkActor::New();
        CMN_ASSERT(SurfaceActor);
        SurfaceActor->SetMapper(SurfaceMapper);
        // SurfaceActor->GetProperty()->SetSpecular(.3);
        // SurfaceActor->GetProperty()->SetSpecularPower(20);
        SurfaceActor->GetProperty()->SetOpacity(1.0); // to allow to see thru
    
        // Scale the actors.
        SurfaceActor->SetScale(0.10);

        this->AddPart(this->SurfaceActor);

        // Create a frame for the data volume.
        if (HasOutline) {
            OutlineData = vtkOutlineFilter::New();
            CMN_ASSERT(OutlineData);
            OutlineData->SetInputConnection(SurfaceReader->GetOutputPort());
            OutlineMapper = vtkPolyDataMapper::New();
            CMN_ASSERT(OutlineMapper);
            OutlineMapper->SetInputConnection(OutlineData->GetOutputPort());
            OutlineMapper->ImmediateModeRenderingOn();
            OutlineActor = vtkActor::New();
            CMN_ASSERT(OutlineActor);
            OutlineActor->SetMapper(OutlineMapper);
            OutlineActor->GetProperty()->SetColor(1,1,1);
            
            // Scale the actors.
            OutlineActor->SetScale(0.05);
            this->AddPart(this->OutlineActor);
        }
        return true;
    }

	inline bool UpdateVTKObjects(void) {
        return true;
    }

    inline void SetColor(double r, double g, double b) {
        SurfaceActor->GetProperty()->SetDiffuseColor(r, g, b);
    }

    inline void SetOpacity(double opacity) {
        SurfaceActor->GetProperty()->SetOpacity(opacity);
    }

    vctDouble3 GetCenter(void) {
        vctDouble3 center;
        if (HasOutline) {
            center.Assign(OutlineActor->GetCenter());
        }
        return center;
    }

protected:
    std::string InputFile;
    vtkPolyDataReader * SurfaceReader;
    vtkPolyDataMapper * SurfaceMapper;
    vtkActor * SurfaceActor;
    bool HasOutline;
    vtkOutlineFilter * OutlineData;
    vtkPolyDataMapper * OutlineMapper;
    vtkActor * OutlineActor;
};

CMN_DECLARE_SERVICES_INSTANTIATION(ImageViewerKidneySurfaceVisibleObject);
CMN_IMPLEMENT_SERVICES(ImageViewerKidneySurfaceVisibleObject);



ImageViewerKidney::ImageViewerKidney(const std::string & name):
    ui3BehaviorBase(std::string("ImageViewerKidney::") + name, 0),
    Widget3D(0),
    Outer(0),
    OuterShow(true),
    Tree(0),
    TreeShow(true),
    Widget3DHandlesActive(true)
{
    this->Widget3D = new ui3Widget3D("ImageViewerKidney");
    this->AddWidget3D(this->Widget3D);

    this->Outer = new ImageViewerKidneySurfaceVisibleObject("E:/Demos/kidney-data/high_v2_kidney_right.vtk");
    this->Widget3D->Add(this->Outer);

    this->Tree = new ImageViewerKidneySurfaceVisibleObject("E:/Demos/kidney-data/high_v2_tree_right.vtk");
    this->Widget3D->Add(this->Tree);

    this->Tumor = new ImageViewerKidneySurfaceVisibleObject("E:/Demos/kidney-data/high_v2_tumor.vtk");
    this->Widget3D->Add(this->Tumor);

    CMN_ASSERT(this->Widget3D);
}


ImageViewerKidney::~ImageViewerKidney()
{
    if (this->Widget3D) {
        delete this->Widget3D;
    }
}


void ImageViewerKidney::ToggleHandles(void)
{
    if (this->Widget3D->HandlesActive()) {
        this->Widget3DHandlesActive = false;
        this->Widget3D->SetHandlesActive(false);
    } else {
        this->Widget3DHandlesActive = false;
        this->Widget3D->SetHandlesActive(true);
    }
}


void ImageViewerKidney::ToggleOuter(void)
{
    if (this->OuterShow) {
        this->OuterShow = false;
        this->Outer->Hide();
    } else {
        this->OuterShow = true;
        this->Outer->Show();
    }
}


void ImageViewerKidney::ToggleTree(void)
{
    if (this->TreeShow) {
        this->TreeShow = false;
        this->Tree->Hide();
    } else {
        this->TreeShow = true;
        this->Tree->Show();
    }
}


void ImageViewerKidney::ToggleTumor(void)
{
    if (this->TumorShow) {
        this->TumorShow = false;
        this->Tumor->Hide();
    } else {
        this->TumorShow = true;
        this->Tumor->Show();
    }
}


void ImageViewerKidney::ConfigureMenuBar(void)
{
    this->MenuBar->AddClickButton("ToggleOuter",
                                  1,
                                  "square.png",
                                  &ImageViewerKidney::ToggleOuter,
                                  this);
    this->MenuBar->AddClickButton("ToggleTree",
                                  2,
                                  "triangle.png",
                                  &ImageViewerKidney::ToggleTree,
                                  this);
    this->MenuBar->AddClickButton("ToggleTumor",
                                  2,
                                  "sphere.png",
                                  &ImageViewerKidney::ToggleTumor,
                                  this);
    this->MenuBar->AddClickButton("Move",
                                  6,
                                  "move.png",
                                  &ImageViewerKidney::ToggleHandles,
                                  this);
}


bool ImageViewerKidney::RunForeground(void)
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

bool ImageViewerKidney::RunBackground(void)
{
    // detect transition
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
        this->Widget3D->Show();
        this->Widget3D->SetHandlesActive(false);
    }
    return true;
}

bool ImageViewerKidney::RunNoInput(void)
{
    if (this->Manager->MastersAsMice() != this->PreviousMaM) {
        this->PreviousMaM = this->Manager->MastersAsMice();
        this->Widget3D->SetHandlesActive(false);
    }
    return true;
}


void ImageViewerKidney::OnQuit(void)
{
    this->Widget3D->Hide();
}


void ImageViewerKidney::OnStart(void)
{
    this->Position.X() = 0.0;
    this->Position.Y() = 0.0;
    this->Position.Z() = -50.0;
    this->Widget3D->SetPosition(this->Position);
    this->Widget3D->SetSize(7.0);
    this->Widget3D->Show();
    Outer->SetColor(1.0, 0.49, 0.25);
    // Outer->SetOpacity(0.8);
    Tree->SetColor(0.6, 0.6, 0.1);
    Tumor->SetColor(0.5, 0.5, 0.5);

    // vctDouble3 center = -(this->Outer->GetCenter());
    vctDouble3 center;
    center.Assign(5.0, 5.0, 0.0); // hard coded value :-)
    this->Outer->SetPosition(center);
    this->Tree->SetPosition(center);
    this->Tumor->SetPosition(center);
}


void ImageViewerKidney::PrimaryMasterButtonCallback(const prmEventButton & event)
{
#if 0
    if (event.Type() == prmEventButton::PRESSED) {
        this->Following = true;
        CMN_LOG_CLASS_VERY_VERBOSE << "Primary master button pressed, following started" << std::endl;
    } else if (event.Type() == prmEventButton::RELEASED) {
        this->Following = false;
        CMN_LOG_CLASS_VERY_VERBOSE << "Primary master button pressed, following ended" << std::endl;
    }
#endif
}
