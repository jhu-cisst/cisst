/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

Author(s):  Mark Finkelstein, Ali Uneri, Peter Kazanzides
Created on: 2009-08-17

(C) Copyright 2007-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaSocketServer.h>

#if (CISST_OS == CISST_WINDOWS)
#include <cisstOSAbstraction/osaSleep.h>
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#define WINSOCKVERSION MAKEWORD(2,2)
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // for close
#if (CISST_OS == CISST_QNX)
#include <sys/select.h>
#endif
#endif


osaSocketServer::osaSocketServer(void)
{
    int retval = 0;
#if (CISST_OS == CISST_WINDOWS)
    WSADATA wsaData;
    retval = WSAStartup(WINSOCKVERSION, &wsaData);
    if (retval != 0) {
        CMN_LOG_CLASS_RUN_ERROR << "osaSocketServer: WSAStartup failed with error code " << retval << std::endl;
        return;
    }
#endif

    ServerSocketFD = socket(PF_INET, SOCK_STREAM, 0);
    if (ServerSocketFD == INVALID_SOCKET) {
        CMN_LOG_CLASS_RUN_ERROR << "osaSocketServer: failed to create a socket" << std::endl;
    }
    CMN_LOG_CLASS_RUN_VERBOSE << "osaSocketServer: created socket server " << ServerSocketFD << std::endl;

    int optval = 1;
    retval = setsockopt(ServerSocketFD, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&optval), sizeof(optval));
    if (retval == SOCKET_ERROR) {
        CMN_LOG_CLASS_RUN_ERROR << "osaSocketServer: failed to set socket options" << std::endl;
    }

    // Change to non-blocking socket
#if (CISST_OS == CISST_WINDOWS)
    unsigned long arg = 1L;
    if (ioctlsocket(ServerSocketFD, FIONBIO, &arg) == SOCKET_ERROR) {
        CMN_LOG_CLASS_RUN_ERROR << "osaSocketServer: failed to set socket to non-blocking mode" << std::endl;
    }
#else
    int arg=0;
    if (ioctl(ServerSocketFD, FIONBIO, &arg) == -1 ) {
        CMN_LOG_CLASS_RUN_ERROR << "osaSocketServer: failed to set socket to non-blocking mode" << std::endl;
    }
#endif
}


osaSocketServer::~osaSocketServer(void)
{
    Close();
#if (CISST_OS == CISST_WINDOWS)
    WSACleanup();
#endif
}


bool osaSocketServer::AssignPort(unsigned short port)
{
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int retval = bind(ServerSocketFD, reinterpret_cast<struct sockaddr *>(&serverAddr), sizeof(serverAddr));
    if (retval == SOCKET_ERROR) {
        CMN_LOG_CLASS_RUN_ERROR << "AssignPort: failed to bind socket" << std::endl;
        return false;
    }
    return true;
}


bool osaSocketServer::Listen(int backlog)
{
    int retval = listen(ServerSocketFD, backlog);
    if (retval == SOCKET_ERROR) {
        CMN_LOG_CLASS_RUN_ERROR << "Listen: failed to listen" << std::endl;
        return false;
    }
    return true;
}


osaSocket * osaSocketServer::Accept(void)
{
    struct sockaddr_in serverAddr;
    socklen_t s = sizeof(serverAddr);
#if (CISST_OS == CISST_WINDOWS)
    int newSocketFD = INVALID_SOCKET;
    for (int numTrials = 0; numTrials < 10; numTrials++) {
        newSocketFD = accept(ServerSocketFD, (struct sockaddr *)&serverAddr, &s);
        if (newSocketFD == INVALID_SOCKET) {
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK) {
                // Resource temporarily unavailable. Try again.
                osaSleep(10.0 * cmn_ms);
            }
            else {
                CMN_LOG_CLASS_RUN_ERROR << "osaSocketServer::Accept failed, error = "
                                        << err << std::endl;
                break;
            }
        }
        else {
            if (numTrials > 0)
                CMN_LOG_CLASS_RUN_VERBOSE << "osaSocketServer::Accept succeeded after "
                                          << numTrials << " retries" << std::endl;
            break;
        }
    }
#else
    int newSocketFD = accept(ServerSocketFD, (struct sockaddr *)&serverAddr, &s);
#endif
    if (newSocketFD == INVALID_SOCKET) {
        return 0;
    }
    CMN_LOG_CLASS_INIT_DEBUG << "Accept: connection request accepted " <<  inet_ntoa (serverAddr.sin_addr)<<":"<<ntohs(serverAddr.sin_port) << std::endl;
    osaSocket * newSocket = new osaSocket(&newSocketFD);
    //connected so set the sockets state to true;
    newSocket->Connected = true;
    //Clients.insert(std::pair<int, osaSocket *>(newSocketFD, newSocket));
    return newSocket;
}


void osaSocketServer::Close(void)
{
    if (ServerSocketFD >= 0) {
#if (CISST_OS == CISST_WINDOWS)
        closesocket(ServerSocketFD);
#else
        close(ServerSocketFD);
#endif
        CMN_LOG_CLASS_RUN_VERBOSE << "Close: closed socket server " << ServerSocketFD << std::endl;
        ServerSocketFD = -1;
    }
}
