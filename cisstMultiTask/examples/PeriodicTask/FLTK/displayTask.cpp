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

#include "displayTask.h"

// Following needed for Windows; does not seem to matter for Linux
template int mtsComponentFLTK<displayUI>::StartRunLoop(void);

CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG_TEMPLATED(displayTask, mtsTaskContinuous, std::string);
