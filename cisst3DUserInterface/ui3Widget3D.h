/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:	2009-09-13

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _ui3Widget3D_h
#define _ui3Widget3D_h

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassServices.h>
#include <cisstCommon/cmnClassRegisterMacros.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctTransformationTypes.h>

#include <cisst3DUserInterface/ui3ForwardDeclarations.h>
#include <cisst3DUserInterface/ui3VTKForwardDeclarations.h>
#include <cisst3DUserInterface/ui3SceneManager.h>
#include <cisst3DUserInterface/ui3VisibleList.h>
#include <cisst3DUserInterface/ui3Selectable.h>

// Always include last!
#include <cisst3DUserInterface/ui3Export.h>


class ui3Widget3DHandle;

/*!  
  Cheap implementation of Composite Pattern, group of visible
  objects behaving like a single visible object
*/
class CISST_EXPORT ui3Widget3D: public ui3VisibleList
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    friend class ui3SceneManager;
    friend class ui3BehaviorBase;
    friend class ui3Manager;

public:
    typedef ui3VisibleList BaseType;

    ui3Widget3D(const std::string & name);

    /*!
     Destructor
    */
    ~ui3Widget3D(void) {};

    bool Add(ui3VisibleObject * object);

    CMN_DECLARE_MEMBER_AND_ACCESSORS(double, HandlesSpacing);

public:
    void SetSize(double halfSize);

    /*! Determines if the handles are active, i.e. visible and allow to move the 3D widget */
    void SetHandlesActive(bool handlesActive);
    inline bool HandlesActive(void) const {
        return this->HandlesActiveMember;
    }

protected:
    ui3VisibleList * UserObjects;
    ui3VisibleList * Handles;
    ui3Widget3DHandle * SideHandles[4];
    ui3Widget3DHandle * CornerHandles[4];

    CMN_DECLARE_MEMBER_AND_ACCESSORS(bool, SideHandlesActive);
    CMN_DECLARE_MEMBER_AND_ACCESSORS(bool, CornerHandlesActive);

    bool HandlesActiveMember;

    void UpdatePosition(void);

    int PreviousFirstSideHandle;
    int PreviousSecondSideHandle;
    int PreviousCornerHandle;

    vctFrm3 CurrentTransformation;

	void ComputeTransform(double pointa[3], double pointb[3],
                          double point1[3], double point2[3], 
                          double object_displacement[3],
				          double object_rotation[4]);
};

CMN_DECLARE_SERVICES_INSTANTIATION(ui3Widget3D);




class ui3Widget3DHandle: public ui3Selectable
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    unsigned int HandleNumber;
    ui3Widget3D * Widget3D;
    vtkSphereSource * Source;
    vtkPolyDataMapper * Mapper;
    vtkActor * Actor;

public:
    ui3Widget3DHandle(unsigned int handleNumber, ui3Widget3D * widget3D);
    
    ~ui3Widget3DHandle();

    bool CreateVTKObjects(void);

    inline bool UpdateVTKObjects(void) { return true; };

    void UpdateColor(bool selected);

    double GetIntention(const vctFrm3 & cursorPosition) const;

    void ShowIntention(void);

    inline void OnSelect(void) {};

    inline void OnRelease(void) {};
};

CMN_DECLARE_SERVICES_INSTANTIATION(ui3Widget3DHandle);


#endif // _ui3Widget3D_h
