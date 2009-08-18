/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ali Uneri
  Created on: 2009-08-10

  (C) Copyright 2007-2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

/*!
  \file
  \brief Declaration of devOpenIGTLinkServer
*/

#ifndef _devOpenIGTLinkServer_h
#define _devOpenIGTLinkServer_h

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>
#include <cisstDevices/devConfig.h>
// always include last
#include <cisstDevices/devExport.h>

#ifdef CISST_DEV_HAS_OPENIGTLINK

#include <igtlServerSocket.h>
#include <igtlTransformMessage.h>

class CISST_EXPORT devOpenIGTLinkServer: public mtsTaskPeriodic
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

protected:
    /*! \brief Hard copies the rotation and translation from a
               prmPositionCartesianGet to a 2D float array.
        \param frameCISST
        \param frameIGT */
    void FrameCISSTtoIGT(const prmPositionCartesianGet & frameCISST,
                         igtl::Matrix4x4 & frameIGT);

    /*! \brief Hard copies a 2D float array to the rotation and translation of
               a prmPositionCartesianGet.
        \param frameIGT
        \param frameCISST */
    void FrameIGTtoCISST(const igtl::Matrix4x4 & frameIGT,
                         prmPositionCartesianGet & frameCISST);

    /*! \brief Packs and sends the frame through the socket interface.
        \param frameCISST */
    void SendFrame(const prmPositionCartesianGet & frameCISST);

    /*! \brief Unpacks and fills the frame passed frame with received data.
        \param frameCISST
        \returns True if succeeds */
    bool ReceiveFrame(prmPositionCartesianGet & frameCISST);

    mtsFunctionRead GetPositionCartesian;

    igtl::ServerSocket::Pointer ServerSocket;
    igtl::Socket::Pointer Socket;
    igtl::TransformMessage::Pointer FrameMessage;
    igtl::MessageHeader::Pointer Header;

    std::string DeviceName;
    int Port;

    igtl::Matrix4x4 FrameIGT;
    prmPositionCartesianGet FrameCISST;

public:
    devOpenIGTLinkServer(const std::string & taskName, const double period,
                         const int port);
    ~devOpenIGTLinkServer(void) {};

    void Configure(const std::string & CMN_UNUSED(filename)) {};
    void Startup(void);
    void Run(void);
    void Cleanup(void) {};
};

CMN_DECLARE_SERVICES_INSTANTIATION(devOpenIGTLinkServer);

#endif  // CISST_DEV_HAS_OPENIGTLINK

#endif  // _devOpenIGTLinkServer_h
