/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsCommandBase.h,v 1.5 2008/09/05 04:31:10 anton Exp $

  Author(s):  Ankur Kapoor, Anton Deguet 
  Created on: 2006-05-02

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
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
#include <cisstCommon/cmnGenericObjectProxy.h>

#include <cisstMultiTask/mtsForwardDeclarations.h>

#include <iostream>
#include <sstream>

/*!
  \ingroup cisstMultiTask
*/
class mtsCommandBase {
public:
    std::string Name;

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
    };
    
    /*! The constructor. Does nothing */
    inline mtsCommandBase(void):Name("??") {}
    inline mtsCommandBase(const std::string & name): Name(name) {}
    
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

