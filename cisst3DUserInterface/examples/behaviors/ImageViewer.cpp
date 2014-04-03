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

#include <ImageViewer.h>

#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisst3DUserInterface/ui3Manager.h>

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


class ImageViewerSkinVisibleObject: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
public:
    inline ImageViewerSkinVisibleObject(vtkVolumeReader * input):
        ui3VisibleObject(),
        Input(input),
        SkinExtractor(0),
        SkinNormals(0),
        SkinStripper(0),
        SkinMapper(0),
        SkinActor(0)
    {}

    inline ~ImageViewerSkinVisibleObject()
    {
        if (this->SkinActor) {
            this->SkinActor->Delete();
            this->SkinActor = 0;
        }
        if (this->SkinMapper) {
            this->SkinMapper->Delete();
            this->SkinMapper = 0;
        }
        if (this->SkinStripper) {
            this->SkinStripper->Delete();
            this->SkinStripper = 0;
        }
        if (this->SkinNormals) {
            this->SkinNormals->Delete();
            this->SkinNormals = 0;
        }
        if (this->SkinExtractor) {
            this->SkinExtractor->Delete();
            this->SkinExtractor = 0;
        }
    }

    inline bool CreateVTKObjects(void) {
        // Extract skin isosurfaces.
        SkinExtractor = vtkContourFilter::New();
        CMN_ASSERT(SkinExtractor);
        CMN_ASSERT(this->Input);
        SkinExtractor->SetInputConnection(this->Input->GetOutputPort());
        SkinExtractor->SetValue(0, 500);
        SkinNormals = vtkPolyDataNormals::New();
        CMN_ASSERT(SkinNormals);
        SkinNormals->SetInputConnection(SkinExtractor->GetOutputPort());
        SkinNormals->SetFeatureAngle(60.0);
        SkinStripper = vtkStripper::New();
        CMN_ASSERT(SkinStripper);
        SkinStripper->SetInputConnection(SkinNormals->GetOutputPort());
        SkinMapper = vtkPolyDataMapper::New();
        CMN_ASSERT(SkinMapper);
        SkinMapper->SetInputConnection(SkinStripper->GetOutputPort());
        SkinMapper->ScalarVisibilityOff();
        SkinMapper->ImmediateModeRenderingOn();
        SkinActor = vtkActor::New();
        CMN_ASSERT(SkinActor);
        SkinActor->SetMapper(SkinMapper);
        SkinActor->GetProperty()->SetDiffuseColor(1, .49, .25);
        SkinActor->GetProperty()->SetSpecular(.3);
        SkinActor->GetProperty()->SetSpecularPower(20);

        // Set skin to semi-transparent.
        SkinActor->GetProperty()->SetOpacity(1.0);

        // Scale the actors.
        SkinActor->SetScale(0.05);

        this->AddPart(this->SkinActor);
        return true;
    }

    inline bool UpdateVTKObjects(void) {
        return true;
    }

protected:
    vtkVolumeReader * Input; // this class does NOT own the input
    vtkContourFilter * SkinExtractor;
    vtkPolyDataNormals * SkinNormals;
    vtkStripper * SkinStripper;
    vtkPolyDataMapper * SkinMapper;
    vtkActor * SkinActor;
};

CMN_DECLARE_SERVICES_INSTANTIATION(ImageViewerSkinVisibleObject);
CMN_IMPLEMENT_SERVICES(ImageViewerSkinVisibleObject);


