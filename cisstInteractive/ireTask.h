/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Peter Kazanzides
  Created on: 2010-12-10

  (C) Copyright 2010-2025 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file
  \brief Declaration of ireTask
*/


#ifndef _ireTask_h
#define _ireTask_h

#include <cisstMultiTask/mtsTaskContinuous.h>
#include <cisstInteractive/ireTaskConstructorArg.h>

#include <cisstInteractive/ireExport.h>


/*!
  \brief Interactive Research Environment (IRE) Task

  \ingroup cisstInteractive
*/

class CISST_EXPORT ireTask : public mtsTaskContinuous {
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION_ONEARG, CMN_LOG_ALLOW_DEFAULT);
    typedef mtsTaskContinuous BaseType;

    IRE_Shell Shell;
    std::string StartupCommands;  // startup string

    void Initialize(void);
    void Log(const mtsLogMessage & log);

public:
    ireTask(const std::string &name = "IRE",
            IRE_Shell shell = IRE_WXPYTHON,
            const std::string &startup = "");
    ireTask(const ireTaskConstructorArg &arg);

    /*! Destructor. */
    virtual ~ireTask();

    void Startup(void);
    void Run(void);
    void Cleanup(void);
};

CMN_DECLARE_SERVICES_INSTANTIATION(ireTask)

#endif // _ireTask_h

