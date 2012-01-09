/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsMonitorFilterBasics.h 3034 2011-10-09 01:53:36Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2012-01-07

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

/*!
  \file
  \brief Defines basic types of filters used for fault detection and diagnosis
*/

#ifndef _mtsMonitorFilterBasics_h
#define _mtsMonitorFilterBasics_h

#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstMultiTask/mtsGenericObjectProxy.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsMonitorFilterBase.h>

#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask
*/

//-----------------------------------------------------------------------------
//  Bypass Filter
//
class mtsMonitorFilterBypass : public mtsMonitorFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    /*! Default constructor is provided only to satisfy the requirement of 
        cmnGenericObject.  Do not use this -- Bypass filter with no input has 
        no meaning. */
    mtsMonitorFilterBypass(); // DO NOT USE

    mtsMonitorFilterBypass(const std::string & inputName);
    ~mtsMonitorFilterBypass();

    void ToStream(std::ostream & outputStream) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsMonitorFilterBypass)

#endif // _mtsMonitorFilterBasics_h

