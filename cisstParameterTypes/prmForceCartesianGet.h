/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:	2009-02-06

  (C) Copyright 2009-2023 Johns Hopkins University (JHU), All Rights Reserved.

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

#include <cisstMultiTask/mtsGenericObject.h>

#include <Eigen/Dense>

// Always include last
#include <cisstParameterTypes/prmExport.h>

/*! Cartesian force get command argument */
class CISST_EXPORT prmForceCartesianGet: public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    typedef mtsGenericObject BaseType;

    /*! default constructor */
    inline prmForceCartesianGet(void):
        mtsGenericObject()
    {}

    /*! constructor with all parameters */
    inline prmForceCartesianGet(const Eigen::Vector<double, 6> &force):
        ForceMember(force)
    {
        this->MaskMember.fill(true);
    }

    /*!destructor
     */
    virtual ~prmForceCartesianGet();

    /*! Set and Get methods for force */
    //@{
    typedef Eigen::Vector<double, 6> ForceType; // to prevent annoying macro issues
    CMN_DECLARE_MEMBER_AND_ACCESSORS(ForceType, Force);
    //@}

    /*! Set and Get methods for mask */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(Eigen::ArrayX<bool>, Mask);
    //@}
    CMN_DECLARE_MEMBER_AND_ACCESSORS(std::string, ReferenceFrame);
    CMN_DECLARE_MEMBER_AND_ACCESSORS(std::string, MovingFrame);

public:
    /*! Const reference to the first 3 elements (force part) of the
      "Force" vector. */
    inline auto F(void) const {
        return ForceMember.head<3>();
    }

    /*! Reference to the first 3 elements (force part) of the "Force"
      vector. */
    inline auto F(void) {
        return ForceMember.head<3>();
    }

    /*! Const reference to the last 3 elements (torque part) of the
      "Force" vector. */
    inline auto T(void) const {
        return ForceMember.tail<3>();
    }

    /*! Reference to the last 3 elements (torque part) of the "Force"
      vector. */
    inline auto T(void) {
        return ForceMember.tail<3>();
    }

    /*! Human readable output to stream. */
    void ToStream(std::ostream & outputStream) const override;

    /*! To stream raw data. */
    void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                     bool headerOnly = false, const std::string & headerPrefix = "") const override;

    /*! Binary serialization */
    void SerializeRaw(std::ostream & outputStream) const override;

    /*! Binary deserialization */
    void DeSerializeRaw(std::istream & inputStream) override;

}; // _prmForceCartesianGet_h


CMN_DECLARE_SERVICES_INSTANTIATION(prmForceCartesianGet);


#endif // _prmForceCartesianGet_h
