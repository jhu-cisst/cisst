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

#include <cisstDevices/devOpenIGTLinkServer.h>

CMN_IMPLEMENT_SERVICES(devOpenIGTLinkServer);


devOpenIGTLinkServer::devOpenIGTLinkServer(const std::string & taskName, const double period,
                                           const int port):
    mtsTaskPeriodic(taskName, period, false, 500)
{
    mtsRequiredInterface * requiredInterface = AddRequiredInterface("PositionCartesian");
    if (requiredInterface) {
        requiredInterface->AddFunction("GetPositionCartesian", GetPositionCartesian);
    }

    ServerSocket = igtl::ServerSocket::New();
    Socket = NULL;
    FrameMessage = igtl::TransformMessage::New();
    Header = igtl::MessageHeader::New();

    DeviceName = taskName;
    Port = port;

    // set FrameIGT to identity matrix
    FrameCISSTtoIGT(FrameCISST, FrameIGT);
}


void devOpenIGTLinkServer::Startup(void)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Startup: creating a server socket on port " << Port << std::endl;
    int response = ServerSocket->CreateServer(Port);
    if (response != 0) {
        CMN_LOG_CLASS_INIT_ERROR << "Startup: failed to create a server socket" << std::endl;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "Startup: waiting for connection" << std::endl;

    FrameMessage->SetDeviceName(DeviceName.c_str());
}


void devOpenIGTLinkServer::Run(void)
{
    if (Socket.IsNull()) {
        Socket = ServerSocket->WaitForConnection(1000);
    } else {
        // send frame
        GetPositionCartesian(FrameCISST);
        SendFrame(FrameCISST);

        // receive frame
        Header->InitPack();
        int response = Socket->Receive(Header->GetPackPointer(), Header->GetPackSize());

        if (response == 0) {
            CMN_LOG_CLASS_RUN_VERBOSE << "Run: client disconnected" << std::endl;
            Socket->CloseSocket();
            Socket = NULL;
        } else {
            Header->Unpack();
            if (strcmp(Header->GetDeviceType(), "TRANSFORM") == 0) {
                ReceiveFrame(FrameCISST);
            } else if (strcmp(Header->GetDeviceType(), "POSITION") == 0) {
                // not implemented
                CMN_LOG_CLASS_RUN_DEBUG << "Run: position received, skipping" << std::endl;
                Socket->Skip(Header->GetBodySizeToRead(), 0);
            } else if (strcmp(Header->GetDeviceType(), "IMAGE") == 0) {
                // not implemented
                CMN_LOG_CLASS_RUN_DEBUG << "Run: image received, skipping" << std::endl;
                Socket->Skip(Header->GetBodySizeToRead(), 0);
            } else if (strcmp(Header->GetDeviceType(), "STATUS") == 0) {
                // not implemented
                CMN_LOG_CLASS_RUN_DEBUG << "Run: status received, skipping" << std::endl;
                Socket->Skip(Header->GetBodySizeToRead(), 0);
            } else {
                Socket->Skip(Header->GetBodySizeToRead(), 0);
                CMN_LOG_CLASS_RUN_ERROR << "Run: Unknown data type received" << std::endl;
            }
        }
    }
}


void devOpenIGTLinkServer::FrameCISSTtoIGT(const prmPositionCartesianGet & frameCISST,
                                           igtl::Matrix4x4 & frameIGT)
{
    for (unsigned int r = 0; r < 3; r++) {
        frameIGT[r][3] = frameCISST.Position().Translation().Element(r);
        for (unsigned int c = 0; c < 3; c++) {
            frameIGT[r][c] = frameCISST.Position().Rotation().Element(r,c);
        }
    }
}


void devOpenIGTLinkServer::FrameIGTtoCISST(const igtl::Matrix4x4 & frameIGT,
                                           prmPositionCartesianGet & frameCISST)
{
    for (unsigned int r = 0; r < 3; r++) {
        frameCISST.Position().Translation().Element(r) = frameIGT[r][3];
        for (unsigned int c = 0; c < 3; c++) {
            frameCISST.Position().Rotation().Element(r,c) = frameIGT[r][c];
        }
    }
}


void devOpenIGTLinkServer::SendFrame(const prmPositionCartesianGet & frameCISST)
{
    FrameCISSTtoIGT(frameCISST, FrameIGT);
    FrameMessage->SetMatrix(FrameIGT);
    FrameMessage->Pack();
    Socket->Send(FrameMessage->GetPackPointer(),
                 FrameMessage->GetPackSize());
    CMN_LOG_CLASS_RUN_DEBUG << "SendFrame: sent " << FrameCISST << std::endl;
}


bool devOpenIGTLinkServer::ReceiveFrame(prmPositionCartesianGet & frameCISST)
{
    FrameMessage->SetMessageHeader(Header);
    FrameMessage->AllocatePack();

    Socket->Receive(FrameMessage->GetPackBodyPointer(), FrameMessage->GetPackBodySize());
    int crc = FrameMessage->Unpack(1);

    if (crc & igtl::MessageHeader::UNPACK_BODY) {
        FrameMessage->GetMatrix(FrameIGT);
        FrameIGTtoCISST(FrameIGT, frameCISST);
        CMN_LOG_CLASS_RUN_DEBUG << "ReceiveFrame: received " << frameCISST << std::endl;
    } else {
        CMN_LOG_CLASS_RUN_ERROR << "ReceiveFrame: cyclic redundancy check failed" << std::endl;
        return false;
    }
    return true;
}
