/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides
  Created on: 2013-08-06

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of mtsSocketProxyClient
  \ingroup cisstMultiTask
*/

#ifndef _mtsSocketProxyClient_h
#define _mtsSocketProxyClient_h

#include <cisstOSAbstraction/osaSocket.h>
#include <cisstMultiTask/mtsTaskContinuous.h>

#include <cisstMultiTask/mtsForwardDeclarations.h>

class CommandWrapperBase;

#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsSocketProxyClient : public mtsTaskContinuous
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

 protected:

    osaSocket Socket;

    // For memory cleanup
    std::vector<CommandWrapperBase *> CommandWrappers;

    /*! \brief Create client proxy
      \param providedInterfaceDescription Complete information about provided
      interface to be created with arguments serialized
      \return True if success, false otherwise */
    bool CreateClientProxy(const std::string & providedInterfaceName, const InterfaceProvidedDescription & providedInterfaceDescription);

 public:
    /*! Constructor
        \param name Name of the client proxy component
        \param ip IP address for corresponding server proxy
        \param port Port for corresponding server proxy (UDP socket)
    */
    mtsSocketProxyClient(const std::string & name, const std::string &ip, short port);

    /*! Destructor */
    virtual ~mtsSocketProxyClient();

    void Configure(const std::string &) {}

    void Startup(void);

    void Run(void);

    void Cleanup(void);

};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsSocketProxyClient)

#endif // _mtsSocketProxyClient_h
