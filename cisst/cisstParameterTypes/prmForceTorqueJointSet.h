/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
Author(s):	Simon Leoanrd
Created on:	2011-08-23

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
\brief Joint position move parameters.
*/


#ifndef _prmForceTorqueJointSet_h
#define _prmForceTorqueJointSet_h

// basic includes
#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstParameterTypes/prmMotionBase.h>
#include <cisstParameterTypes/prmExport.h>

/*! motion command arguments for joint angle based motion
*/
class CISST_EXPORT prmForceTorqueJointSet: public prmMotionBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);


protected:


    /*! masks correpsonding elements in goal, if an element is set
    true --> corresponding goal should be used */
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctBoolVec, Mask);

    /*! a motion goal in joint angles. only goals corresponding to
    mask elements set true are used */
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, ForceTorque);

public:

    typedef unsigned int size_type;
    typedef prmMotionBase BaseType;
    typedef prmForceTorqueJointSet ThisType;

    /*! default constructor - does nothing for now */
    inline prmForceTorqueJointSet(){ }

    /*! this constructor sets the size of the vector type members */
    inline prmForceTorqueJointSet(size_type size){ SetSize(size); }

    void SetSize(size_type size);

    /*! constructor with all possible parameters */
    prmForceTorqueJointSet(const vctBoolVec & mask,
                           const vctDoubleVec & forcetorque ):
        MaskMember(mask),
        ForceTorqueMember(forcetorque)
    {}

    /*! destructor */
    virtual ~prmForceTorqueJointSet();

    /*! Binary serialization */
    void SerializeRaw(std::ostream & outputStream) const;

    /*! Binary deserialization */
    void DeSerializeRaw(std::istream & inputStream);


}; // _prmForceTorqueJointSet_h

CMN_DECLARE_SERVICES_INSTANTIATION(prmForceTorqueJointSet);

#endif

