/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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

#ifndef _ui3SceneManager_h
#define _ui3SceneManager_h

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassServices.h>
#include <cisstCommon/cmnClassRegisterMacros.h>
#include <cisstVector/vctDynamicVector.h>
#include <cisstOSAbstraction/osaMutex.h>
#include <cisst3DUserInterface/ui3ForwardDeclarations.h>
#include <cisst3DUserInterface/ui3VTKForwardDeclarations.h>

// Always include last!
#include <cisst3DUserInterface/ui3Export.h>

/*!
 Wraps VTK object creation and access into a unified thread safe interface.

 \note
 Responsibe to maintain the consistency and  the thread safety of 3D objects
 (actors and assemblies). All actors and assemblies shall be created through this
 class and should be used only within the corresponding Get and Release calls.
*/
class CISST_EXPORT ui3SceneManager: public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    friend class ui3VTKRenderer;
    friend class ui3Manager;
    friend class ui3ManagerCVTKRendererProc;

public:
    typedef void * VTKHandleType;

    /*!
     Constructor
    */
    ui3SceneManager();

    /*!
     Destructor
    */
    ~ui3SceneManager();

    /*!
     Adds a renderer to the list of renderers.
     All objects that are added to the scene will be registered to all renderers.
    */
    bool AddRenderer(ui3VTKRenderer* renderer);

    /*!
     Creates a VTK actor, adds it to the list of actors and returns the associated
     handle.

     \return                Handle of the newly created actor
    */
    VTKHandleType Add(ui3VisibleObject * object);

    /*!
     Deletes a VTK actor from the list of actors.

     \param actor           Handle of the actor to be retrieved
    */
    bool Delete(VTKHandleType propHandle);

    /*!
     Locks the synchronization mutex of the scene manager and returns the VTK actor
     associated with the specified handle.

     \param actor           Handle of the actor to be retrieved
     \return                VTK actor object pointer
    */
    vtkProp3D * Lock(VTKHandleType propHandle);

    /*!
     Unlocks the mutex of the scene manager.

     \param actor           Handle of the actor to be released
    */
    bool Unlock(VTKHandleType propHandle);

	vtkPropAssembly * GetVTKProp(void);

protected:
    inline void Lock(void)
    {
        this->Mutex.Lock();
    }

    inline void Unlock(void)
    {
        this->Mutex.Unlock();
    }

protected:
    ui3VisibleList * VisibleObjects;

    /*!
     Synchronization object that ensures the thread safety of VTK calls.
    */
    osaMutex Mutex;

    VTKHandleType LockHandle;

    vctDynamicVector<ui3VTKRenderer*> Renderers;
};


CMN_DECLARE_SERVICES_INSTANTIATION(ui3SceneManager);


#endif // _ui3SceneManager_h

