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

#include <cisst3DUserInterface/ui3VTKRenderer.h>

#include <cisstOSAbstraction/osaSleep.h>
#include <cisst3DUserInterface/ui3SceneManager.h>
#include <cisst3DUserInterface/ui3VisibleObject.h>

#include <vtkAssembly.h>
#include <vtkPropAssembly.h>
#include <vtkOpenGLRenderer.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCamera.h>

CMN_IMPLEMENT_SERVICES(ui3VTKRenderer);


ui3VTKRenderer::ui3VTKRenderer(ui3SceneManager* scene,
                               unsigned int width, unsigned int height,
                               double viewangle, vctFrm3 & cameraframe, vct2 opticalcenteroffset,
                               svlRenderTargetBase* target) :
    SceneManager(scene),
    Renderer(0),
    RenderWindow(0),
    RenderWindowInteractor(0),
    Camera(0),
    OffScreenBuffer(0),
    Width(width),
    Height(height),
    ViewAngle(viewangle),
    CameraFrame(cameraframe),
    OpticalCenterOffset(opticalcenteroffset),
    Target(target)
{
    CMN_ASSERT(this->SceneManager);

    // Create render window
    this->Renderer = vtkOpenGLRenderer::New();
    CMN_ASSERT(this->Renderer);

    if (this->Target) {
        // Setup off-screen rendering
        this->RenderWindow = vtkOpenGLRenderWindow::New();
        CMN_ASSERT(this->RenderWindow);
        this->RenderWindow->OffScreenRenderingOn();
        this->RenderWindow->DoubleBufferOff();
        this->OffScreenBuffer = vtkUnsignedCharArray::New();
        CMN_ASSERT(this->OffScreenBuffer);
        this->OffScreenBuffer->Resize(this->Width * this->Height * 3);
    }
    else {
        // Setup regular in window rendering
        this->RenderWindow = vtkOpenGLRenderWindow::New();
        CMN_ASSERT(this->RenderWindow);
    }
    this->RenderWindow->AddRenderer(this->Renderer);
//    this->RenderWindow->SetFullScreen(1);
    this->RenderWindow->SetSize(this->Width, this->Height);
//    this->RenderWindow->SetWindowName("Renderer");

    this->RenderWindowInteractor = vtkRenderWindowInteractor::New();
    CMN_ASSERT(this->RenderWindowInteractor);
    this->RenderWindowInteractor->SetRenderWindow(this->RenderWindow);

    // Create camera
    // TO DO: set camera rotation according to specified frame
    this->Camera = vtkCamera::New();
    this->Camera->SetViewUp(0.0, 1.0, 0.0);
    this->Camera->SetPosition(this->CameraFrame.Translation().X(),
                              this->CameraFrame.Translation().Y(),
                              this->CameraFrame.Translation().Z() + 1.0);
    this->Camera->SetFocalPoint(this->CameraFrame.Translation().X(),
                                this->CameraFrame.Translation().Y(),
                                this->CameraFrame.Translation().Z());
    this->Camera->SetClippingRange(0.1, 10000.0);
    this->Camera->SetViewAngle(this->ViewAngle);
    this->Renderer->SetActiveCamera(this->Camera);

    // Initialize renderer
    this->RenderWindowInteractor->Initialize();
}


ui3VTKRenderer::~ui3VTKRenderer()
{
}


void ui3VTKRenderer::Render(void)
{
    if (this->RenderWindow) {

        this->SceneManager->Lock();
            this->RenderWindow->Render();
        this->SceneManager->Unlock();

        if (this->Target) {
            // Push VTK off-screen frame buffer to external render target
            this->RenderWindow->GetPixelData(0, 0, this->Width - 1, this->Height - 1, 0, this->OffScreenBuffer);
            this->Target->SetImage(this->OffScreenBuffer->GetPointer(0),
                                   static_cast<int>(this->OpticalCenterOffset.X()),
                                   static_cast<int>(this->OpticalCenterOffset.Y()),
                                   true);
        }
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "Render: attempt to render before the VTK Window Renderer has been created" << std::endl;
    }
}


double ui3VTKRenderer::GetViewAngle(void)
{
    return this->ViewAngle;
}


void ui3VTKRenderer::GetOpticalCenterOffset(vct2 & offset)
{
    offset = this->OpticalCenterOffset;
}


void ui3VTKRenderer::SetWindowPosition(int x, int y)
{
    this->RenderWindow->SetPosition(x, y);
}


void ui3VTKRenderer::Add(ui3VisibleObject * object)
{
    this->Renderer->AddViewProp(object->GetVTKProp());
}

