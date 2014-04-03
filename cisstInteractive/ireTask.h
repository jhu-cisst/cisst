/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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


//enum IRE_Shell { IRE_WXPYTHON, IRE_IPYTHON, IRE_PYTHON };
enum IRE_Shell { IRE_WXPYTHON, IRE_IPYTHON };

/*!
  \brief Interactive Research Environment (IRE) Task

  \ingroup cisstInteractive
*/

class ireTaskConstructorArg : public mtsGenericObject {
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
public:
    std::string Name;
    IRE_Shell   Shell;
    std::string Startup;

    ireTaskConstructorArg() : mtsGenericObject(), Shell(IRE_WXPYTHON) {}
    ireTaskConstructorArg(const std::string &name, IRE_Shell shell = IRE_WXPYTHON, const std::string &startup = "") :
        mtsGenericObject(), Name(name), Shell(shell), Startup(startup) {}
    ireTaskConstructorArg(const ireTaskConstructorArg &other) :
        mtsGenericObject(), Name(other.Name), Shell(other.Shell), Startup(other.Startup) {}
    ~ireTaskConstructorArg() {}

    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);

    void ToStream(std::ostream & outputStream) const;

    /*! Raw text output to stream */
    virtual void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                             bool headerOnly = false, const std::string & headerPrefix = "") const;

    /*! Read from an unformatted text input.
      Returns true if successful. */
    virtual bool FromStreamRaw(std::istream & inputStream, const char delimiter = ' ');
};

CMN_DECLARE_SERVICES_INSTANTIATION(ireTaskConstructorArg);

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

