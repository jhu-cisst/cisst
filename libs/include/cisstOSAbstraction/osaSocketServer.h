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

/*!
  \file
  \brief Declaration of osaSocketServer
  \ingroup cisstOSAbstraction

  This class implements a TCP server capable of handling multiple clients;
  for every client connection, the Accept method returns a new socket that can be
  used to service requests from that client.
  It is initialized as follows:
    \code
    osaSocketServer socketServer;
    socketServer.AssignPort(serverPort);
    socketServer.Listen();
    osaSocket *server;
    do {
        server = socketServer.Accept();
    } while (server != 0);
    \endcode

  \note Please refer to osAbstractionTutorial/sockets for usage examples.
*/

#ifndef _osaSocketServer_h
#define _osaSocketServer_h

#include <cisstCommon/cmnClassRegister.h>
#include <cisstCommon/cmnLogger.h>
#include <cisstCommon/cmnPortability.h>
#include <cisstOSAbstraction/osaSocket.h>
// always include last
#include <cisstOSAbstraction/osaExport.h>

class CISST_EXPORT osaSocketServer: public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    /*! \brief Default constructor */
    osaSocketServer(void);

    /*! \brief Destructor */
    ~osaSocketServer(void);

    /*! \brief Set the port for receiving data
        \param port The port number
        \return true on success */
    bool AssignPort(unsigned short port);

    /*! \brief Listen for incoming connections
        \return true on success */
    bool Listen(int backlog = 5);

    /*! \brief Accept an incoming connection
        \return Pointer to the accepted socket or 0 on failure */
    osaSocket * Accept(void);

    /*! \brief Check if a particular socket is still connected
        \param socket Pointer to the socket to be checked
        \return true if connected */
    bool IsConnected(osaSocket * socket);

    /*! \param timeoutSec Timeout in seconds.
        \return A socket to read from or 0 on failure */
    osaSocket * Select(const double timeoutSec = 0);

    /*! \brief Close the socket */
    void Close(void);

protected:
    /*! \brief Iteratively close all client sockets */
    void CloseClients(void);

    int SocketFD;
    std::map<int, osaSocket *> Clients;
};

CMN_DECLARE_SERVICES_INSTANTIATION(osaSocketServer);

#endif  // _osaSocketServer_h
