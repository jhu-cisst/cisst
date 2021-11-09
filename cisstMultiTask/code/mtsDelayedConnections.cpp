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

#include <cisstMultiTask/mtsDelayedConnections.h>
#include <cisstMultiTask/mtsManagerLocal.h>

void mtsDelayedConnections::Add(const std::string & clientComponentName,
                                const std::string & clientInterfaceName,
                                const std::string & serverComponentName,
                                const std::string & serverInterfaceName)
{
    ConnectionType newConnection(clientComponentName,
                                 clientInterfaceName,
                                 serverComponentName,
                                 serverInterfaceName);
    ConnectionsType::const_iterator found = std::find(Connections.begin(),
                                                      Connections.end(),
                                                      newConnection);
    if (found == Connections.end()) {
        Connections.push_back(newConnection);
    }
}

void mtsDelayedConnections::Connect(const bool purge)
{
    mtsComponentManager * componentManager = mtsComponentManager::GetInstance();
    const ConnectionsType::const_iterator end = Connections.end();
    ConnectionsType::const_iterator connection;
    for (connection = Connections.begin();
         connection != end;
         ++connection) {
        componentManager->Connect(connection->ClientComponentName,
                                  connection->ClientInterfaceName,
                                  connection->ServerComponentName,
                                  connection->ServerInterfaceName);
    }
    if (purge) {
        Connections.clear();
    }
}
