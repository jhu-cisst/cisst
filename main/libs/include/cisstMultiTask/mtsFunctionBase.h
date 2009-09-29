/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsFunctionBase.h 75 2009-02-24 16:47:20Z adeguet1 $

  Author(s):  Peter Kazanzides, Anton Deguet

  (C) Copyright 2007-2008 Johns Hopkins University (JHU), All Rights Reserved.

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

#include <cisstMultiTask/mtsForwardDeclarations.h>

class mtsFunctionBase {
    
protected:
    /*! Default constructor. */
    mtsFunctionBase(void) {}
    
    /*! Destructor. */
    virtual ~mtsFunctionBase() {}

public:
    /*! Return whether function is valid (i.e., command pointer is non-zero) */
    virtual bool IsValid(void) const = 0;

    /*! Bind the function object to a command
      \param interface Pointer to an interface whose command is to be queried
      \param commandName Name of command
      \result Boolean value, true if success, false otherwise
    */
    virtual bool Bind(const mtsDeviceInterface * associatedInterface, const std::string & commandName) = 0;

    /*! Add the function object to the required interface
      \param interface Required interface
      \param commandName Name of command to bind with (string)
      \param isRequired Whether or not the command is required (false if command is optional)
      \result Boolean value, true if success, false otherwise
    */
    virtual bool AddToRequiredInterface(mtsRequiredInterface & intfc, const std::string & commandName,
                                        bool isRequired = true) = 0;

    /*! Human readable output to stream. */
    virtual void ToStream(std::ostream & outputStream) const = 0;
};


/*! Stream out operator. */
inline std::ostream & operator << (std::ostream & output,
                                   const mtsFunctionBase & function) {
    function.ToStream(output);
    return output;
}


#endif // _mtsFunctionBase_h

