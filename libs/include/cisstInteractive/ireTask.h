/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Peter Kazanzides
  Created on: 2010-12-10

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

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

#include <cisstInteractive/ireExport.h>


/*!
  \brief Interactive Research Environment (IRE) Task

  \ingroup cisstInteractive
*/

class CISST_EXPORT ireTask : public mtsTaskContinuous {

    std::string StartupCommands;  // startup string

public:
    typedef mtsTaskContinuous BaseType;

    ireTask(const std::string &name = "IRE",
            const std::string &startup = "");

    /*! Destructor. */
    virtual ~ireTask();

    void Startup(void);
    void Run(void);
    void Cleanup(void);
};

#endif // _ireTask_h

