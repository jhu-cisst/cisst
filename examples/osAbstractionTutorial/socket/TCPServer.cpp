/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides
  Created on: 2009

  (C) Copyright 2007-2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon.h>
#include <cisstOSAbstraction.h>

int main(void)
{
    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("osaSocket", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("osaSocketServer", CMN_LOG_LOD_VERY_VERBOSE);

    osaSocketServer socketServer;
    osaSocket * socket;
    unsigned short port = 1234;
    std::string response;
    bool isServerConnected = false;
    char buffer[512];
    int bytesRead;

    std::cout << "> Listen on port 1234? [y/n] ";
    std::cin >> response;
    if (response.compare("n") == 0) {
        std::cout << "> Enter port: ";
        std::cin >> port;
    }
    while (!socketServer.AssignPort(port)) {
        std::cout << "Will try again in 5 seconds" << std::endl;
        osaSleep(5.0 * cmn_s);
    }
    isServerConnected = socketServer.Listen();

    std::cout << std::endl
              << "Started server on "
              << osaSocket::GetLocalhostIP() << ":" << port << std::endl
              << std::endl;

    while (isServerConnected) {
        socket = socketServer.Accept();
        if (socket != NULL) {
            socket->Send("Server has accepted your request");
        }

        socket = socketServer.Select();
        if (socket == NULL) {
            continue;
        }

        // receive
        bytesRead = socket->Receive(buffer, sizeof(buffer));
        if (bytesRead > 0) {
            for (int i = 0; i < bytesRead; i++) {
                buffer[i] = toupper(buffer[i]);
            }
            buffer[bytesRead] = 0;

            if (strcmp(buffer, "EXITCLIENT") == 0) {
                socket->Close();
                continue;
            } else if (strcmp(buffer, "EXIT") == 0) {
                socket->Close();
                isServerConnected = false;
                continue;
            }

            // send
            socket->Send(buffer);
        }
        osaSleep(100.0 * cmn_ms);
    }
    socketServer.Close();

    return 0;
}
