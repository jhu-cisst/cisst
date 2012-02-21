/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsFaultDetectorBase.cpp 3303 2012-01-02 16:33:23Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2012-02-07

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#include <cisstMultiTask/mtsFaultDetectorBase.h>

mtsFaultDetectorBase::mtsFaultDetectorBase(const std::string & detectorName) 
    : mtsMonitorFilterBase(mtsMonitorFilterBase::FAULT_DETECTOR, detectorName),
      TargetFault(0)
{
}

mtsFaultDetectorBase::~mtsFaultDetectorBase()
{
    if (TargetFault)
        delete TargetFault;
}

bool mtsFaultDetectorBase::RegisterFault(mtsFaultBase * targetFault)
{
    if (!targetFault) {
        CMN_LOG_CLASS_RUN_ERROR << "RegisterFault: invalid target fault instance" << std::endl;
        return false;
    }
    if (TargetFault) {
        CMN_LOG_CLASS_RUN_ERROR << "RegisterFault: Target fault already registered: \"" << TargetFault << "\"" << std::endl;
        return false;
    }

    TargetFault = targetFault;

    CMN_LOG_CLASS_RUN_VERBOSE << "RegisterFault: target fault associated: \"" << TargetFault << "\"" << std::endl;

    return true;
}

void mtsFaultDetectorBase::DoFiltering(bool debug)
{
    CheckFault(debug);
}
