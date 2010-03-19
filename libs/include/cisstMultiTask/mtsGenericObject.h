/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):	Anton Deguet
  Created on:	2009-04-13

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file
  \brief Defines mtsGenericObject
*/

#ifndef _mtsGenericObject_h
#define _mtsGenericObject_h

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassServices.h>
#include <cisstCommon/cmnSerializer.h>
#include <cisstCommon/cmnDeSerializer.h>

#include <cisstMultiTask/mtsMacros.h>

// Always include last!
#include <cisstMultiTask/mtsExport.h>

#include <string>
#include <iostream>


/*!
  \brief Base class for data object in cisstMultiTask.

  \ingroup cisstMultiTask

  \sa cmnGenericObject
*/
class CISST_EXPORT mtsGenericObject: public cmnGenericObject {


    /*! Time stamp.  When used in conjunction with mtsStateTable, the
      time stamp will be automatically updated using
      AutomaticTimeStamp. */
    CMN_DECLARE_MEMBER_AND_ACCESSORS(double, Timestamp);

    /*! Turn on/off automatic timestamping by the state table.  All
      objects stored in a state table can be automatically timestamped
      at the end of the Run method (see mtsTask and derived classes).
      If this flag is set to false, data objects will have to be
      timestamped manually.  This flag is set to true by default. */
    CMN_DECLARE_MEMBER_AND_ACCESSORS(bool, AutomaticTimestamp);

    /*! General flag used to indicate if the data is valid.  This flag
      has to be updated by the user.  This data member is initialized
      to false by the constructor. */
    CMN_DECLARE_MEMBER_AND_ACCESSORS(bool, Valid);

public:

    /*! Constructor, initialize Timestamp to 0 and Valid flag to
      false. */
    inline mtsGenericObject(void):
        TimestampMember(0.0),
        AutomaticTimestampMember(true),
        ValidMember(false)
    {}

    /*! Constructor with initialization for each member */
    inline mtsGenericObject(double timestamp, bool automaticTimestamp, bool valid):
        TimestampMember(timestamp),
        AutomaticTimestampMember(automaticTimestamp),
        ValidMember(valid)
    {}
        
    /*! Copy constructor */
    inline mtsGenericObject(const mtsGenericObject & other):
        cmnGenericObject(),
        TimestampMember(other.TimestampMember),
        AutomaticTimestampMember(other.AutomaticTimestampMember),
        ValidMember(other.ValidMember)
    {}

    /*! Destructor.  Does nothing specific. */
    inline virtual ~mtsGenericObject(void)
    {}

    /*! Set timestamp if AutomaticTimeStamp is set to true. This is
      only meaningful if the derived class contains a timestamp and
      overrides this method.
      \param timeStamp time stamp in seconds
      \returns true if time stamp was set. */
    bool SetTimestampIfAutomatic(double timestamp);

    /*! Human readable text output.  This method only streams the data
      members of mtsGenericObject, i.e. the Timestamp and Valid flag.
      It should be called by any derived class re-implementing
      ToStream. */
    virtual void ToStream(std::ostream & outputStream) const;

    /*! Raw text output to stream */
    virtual void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                             bool headerOnly = false, const std::string & headerPrefix = "") const;

    /*! Binary serialization */
    virtual void SerializeRaw(std::ostream & outputStream) const;

    /*! Binary deserialization */
    virtual void DeSerializeRaw(std::istream & inputStream);

    /*! Methods for data visualization.  Derived classes should override
        the following methods in order to be properly processed by the data
        visualizer of the global component manager. */

    /*! Return a number of data (which can be visualized, i.e., type-casted
        to double) */
    virtual unsigned int GetNumberOfScalar(const bool CMN_UNUSED(visualizable) = true) const {
        return 0;
    }

    /*! Return the index-th (zero-based) value of data typecasted to double. 
        Note that an index has to be defined such that all indicies are continuous.
        (This is required by the data visualizer of the GCM UI) */
    virtual double GetScalarAsDouble(const size_t CMN_UNUSED(index)) const {
        return 0.0;
    }

    /*! Return the name of index-th (zero-based) data typecasted to double. */
    virtual std::string GetScalarName(const size_t CMN_UNUSED(index)) const {
        return "N/A";
    }
};


#endif // _mtsGenericObject_h

