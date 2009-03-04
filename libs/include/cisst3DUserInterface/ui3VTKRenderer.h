/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ui3VTKRenderer.h,v 1.8 2009/02/17 06:02:52 anton Exp $

  Author(s):	Balazs Vagvolgyi, Simon DiMaio, Anton Deguet
  Created on:	2008-05-23

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _ui3VTKRenderer_h
#define _ui3VTKRenderer_h

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassServices.h>
#include <cisstCommon/cmnClassRegisterMacros.h>

#include <cisst3DUserInterface/ui3ForwardDeclarations.h>
#include <cisst3DUserInterface/ui3VTKForwardDeclarations.h>

/*!
 Class that implements the interface between VTK and the SAW framework.
 It provides methods for creating and manipulating 3D objects and rendering
 scenes on the screen and/or on a frame buffer.
*/
class ui3VTKRenderer: public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);

public:
    /*!
     Constructor
    */
    ui3VTKRenderer(void);

    /*!
     Destructor
    */
    ~ui3VTKRenderer();

    void Render(void);

    void SetViewAngle(double angle);
    double GetViewAngle(void);

    void Add(ui3VisibleObject * object);

private:
    vtkRenderer * Renderer;
    vtkRenderWindow * RenderWindow;
    vtkRenderWindowInteractor * RenderWindowInteractor;
    vtkCamera * Camera;
    
    double ViewAngle;
};


CMN_DECLARE_SERVICES_INSTANTIATION(ui3VTKRenderer);


#endif // _ui3VTKRenderer_h

