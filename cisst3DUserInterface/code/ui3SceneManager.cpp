/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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


#include <cisst3DUserInterface/ui3SceneManager.h>
#include <cisst3DUserInterface/ui3VisibleObject.h>
#include <cisst3DUserInterface/ui3VisibleList.h>
#include <cisst3DUserInterface/ui3VTKRenderer.h>

#include <vtkActor.h>
#include <vtkPropAssembly.h>
#include <vtkProp3D.h>


CMN_IMPLEMENT_SERVICES(ui3SceneManager);


ui3SceneManager::ui3SceneManager(void):
    VisibleObjects(0)
{
    this->VisibleObjects = new ui3VisibleList("Scene objects");
    this->VisibleObjects->IsSceneList = true;
    this->VisibleObjects->Show(); // leave visibility control to each part
    CMN_ASSERT(this->VisibleObjects);
}


ui3SceneManager::~ui3SceneManager(void)
{
    if (this->VisibleObjects) {
        delete this->VisibleObjects;
    }
}


bool ui3SceneManager::AddRenderer(ui3VTKRenderer * renderer)
{
    size_t rendererIndex = this->Renderers.size();
    this->Renderers.resize(rendererIndex + 1);
    this->Renderers[rendererIndex] = renderer;
    renderer->Add(this->VisibleObjects);
    return true; // to be fixed later
}


ui3SceneManager::VTKHandleType ui3SceneManager::Add(ui3VisibleObject * object)
{
    this->VisibleObjects->Add(object);
    return 0;
}


bool ui3SceneManager::Delete(VTKHandleType propHandle)
{
#if 0
    // make sure we have this handle registered
    PropIteratorType iterator = this->PropMap.find(propHandle);
    if (iterator != this->PropMap.end()) {
        // use VTK Delete() first
        (*iterator).second->Delete();
        // then erase from map
        this->PropMap.erase(iterator);
        return true;
    }
#endif
    return false;
}


vtkProp3D * ui3SceneManager::Lock(VTKHandleType propHandle)
{
    // lock the whole scene
    this->Mutex.Lock();
    this->LockHandle = propHandle;
    // should we test to see if this handle actually exists?
    return reinterpret_cast<vtkProp3D *>(propHandle);
}


bool ui3SceneManager::Unlock(VTKHandleType propHandle)
{
    // check if we are trying to release with the correct handle
    if (propHandle != this->LockHandle) {
        CMN_LOG_CLASS_RUN_ERROR << "Release: attempt to release with wrong handle" << std::endl;
        return false;
    }
    // set handle to 0 and release
    this->LockHandle = 0;
    this->Mutex.Unlock();
    return true;
}
