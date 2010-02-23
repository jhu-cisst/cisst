/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Anton Deguet
  Created on: 2006-05-02

  (C) Copyright 2006-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines a base class for a command
*/

#ifndef _mtsCommandBase_h
#define _mtsCommandBase_h

// These two includes are not required to compile this class but are
// pretty much always needed in conjunction with commands.  To ease
// the user's life, we include them now.
#include <cisstMultiTask/mtsGenericObjectProxy.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

#include <iostream>
#include <sstream>

/*!
  \ingroup cisstMultiTask
*/
class mtsCommandBase {

private:
    /*! Private copy constructor to prevent copies */
    inline mtsCommandBase(const mtsCommandBase & CMN_UNUSED(other));

protected:
    /*! Name used for the command.  The name is provided to the
      constructor and can be accessed using the method GetName(). */
    std::string Name;

    /*! Flag used to determine is the command actually executes the
      provided method or function.  This "gated" command can be useful
      to turn on/off and event callback or to prevent calling a method
      owned by an object being deleted. */
    bool EnableFlag;

public:
    /* use to bitshift and or for return value of a composite
       would limit the number of composite interfaces to 31 for
       an int return value
    */
    enum { RETURN_TYPE_BIT_SIZE = 1 };

    /* the error value is -ve of the return value */
    enum ReturnType {
        DEV_OK = 0,
        DEV_NOT_OK = 1,
        BAD_COMMAND = 12,
        NO_MAILBOX = 13,
        BAD_INPUT = 14,
        NO_INTERFACE = 15,
        MAILBOX_FULL = 16,
        DISABLED = 17,
        COMMAND_FAILED = 18  // Read or QualifiedRead returned 'false'
    };

    /*! The constructor. Does nothing */
    inline mtsCommandBase(void):
        Name("??"),
        EnableFlag(true)
    {}

    /*! Constructor with command name. */
    inline mtsCommandBase(const std::string & name):
        Name(name),
        EnableFlag(true)
    {}

    /*! The destructor. Does nothing */
    virtual ~mtsCommandBase() {}

    /*! For debugging. Generate a human readable output for the
      command object */
    //@{
    virtual std::string ToString(void) const  {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    };
    virtual void ToStream(std::ostream & out) const = 0;
    //@}

    /*! Returns number of arguments (parameters) expected by Execute
      method.  Must be overloaded in derived classes. */
    virtual unsigned int NumberOfArguments(void) const = 0;

    /*! Set and access the "enable" flag.  This flag is used to
      determine if the command actually uses the provided method or
      function when it is executed. */
    //@{
    inline void Enable(void) {
        this->EnableFlag = true;
    }

    inline void Disable(void) {
        this->EnableFlag = false;
    }

    inline bool IsEnabled(void) const {
        return this->EnableFlag;
    }

    inline bool IsDisabled(void) const {
        return !(this->EnableFlag);
    }
    //@}

    /*! Get the command name.  This method doesn't allow to change the
      command name. */
    inline const std::string & GetName(void) const {
        return this->Name;
    }
};


/*! Stream out operator for all classes derived from mtsCommandBase.
  This operator uses the ToStream method so that the output can be
  different for each derived class. */
inline std::ostream & operator << (std::ostream & output,
                                   const mtsCommandBase & command) {
    command.ToStream(output);
    return output;
}


#endif // _mtsCommandBase_h

