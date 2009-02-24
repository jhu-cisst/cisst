/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ui3VisibleObject.h,v 1.4 2009/02/23 16:55:06 anton Exp $

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

#ifndef _ui3VisibleObject_h
#define _ui3VisibleObject_h

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassServices.h>
#include <cisstCommon/cmnClassRegisterMacros.h>
#include <cisstVector/vctTransformationTypes.h>

#include <cisst3DUserInterface/ui3ForwardDeclarations.h>
#include <cisst3DUserInterface/ui3VTKForwardDeclarations.h>
#include <cisst3DUserInterface/ui3SceneManager.h>


/*!
 Provides a base class for all visible objects.
*/
class ui3VisibleObject: public cmnGenericObject
{   
    friend class ui3SceneManager;

public:

    ui3VisibleObject(ui3Manager * manager);

    /*!
     Destructor
    */
    inline virtual ~ui3VisibleObject(void) {};

    virtual bool CreateVTKObjects(void) = 0;

    virtual vtkProp3D * GetVTKProp(void);

    void Show(void);

    void Hide(void);

    void SetPosition(vctDouble3 & position);

    void SetOrientation(vctDoubleMatRot3 & rotationMatrix);

    void SetTransformation(vctDoubleFrm3 & frame);

    void Lock(void);

    void Unlock(void);

protected:
    
    typedef ui3SceneManager::VTKHandleType VTKHandleType;

    inline void SetVTKHandle(VTKHandleType handle) {
        this->VTKHandle = handle;
    }

    vtkAssembly * Assembly;
    ui3Manager * Manager;
    VTKHandleType VTKHandle; 
};


#endif // _ui3VisibleObject_h