class ImageViewerBoneVisibleObject: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
public:
    inline ImageViewerBoneVisibleObject(vtkVolumeReader * input):
        ui3VisibleObject(),
        Input(input),
        BoneExtractor(0),
        BoneNormals(0),
        BoneStripper(0),
        BoneMapper(0),
        BoneActor(0)
    {}

    inline ~ImageViewerBoneVisibleObject()
    {
        if (this->BoneActor) {
            this->BoneActor->Delete();
            this->BoneActor = 0;
        }
        if (this->BoneMapper) {
            this->BoneMapper->Delete();
            this->BoneMapper = 0;
        }
        if (this->BoneStripper) {
            this->BoneStripper->Delete();
            this->BoneStripper = 0;
        }
        if (this->BoneNormals) {
            this->BoneNormals->Delete();
            this->BoneNormals = 0;
        }
        if (this->BoneExtractor) {
            this->BoneExtractor->Delete();
            this->BoneExtractor = 0;
        }
    }

    inline bool CreateVTKObjects() {
        BoneExtractor = vtkContourFilter::New();
        CMN_ASSERT(BoneExtractor);
        BoneExtractor->SetInputConnection(this->Input->GetOutputPort());
        BoneExtractor->SetValue(0, 1150);
        BoneNormals = vtkPolyDataNormals::New();
        CMN_ASSERT(BoneNormals);
        BoneNormals->SetInputConnection(BoneExtractor->GetOutputPort());
        BoneNormals->SetFeatureAngle(60.0);
        BoneStripper = vtkStripper::New();
        CMN_ASSERT(BoneStripper);
        BoneStripper->SetInputConnection(BoneNormals->GetOutputPort());
        BoneMapper = vtkPolyDataMapper::New();
        CMN_ASSERT(BoneMapper);
        BoneMapper->SetInputConnection(BoneStripper->GetOutputPort());
        BoneMapper->ScalarVisibilityOff();
        BoneMapper->ImmediateModeRenderingOn();
        BoneActor = vtkActor::New();
        CMN_ASSERT(BoneActor);
        BoneActor->SetMapper(BoneMapper);
        BoneActor->GetProperty()->SetDiffuseColor(1.0, 1.0, .9412);

        BoneActor->SetScale(0.05);
        this->AddPart(this->BoneActor);
        return true;
    }

    inline bool UpdateVTKObjects(void) {
        return true;
    }

protected:
    vtkVolumeReader * Input; // this class does NOT own the input
    vtkContourFilter * BoneExtractor;
    vtkPolyDataNormals * BoneNormals;
    vtkStripper * BoneStripper;
    vtkPolyDataMapper * BoneMapper;
    vtkActor * BoneActor;
};

CMN_DECLARE_SERVICES_INSTANTIATION(ImageViewerBoneVisibleObject);
CMN_IMPLEMENT_SERVICES(ImageViewerBoneVisibleObject);


class ImageViewerOutlineVisibleObject: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
public:
    inline ImageViewerOutlineVisibleObject(vtkVolumeReader * input):
        ui3VisibleObject(),
        Input(input)
    {}

    inline ~ImageViewerOutlineVisibleObject()
    {
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
        // Create a frame for the data volume.
        OutlineData = vtkOutlineFilter::New();
        CMN_ASSERT(OutlineData);
        OutlineData->SetInputConnection(Input->GetOutputPort());
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
        return true;
    }

    inline bool UpdateVTKObjects(void) {
        return true;
    }

    vctDouble3 GetCenter(void) {
        vctDouble3 center;
        center.Assign(OutlineActor->GetCenter());
        return center;
    }

protected:
    vtkVolumeReader * Input; // this class does NOT own the input
    vtkOutlineFilter * OutlineData;
    vtkPolyDataMapper *OutlineMapper;
    vtkActor * OutlineActor;
};

CMN_DECLARE_SERVICES_INSTANTIATION(ImageViewerOutlineVisibleObject);
CMN_IMPLEMENT_SERVICES(ImageViewerOutlineVisibleObject);


