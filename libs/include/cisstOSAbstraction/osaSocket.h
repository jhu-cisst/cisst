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

/*!
  \file
  \brief Declaration of osaSocket
  
  This is a simple implementation of an unconnected datagram (UDP) socket.
 */

#ifndef _osaSocket_h
#define _osaSocket_h

// Always include last
#include <cisstOSAbstraction/osaExport.h>

class CISST_EXPORT osaSocket {

    int connectionFd;
    enum {INTERNALS_SIZE = 16};
    char Internals[INTERNALS_SIZE];

    /*! Return the size of the actual object used by the OS.  This is
        used for testing only. */
    static unsigned int SizeOfInternals(void);
    friend class osaSocketTest;

public:
    osaSocket();
    ~osaSocket();

    /*! Set the port for receiving data (only needed for server)
          \param port the port number */
    void AssignPort(unsigned short port);

    /*! Set the server address (used only by clients)
          \param the server's IP address (e.g., 192.0.0.1)
          \param the server's port number */
    void SetServer(const char *host, unsigned short port);

    /*! Send a byte array via the socket.
          \param bufsend Buffer holding bytes to be sent
          \param msglen Number of bytes to send
          \returns number of bytes sent (-1 if error) */
    int Send(char *bufsend, unsigned int msglen);

    /*! Send a null-terminated string via the socket.
          \param bufsend Buffer holding string to be sent
          \returns number of bytes sent (-1 if error) */
     int SendString(char *bufsend);

    /*! Non-blocking receive. If data is present, returns it in bufrecv.
          \param bufrecv Buffer to store received data
          \param maxlen Maximum number of bytes to receive
          \returns the number of bytes received */
    int Receive(char *bufrecv, unsigned int maxlen);

    /*! Close the socket */
    void Close(void);
};

#endif // _osaSocket_h
