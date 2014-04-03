/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Peter Kazanzides, Anton Deguet

  (C) Copyright 2007-2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/



/*!
  \file
  \brief Defines a base function object to allow heterogeneous containers of functions.
*/

#ifndef _mtsFunctionBase_h
#define _mtsFunctionBase_h

#include <cisstOSAbstraction/osaForwardDeclarations.h>
#include <cisstMultiTask/mtsExecutionResult.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

class mtsEventReceiverWrite;

class CISST_EXPORT mtsFunctionBase {

private:
    mtsFunctionBase(void); // default constructor should not be used.

protected:
    /*! Default constructor. */
    mtsFunctionBase(const bool isProxy);

    /*! Destructor. */
    virtual ~mtsFunctionBase();

    /*! Reference to an existing thread signal used to block the execution. */
    osaThreadSignal * ThreadSignal;

    /*! Event receiver for events containing return value or indication that
      blocking command has finished. */
    mtsEventReceiverWrite *CompletionCommand;

    /*! Indicates if this function is used by a proxy required
      interface.  If this is the case, blocking commands should not
      block the proxy component. */
    bool IsProxy;

public:
    /*! Detach the function from the command used.  Internally, sets the command pointer to 0 */
    virtual bool Detach(void) = 0;

    /*! Return whether function is valid (i.e., command pointer is non-zero) */
    virtual bool IsValid(void) const = 0;

    /*! Human readable output to stream. */
    virtual void ToStream(std::ostream & outputStream) const = 0;

    /*! Initialize the completion command (mtsEventReceiverWrite), creating it if necessary. */
    virtual void InitCompletionCommand(const std::string &name);

    /*! Set the thread signal used for blocking commands */
    void SetThreadSignal(osaThreadSignal * threadSignal);

    /*! Wait for internal thread signal */
    void ThreadSignalWait(void) const;

    /*! Wait for return value (read, qualified read, void return, write return) */
    mtsExecutionResult WaitForResult(mtsGenericObject &arg) const;

    /*! Wait for execution result (blocking void, blocking write) */
    mtsExecutionResult WaitForResult(void) const;

};


/*! Stream out operator. */
inline std::ostream & operator << (std::ostream & output,
                                   const mtsFunctionBase & function) {
    function.ToStream(output);
    return output;
}


#endif // _mtsFunctionBase_h