class ImageViewerSlicesVisibleObject: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
public:
    inline ImageViewerSlicesVisibleObject(vtkVolumeReader * input):
        ui3VisibleObject(),
        Input(input),
        bwLut(0),
        hueLut(0),
        satLut(0),
        sagittalColors(0),
        sagittal(0),
        axialColors(0),
        axial(0),
        coronalColors(0),
        coronal(0)
    {}

    inline ~ImageViewerSlicesVisibleObject()
    {
    }

    inline bool CreateVTKObjects(void) {
        // Create slice planes.
        // Start by creating a black/white lookup table.
        bwLut = vtkLookupTable::New();
        bwLut->SetTableRange (0, 2000);
        bwLut->SetSaturationRange (0, 0);
        bwLut->SetHueRange (0, 0);
        bwLut->SetValueRange (0, 1);
        bwLut->Build(); //effective built

        // Now create a lookup table that consists of the full hue circle
        // (from HSV).
        hueLut = vtkLookupTable::New();
        hueLut->SetTableRange (0, 2000);
        hueLut->SetHueRange (0, 1);
        hueLut->SetSaturationRange (1, 1);
        hueLut->SetValueRange (1, 1);
        hueLut->Build(); //effective built

        // Finally, create a lookup table with a single hue but having a range
        // in the saturation of the hue.
        satLut = vtkLookupTable::New();
        satLut->SetTableRange (0, 2000);
        satLut->SetHueRange (.6, .6);
        satLut->SetSaturationRange (0, 1);
        satLut->SetValueRange (1, 1);
        satLut->Build(); //effective built

        // Create the first sagittal slice plane.
        sagittalColors = vtkImageMapToColors::New();
        sagittalColors->SetInputConnection(Input->GetOutputPort());
        sagittalColors->SetLookupTable(bwLut);
        sagittal = vtkImageActor::New();
        sagittal->SetInput(sagittalColors->GetOutput());
        sagittal->SetDisplayExtent(32,32, 0,63, 0,92);

        // Create the second (axial) plane of the three planes. We use the
        // same approach as before except that the extent differs.
        axialColors = vtkImageMapToColors::New();
        axialColors->SetInputConnection(Input->GetOutputPort());
        axialColors->SetLookupTable(bwLut);
        axial = vtkImageActor::New();
        axial->SetInput(axialColors->GetOutput());
        axial->SetDisplayExtent(0,63, 0,63, 46, 46);

        // Create the third (coronal) plane of the three planes. We use
        // the same approach as before except that the extent differs.
        coronalColors = vtkImageMapToColors::New();
        coronalColors->SetInputConnection(Input->GetOutputPort());
        coronalColors->SetLookupTable(bwLut);
        coronal = vtkImageActor::New();
        coronal->SetInput(coronalColors->GetOutput());
        coronal->SetDisplayExtent(0,63, 32,32, 0,92);
        axial->SetScale(0.05);
        coronal->SetScale(0.05);
        sagittal->SetScale(0.05);

        this->AddPart(this->axial);
        this->AddPart(this->coronal);
        this->AddPart(this->sagittal);
        return true;
    }

    inline bool UpdateVTKObjects(void) {
        return true;
    }


    void ShowHide(bool showAxial, bool showCoronal, bool showSagittal) {
        this->Lock();
        axial->SetVisibility(showAxial);
        coronal->SetVisibility(showCoronal);
        sagittal->SetVisibility(showSagittal);
        this->Unlock();
    }

protected:
    vtkVolumeReader * Input; // this class does NOT own the input
    vtkLookupTable * bwLut;
    vtkLookupTable * hueLut;
    vtkLookupTable * satLut;

    vtkImageMapToColors * sagittalColors;
    vtkImageActor * sagittal;
    vtkImageMapToColors * axialColors;
    vtkImageActor * axial;
    vtkImageMapToColors * coronalColors;
    vtkImageActor * coronal;
};

CMN_DECLARE_SERVICES_INSTANTIATION(ImageViewerSlicesVisibleObject);
CMN_IMPLEMENT_SERVICES(ImageViewerSlicesVisibleObject);


ImageViewer::ImageViewer(const std::string & name):
    ui3BehaviorBase(std::string("ImageViewer::") + name, 0),
    Widget3D(0),
    Skin(0),
    SkinShow(true),
    Bone(0),
    BoneShow(true),
    Outline(0),
    Slices(0),
    SlicesAxialShow(true),
    SlicesCoronalShow(true),
    SlicesSagittalShow(true),
    Widget3DHandlesActive(true)
{
    this->Widget3D = new ui3Widget3D("ImageViewer");
    this->AddWidget3D(this->Widget3D);

    // hard coded for now :-(
    VolumeReader = vtkVolume16Reader::New();
    VolumeReader->SetDataDimensions(64,64);
    VolumeReader->SetDataByteOrderToLittleEndian();
    VolumeReader->SetFilePrefix("E:/Demos/VTKData/headsq/quarter");
    VolumeReader->SetImageRange(1, 93);
    VolumeReader->SetDataSpacing(3.2, 3.2, 1.5);

    this->Skin = new ImageViewerSkinVisibleObject(VolumeReader);
    this->Widget3D->Add(this->Skin);

    this->Bone = new ImageViewerBoneVisibleObject(VolumeReader);
    this->Widget3D->Add(this->Bone);

    this->Outline = new ImageViewerOutlineVisibleObject(VolumeReader);
    this->Widget3D->Add(this->Outline);

#if 0
    this->Slices = new ImageViewerSlicesVisibleObject(VolumeReader);
    this->Widget3D->Add(this->Slices);
#endif
    CMN_ASSERT(this->Widget3D);
}


ImageViewer::~ImageViewer()
{
    if (this->Widget3D) {
        delete this->Widget3D;
    }
}


