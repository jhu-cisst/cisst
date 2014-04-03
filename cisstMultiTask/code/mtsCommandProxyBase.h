/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2010-01-20

  (C) Copyright 2010-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsCommandProxyBase_h
#define _mtsCommandProxyBase_h

#include "mtsProxyBaseCommon.h"
#include "mtsComponentInterfaceProxyServer.h"
#include "mtsComponentInterfaceProxyClient.h"
#include <cisstMultiTask/mtsForwardDeclarations.h>

class mtsCommandProxyBase {
protected:
    /*! Pointer to mtsFunctionXXX object at the peer's memory space */
    mtsCommandIDType CommandID;

    /*! Client ID */
    mtsComponentInterfaceProxyServer::ClientIDType ClientID;

    /*! Network (ICE) proxy which enables communication with the connected
        interface across a network. This is of type either
        mtsComponentInterfaceProxyClient or mtsComponentInterfaceProxyServer */
    mtsProxyBaseCommon<mtsComponentProxy> * NetworkProxy;

    /*! Actual pointer to network proxy. Either one of them should be set and
        the other one should be NULL. */
    mtsComponentInterfaceProxyClient * NetworkProxyClient;
    mtsComponentInterfaceProxyServer * NetworkProxyServer;


    /*! Indicate if all arguments are supported, i.e. if the program
      has access to the argument(s) class services in
      cmnClassServices.  The client process might not be linked to the
      library that defines the argument symbol.  One can also use
      dynamic loading to make sure all symbols are available.  This is
      assumed to be true unless the method ArgumentNotSupported is
      called. */
    bool ArgumentsSupportedFlag;

public:
    /*! Constructor */
    mtsCommandProxyBase() : CommandID(0), ClientID(0), NetworkProxy(0), NetworkProxyClient(0), NetworkProxyServer(0), ArgumentsSupportedFlag(true)
    {}

    /*! Set network proxy */
    bool SetNetworkProxy(mtsProxyBaseCommon<mtsComponentProxy> * networkProxy, const mtsComponentInterfaceProxyServer::ClientIDType clientID = 0) {
        ClientID = clientID;

        NetworkProxyClient = dynamic_cast<mtsComponentInterfaceProxyClient*>(networkProxy);
        NetworkProxyServer = dynamic_cast<mtsComponentInterfaceProxyServer*>(networkProxy);

        return ((!NetworkProxyClient && NetworkProxyServer) || (NetworkProxyClient && !NetworkProxyServer));
    }

    /*! Set command id */
    virtual void SetCommandID(const mtsCommandIDType & commandID) {
        CommandID = commandID;
    }

    /*! Test if all arguments are supported.  See ArgumentsSupportedFlag. */
    inline bool ArgumentsSupported(void) const {
        return this->ArgumentsSupportedFlag;
    }

    /*! Indicate that one or more argument is not supported. */
    inline void SetArgumentSupported(bool argumentSupported) {
        this->ArgumentsSupportedFlag = argumentSupported;
    }

    /*! Generate human readable description of this object */
    void ToStreamBase(const std::string & className, const std::string & commandName, const mtsCommandIDType & commandID, const bool enabled, std::ostream & outputStream) const {
        outputStream << className << ": " << commandName << ", " << commandID << " with ";
        if (NetworkProxyServer) {
            outputStream << NetworkProxyServer->ClassServices()->GetName();
        } else {
            outputStream << NetworkProxyClient->ClassServices()->GetName();
        }
        outputStream << ": currently " << (enabled ? "enabled" : "disabled");
    }
};

#endif // _mtsCommandProxyBase_h
