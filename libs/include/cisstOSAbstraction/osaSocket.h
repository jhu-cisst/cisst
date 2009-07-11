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
  
  This is an implementation for an unconnected datagram (UDP) socket, with partial support
  for a connected stream (TCP) socket (specifically, just the client side). 
  It also supports connected datagram sockets, though there is no real advantage to
  using them (technical detail: the class uses sendto and recvfrom, rather than send and
  recv, regardless of whether the socket is connected or not).

  In general, datagrams are sent to the address from which the last datagram was received;
  the initial destination address is set via a call to SetDestination (or, via a call to
  Connect, which calls SetDestination). For a UDP client/server application:

  UDP server initialization:  ssock.AssignPort(server_port);
  UDP client initialization:  csock.SetDestination(server_ip, server_port);

  TCP server initialization:  not supported (need Listen and Accept)
  TCP client initialization:  csock.Connect(server_ip, server_port);

  Now, messages can be sent/received by calling the Send and Receive methods. Note that
  it is not necessary for the client to assign a port number, though it is fine for it
  to do so. For cleanup, the client and server should call the Close method.

  If the socket receives a message from an address that is different from the current
  destination, the destination is updated. For example, if a socket receives a message
  from address 192.168.0.1, all subsequent messages are sent to that address.

  \note This is a fairly minimal socket implementation. Currently, the Receive method is
  non-blocking; it would be nice to have a Receive method that blocks until a message
  is received. Also, it would be nice to be able to use named IP addresses, rather than
  just numbers.

 */

#ifndef _osaSocket_h
#define _osaSocket_h

#include <cisstCommon/cmnClassRegister.h>
// Always include last
#include <cisstOSAbstraction/osaExport.h>

class CISST_EXPORT osaSocket : public cmnGenericObject {

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    int connectionFd;
    enum {INTERNALS_SIZE = 16};
    char Internals[INTERNALS_SIZE];

    /*! Return the size of the actual object used by the OS.  This is
        used for testing only. */
    static unsigned int SizeOfInternals(void);
    friend class osaSocketTest;

public:
    enum SocketType { DATAGRAM, STREAM };

    osaSocket(SocketType stype = osaSocket::DATAGRAM);
    ~osaSocket();

    /*! Set the port for receiving data (only needed for server)
          \param port the port number */
    void AssignPort(unsigned short port);

    /*! Set the destination address (used only by UDP clients; for
          TCP clients, use Connect)
          \param host the server's IP address (e.g., 192.0.0.1)
          \param port the server's port number */
    void SetDestination(const char *host, unsigned short port);

    /*! Connect to a server; this is required for stream (TCP) sockets and
        can be used for datagram (UDP) sockets, though it provides no benefit
        in that case (use SetDestination instead).
          \param host the server's IP address (e.g., 192.0.0.1)
          \param port the server's port number
          \returns true if the connection was successful */
    bool Connect(const char *host, unsigned short port);

    /*! Send a byte array via the socket.
          \param bufsend Buffer holding bytes to be sent
          \param msglen Number of bytes to send
          \returns number of bytes sent (-1 if error) */
    int Send(const char *bufsend, unsigned int msglen);

    /*! Send a null-terminated string via the socket.
          \param bufsend Buffer holding string to be sent
          \returns number of bytes sent (-1 if error) */
    int SendString(const char *bufsend);
    int SendString(const std::string &bufsend);

    /*! Non-blocking receive. If data is present, returns it in bufrecv.
          \param bufrecv Buffer to store received data
          \param maxlen Maximum number of bytes to receive
          \returns the number of bytes received */
    int Receive(char *bufrecv, unsigned int maxlen);

    /*! Close the socket */
    void Close(void);
};

CMN_DECLARE_SERVICES_INSTANTIATION(osaSocket);

#endif // _osaSocket_h
