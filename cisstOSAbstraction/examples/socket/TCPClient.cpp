/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Peter Kazanzides
  Created on: 2009

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnLogger.h>
#include <cisstCommon/cmnUnits.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstOSAbstraction/osaSocket.h>

int main(void)
{
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("osaSocket", CMN_LOG_ALLOW_ALL);

    osaSocket socket(osaSocket::TCP);
    std::string host = "127.0.0.1";
    unsigned short port = 1234;
    std::string response;
    bool isClientConnected = false;
    std::string message;
    char buffer[512];
    int bytesRead;

    std::cout << "> Connect to 127.0.0.1:1234? [y/n] ";
    std::cin >> response;
    if (response.compare("n") == 0) {
        std::cout << "> Enter host: ";
        std::cin >> host;
        std::cout << "> Enter port: ";
        std::cin >> port;
    }
    isClientConnected = socket.Connect(host.c_str(), port);

    std::cout << std::endl
              << "Send a string message and server will respond with the uppercase version" << std::endl
              << "  'exitclient' closes the client" << std::endl
              << "  'exit' closes both client and server" << std::endl
              << std::endl;

    while (isClientConnected) {
        // receive
#if 1
        do {
            osaSleep(100.0 * cmn_ms);
            bytesRead = socket.Receive(buffer, sizeof(buffer));
        } while (bytesRead <= 0);
        buffer[bytesRead] = 0;
#else
        std::string rec;
        // socket >> rec;
        std::cout << "Received \"" << rec << "\"" << std::endl;
#endif
        // send
        std::cout << "> Message: ";
        std::cin >> message;
        if (message.length() == 0) {
            continue;
        } else if (message.compare("exitclient") == 0) {
            isClientConnected = false;
        } else if (message.compare("exit") == 0) {
            isClientConnected = false;
        }
#ifdef OSA_SOCKET_WITH_STREAM
        socket << message;
#else
        socket.Send(message);
#endif // OSA_SOCKET_WITH_STREAM
    }
    socket.Close();

    return 0;
}
