/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):	Anton Deguet
  Created on:	2009-02-06

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*! 
  \file 
  \brief Cartesian force get parameters.
*/


#ifndef _prmForceCartesianGet_h
#define _prmForceCartesianGet_h


#include <cisstMultiTask/mtsStateIndex.h>
#include <cisstParameterTypes/prmTransformationBase.h>
#include <cisstParameterTypes/prmTransformationManager.h>
#include <cisstParameterTypes/prmTypes.h>
#include <cisstParameterTypes/prmMacros.h>

// Always include last
#include <cisstParameterTypes/prmExport.h>

/*! Cartesian force get command argument */
class CISST_EXPORT prmForceCartesianGet: public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, 5);

 public:
    /*! default constructor */
    inline prmForceCartesianGet(void):
        MovingFrameMember(0),
        ReferenceFrameMember(0)
    {}
    
    /*! constructor with all parameters */
    inline prmForceCartesianGet(const prmTransformationBasePtr & movingFrame, 
                                const prmTransformationBasePtr & referenceFrame, 
                                const vctDouble6 & force,
                                const mtsStateIndex & stateIndex):
        MovingFrameMember(movingFrame),
        ReferenceFrameMember(referenceFrame),
        ForceMember(force),
        StateIndexMember(stateIndex)
    {
        this->MaskMember.SetAll(true);
    }
    
    /*!destructor
     */
    virtual ~prmForceCartesianGet();
    

    /*! Set and Get methods for the reference frame for current
        force.  This is defined by a node in the transformation
        tree. */
    //@{
    PRM_DECLARE_MEMBER_AND_ACCESSORS(prmTransformationBasePtr, MovingFrame);
    //@}


    /*! Set and Get methods for the moving frame for current
        force.  This is defined by a node in the transformation
        tree. */
    //@{
    PRM_DECLARE_MEMBER_AND_ACCESSORS(prmTransformationBasePtr, ReferenceFrame);
    //@}


    /*! Set and Get methods for force */
    //@{
    PRM_DECLARE_MEMBER_AND_ACCESSORS(vctDouble6, Force);
    //@}


    /*! Set and Get methods for mask */
    //@{
    PRM_DECLARE_MEMBER_AND_ACCESSORS(vctBool6, Mask);
    //@}


    /*! Set and Get methods for state index.  Current state index, as
      provided for writer of the task providing the force
      data. */
    //@{
    PRM_DECLARE_MEMBER_AND_ACCESSORS(mtsStateIndex, StateIndex);
    //@}

    /*! Human readable output to stream. */
    void ToStream(std::ostream & outputStream) const;


}; // _prmForceCartesianGet_h


CMN_DECLARE_SERVICES_INSTANTIATION(prmForceCartesianGet);


#endif // _prmForceCartesianGet_h

