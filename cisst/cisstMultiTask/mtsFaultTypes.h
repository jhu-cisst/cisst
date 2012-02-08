/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsFaultTypes.h 3034 2011-10-09 01:53:36Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2012-02-07

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

/*!
  \file
  \brief Declaration of various types of faults
  \ingroup cisstMultiTask
 */


#ifndef _mtsFaultTypes_h
#define _mtsFaultTypes_h

#include <cisstMultiTask/mtsFaultBase.h>

#include <cisstMultiTask/mtsExport.h>

//-----------------------------------------------------------------------------
//  Component Layer - Functional Integrity - Thread Periodicity
//
class CISST_EXPORT mtsFaultComponentThreadPeriodicity: public mtsFaultBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    /*! Constructors and destructor */
    mtsFaultComponentThreadPeriodicity(void);
    virtual ~mtsFaultComponentThreadPeriodicity();

    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsFaultComponentThreadPeriodicity);

#endif // _mtsFaultComponentThreadPeriodicity_h
