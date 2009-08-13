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

#include <cisstDevices/devOpenIGTLink.h>

CMN_IMPLEMENT_SERVICES(devOpenIGTLink);

devOpenIGTLink::devOpenIGTLink(const std::string & taskName, double period,
                               const std::string & host, int port):
    mtsTaskPeriodic(taskName, period, false, 500)
{
    mtsRequiredInterface * requiredInterface = AddRequiredInterface("CartesianPosition");
    if (requiredInterface) {
        requiredInterface->AddFunction("GetCartesianPosition", GetCartesianPosition);
    }

    Client = igtl::ClientSocket::New();
    FrameMessage = igtl::TransformMessage::New();

    Connect(host, port);
    FrameMessage->SetDeviceName(taskName.c_str());

    for (unsigned int r = 0; r < 4; r++) {
        for (unsigned int c = 0; c < 4; c++) {
            if (r == c) {
                FrameIGT[r][c] = 1.0;
            } else {
                FrameIGT[r][c] = 0.0;
            }
        }
    }
}

devOpenIGTLink::~devOpenIGTLink()
{
    Disconnect();
}

void devOpenIGTLink::Run()
{
    GetCartesianPosition(FrameCISST);
    CMN_LOG_CLASS_RUN_DEBUG << "Run: sending frame " << FrameCISST << std::endl;

    for (unsigned int r = 0; r < 3; r++) {
        FrameIGT[r][3] = FrameCISST.Position().Translation()[r];
        for (unsigned int c = 0; c < 3; c++) {
            FrameIGT[r][c] = FrameCISST.Position().Rotation().Element(r,c);
        }
    }

    FrameMessage->SetMatrix(FrameIGT);
    FrameMessage->Pack();
    Client->Send(FrameMessage->GetPackPointer(),
                 FrameMessage->GetPackSize());
}

void devOpenIGTLink::Connect(const std::string & host, int port)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Connect: opening connection." << std::endl;
    int r = Client->ConnectToServer(host.c_str(), port);
    if (r != 0) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to connect to server." << std::endl;
    }
}

void devOpenIGTLink::Disconnect()
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Disconnect: closing connection." << std::endl;
    Client->CloseSocket();
}
