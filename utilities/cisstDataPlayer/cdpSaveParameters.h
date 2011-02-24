/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Marcin Balicki
  Created on: 2011-02-10

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

/*!
\file
\brief Container for save parameters.
*/


#ifndef _cdpSaveParameters_h
#define _cdpSaveParameters_h

#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstMultiTask/mtsGenericObjectProxy.h>


class cdpSaveParameters: public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    typedef cdpSaveParameters ThisType;

    /*! default constructor - does nothing for now */
    cdpSaveParameters();

    /*! Copy constructor. */
    inline cdpSaveParameters(const ThisType & other):
        mtsGenericObject( other ),
        StartMember( other.Start() ),
        EndMember( other.End() ),
        PathMember( other.Path() ),
        PrefixMember( other.Prefix() )
    {}


    /*! Human readable output to stream. */
    void ToStream(std::ostream & outputStream) const;

    /*! destructor */
    ~cdpSaveParameters() {};

    /*! Allocate memory based on an existing object of the same type.  The
    object is provided via a cmnGenericObject pointer.  If a dynamic cast
    to this type is not possible, this method returns false. */
    bool inline Allocate(const cmnGenericObject * model)
    {
        const ThisType * pointer = dynamic_cast<const ThisType *>(model);
        if (pointer == 0) {
            return false;
        }
        return true;
    }


    /*! Set and Get methods for the values */
    //@{
    MTS_DECLARE_MEMBER_AND_ACCESSORS(double, Start);
    //@}

    /*! Set and Get methods Mask. True indicates an existting force/torque axis  */
    //@{
    MTS_DECLARE_MEMBER_AND_ACCESSORS(double, End);
    //@}

    /*! Set and Get methods for IsSaturated vector  */
    //@{
    MTS_DECLARE_MEMBER_AND_ACCESSORS(mtsStdString, Path);
    //@}

    /*!  */
    //@{
    MTS_DECLARE_MEMBER_AND_ACCESSORS(mtsStdString, Prefix);
    //@}


};

CMN_DECLARE_SERVICES_INSTANTIATION(cdpSaveParameters);

#endif // _cdpSaveParameters_h
