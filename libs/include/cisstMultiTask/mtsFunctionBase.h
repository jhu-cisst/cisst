/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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
    /*! Detach the function from the command used.  Internally, sets the command pointer to 0 */
    virtual bool Detach(void) = 0;

    /*! Return whether function is valid (i.e., command pointer is non-zero) */
    virtual bool IsValid(void) const = 0;

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

