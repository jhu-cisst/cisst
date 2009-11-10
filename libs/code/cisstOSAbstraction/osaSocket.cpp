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

#include <cisstOSAbstraction/osaSocket.h>

CMN_IMPLEMENT_SERVICES(osaSocket);

#if (CISST_OS == CISST_WINDOWS)
#include <Winsock2.h>
typedef int socklen_t;
#define WINSOCKVERSION MAKEWORD(2,2)
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>  // for memset
#endif

struct osaSocketInternals {
   struct sockaddr_in ServerAddr;
};

#define SERVER_ADDR (reinterpret_cast<struct osaSocketInternals *>(Internals)->ServerAddr)


unsigned int osaSocket::SizeOfInternals(void)
{
    return sizeof(osaSocketInternals);
}


osaSocket::osaSocket(SocketTypes type)
#ifdef OSA_SOCKET_WITH_STREAM
:
    std::iostream(&Streambuf),
    Streambuf(this)
#endif // OSA_SOCKET_WITH_STREAM
{
    CMN_ASSERT(sizeof(Internals) >= SizeOfInternals());
    memset(&SERVER_ADDR, 0, sizeof(SERVER_ADDR));

#if (CISST_OS == CISST_WINDOWS)
    WSADATA wsaData;
    int retval = WSAStartup(WINSOCKVERSION, &wsaData);
    if (retval != 0) {
        CMN_LOG_CLASS_INIT_ERROR << "osaSocket: WSAStartup() failed with error code " << retval << std::endl;
        return;
    }
#endif

    SocketType = type;
    SocketFD = socket(PF_INET, (type == UDP) ? SOCK_DGRAM : SOCK_STREAM, 0);
    if (SocketFD == -1) {
        CMN_LOG_CLASS_INIT_ERROR << "osaSocket: failed to create a socket" << std::endl;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "osaSocket: created socket " << SocketFD << std::endl;
}


osaSocket::osaSocket(int socketFD)
#ifdef OSA_SOCKET_WITH_STREAM
:
    std::iostream(&Streambuf),
    Streambuf(this)
#endif // OSA_SOCKET_WITH_STREAM
{
    SocketType = TCP;
    SocketFD = socketFD;
    if (SocketFD == -1) {
        CMN_LOG_CLASS_INIT_ERROR << "osaSocket: failed to create a socket" << std::endl;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "osaSocket: created socket " << SocketFD << std::endl;
}


osaSocket::~osaSocket(void)
{
    Close();
}


std::string osaSocket::GetLocalhostIP(void)
{
    char hostname[256] = { 0 };
    gethostname(hostname, 255);
    CMN_LOG_RUN_VERBOSE << "GetLocalhostIP: hostname is " << hostname << std::endl;

    struct hostent * he = gethostbyname(hostname);
    if (!he) {
        CMN_LOG_RUN_ERROR << "GetLocalhostIP: invalid host" << std::endl;
        return "";
    }
    struct in_addr localAddr;
    memcpy(&localAddr, he->h_addr_list[0], sizeof(struct in_addr));
    return inet_ntoa(localAddr);
}


bool osaSocket::AssignPort(unsigned short port)
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


void osaSocket::SetDestination(const std::string & host, unsigned short port)
{
    memset(&SERVER_ADDR, 0, sizeof(SERVER_ADDR));
    SERVER_ADDR.sin_family = AF_INET;
    SERVER_ADDR.sin_port = htons(port);
    SERVER_ADDR.sin_addr.s_addr = GetIP(host);

    CMN_LOG_CLASS_INIT_VERBOSE << "SetDestination: destination set to "
                               << host << ":" << port << std::endl;
}


bool osaSocket::Connect(void)
{
    int retval = connect(SocketFD, reinterpret_cast<struct sockaddr *>(&SERVER_ADDR), sizeof(SERVER_ADDR));
    if (retval == -1) {
        CMN_LOG_CLASS_RUN_ERROR << "Connect: failed to connect" << std::endl;
        return false;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "Connect: connection established" << std::endl;
    return true;
}

bool osaSocket::Connect(const std::string & host, unsigned short port)
{
    SetDestination(host, port);
    return Connect();
}

int osaSocket::Send(const char * bufsend, unsigned int msglen)
{
    int retval = 0;
    if (SocketType == UDP) {
        socklen_t length = sizeof(SERVER_ADDR);
        retval = sendto(SocketFD, bufsend, msglen, 0, reinterpret_cast<struct sockaddr *>(&SERVER_ADDR), length);
    } else if (SocketType == TCP) {
        retval = send(SocketFD, bufsend, msglen, 0);
    }
    if (retval == -1) {
        CMN_LOG_CLASS_RUN_ERROR << "Send: failed to send" << std::endl;
        return -1;
    }
    CMN_LOG_CLASS_RUN_DEBUG << "Send: sent " << retval << " bytes: " << bufsend << std::endl;
    return retval;
}


int osaSocket::Receive(char * bufrecv, unsigned int maxlen)
{
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(SocketFD, &readfds);
    timeval timeout = { 0, 0 };

    int retval = select(SocketFD + 1, &readfds, NULL, NULL, &timeout);
    if (retval > 0) {
        struct sockaddr_in fromAddr;

        if (SocketType == UDP) {
            socklen_t length = sizeof(fromAddr);
            retval = recvfrom(SocketFD, bufrecv, maxlen, 0, reinterpret_cast<struct sockaddr *>(&fromAddr), &length);
        } else if (SocketType == TCP) {
            retval = recv(SocketFD, bufrecv, maxlen, 0);
        }
        if (retval > 0) {
            if (static_cast<unsigned int>(retval) < maxlen - 1) {
                bufrecv[retval] = 0;  // NULL terminate the string
                CMN_LOG_CLASS_RUN_DEBUG << "Receive: received " << retval << " bytes: " << bufrecv << std::endl;
            } else {
                CMN_LOG_CLASS_RUN_WARNING << "Receive: received more than maximum length" << std::endl;
            }

            if (SocketType == UDP) {
                if (SERVER_ADDR.sin_addr.s_addr == 0) {
                    CMN_LOG_CLASS_RUN_VERBOSE << "Receive: setting destination address to "
                                              << inet_ntoa(fromAddr.sin_addr) << ":" << ntohs(fromAddr.sin_port) << std::endl;
                    SERVER_ADDR = fromAddr;
                } else if ((SERVER_ADDR.sin_addr.s_addr != fromAddr.sin_addr.s_addr) ||
                           (SERVER_ADDR.sin_port != fromAddr.sin_port)) {
                    CMN_LOG_CLASS_RUN_VERBOSE << "Receive: updating destination from "
                                              << inet_ntoa(SERVER_ADDR.sin_addr) << ":" << ntohs(SERVER_ADDR.sin_port)
                                              << " to "
                                              << inet_ntoa(fromAddr.sin_addr) << ":" << ntohs(fromAddr.sin_port) << std::endl;
                    SERVER_ADDR = fromAddr;
                }
            }
        }
    } else if (retval == -1) {
        CMN_LOG_CLASS_RUN_ERROR << "Receive: failed to receive" << std::endl;
    }
    return retval;
}

// This could be static or external to the osaSocket class
unsigned long osaSocket::GetIP(const std::string & host) const
{
    hostent * he = gethostbyname(host.c_str());
    if (he) {
        if (he->h_addr_list && he->h_addr_list[0]) {
            return *reinterpret_cast<unsigned long *>(he->h_addr_list[0]);
        }
    }
    CMN_LOG_CLASS_RUN_ERROR << "GetIP: invalid host " << host << std::endl;
    return 0;
}


void osaSocket::Close(void)
{
    if (SocketFD >= 0) {
#if (CISST_OS == CISST_WINDOWS)
        closesocket(SocketFD);
#else
        close(SocketFD);
#endif
        CMN_LOG_CLASS_INIT_VERBOSE << "Close: closed socket " << SocketFD << std::endl;
        SocketFD = -1;
    }
}
