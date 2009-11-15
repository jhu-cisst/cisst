/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Mark Finkelstein, Ali Uneri
  Created on: 2009-08-17

  (C) Copyright 2007-2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaSocketServer.h>

CMN_IMPLEMENT_SERVICES(osaSocketServer);

#if (CISST_OS == CISST_WINDOWS)
#include <Winsock2.h>
#define WINSOCKVERSION MAKEWORD(2,2)
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#if (CISST_OS == CISST_QNX)
#include <sys/select.h>
#endif
#endif


osaSocketServer::osaSocketServer(void)
{
#if (CISST_OS == CISST_WINDOWS)
    WSADATA wsaData;
    int retval = WSAStartup(WINSOCKVERSION, &wsaData);
    if (retval != 0) {
        CMN_LOG_CLASS_INIT_ERROR << "osaSocketServer: WSAStartup failed with error code " << retval << std::endl;
        return;
    }
#endif

    SocketFD = socket(PF_INET, SOCK_STREAM, 0);
    if (SocketFD == -1) {
        CMN_LOG_CLASS_INIT_ERROR << "osaSocketServer: failed to create a socket" << std::endl;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "osaSocketServer: created socket server " << SocketFD << std::endl;

    // Change to non-blocking socket
#if (CISST_OS == CISST_WINDOWS)
    unsigned long arg = 1L;
    ioctlsocket(SocketFD, FIONBIO, &arg);
#else
    char arg;
    ioctl(SocketFD, FIONBIO, &arg);
#endif
}


osaSocketServer::~osaSocketServer(void)
{
    Close();
}


bool osaSocketServer::AssignPort(unsigned short port)
{
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int retval = bind(SocketFD, reinterpret_cast<struct sockaddr *>(&serverAddr), sizeof(serverAddr));
    if (retval == -1) {
        CMN_LOG_CLASS_INIT_ERROR << "AssignPort: failed to bind socket" << std::endl;
        return false;
    }
    return true;
}


bool osaSocketServer::Listen(int backlog)
{
    int retval = listen(SocketFD, backlog);
    if (retval == -1) {
        CMN_LOG_CLASS_INIT_ERROR << "Listen: failed to listen" << std::endl;
        return false;
    }
    return true;
}


osaSocket * osaSocketServer::Accept(void)
{
    int newSocketFD = accept(SocketFD, 0, 0);
    if (newSocketFD == -1) {
        return 0;
    }
    CMN_LOG_CLASS_RUN_VERBOSE << "Accept: connection request accepted" << std::endl;
    osaSocket * newSocket = new osaSocket(newSocketFD);
    Clients.insert(std::pair<int, osaSocket *>(newSocketFD, newSocket));
    return newSocket;
}


bool osaSocketServer::IsConnected(osaSocket * socket)
{
    CMN_LOG_CLASS_RUN_WARNING << "IsConnected: not yet fully reliable" << std::endl;
    if (Clients.find(socket->GetIdentifier()) != Clients.end()) {
        return true;
    }
    return false;
}


osaSocket * osaSocketServer::Select(void)
{
    fd_set readfds;
    FD_ZERO(&readfds);
    timeval timeout = { 0, 0 };
    char buffer;

    std::map<int, osaSocket *>::iterator it;
    for (it = Clients.begin(); it != Clients.end(); it++) {
        if (send(it->first, &buffer, 0, 0) == -1) {
            it->second->Close();
            delete it->second;
            Clients.erase(it);
            if (!Clients.size()) {
                return 0;
            }
        } else {
            FD_SET(it->first, &readfds);
            int retval = select(it->first + 1, &readfds, NULL, NULL, &timeout);
            if (retval > 0) {
                if (recv(it->first, &buffer, 1, MSG_PEEK) == -1) {
                    it->second->Close();
                    delete it->second;
                    Clients.erase(it);
                    if (!Clients.size()) {
                        return 0;
                    }
                }
                return it->second;
            }
        }
    }
    return 0;
}


void osaSocketServer::CloseClients(void)
{
    std::map<int, osaSocket *>::iterator it;
    for (it = Clients.begin(); it != Clients.end(); it++) {
        it->second->Close();
        delete it->second;
        Clients.erase(it);
    }
}


void osaSocketServer::Close(void)
{
    if (SocketFD >= 0) {
        CloseClients();
#if (CISST_OS == CISST_WINDOWS)
        closesocket(SocketFD);
#else
        close(SocketFD);
#endif
        CMN_LOG_CLASS_INIT_VERBOSE << "Close: closed socket server " << SocketFD << std::endl;
        SocketFD = -1;
    }
}
