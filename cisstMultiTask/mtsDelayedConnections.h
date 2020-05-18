/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2020-05-13

  (C) Copyright 2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsDelayedConnections_h
#define _mtsDelayedConnections_h

#include <cisstMultiTask/mtsForwardDeclarations.h>

// Always include last!
#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsDelayedConnections {
 public:
    void Add(const std::string & clientComponentName,
             const std::string & clientInterfaceName,
             const std::string & serverComponentName,
             const std::string & serverInterfaceName);
    void Connect(const bool purge = true);

    class ConnectionType {
    public:
        inline ConnectionType(const std::string & clientComponentName,
                              const std::string & clientInterfaceName,
                              const std::string & serverComponentName,
                              const std::string & serverInterfaceName):
            ClientComponentName(clientComponentName),
            ClientInterfaceName(clientInterfaceName),
            ServerComponentName(serverComponentName),
            ServerInterfaceName(serverInterfaceName)
        {}
        std::string ClientComponentName;
        std::string ClientInterfaceName;
        std::string ServerComponentName;
        std::string ServerInterfaceName;
    };

 protected:
    typedef std::list<ConnectionType> ConnectionsType;
    ConnectionsType Connections;
};

inline bool operator==(const mtsDelayedConnections::ConnectionType & left,
                       const mtsDelayedConnections::ConnectionType & right)
{
    return (left.ClientComponentName == right.ClientComponentName)
        && (left.ClientInterfaceName == right.ClientInterfaceName)
        && (left.ServerComponentName == right.ServerComponentName)
        && (left.ServerInterfaceName == right.ServerInterfaceName);
}

#endif // _mtsDelayedConnections_h
