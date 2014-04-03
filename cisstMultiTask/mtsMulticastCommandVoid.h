/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2013 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines a command with one argument sent to multiple interfaces
*/


#ifndef _mtsMulticastCommandVoid_h
#define _mtsMulticastCommandVoid_h


#include <cisstMultiTask/mtsCommandVoid.h>
#include <vector>

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  This class contains a vector of two or more command
  objects. Currently it is used by composite interface, where the
  command needs to be chained, such that it is executed on more than
  one interface. This is the no argument version, that is the execute
  method takes no argument.
 */
class CISST_EXPORT mtsMulticastCommandVoid: public mtsCommandVoid
{
public:
    typedef mtsCommandVoid BaseType;
    typedef std::vector<BaseType *> VectorType;

protected:
    /*! Vector to commands that constitute the composite command. */
    VectorType Commands;

public:
    /*! Default constructor. Does nothing. */
    mtsMulticastCommandVoid(const std::string & name);

    /*! Default destructor. Does nothing. */
    ~mtsMulticastCommandVoid();

    /*! Add a command to the composite. */
    virtual bool AddCommand(BaseType * command);

    /*! Remove a command from the composite. */
    virtual bool RemoveCommand(BaseType * command);

    /*! Execute all the commands in the composite. */
    virtual mtsExecutionResult Execute(mtsBlockingType CMN_UNUSED(blocking));

    /* documented in base class */
    virtual void ToStream(std::ostream & outputStream) const;
};


#endif // _mtsMulticastCommandVoid_h

