/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ui3VisibleObject.h 227 2009-04-03 21:39:16Z adeguet1 $

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

#ifndef _ui3VisibleList_h
#define _ui3VisibleList_h

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassServices.h>
#include <cisstCommon/cmnClassRegisterMacros.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctTransformationTypes.h>

#include <cisst3DUserInterface/ui3ForwardDeclarations.h>
#include <cisst3DUserInterface/ui3VTKForwardDeclarations.h>
#include <cisst3DUserInterface/ui3SceneManager.h>
#include <cisst3DUserInterface/ui3VisibleObject.h>


/*!  
  Cheap implementation of Composite Pattern, group of visible
  objects behaving like a single visible object
*/
class ui3VisibleList: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);

    friend class ui3SceneManager;

public:

    ui3VisibleList(ui3Manager * manager);

    /*!
     Destructor
    */
    ~ui3VisibleList(void) {};

    bool CreateVTKObjects(void);

    void Add(ui3VisibleObject * object) {
        this->Objects.push_back(object);
    }

protected:
    
    typedef std::list <ui3VisibleObject *> ListType;
    ListType Objects;
};


CMN_DECLARE_SERVICES_INSTANTIATION(ui3VisibleList);


#endif // _ui3VisibleList_h
