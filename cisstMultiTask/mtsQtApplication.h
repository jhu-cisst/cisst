/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Peter Kazanzides
  Created on: 2012-06-26

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsQtApplication_h
#define _mtsQtApplication_h

#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstMultiTask/mtsTaskContinuous.h>

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

class QApplication;
class QBasicTimer;
class BasicTimerHandler;

/*!
  \file
  \brief Declaration of mtsQtApplication
*/


/*!
  \ingroup cisstMultiTaskQt

  mtsQtApplication provides a component wrapper for the single
  QApplication object that must be created when using the Qt toolkit.
  This component is derived from mtsTaskContinuous, and is set up
  to use the current (main) thread.
  The QApplication object is dynamically created in the class
  constructor, and the Run method calls QApplication::Exec, which
  does not return.

*/

class CISST_EXPORT mtsQtApplicationConstructorArg : public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    std::string Name;
    std::vector<std::string> Args;   // argc, argv

public:
    mtsQtApplicationConstructorArg();
    mtsQtApplicationConstructorArg(const std::string &name, int argc, char **argv);
    mtsQtApplicationConstructorArg(const mtsQtApplicationConstructorArg &other);
    virtual ~mtsQtApplicationConstructorArg();

    std::string GetName() const { return Name; }
    int GetArgc() const { return Args.size(); }
    void GetArgv(char ***argvp) const;

    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);

    void ToStream(std::ostream & outputStream) const;

    /*! Raw text output to stream */
    virtual void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                             bool headerOnly = false, const std::string & headerPrefix = "") const;

    /*! Read from an unformatted text input (e.g., one created by ToStreamRaw).
      Returns true if successful. */
    virtual bool FromStreamRaw(std::istream & inputStream, const char delimiter = ' ');
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsQtApplicationConstructorArg);

class CISST_EXPORT mtsQtApplication: public mtsTaskContinuous
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION_ONEARG, CMN_LOG_ALLOW_ALL)

    QApplication * QtApp;   // Could also use global variable qApp
    int ArgcCopy;
    char **ArgvCopy;
    QBasicTimer * Timer;
    BasicTimerHandler * TimerHandler;

public:
    mtsQtApplication(const std::string &name, int &argc, char **argv);
    mtsQtApplication(const mtsQtApplicationConstructorArg &arg);
    virtual ~mtsQtApplication();

    void Startup(void);
    void Run(void);
    void Process(void);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsQtApplication);

#endif  // _mtsQtApplication_h