void ImageViewer::ToggleHandles(void)
{
    if (this->Widget3D->HandlesActive()) {
        this->Widget3DHandlesActive = false;
        this->Widget3D->SetHandlesActive(false);
    } else {
        this->Widget3DHandlesActive = false;
        this->Widget3D->SetHandlesActive(true);
    }
}


void ImageViewer::ToggleSkin(void)
{
    if (this->SkinShow) {
        this->SkinShow = false;
        this->Skin->Hide();
    } else {
        this->SkinShow = true;
        this->Skin->Show();
    }
}


void ImageViewer::ToggleBone(void)
{
    if (this->BoneShow) {
        this->BoneShow = false;
        this->Bone->Hide();
    } else {
        this->BoneShow = true;
        this->Bone->Show();
    }
}


void ImageViewer::ToggleSlicesAxial(void)
{
    this->SlicesAxialShow = !this->SlicesAxialShow;
    this->Slices->ShowHide(this->SlicesAxialShow, this->SlicesCoronalShow, this->SlicesSagittalShow);
}


void ImageViewer::ToggleSlicesCoronal(void)
{
    this->SlicesCoronalShow = !this->SlicesCoronalShow;
    this->Slices->ShowHide(this->SlicesAxialShow, this->SlicesCoronalShow, this->SlicesSagittalShow);
}


void ImageViewer::ToggleSlicesSagittal(void)
{
    this->SlicesSagittalShow = !this->SlicesSagittalShow;
    this->Slices->ShowHide(this->SlicesAxialShow, this->SlicesCoronalShow, this->SlicesSagittalShow);
}


void ImageViewer::ConfigureMenuBar(void)
{
    this->MenuBar->AddClickButton("ToggleSkin",
                                  1,
                                  "empty.png",
                                  &ImageViewer::ToggleSkin,
                                  this);
    this->MenuBar->AddClickButton("ToggleBone",
                                  2,
                                  "empty.png",
                                  &ImageViewer::ToggleBone,
                                  this);
#if 0
    this->MenuBar->AddClickButton("ToggleSlicesAxial",
                                  3,
                                  "empty.png",
                                  &ImageViewer::ToggleSlicesAxial,
                                  this);
    this->MenuBar->AddClickButton("ToggleSlicesCoronal",
                                  4,
                                  "empty.png",
                                  &ImageViewer::ToggleSlicesCoronal,
                                  this);
    this->MenuBar->AddClickButton("ToggleSlices",
                                  5,
                                  "empty.png",
                                  &ImageViewer::ToggleSlicesSagittal,
                                  this);
#endif
    this->MenuBar->AddClickButton("Move",
                                  6,
                                  "move.png",
                                  &ImageViewer::ToggleHandles,
                                  this);
}


bool ImageViewer::RunForeground(void)
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
    // running in foreground GUI mode
    prmPositionCartesianGet position;
    this->GetPrimaryMasterPosition(position);

    /*
    if (this->Selected) {
        vctDouble3 deltaCursor;
        deltaCursor.DifferenceOf(position.Position().Translation(),
                                 this->PreviousCursorPosition);
        this->Position.Add(deltaCursor);
        this->Widget3D->SetPosition(this->Position);
        this->Widget3D->SetOrientation(position.Position().Rotation());
    }
    this->PreviousCursorPosition.Assign(position.Position().Translation());
    */
    return true;
}

bool ImageViewer::RunBackground(void)
{
    // detect transition
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
        this->Widget3D->Show();
        this->Widget3D->SetHandlesActive(false);
    }
    return true;
}

bool ImageViewer::RunNoInput(void)
{
    if (this->Manager->MastersAsMice() != this->PreviousMaM) {
        this->PreviousMaM = this->Manager->MastersAsMice();
        this->Widget3D->SetHandlesActive(false);
    }
    return true;
}


void ImageViewer::OnQuit(void)
{
    this->Widget3D->Hide();
}


void ImageViewer::OnStart(void)
{
    this->Position.X() = 0.0;
    this->Position.Y() = 0.0;
    this->Position.Z() = -50.0;
    this->Widget3D->SetPosition(this->Position);
    this->Widget3D->SetSize(7.0);
    this->Widget3D->Show();
    vctDouble3 center = -(this->Outline->GetCenter());
    this->Skin->SetPosition(center);
    this->Bone->SetPosition(center);
    // this->Slices->SetPosition(center);
    this->Outline->SetPosition(center);
}


void ImageViewer::PrimaryMasterButtonCallback(const prmEventButton & event)
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
