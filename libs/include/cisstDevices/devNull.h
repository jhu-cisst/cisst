/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Peter Kazanzides
  Created on: 2008-09-29

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _devNull_h
#define _devNull_h

/*!
  \file
  \brief Defines a null device.
*/

#include <cisstMultiTask/mtsComponent.h>

// Always include last
#include <cisstDevices/devExport.h>

/*!
  \ingroup cisstMultiTask

  This class provides a null device.
*/
class CISST_EXPORT devNull: public mtsComponent {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    devNull(const std::string & name) : mtsComponent(name) {}
    ~devNull() {}

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
	void Create(void * CMN_UNUSED(data) = 0) {}
    void Run() {}
	void Start(void) {}
	void Kill(void) {}
    void Cleanup(void) {};
};


CMN_DECLARE_SERVICES_INSTANTIATION(devNull)


#endif // _devNull_h

