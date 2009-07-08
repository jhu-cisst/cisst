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
#include <string.h>
#include <iostream>


// main function
int main(void) {
    // add cout for all log
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::HaltDefaultLog();
    cmnLogger::ResumeDefaultLog(CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("osaSocket", CMN_LOG_LOD_VERY_VERBOSE);

    osaSocket csock;
    csock.SetDestination("127.0.0.1", 1234);

    std::string str;
    char buffer[512];
    int bytesRead;
    bool done = false;

    CMN_LOG_RUN_DEBUG << "Client for socket test program." << std::endl;
#if 0
    unsigned short portNum;
    std::cout << "Enter port: ";
    std::cin >> portNum;
    std::cout << "Setting client port to " << portNum << std::endl;
    csock.AssignPort(portNum);
#endif
    CMN_LOG_RUN_DEBUG << "Enter message, server will respond with uppercase string." << std::endl;
    CMN_LOG_RUN_DEBUG << "Type 'q' to exit client, 'quit' to exit both client and server." << std::endl;
    while (!done) {
        std::cout << "Enter message: ";
        getline(std::cin, str);
        if (str.length() == 0) continue;
        if (str != "q") {
            csock.SendString(str);
            do {
               osaSleep(0.2 * cmn_s);
               bytesRead = csock.Receive(buffer, sizeof(buffer)-1);
            } while (bytesRead <= 0);
            buffer[bytesRead] = 0;
            std::cout << "Client received: " << buffer << std::endl;
            if (strcmp(buffer, "QUIT") == 0)
                done = true;
        }
        else
            done = true;
    }
    return 0;
}

