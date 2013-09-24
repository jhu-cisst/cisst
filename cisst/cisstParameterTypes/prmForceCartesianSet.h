/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Rajesh Kumar, Anton Deguet
  Created on:	2008-03-12

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
  \brief Cartesian position move parameters.
*/


#ifndef _prmForceCartesianSet_h
#define _prmForceCartesianSet_h

#include <cisstVector/vctFixedSizeVector.h>
#include <cisstParameterTypes/prmMotionBase.h>

// Always include last
#include <cisstParameterTypes/prmExport.h>

/*! Cartesian position motion command arguments - default motion is a line between the current position and the goal
*/

class CISST_EXPORT prmForceCartesianSet: public prmMotionBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
public:

    typedef prmMotionBase BaseType;
    typedef vctFixedSizeVector<double, 6> ForceType;
    typedef vctFixedSizeVector<bool, 6> MaskType;

 protected:
    /*! Set and Get methods for the reference frame for current
        force, mask, movinf and reference frames.  This is
        defined by a node in the transformation tree. */
    CMN_DECLARE_MEMBER_AND_ACCESSORS(ForceType, Force);
    CMN_DECLARE_MEMBER_AND_ACCESSORS(MaskType, Mask);

 public:
    /*! default constructor */
    prmForceCartesianSet():
        ForceMember(0.0),
        MaskMember(true)
    {}

    /*!constructor with all parameters */
    prmForceCartesianSet(const ForceType & force,
                         const MaskType & mask):
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

    /*! Human readable output to stream. */
    void ToStream(std::ostream & outputStream) const;

    /*! To stream raw data. */
    void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                     bool headerOnly = false, const std::string & headerPrefix = "") const;

    /*! Binary serialization */
    void SerializeRaw(std::ostream & outputStream) const;

    /*! Binary deserialization */
    void DeSerializeRaw(std::istream & inputStream);

}; // _prmForceCartesianSet_h


CMN_DECLARE_SERVICES_INSTANTIATION(prmForceCartesianSet);


#endif // _prmForceCartesianSet_h

