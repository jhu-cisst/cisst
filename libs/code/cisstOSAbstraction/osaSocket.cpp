/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Peter Kazanzides

  (C) Copyright 2007-2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnAssert.h>
#include <cisstCommon/cmnLogger.h>
#include <cisstOSAbstraction/osaSocket.h>

#if (CISST_OS == CISST_WINDOWS)
#include <Winsock2.h>
#define WINSOCKVERSION MAKEWORD( 2,2 )
typedef int socklen_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>   // for memset
#endif


CMN_IMPLEMENT_SERVICES(osaSocket);

struct osaSocketInternals {
   struct sockaddr_in destAddr;
};

#define DEST_ADDR (reinterpret_cast<osaSocketInternals*>(Internals)->destAddr)

osaSocket::osaSocket(SocketType stype)
{
    CMN_ASSERT(sizeof(Internals) >= SizeOfInternals());

#if (CISST_OS == CISST_WINDOWS)
    CMN_LOG_CLASS_INIT_VERBOSE << "osaSocket: starting Winsock2" << std::endl;
    // Start up WinSock2
    WSADATA wsaData;
    if( WSAStartup( WINSOCKVERSION, &wsaData) != 0 ){
        CMN_LOG_CLASS_INIT_ERROR << "osaSocket: could not start winsock2" << std::endl;
        return;
    }
#endif

    // Create socket
    connectionFd = socket(AF_INET, (stype == DATAGRAM)?SOCK_DGRAM:SOCK_STREAM, 0);

    // Clear destination address
    memset(&DEST_ADDR, 0, sizeof(DEST_ADDR));
}

osaSocket::~osaSocket()
{
    Close();
}

unsigned int osaSocket::SizeOfInternals(void) {
    return sizeof(osaSocketInternals);
}


void osaSocket::AssignPort(unsigned short port)
{
    struct sockaddr_in localAddr;

    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(port);
  
    int rc = bind(connectionFd, 
                  (struct sockaddr *) &localAddr, sizeof(localAddr));

    if (rc < 0)
        CMN_LOG_CLASS_INIT_ERROR << "Socket bind returns " << rc << std::endl;
}

void osaSocket::SetDestination(const char *host, unsigned short port)
{
    memset(&DEST_ADDR, 0, sizeof(DEST_ADDR));
    DEST_ADDR.sin_family = AF_INET;
    DEST_ADDR.sin_port = htons(port);
    DEST_ADDR.sin_addr.s_addr = inet_addr(host);
        //new way: inet_pton(AF_INET, host, &(DEST_ADDR.sin_addr));
    CMN_LOG_CLASS_RUN_VERBOSE << "osaSocket setting destination to " <<
        host << ":" << port << std::endl;
}

bool osaSocket::Connect(const char *host, unsigned short port)
{
    SetDestination(host, port);
    return (connect(connectionFd, (struct sockaddr *)&DEST_ADDR, sizeof(DEST_ADDR)) == 0);
}

int osaSocket::Send(const char *bufsend, unsigned int msglen)
{
    if (DEST_ADDR.sin_family == 0) {
        CMN_LOG_CLASS_RUN_ERROR << "osaSocket::Send: destination address not set" << std::endl;
        return -1;
    }
    socklen_t length = sizeof(DEST_ADDR);
    return sendto(connectionFd, bufsend, msglen, 0, (struct sockaddr *)&DEST_ADDR, length);
}

int osaSocket::SendString(const char *bufsend)
{
    return Send(bufsend, strlen(bufsend));
}

int osaSocket::SendString(const std::string &bufsend)
{ 
    return Send(bufsend.c_str(), bufsend.length());
}

int osaSocket::Receive(char *bufrecv, unsigned int maxlen)
{
    timeval tmo = { 0, 0 };
    fd_set readfds;
    struct sockaddr_in fromAddr;
    FD_ZERO(&readfds);
    FD_SET(connectionFd, &readfds);
    int n = select(connectionFd+1,&readfds,0,0,&tmo);
    if (n > 0) {
        // Receive data
        socklen_t length = sizeof(fromAddr);
        n = recvfrom(connectionFd, bufrecv, maxlen, 0, (struct sockaddr *)&fromAddr, &length);
        if (n > 0) {
            if (static_cast<unsigned int>(n) < maxlen-1) {
                bufrecv[n] = 0;  // NULL terminate the string
                CMN_LOG_CLASS_RUN_VERBOSE << "Received " << n << " chars: " << bufrecv << std::endl;
            }
            else
                CMN_LOG_CLASS_RUN_VERBOSE << "Received " << n << " chars." << std::endl;
            if (DEST_ADDR.sin_family == 0) {
                CMN_LOG_CLASS_INIT_VERBOSE << "osaSocket setting destination address to " <<
                    inet_ntoa(fromAddr.sin_addr) << ":" << ntohs(fromAddr.sin_port) << std::endl;
                DEST_ADDR = fromAddr;
            }
            else if ((DEST_ADDR.sin_addr.s_addr != fromAddr.sin_addr.s_addr) ||
                     (DEST_ADDR.sin_port != fromAddr.sin_port)) {
                CMN_LOG_CLASS_RUN_VERBOSE << "osaSocket updating destination from " <<
                    inet_ntoa(DEST_ADDR.sin_addr) << ":" << ntohs(DEST_ADDR.sin_port) << " to " <<
                    inet_ntoa(fromAddr.sin_addr) << ":" << ntohs(fromAddr.sin_port) << std::endl;
                DEST_ADDR = fromAddr;
            }
        }
    }
    else if (n < 0)
        CMN_LOG_CLASS_RUN_ERROR << "Socket recv returned " << n << std::endl;
    return n;
}

void osaSocket::Close(void)
{
   if (connectionFd >= 0) {
#if (CISST_OS == CISST_WINDOWS)
      closesocket(connectionFd);
#else
      close(connectionFd);
#endif
      connectionFd = -1;
   }
}
