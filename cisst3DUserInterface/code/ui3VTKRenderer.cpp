/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs Vagvolgyi, Simon DiMaio, Anton Deguet
  Created on: 2008-06-10

  (C) Copyright 2008-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisst3DUserInterface/ui3VTKRenderer.h>

#include <cisstOSAbstraction/osaSleep.h>
#include <cisst3DUserInterface/ui3SceneManager.h>
#include <cisst3DUserInterface/ui3VisibleObject.h>
#include <cisst3DUserInterface/ui3VisibleList.h>

#include <vtkAssembly.h>
#include <vtkPropAssembly.h>
#include <vtkOpenGLRenderer.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCamera.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPropPicker.h>

CMN_IMPLEMENT_SERVICES(ui3VTKRenderer);


ui3VTKRenderer::ui3VTKRenderer(ui3SceneManager* scene,
                               unsigned int width, unsigned int height,
                               double zoom, bool borderless,
                               svlCameraGeometry & camgeometry, unsigned int camid,
                               svlRenderTargetBase* target) :
    SceneManager(scene),
    Renderer(0),
    RenderWindow(0),
    RenderWindowInteractor(0),
    Camera(0),
    OffScreenBuffer(0),
    PropPicker(0),
    Width(width),
    Height(height),
    CameraGeometry(camgeometry),
    CameraID(camid),
    Target(target)
{
    CMN_ASSERT(this->SceneManager);

    // Create render window
    this->Renderer = vtkRenderer::New();
    CMN_ASSERT(this->Renderer);

    if (this->Target) {
        // Setup off-screen rendering
        this->RenderWindow = vtkRenderWindow::New();
        CMN_ASSERT(this->RenderWindow);
        this->RenderWindow->OffScreenRenderingOn();
        this->RenderWindow->DoubleBufferOff();
        this->OffScreenBuffer = vtkUnsignedCharArray::New();
        CMN_ASSERT(this->OffScreenBuffer);
        this->OffScreenBuffer->Resize(this->Width * this->Height * 3);
    }
    else {
        // Setup regular in window rendering
        this->RenderWindow = vtkRenderWindow::New();
        CMN_ASSERT(this->RenderWindow);
    }
    this->RenderWindow->AddRenderer(this->Renderer);
    //    this->RenderWindow->SetFullScreen(1);
    this->RenderWindow->SetSize(this->Width, this->Height);
    if (borderless) this->RenderWindow->BordersOff();
    //    this->RenderWindow->SetWindowName("Renderer");

    this->RenderWindowInteractor = vtkRenderWindowInteractor::New();
    CMN_ASSERT(this->RenderWindowInteractor);
    this->RenderWindowInteractor->SetRenderWindow(this->RenderWindow);

    // Create camera
    double viewangle;
    vctDouble3 viewup, position, axis;
    this->CameraGeometry.GetPositionAxisViewUp(position, axis, viewup, this->CameraID);
    OpticalCenterOffset[0] = this->CameraGeometry.GetIntrinsics(CameraID).cc[0] - this->Width / 2.0;
    OpticalCenterOffset[1] = this->CameraGeometry.GetIntrinsics(CameraID).cc[1] - this->Height / 2.0;

    this->Camera = vtkCamera::New();
    this->Camera->SetViewUp(viewup[0], viewup[1], viewup[2]);
    this->Camera->SetPosition(position[0], position[1], position[2]);
    this->Camera->SetFocalPoint(position[0] + axis[0], position[1] + axis[1], position[2] + axis[2]);
    this->Camera->SetClippingRange(0.1, 20000.0);
    viewangle = this->CameraGeometry.GetViewAngleVertical(this->Height, this->CameraID);
    this->Camera->SetViewAngle(viewangle / zoom);
    this->Renderer->SetActiveCamera(this->Camera);

    // Initialize renderer
    this->RenderWindowInteractor->Initialize();

    // Create Prop Picker
    this->PropPicker = vtkPropPicker::New();
}


ui3VTKRenderer::~ui3VTKRenderer()
{
}

void ui3VTKRenderer::Render(bool pickRequested, const vct3& pickerPoint)
{
    if (this->RenderWindow) {

        this->SceneManager->Lock();
        {
            this->Renderer->Modified();
            if (pickRequested) {
                int pickerResult = 0;
                std::cerr << "IsPicking Before: "<< this->Renderer->GetIsPicking();
                pickerResult = PropPicker->PickProp(this->Width/2.0, this->Height/2.0,this->Renderer);
                std::cerr << "IsPicking After: "<< this->Renderer->GetIsPicking();

                if(pickerResult) {
                    std::cerr << "+" ;
                } else {
                    std::cerr << "-" ;
                }
            }
            this->RenderWindow->Render();
        }
        this->SceneManager->Unlock();
        if (this->Target) {
            // Push VTK off-screen frame buffer to external render target
            this->RenderWindow->GetPixelData(0, 0, this->Width - 1, this->Height - 1, 0, this->OffScreenBuffer);
            this->Target->SetImage(this->OffScreenBuffer->GetPointer(0),
                                   static_cast<int>(this->OpticalCenterOffset[0]),
                                   static_cast<int>(this->OpticalCenterOffset[1]),
                                   true);
        }

    } else {
        CMN_LOG_CLASS_INIT_ERROR << "Render: attempt to render before the VTK Window Renderer has been created" << std::endl;
    }
}


void ui3VTKRenderer::SetWindowPosition(int x, int y)
{
    this->RenderWindow->SetPosition(x, y);
    this->RenderWindow->BordersOff();
}


void ui3VTKRenderer::Add(ui3VisibleObject * object)
{
    CMN_ASSERT(object->GetVTKProp());
    this->Renderer->AddViewProp(object->GetVTKProp());
}
