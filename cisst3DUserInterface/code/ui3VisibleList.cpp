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


#include <cisst3DUserInterface/ui3VisibleList.h>
#include <cisst3DUserInterface/ui3SceneManager.h>

#include <vtkAssembly.h>
#include <vtkProperty.h>
#include <vtkMatrix4x4.h>


CMN_IMPLEMENT_SERVICES(ui3VisibleList);


ui3VisibleList::ui3VisibleList(const std::string & name):
    ui3VisibleObject(name + " (list)"),
    UpdateNeededMember(true)
{
}


bool ui3VisibleList::Add(ui3VisibleObject * objectPointer)
{
    // verify that the pointer is not already in the list
    if (std::find(this->Objects.begin(),
                  this->Objects.end(),
                  objectPointer) != this->Objects.end()) {
        CMN_LOG_CLASS_RUN_ERROR << "Add: object \"" << objectPointer->Name() << "\" already in list \""
                                << this->Name() << "\"" << std::endl;
        return false;
    }
    // otherwise, add it to the list
	this->Objects.push_back(objectPointer);
	objectPointer->ParentList = this;
	this->RecursiveUpdateNeeded();
	CMN_LOG_CLASS_RUN_VERBOSE << "Add: object \"" << objectPointer->Name() << "\" added to list \""
	                          << this->Name() << "\"" << std::endl;
    return true;
}


void ui3VisibleList::RecursiveUpdateNeeded(void)
{
    this->UpdateNeededMember = true;
    this->Assembly->Modified();
    if (this->ParentList) {
        this->ParentList->RecursiveUpdateNeeded();
    }
}


bool ui3VisibleList::UpdateVTKObjects(void)
{
    bool result = true;
    const ListType::iterator end = Objects.end();
    ListType::iterator iterator;

    for (iterator = Objects.begin();
         iterator != end;
         ++iterator) {

        result &= ((*iterator)->UpdateVTKObjects());
    }
    return result;
}


bool ui3VisibleList::Update(ui3SceneManager * sceneManager)
{
    // debug
    CMN_ASSERT(sceneManager);

    if (!this->UpdateNeededMember) {
        return false; // there was no update
    }

    // pseudo creation, just set the scene manager pointer
    this->SceneManager = sceneManager;

    // if an object has been added, we check all of them
    bool result = true;
    const ListType::iterator end = Objects.end();
    ListType::iterator iterator;
    vtkProp3D * objectVTKProp;
    VTKHandleType propHandle;

    ui3VisibleList * listPointer;
    ui3VisibleObject * objectPointer;

    for (iterator = Objects.begin();
         iterator != end;
         ++iterator) {

        // check if the object is a list or not
        objectPointer = (*iterator);
        listPointer = dynamic_cast<ui3VisibleList *>(objectPointer);

        if (listPointer) {
            // this is a list, i.e. a node

            // check if the corresponding VTK object has been added
            if (!listPointer->Created()) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Update: list \"" << listPointer->Name()
                                          << "\" added in update for \""
                                          << this->Name() << "\"" << std::endl;
                // set vtk handle on visible object
                objectVTKProp = listPointer->GetVTKProp();
                CMN_ASSERT(objectVTKProp);

                // convert pointer to (void *)
                propHandle = reinterpret_cast<VTKHandleType>(objectVTKProp);
                listPointer->SetVTKHandle(propHandle);
 
                // add to assembly
                this->Assembly->AddPart(objectVTKProp);

                // mark as created so we don't create it again
                listPointer->SetCreated(true);
            }

            // pass the update to children
            listPointer->Update(sceneManager);


        } else {
            // check it this object has already been created
            if (!objectPointer->Created()) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Update: calling Update for object \"" << objectPointer->Name()
                                          << "\" in update for \""
                                          << this->Name() << "\"" << std::endl;
                result &= objectPointer->Update(sceneManager);

                // add to assembly
                objectVTKProp = objectPointer->GetVTKProp();
                this->Assembly->AddPart(objectVTKProp);
            }
        }
        objectPointer->PropagateVisibility(objectPointer->Visible());
    }

    this->UpdateNeededMember = false;

    return true;
}


bool ui3VisibleList::CreateVTKObjects(void)
{
    return true;
}


void ui3VisibleList::PropagateVisibility(bool visible)
{
    const ListType::iterator end = Objects.end();
    ListType::iterator iterator;

    if (visible) {
        for (iterator = Objects.begin();
             iterator != end;
             ++iterator) {
            (*iterator)->PropagateVisibility(this->Visible());
        }
    } else {
        for (iterator = Objects.begin();
             iterator != end;
             ++iterator) {
            (*iterator)->PropagateVisibility(false);
        }
    }
}
