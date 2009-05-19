/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ui3VisibleList.cpp 137 2009-03-11 18:51:15Z adeguet1 $

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
#include <cisst3DUserInterface/ui3MenuBar.h>
#include <cisst3DUserInterface/ui3Manager.h>

#include <vtkAssembly.h>
#include <vtkProperty.h>
#include <vtkMatrix4x4.h>


CMN_IMPLEMENT_SERVICES(ui3VisibleList);


ui3VisibleList::ui3VisibleList(ui3Manager * manager):
    ui3VisibleObject(manager)
{
}


bool ui3VisibleList::CreateVTKObjects(void)
{
    bool result = true;
    const ListType::iterator end = Objects.end();
    ListType::iterator iterator;
    vtkProp3D * objectVTKProp;
    VTKHandleType propHandle;

    for (iterator = Objects.begin();
         iterator != end;
         ++iterator) {
        result &= (*iterator)->CreateVTKObjects();

        // set vtk handle on visible object
        objectVTKProp = (*iterator)->GetVTKProp();
        CMN_ASSERT(objectVTKProp);

        // convert pointer to (void *)
        propHandle = reinterpret_cast<VTKHandleType>(objectVTKProp);
        (*iterator)->SetVTKHandle(propHandle);

        // add to assembly
        this->Assembly->AddPart(objectVTKProp);
    }
    return result;
}