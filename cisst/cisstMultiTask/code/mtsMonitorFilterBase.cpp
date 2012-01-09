/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsMonitorBase.cpp 3303 2012-01-02 16:33:23Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2012-01-07

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#include <cisstMultiTask/mtsMonitorFilterBase.h>

int mtsMonitorFilterBase::UID = 0;

mtsMonitorFilterBase::mtsMonitorFilterBase(void)
    : FilterUID(UID++),
      FilterName("NONAME"), 
      Enabled(true),
      OutputSignals(0)
{
}

mtsMonitorFilterBase::mtsMonitorFilterBase(const std::string & filterName)
    : FilterUID(UID++),
      FilterName(filterName),
      Enabled(true),
      OutputSignals(0)
{
}

mtsMonitorFilterBase::~mtsMonitorFilterBase()
{
}

