/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2010-09-26

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines a base class for a callable object with no argument
 */

#ifndef _mtsCallableVoidBase_h
#define _mtsCallableVoidBase_h

#include <cisstMultiTask/mtsExecutionResult.h>

/*!
  \ingroup cisstMultiTask

  A base class command object with an execute method that takes no
  arguments.  This pure virtual base class is derived to support
  either global functions or methods with the signature "void
  method(void)" (non const) */
class mtsCallableVoidBase
{
public:

    /*! The constructor. Does nothing */
    inline mtsCallableVoidBase(void) {}

    /*! The destructor. Does nothing */
    inline virtual ~mtsCallableVoidBase() {}

    /*! The execute method. Abstract method to be implemented by derived
      classes to run the actual operation on the receiver. */
    virtual mtsExecutionResult Execute(void) = 0;

    /*! Human readable description */
    virtual void ToStream(std::ostream & outputStream) const = 0;
};

/*! Stream out operator for all classes derived from mtsCallableVoidBase.
  This operator uses the ToStream method so that the output can be
  different for each derived class. */
inline std::ostream & operator << (std::ostream & outputStream,
                                   const mtsCallableVoidBase & callable) {
    callable.ToStream(outputStream);
    return outputStream;
}

#endif // _mtsCallableVoidBase_h

