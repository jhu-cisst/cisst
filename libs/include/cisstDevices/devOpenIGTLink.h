/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ali Uneri
  Created on: 2009-08-10

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

/*!
  \file
  \brief Declaration of devOpenIGTLink
  \ingroup cisstDevices

  This is a wrapper for the Open IGT Link. It can set up both as a server or a
  client by providing the extra host argument, where you specify the hostname
  or IP address of the server.
    \code
    devOpenIGTLink("trackerServer", 50.0 * cmn_ms, 18944);
    devOpenIGTLink("trackerClient", 50.0 * cmn_ms, "localhost", 18944);
    \endcode

  In both cases, the created task requires a frame (prmPositionCartesianGet) to
  send to the connected device and in return provides a frame from the
  connected device. Also, currently the server is set up to handle a single
  client, but this can changed should there be need, since osaSocketServer has
  support for multiple clients.

  igtlutil library is required to compile and run the examples. The easiest way
  to obtain this is to build the OpenIGTLink Library and link to it using
  CISST_DEV_HAS_OPENIGT option.

  SVN repository of the source code:
  http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink

  Build instructions for various platforms:
  http://www.na-mic.org/Wiki/index.php/OpenIGTLink/Library/Build

  \note Please refer to devicesTutorial/example4 for usage examples.

  \todo Handle multiple connections by storing the returned socket pointer in an array.
  \todo Check for cyclic redundancy (CRC).
  \todo Handle message types besides TRANSFORM.
*/

#ifndef _devOpenIGTLink_h
#define _devOpenIGTLink_h

#include <cisstOSAbstraction/osaSocket.h>
#include <cisstOSAbstraction/osaSocketServer.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>
#include <cisstDevices/devExport.h>  // always include last

#include <igtl_util.h>
#include <igtl_header.h>
#include <igtl_transform.h>

class CISST_EXPORT devOpenIGTLink: public mtsTaskPeriodic
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    /*! \brief Server constructor */
    devOpenIGTLink(const std::string & taskName, const double period,
                   const unsigned short port);

    /*! \brief Client constructor */
    devOpenIGTLink(const std::string & taskName, const double period,
                   const std::string & host, const unsigned short port);

    /*! \brief Destructor */
    ~devOpenIGTLink(void);

    void Configure(const std::string & CMN_UNUSED(filename)) {};
    void Startup(void);
    void Run(void);
    void Cleanup(void) {};

protected:
    /*! \brief Common initializing operations for both server and client */
    void Initialize(void);

    /*! \brief Sends the frame through the existing socket interface
        \param frameCISST Frame to be sent
        \return true on success */
    bool SendFrame(const prmPositionCartesianGet & frameCISST);

    /*! \brief Receives the header of the incoming message
        \param messageType Type of the message (i.e. TRANSORM)
        \return false if there's no message or recv() fails */
    bool ReceiveHeader(std::string & messageType);

    /*! \brief Receives an incoming frame
        \param frameCISST Frame to be received
        \return true on success */
    bool ReceiveFrame(prmPositionCartesianGet & frameCISST);

    /*! \brief Skips the received message */
    void SkipMessage(void);

    /*! \brief Hard copies the rotation and translation from a
               prmPositionCartesianGet to a float array of size 12
        \param frameCISST Frame to be converted
        \param frameIGT Result of conversion */
    void FrameCISSTtoIGT(const prmPositionCartesianGet & frameCISST,
                         igtl_float32 * frameIGT);

    /*! \brief Hard copies a float array of size 12 to the rotation and
               translation of a prmPositionCartesianGet
        \param frameIGT Frame to be converted
        \param frameCISST Converted frame */
    void FrameIGTtoCISST(const igtl_float32 * frameIGT,
                         prmPositionCartesianGet & frameCISST);

    mtsFunctionRead GetPositionCartesian;

    enum ConnectionTypes { SERVER, CLIENT };
    int ConnectionType;

    std::string DeviceName;
    std::string Host;
    unsigned short Port;
    bool IsConnected;

    osaSocketServer * SocketServer;
    osaSocket * Socket;
    std::vector<osaSocket *> Sockets;

    std::string MessageType;
    igtl_header HeaderIGT;
    igtl_float32 FrameIGT[12];
    prmPositionCartesianGet FrameSend;
    prmPositionCartesianGet FrameRecv;
};

CMN_DECLARE_SERVICES_INSTANTIATION(devOpenIGTLink);

#endif  // _devOpenIGTLink_h
