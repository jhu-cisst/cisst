/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Ali Uneri

  (C) Copyright 2007-2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _devOpenIGTLink_h
#define _devOpenIGTLink_h

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>
#include <cisstDevices/devConfig.h>
// always include last
#include <cisstDevices/devExport.h>

#ifdef CISST_DEV_HAS_OPENIGTLINK

#include <igtlClientSocket.h>
#include <igtlTransformMessage.h>

class CISST_EXPORT devOpenIGTLink: public mtsTaskPeriodic
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

protected:
    igtl::ClientSocket::Pointer Client;
    igtl::TransformMessage::Pointer FrameMessage;
    igtl::Matrix4x4 FrameIGT;
    prmPositionCartesianGet FrameCISST;

    mtsFunctionRead GetPositionCartesian;

public:
    devOpenIGTLink(const std::string & taskName, double period,
                   const std::string & host, int port);
    ~devOpenIGTLink();
    
    void Configure(const std::string & CMN_UNUSED(filename)) {};
    void Startup() {};
    void Run();
    void Cleanup() {};
    
    void Connect(const std::string & host, int port);
    void Disconnect();
};

CMN_DECLARE_SERVICES_INSTANTIATION(devOpenIGTLink);

#endif  // CISST_DEV_HAS_OPENIGTLINK

#endif  // _devOpenIGTLink_h
