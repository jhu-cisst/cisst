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
  and TCP (stream) sockets. The BSD socket API is used on Unix-like systems, while
  the Winsock2 API is used on Windows.

  For UDP, both a server and client can be defined and set as follows:
    \code
    server.AssignPort(serverPort);
    client.SetDestination(serverHost, serverPort);
    \endcode
  where serverHost could either be the hostname or the IP address of the
  server. UDP sockets update their destination to the origin of the last message
  received.

  For the TCP case, the client has to additionally call the Connect() method, while
  the server is created using the osaSocketServer class.
    \code
    client.SetDestination(serverIP, serverPort);
    client.Connect();
    // or
    client.Connect(serverIP, serverPort);
    \endcode
  The TCP server is defined using osaSocketServer, which calls an overloaded
  osaSocket constructor upon accepting a connection (see osaSocketServer class).

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


#define OSA_SOCKET_WITH_STREAM

#ifdef OSA_SOCKET_WITH_STREAM
// forward declaration
class osaSocket;

template <class _element, class _trait = std::char_traits<_element> >
class osaSocketStreambuf: public std::basic_streambuf<_element, _trait>
{
public:
  
    typedef std::basic_streambuf<_element, _trait> BaseClassType;

    osaSocketStreambuf(osaSocket * socket):
        Socket(socket)
    {
        CMN_ASSERT(this->Socket);
    }

protected:
    typedef typename std::basic_streambuf<_element, _trait>::int_type int_type;
  
    /*! Override the basic_streambuf sync for the current file
      output. */
    virtual int sync(void);
  
    /*! Override the basic_streambuf xsputn for the current file
      output. */
    virtual std::streamsize xsputn(const _element * s, std::streamsize n);

    /*! Override the basic_streambuf xsgetn for the current file
      output. */
    virtual std::streamsize xsgetn(_element * s, std::streamsize n);
    
    /*! Override the basic_streambuf overflow. overflow() is called
      when sputc() discovers it does not have space in the storage
      buffer. In our case, it's always. See more on it in the
      basic_streambuf documentation.
     */
    virtual int_type overflow(int_type c = _trait::eof());
  
private:
    osaSocket * Socket;
};



template <class _element, class _trait>
int osaSocketStreambuf<_element, _trait>::sync(void)
{
    // do nothing, flush on this->socket? 
    return 0;
}


template <class _element, class _trait>
std::streamsize
osaSocketStreambuf<_element, _trait>::xsputn(const _element *s, std::streamsize n)
{
    return this->Socket->Send(s, n);
}


template <class _element, class _trait>
std::streamsize
osaSocketStreambuf<_element, _trait>::xsgetn(_element * s, std::streamsize n)
{
    return this->Socket->Receive(s, n);
}


template <class _element, class _trait>
typename osaSocketStreambuf<_element, _trait>::int_type 
osaSocketStreambuf<_element, _trait>::overflow(int_type c)
{
    // follow the basic_streambuf standard
    if (_trait::eq_int_type(_trait::eof(), c))
        return (_trait::not_eof(c));
    char cCopy = _trait::to_char_type(c);
    return this->Socket->Send(&cCopy, 1);
}

#endif // OSA_SOCKET_WITH_STREAM

class CISST_EXPORT osaSocket: public cmnGenericObject
#ifdef OSA_SOCKET_WITH_STREAM
, public std::iostream
#endif // OSA_SOCKET_WITH_STREAM
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    enum { INTERNALS_SIZE = 16 };
    char Internals[INTERNALS_SIZE];

    /*! \brief Used for testing
        \return Size of the actual object used by the OS */
    static unsigned int SizeOfInternals(void);

#ifdef OSA_SOCKET_WITH_STREAM
    osaSocketStreambuf<char> Streambuf;
#endif // OSA_SOCKET_WITH_STREAM

public:
    enum SocketTypes { UDP, TCP };

    /*! \brief Default constructor */
    osaSocket(SocketTypes type = TCP);

    /*! \brief osaSocketServer constructor (for use by osaSocketServer) */
    osaSocket(int socketFD);

    /*! \brief Destructor */
    ~osaSocket(void);

    /*! \return Socket file descriptor */
    int GetIdentifier(void) const {
        return SocketFD;
    };

    /*! \return IP address of the localhost as a string */
    // Perhaps this should be outside the class
    static std::string GetLocalhostIP(void);

    /*! \brief Sets the port of a UDP server */
    bool AssignPort(unsigned short port);

    /*! \brief Set the destination address for UDP or TCP socket
        \param host Server's hostname or IP address (e.g. localhost, 127.0.0.1)
        \param port Server's port number */
    void SetDestination(const std::string & host, unsigned short port);

    /*! \brief Connect to the server; required for TCP sockets and should be
               used after SetDestination()
        \return true if the connection was successful */
    bool Connect(void);
 
    /*! \brief Connect to the server; required for TCP sockets; includes call
               to SetDestination()
        \param host Server's hostname or IP address (e.g. localhost, 127.0.0.1)
        \param port Server's port number
        \return true if the connection was successful */
    bool Connect(const std::string & host, unsigned short port);
 
    /*! \brief Send a byte array via the socket
        \param bufsend Buffer holding bytes to be sent
        \param msglen Number of bytes to send
        \return Number of bytes sent (-1 if error) */
    int Send(const char * bufsend, unsigned int msglen);

    /*! \brief Send a string via the socket
        \param bufsend String to be sent
        \return Number of bytes sent (-1 if error) */
    int Send(const std::string & bufsend) {
        return Send(bufsend.c_str(), bufsend.length());
    };

    /*! \brief Receive a byte array via the socket
        \param bufrecv Buffer to store received data
        \param maxlen Maximum number of bytes to receive
        \param timeoutSec Timeout in seconds. 
        \return Number of bytes received. 0 if timeout is reached and/or no data is received. */
    int Receive(char * bufrecv, unsigned int maxlen, const double timeoutSec = 0.0);

    /*! \brief Close the socket */
    void Close(void);

#ifdef OSA_SOCKET_WITH_STREAM
    /*! Provide a pointer to the stream buffer */
    virtual std::basic_streambuf<char> * rdbuf(void) {
        return &Streambuf;
    }
#endif // OSA_SOCKET_WITH_STREAM

protected:
    /*! \return IP address (as a number) for the given host */
    unsigned long GetIP(const std::string & host) const;

    SocketTypes SocketType;
    int SocketFD;
};

CMN_DECLARE_SERVICES_INSTANTIATION(osaSocket);

#endif  // _osaSocket_h
