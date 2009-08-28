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

/*!
  \file
  \brief Declaration of osaSocket
  \ingroup cisstOSAbstraction

  This is a cross-platform socket library with basic support for UDP (datagram)
  and TCP (stream) sockets. BSD socket API is used on Unix-like systems, while
  Winsock2 API is used on Windows.

  For the UDP, both a server and client can be defined and set as such
    \code
    server.AssignPort(serverPort);
    client.SetDestination(serverHost, serverPort);
    \endcode
  where serverHost could either be the hostname or the IP address of the
  server. UDP sockets update their destination to the origin of last message
  received.

  For the TCP case, client has to additionally call the Connect() method, while
  the server is created using the osaSocketServer class.
    \code
    client.SetDestination(serverIP, serverPort);
    client.Connect();
    \endcode
  The TCP server is defined using osaSocketServer, which calls an overloaded
  osaSocket constructor upon accepting a connection.

  \note Please refer to osAbstractionTutorial/sockets for usage examples.
*/

#ifndef _osaSocket_h
#define _osaSocket_h

#include <cisstCommon/cmnAssert.h>
#include <cisstCommon/cmnClassRegister.h>
#include <cisstCommon/cmnLogger.h>
#include <cisstCommon/cmnPortability.h>
// Always include last
#include <cisstOSAbstraction/osaExport.h>

class CISST_EXPORT osaSocket: public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    enum { INTERNALS_SIZE = 16 };
    char Internals[INTERNALS_SIZE];

    /*! \brief Used for testing
        \return Size of the actual object used by the OS */
    static unsigned int SizeOfInternals(void);

public:
    enum SocketTypes { UDP, TCP };

    /*! \brief Default constructor */
    osaSocket(SocketTypes type = TCP);

    /*! \brief osaSocketServer constructor */
    osaSocket(int socketFD);

    /*! \brief Destructor */
    ~osaSocket(void);

    /*! \return Socket file descriptor */
    int GetIdentifier(void) {
        return SocketFD;
    };

    /*! \return IP address of the localhost */
    std::string GetLocalhostIP(void);

    /*! \brief Sets the port of a UDP server */
    bool AssignPort(unsigned short port);

    /*! \param host Server's hostname or IP address (e.g. localhost, 127.0.0.1)
        \param port Server's port number */
    void SetDestination(const std::string & host, unsigned short port);

    /*! \brief Connect to the server; required for TCP sockets and should be
               used after SetDestination()
        \return true if the connection was successful */
    bool Connect(void);

    /*! \brief Send a byte array via the socket
        \param bufsend Buffer holding bytes to be sent
        \param msglen Number of bytes to send
        \return Number of bytes sent (-1 if error) */
    int Send(const char * bufsend, unsigned int msglen);
    int Send(const char * bufsend) {
        return Send(bufsend, strlen(bufsend));
    };
    int Send(const std::string & bufsend) {
        return Send(bufsend.c_str(), bufsend.length());
    };

    /*! \brief Receiva a byte array via the socket
        \param bufrecv Buffer to store received data
        \param maxlen Maximum number of bytes to receive
        \return Number of bytes received */
    int Receive(char * bufrecv, unsigned int maxlen);

    /*! \brief Close the socket */
    void Close(void);

protected:
    /*! \return IP address (as a number) for the given host */
    unsigned long GetIP(const std::string & host);

    SocketTypes SocketType;
    int SocketFD;
};

CMN_DECLARE_SERVICES_INSTANTIATION(osaSocket);

#endif  // _osaSocket_h
