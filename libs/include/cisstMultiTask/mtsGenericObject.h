/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsGenericObject.h 75 2009-02-24 16:47:20Z adeguet1 $

  Author(s):	Anton Deguet
  Created on:	2009-04-13

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
  \brief Defines mtsGenericObject
*/

#ifndef _mtsGenericObject_h
#define _mtsGenericObject_h

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassServices.h>

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
class mtsGenericObject: public cmnGenericObject {


    /*! Time stamp.  When used in conjunction with mtsStateTable, the
      time stamp will be automatically updated using
      AutomaticTimeStamp. */
    MTS_DECLARE_MEMBER_AND_ACCESSORS(double, Timestamp);

    /*! Turn on/off automatic timestamping by the state table.  All
      objects stored in a state table can be automatically timestamped
      at the end of the Run method (see mtsTask and derived classes).
      If this flag is set to false, data objects will have to be
      timestamped manually.  This flag is set to true by default. */
    MTS_DECLARE_MEMBER_AND_ACCESSORS(bool, AutomaticTimestamp);

    /*! General flag used to indicate if the data is valid.  This flag
      has to be updated by the user.  This data member is initialized
      to false by the constructor. */
    MTS_DECLARE_MEMBER_AND_ACCESSORS(bool, Valid);

public:

    /*! Constructor, initialize Timestamp to 0 and Valid flag to
      false. */
    inline mtsGenericObject(void):
        TimestampMember(0.0),
        AutomaticTimestampMember(true),
        ValidMember(false)
    {}

    /*! Destructor.  Does nothing specific. */
    inline virtual ~mtsGenericObject(void)
    {}

    /*! Set timestamp if AutomaticTimeStamp is set to true. This is
      only meaningful if the derived class contains a timestamp and
      overrides this method.
      \param timeStamp time stamp in seconds
      \returns true if time stamp was set. */
    inline bool SetTimestampIfAutomatic(double timestamp) {
        if (this->AutomaticTimestampMember) {
            this->TimestampMember = timestamp;
            return true;
        }
        return false;
    }

    /*! ToStream method.  This method only streams the data members of
      mtsGenericObject, i.e. the Timestamp and Valid flag.  It should
      be called by any derived class re-implementing ToStream. */
    virtual void ToStream(std::ostream & outputStream) const {
        outputStream << "Timestamp (s): " << this->Timestamp();
        if (this->Valid()) {
            outputStream << " (valid)";
        } else {
            outputStream << " (invalid)";
        }
    }

    virtual void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                             bool headerOnly = false, const std::string & headerPrefix = "") const {
        if (headerOnly) {
            outputStream << headerPrefix << "-timestamp" << delimiter
                         << headerPrefix << "-valid";
        } else {
            outputStream << this->Timestamp() << delimiter
                         << this->Valid();
        } 
    }

};


#endif // _mtsGenericObject_h

