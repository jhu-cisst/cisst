/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2010-12-27

  (C) Copyright 2010-2011 Johns Hopkins University (JHU), All Rights
  Reserved.
--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

/*!
  \file
  \brief Defines connection between two interfaces.
*/

#ifndef _mtsConnection_h
#define _mtsConnection_h

#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstMultiTask/mtsParameterTypes.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

#include <cisstMultiTask/mtsExport.h>

/*! Data structure to keep connection information */
class CISST_EXPORT mtsConnection : public mtsGenericObject {

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

protected:
    /* String representation of connection */
    mtsDescriptionConnection ConnectionDescription;

    /* Name of connect request process */
    const std::string RequestProcessName;

    /* Connection status. False if pending connection, true if established connection */
    bool Connected;

    /* Ice server proxy access information (to send messages to client proxy) */
    std::string EndpointInfo;

    /* Time when pending connection becomes invalidated; any pending connection
       should be confirmed before this time limit */
    double TimeoutTime;

public:
    mtsConnection(const mtsDescriptionConnection & description, const std::string & requestProcessName);

    /*! Getters and Setters */
    ConnectionIDType GetConnectionID(void) const;

    std::string GetClientProcessName(void) const;
    std::string GetClientComponentName(void) const;
    std::string GetClientInterfaceName(void) const;
    std::string GetServerProcessName(void) const;
    std::string GetServerComponentName(void) const;
    std::string GetServerInterfaceName(void) const;

    mtsDescriptionConnection GetDescriptionConnection(void) const;
    void GetDescriptionConnection(mtsDescriptionConnection & description) const;

    bool IsConnected(void) const { return Connected; }
    void SetConnected(void) { Connected = true; }

    bool IsRemoteConnection(void) const;

    std::string GetEndpointInfo(void) const { return EndpointInfo; }
    void SetProxyAccessInfo(const std::string & endpointInfo) { EndpointInfo = endpointInfo; }

    /*! In the state of pending connection, check if this connection should be
        invalidated due to connection timeout */
    bool CheckTimeout(void) const;

    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsConnection)

#endif // _mtsConnection_h
