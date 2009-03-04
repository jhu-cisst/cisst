/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ui3VTKRenderer.cpp,v 1.8 2009/02/17 06:02:52 anton Exp $

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
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCamera.h>


CMN_IMPLEMENT_SERVICES(ui3VTKRenderer);


ui3VTKRenderer::ui3VTKRenderer():
    Renderer(0),
    RenderWindow(0),
    RenderWindowInteractor(0),
    Camera(0),
    ViewAngle(30.0)
{
    // Create render window
    this->Renderer = vtkRenderer::New();
    CMN_ASSERT(this->Renderer);

    this->RenderWindow = vtkRenderWindow::New();
    CMN_ASSERT(this->RenderWindow);
    this->RenderWindow->AddRenderer(this->Renderer);
    this->RenderWindow->SetSize(800, 600);
    this->RenderWindow->SetWindowName("Renderer");

    this->RenderWindowInteractor = vtkRenderWindowInteractor::New();
    CMN_ASSERT(this->RenderWindowInteractor);
    this->RenderWindowInteractor->SetRenderWindow(this->RenderWindow);

    // Create camera
    this->Camera = vtkCamera::New();
    this->Camera->SetViewUp(0.0, 1.0, 0.0);
    this->Camera->SetPosition(0.0, 0.0, 1.0);
    this->Camera->SetFocalPoint(0.0, 0.0, 0.0);
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
        this->RenderWindow->Render();
    } else {
        CMN_LOG_CLASS(1) << "Render: attempt to render before the VTK Window Renderer has been created" << std::endl;
    }
}


void ui3VTKRenderer::SetViewAngle(double angle)
{
    this->ViewAngle = angle;
    this->Camera->SetViewAngle(ViewAngle);
}


double ui3VTKRenderer::GetViewAngle(void)
{
    return this->ViewAngle;
}


void ui3VTKRenderer::Add(ui3VisibleObject * object)
{
    this->Renderer->AddViewProp(object->GetVTKProp());
}