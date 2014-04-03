/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs Vagvolgyi, Simon DiMaio, Anton Deguet
  Created on: 2008-05-23

  (C) Copyright 2008-2012 Johns Hopkins University (JHU), All Rights
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
#include <cisstVector/vctTransformationTypes.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>

#include <cisstStereoVision/svlRenderTargets.h>
#include <cisstStereoVision/svlCameraGeometry.h>

#include <cisst3DUserInterface/ui3ForwardDeclarations.h>
#include <cisst3DUserInterface/ui3VTKForwardDeclarations.h>

// Always include last!
#include <cisst3DUserInterface/ui3Export.h>

/*!
  Class that implements the interface between VTK and the SAW framework.
  It provides methods for creating and manipulating 3D objects and rendering
  scenes on the screen and/or on a frame buffer.
*/
class CISST_EXPORT ui3VTKRenderer: public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

 public:
    /*!
      Constructor
    */
    ui3VTKRenderer(ui3SceneManager* scene,
                   unsigned int width, unsigned int height,
                   double zoom, bool borderless,
                   svlCameraGeometry & camgeometry, unsigned int camid,
                   svlRenderTargetBase* target = 0);

    /*!
      Destructor
    */
    ~ui3VTKRenderer();

    void StartInteractor(void);
    void Stop(void);

    void Render(bool pickRequested, const vct3& pickerPoint);

    void SetWindowPosition(int x, int y);

    void Add(ui3VisibleObject * object);

    svlCameraGeometry GetCameraGeometry(void)
    {
        return this->CameraGeometry;
    };

 private:
    ui3SceneManager* SceneManager;

    vtkRenderer * Renderer;
    vtkRenderWindow * RenderWindow;
    vtkRenderWindowInteractor * RenderWindowInteractor;
    vtkCamera * Camera;
    vtkUnsignedCharArray * OffScreenBuffer;
    vtkPropPicker * PropPicker;

    unsigned int Width;
    unsigned int Height;
    svlCameraGeometry CameraGeometry;
    unsigned int CameraID;
    vctDouble2 OpticalCenterOffset;
    svlRenderTargetBase * Target;
};


CMN_DECLARE_SERVICES_INSTANTIATION(ui3VTKRenderer);


#endif // _ui3VTKRenderer_h

