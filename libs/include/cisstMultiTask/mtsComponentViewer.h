/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides
  Created on: 2010-09-07

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

/*!
  \file
  \brief Definition of the task viewer
  \ingroup cisstMultiTask
*/

#ifndef _mtsComponentViewer_h
#define _mtsComponentViewer_h


#include <cisstMultiTask/mtsTaskFromSignal.h>
#include <cisstMultiTask/mtsParameterTypes.h>
#include <cisstMultiTask/mtsManagerComponentBase.h>
#include <cisstMultiTask/mtsManagerComponentServices.h>
#include <cisstOSAbstraction/osaPipeExec.h>
#include <cisstOSAbstraction/osaThread.h>

// Always include last!
#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsComponentViewer : public mtsTaskFromSignal
{
   CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

private:

    // Utility method that sends the whole string except the '\0'
    void WriteString(osaPipeExec & pipe, const std::string & s, double timeoutInSec = 0.1);

protected:

    osaPipeExec UDrawPipe;
    bool UDrawPipeConnected;

    // Use a separate reader thread because osaPipeExec::Read is blocking.
    // This could be eliminated if a non-blocking Read is implemented.
    osaThread ReaderThread;
    void *ReadFromUDrawGraph(int);
    bool ReaderThreadFinished;
    bool WaitingForResponse;
    bool WaitForResponse(double timeoutInSec = 0.1) const;

    bool RedrawGraph; // PK TEMP: use provided interface instead

    bool IsProxyComponent(const std::string & componentName) const;
    bool ConnectToUDrawGraph(void);

    void SendAllInfo(void);

    std::string GetComponentInGraphFormat(const std::string & processName, const std::string & componentName) const;
    std::string GetComponentInUDrawGraphFormat(const std::string & processName, const std::string & componentName,
                                               const mtsComponentState & componentState) const;
    std::string GetStateInUDrawGraphFormat(const mtsComponentState &componentState) const;

    // Event Handlers
    void AddComponent(const mtsDescriptionComponent &componentInfo);
    void ChangeState(const mtsComponentStateChange &componentStateChange);
    void AddConnection(const mtsDescriptionConnection &connectionInfo);
    void RemoveConnection(const mtsDescriptionConnection &connectionInfo);

public:

    mtsComponentViewer(const std::string & name);

    virtual ~mtsComponentViewer();

    void Configure(const std::string & CMN_UNUSED(filename)) {}

    void Startup(void);

    void Run(void);

    void Cleanup(void);

};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponentViewer)

#endif // _mts_ComponentViewer.h
