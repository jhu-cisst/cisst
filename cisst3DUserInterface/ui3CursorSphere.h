/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:	2009-02-04

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _ui3CursorSphere_h
#define _ui3CursorSphere_h


#include <cisstVector/vctTransformationTypes.h>
#include <cisst3DUserInterface/ui3VTKForwardDeclarations.h>
#include <cisst3DUserInterface/ui3CursorBase.h>

// Always include last!
#include <cisst3DUserInterface/ui3Export.h>

class ui3CursorSphereTip;
class ui3CursorSphereAnchor;

/*!  Simple cursor using a sphere for rendering.  Color, diameter and
  transparency are used to show the different states.
*/
class CISST_EXPORT ui3CursorSphere: public ui3CursorBase
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:  
    /*! VTK objects used for the cursor itself, i.e. a simple
      sphere */
    vtkSphereSource * SphereSource;
    vtkPolyDataMapper * SphereMapper;
    vtkActor * SphereActor;

    /*! VTK objects used for the "anchor" */
    vtkLineSource * LineSource;
    vtkPolyDataMapper * LineMapper;
    vtkActor * LineActor;

public:
    /*!
     Constructor: called when instantiating behaviors
    */
    ui3CursorSphere(void);

    /*!
     Destructor
    */
    ~ui3CursorSphere();

    void SetPressed(bool pressed);

    void Set2D(bool is2D);

    void SetClutched(bool clutched);

    ui3VisibleObject * GetVisibleObject(void);

    void SetTransformation(vctDoubleFrm3 & frame);

protected:
    void UpdateColor(void);

    bool IsPressed;
    bool Is2D;
    bool IsClutched;
    
    ui3CursorSphereTip * VisibleTip;
    ui3CursorSphereAnchor * VisibleAnchor;
    ui3VisibleList * VisibleList;
};


CMN_DECLARE_SERVICES_INSTANTIATION(ui3CursorSphere);


#endif // _ui3CursorSphere_h
