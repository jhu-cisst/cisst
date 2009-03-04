/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

  Author(s):	Balazs Vagvolgyi
  Created on:	2009-03-02

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _ui3ImagePlane_h
#define _ui3ImagePlane_h


#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctTransformationTypes.h>
#include <cisstStereoVision/svlStreamDefs.h>
#include <cisst3DUserInterface/ui3VTKForwardDeclarations.h>
#include <cisst3DUserInterface/ui3VisibleObject.h>

#include <vtkActor.h>
#include <vtkTexture.h>
#include <vtkImageData.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>


/*!
 Provides a default behavior and common interface for derived behavior classes.
*/
class ui3ImagePlane: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);

protected:
    vtkTexture              *Texture;
    vtkImageData            *ImageData;
    vtkPlaneSource          *PlaneSrc;
    vtkPolyDataMapper       *Mapper;
    vtkActor                *Actor;

    unsigned int            BitmapWidth;
    unsigned int            BitmapHeight;
    unsigned int            TextureWidth;
    unsigned int            TextureHeight;
    unsigned char           *TextureBuffer;
    double                  PhysicalWidth;
    double                  PhysicalHeight;
    vct3                    PhysicalPositionRelativeToPivot;

public:
    /*!
     Constructor: called when instantiating behaviors
    */
    ui3ImagePlane(ui3Manager * manager);

    /*!
     Destructor
    */
    virtual ~ui3ImagePlane();

    /*!
     Texture size may be larger than the input bitmap.
     Needs to be called before CreateVTKObjects().
    */
    virtual bool SetBitmapSize(unsigned int width, unsigned int height);

    /*!
     Recommended to be called before CreateVTKObjects() in
     order to properly initialize plane size.
     May be called during rendering.
    */
    virtual bool SetPhysicalSize(double width, double height);

    /*!
     Needs to be called before CreateVTKObjects().
    */
    virtual void SetPhysicalPositionRelativeToPivot(vct3 position);

    virtual bool CreateVTKObjects(void);

    /*!
     May be called any time after CreateVTKObjects().
     No critical thread-safety issues.
    */
    virtual void SetAlpha(const unsigned char alpha);

    /*!
     Should be implemented in a thread-safe way
    */
    virtual void SetImage(svlSampleImageBase* image, unsigned int channel = 0);
};


CMN_DECLARE_SERVICES_INSTANTIATION(ui3ImagePlane);


#endif // _ui3ImagePlane_h
