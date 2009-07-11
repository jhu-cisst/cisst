/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

// include what is needed from cisst
#include <cisstCommon.h>
#include <cisstOSAbstraction/osaSocket.h>
#include <cisstOSAbstraction/osaSleep.h>

// system includes
#include <ctype.h>
#include <string.h>
#include <iostream>


// main function
int main(void) {
    // add cout for all log
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::HaltDefaultLog();
    cmnLogger::ResumeDefaultLog(CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("osaSocket", CMN_LOG_LOD_VERY_VERBOSE);

    osaSocket ssock;
    ssock.AssignPort(1234);

    char buffer[512];
    int bytesRead;
    bool done = false;

    CMN_LOG_RUN_DEBUG << "Server for socket test program." << std::endl;
    while (!done) {
        bytesRead = ssock.Receive(buffer, sizeof(buffer));
        if (bytesRead > 0) {
            std::cout << "Server received: " << buffer << std::endl;
            for (int i = 0; i < bytesRead; i++)
                buffer[i] = toupper(buffer[i]);
            buffer[bytesRead] = 0;
            ssock.SendString(buffer);
            if (strcmp(buffer, "QUIT") == 0)
                done = true;
        }
        osaSleep(10 * cmn_ms);
    }

    ssock.Close();
    return 0;
}
