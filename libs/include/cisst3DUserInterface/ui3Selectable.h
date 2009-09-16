/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ui3Selectable.h 858 2009-09-15 13:39:28Z adeguet1 $

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

#ifndef _ui3Selectable_h
#define _ui3Selectable_h

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassServices.h>
#include <cisstCommon/cmnClassRegisterMacros.h>
#include <cisstCommon/cmnAccessorMacros.h>

#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctTransformationTypes.h>

#include <cisst3DUserInterface/ui3ForwardDeclarations.h>
#include <cisst3DUserInterface/ui3VTKForwardDeclarations.h>
#include <cisst3DUserInterface/ui3VisibleObject.h>

// Always include last!
#include <cisst3DUserInterface/ui3Export.h>

/*!
 Provides a base class for all visible objects.
*/
class CISST_EXPORT ui3Selectable: public ui3VisibleObject
{

public:
    typedef ui3VisibleObject BaseType;

    ui3Selectable(const std::string & name = "Unnamed");

    /*!
     Destructor
    */
    virtual ~ui3Selectable(void) {};

    void ResetOverallIntention(void);

    double UpdateOverallIntention(double intention);

    virtual double GetIntention(const vctFrm3 & cursorPosition) const = 0; 

    virtual void ShowIntention() = 0;

    inline void Select(const vctFrm3 initialPosition) {
        this->SetSelected(true);
        this->InitialPosition.Assign(initialPosition);
        this->OnSelect();
    }

    inline void Release(const vctFrm3 finalPosition) {
        this->SetSelected(false);
        this->FinalPosition.Assign(finalPosition);
        this->OnRelease();
    }

    CMN_DECLARE_MEMBER_AND_ACCESSORS(bool, Activated);

    CMN_DECLARE_MEMBER_AND_ACCESSORS(bool, Selected);

public:
    /*! Callbacks to be overloaded by derived classes */
    virtual void OnSelect(void) = 0;
    virtual void OnRelease(void) = 0;

protected:
    vctFrm3 InitialPosition, FinalPosition;
    ui3MasterArm * MasterArm;
    double OverallIntention;
};


#endif // _ui3Selectable_h
