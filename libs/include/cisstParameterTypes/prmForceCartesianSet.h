/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Rajesh Kumar, Anton Deguet
  Created on:	2008-03-12

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*! 
  \file 
  \brief Cartesian position move parameters.
*/


#ifndef _prmForceCartesianSet_h
#define _prmForceCartesianSet_h

#include <cisstParameterTypes/prmTransformationBase.h>
#include <cisstParameterTypes/prmTransformationManager.h>
#include <cisstParameterTypes/prmMotionBase.h>

// Always include last
#include <cisstParameterTypes/prmExport.h>

/*! Cartesian position motion command arguments - default motion is a line between the current position and the goal
*/

class CISST_EXPORT prmForceCartesianSet: public prmMotionBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
public:
    typedef vctFixedSizeVector<double, 6> ForceType;
    typedef vctFixedSizeVector<bool, 6> MaskType;

 protected:
    /*! Set and Get methods for the reference frame for current
        force, mask, movinf and reference frames.  This is 
        defined by a node in the transformation tree. */

    MTS_DECLARE_MEMBER_AND_ACCESSORS(prmTransformationBasePtr, MovingFrame);
    MTS_DECLARE_MEMBER_AND_ACCESSORS(prmTransformationBasePtr, ReferenceFrame);
    MTS_DECLARE_MEMBER_AND_ACCESSORS(ForceType, Force);
    MTS_DECLARE_MEMBER_AND_ACCESSORS(MaskType, Mask);
    
 public:
    /*! default constructor */
    prmForceCartesianSet():
        MovingFrameMember(NULL),
        ReferenceFrameMember(NULL),
        ForceMember(0.0),
        MaskMember(true)
    {}
    
    /*!constructor with all parameters */
    prmForceCartesianSet(const prmTransformationBasePtr & movingFrame, 
                         const prmTransformationBasePtr & referenceFrame, 
                         const ForceType & force,
                         const MaskType & mask):
        MovingFrameMember(movingFrame),
        ReferenceFrameMember(referenceFrame),
        ForceMember(force),
        MaskMember(mask)
    {}

    inline void SetAll(double forceSet, bool mask)
    {
        ForceMember.SetAll(forceSet);
        MaskMember.SetAll(mask);
    }
    
    /*!destructor
     */
    virtual ~prmForceCartesianSet();
    
}; // _prmForceCartesianSet_h


CMN_DECLARE_SERVICES_INSTANTIATION(prmForceCartesianSet);


#endif // _prmForceCartesianSet_h

