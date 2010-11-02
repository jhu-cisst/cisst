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
    std::string command;
    mtsManagerGlobal globalComponentManager;

    // create and start global component manager
    std::cin >> command;
    if (command == "start") {
        if (!globalComponentManager.StartServer()) {
            std::cout << "start failed" << std::endl;
            return 1;
        } else {
            std::cout << "start succeeded" << std::endl;
        }
    } else {
        std::cout << "wrong command" << std::endl;
        return 1;
    }

    // normal operations
    bool stop = false;
    while (!stop) {
        std::cin >> command;
        if (command == std::string("stop")) {
            stop = true;
        } else if (command == std::string("ping")) {
            std::cout << "ok" << std::endl;
        } else {
            std::cout << "unknown command \"" << command << "\"" << std::endl;
        }
        osaSleep(1.0 * cmn_s);
    }

    // stop component manager
    if (!globalComponentManager.StopServer()) {
        std::cout << "stop failed" << std::endl;
        return 1;
    } else {
        std::cout << "stop succeeded" << std::endl;
    }

    // wait to be killed by pipe
    while (true) {
        osaSleep(1.0 * cmn_hour);
    }
 
    return 0;
}
