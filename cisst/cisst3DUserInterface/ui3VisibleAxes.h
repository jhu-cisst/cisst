/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id:

  Author(s):	Gorkem Sevinc, Anton Deguet
  Created on:	2009-06-19

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _ui3VisibleAxes_h
#define _ui3VisibleAxes_h

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassServices.h>
#include <cisstCommon/cmnClassRegisterMacros.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctTransformationTypes.h>

#include <cisst3DUserInterface/ui3ForwardDeclarations.h>
#include <cisst3DUserInterface/ui3VTKForwardDeclarations.h>
#include <cisst3DUserInterface/ui3SceneManager.h>
#include <cisst3DUserInterface/ui3VisibleObject.h>

// Always include last!
#include <cisst3DUserInterface/ui3Export.h>

class CISST_EXPORT ui3VisibleAxes: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
public:
    inline ui3VisibleAxes(void):
        ui3VisibleObject(),
        AxesActor(0)
    {}

    ~ui3VisibleAxes();

    bool CreateVTKObjects(void);

    inline bool UpdateVTKObjects(void) { return true; };

    void SetSize(double size);
    
    void ShowLabels(void);
    
    void HideLabels(void);

protected:
    vtkAxesActor * AxesActor;
};

CMN_DECLARE_SERVICES_INSTANTIATION(ui3VisibleAxes);


#endif // _ui3VisibleAxes_h
