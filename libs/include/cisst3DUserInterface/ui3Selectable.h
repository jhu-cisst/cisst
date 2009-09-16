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

    virtual double GetIntention(const vctFrm3 & cursorPosition) const = 0; 

    virtual void ShowIntention(double intention) = 0;

    CMN_DECLARE_MEMBER_AND_ACCESSORS(bool, Activated);

    CMN_DECLARE_MEMBER_AND_ACCESSORS(bool, Selected);

 protected:
    ui3MasterArm * MasterArm;
};


#endif // _ui3Selectable_h
