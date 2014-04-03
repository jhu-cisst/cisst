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

// Always include last!
#include <cisst3DUserInterface/ui3Export.h>

/*!  
  Cheap implementation of Composite Pattern, group of visible
  objects behaving like a single visible object
*/
class CISST_EXPORT ui3VisibleList: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    friend class ui3SceneManager;

public:

    ui3VisibleList(const std::string & name);

    /*!
     Destructor
    */
    ~ui3VisibleList(void) {};

    bool Update(ui3SceneManager * sceneManager);

    bool CreateVTKObjects(void);

    bool UpdateVTKObjects(void);

    bool Add(ui3VisibleObject * object);

    void RemoveLast(void) {
        this->Objects.pop_back();
    }
    
    ui3VisibleObject * GetLast(void){
        return this->Objects.back();
    }
    
    size_t size(void) const  {
        return this->Objects.size();
    }

    void RecursiveUpdateNeeded(void);

protected:

    void PropagateVisibility(bool visible);

    typedef std::list<ui3VisibleObject *> ListType;
    ListType Objects;


    CMN_DECLARE_MEMBER_AND_ACCESSORS(bool, UpdateNeeded);


};


CMN_DECLARE_SERVICES_INSTANTIATION(ui3VisibleList);


#endif // _ui3VisibleList_h
