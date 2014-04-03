/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _displayTask_h
#define _displayTask_h

#include "displayUI.h"

#ifdef mtsExPeriodicTaskGuiFLTK_EXPORTS
#define CISST_THIS_LIBRARY_AS_DLL
#endif
#include <cisstCommon/cmnExportMacros.h>
#undef CISST_THIS_LIBRARY_AS_DLL

#include <cisstMultiTask/mtsComponentFLTK.h>

typedef mtsComponentFLTK<displayUI> displayTask;
CMN_DECLARE_SERVICES_INSTANTIATION(displayTask)

#endif // _displayTask_h
