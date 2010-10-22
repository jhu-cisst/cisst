/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
  Author(s):  Min Yang Jung, Anton Deguet
  Created on: 2010-01-20

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon/cmnPortability.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsManagerGlobal.h>

int main(int CMN_UNUSED(argc), char ** CMN_UNUSED(argv))
{
    // Create and start global component manager
    mtsManagerGlobal globalComponentManager;
    if (!globalComponentManager.StartServer()) {
        return 1;
    }

    while (1) {
        osaSleep(10 * cmn_ms);
    }

    return 0;
}
