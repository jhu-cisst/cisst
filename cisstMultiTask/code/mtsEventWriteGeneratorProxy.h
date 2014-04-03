/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

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


#ifndef _mtsEventWriteGeneratorProxy_h
#define _mtsEventWriteGeneratorProxy_h


#include <cisstMultiTask/mtsMulticastCommandWriteBase.h>
#include <vector>

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  This class contains a vector of two or more command objects.
  The primary use of this class is to send events to all observers.
 */
class CISST_EXPORT mtsEventWriteGeneratorProxy: public mtsMulticastCommandWriteBase
{
public:
    typedef mtsMulticastCommandWriteBase BaseType;

public:
    /*! Default constructor. Does nothing. */
    mtsEventWriteGeneratorProxy(const std::string & name):
        BaseType(name)
    {}

    /*! Default destructor. Does nothing. */
    ~mtsEventWriteGeneratorProxy() {}

    /*! Add a command to the composite. */
    // virtual void AddCommand(BaseType * command);

    /*! Execute all the commands in the composite. */
    virtual mtsExecutionResult Execute(const mtsGenericObject & argument) {
        if (this->Commands[0]) {
            return this->Commands[0]->Execute(argument, MTS_NOT_BLOCKING);
        }
    }

    /*! Return a pointer on the argument prototype.  Uses the first
      command added to find the argument prototype.  If no command is
      available, return 0 (null pointer) */
    virtual const mtsGenericObject * GetArgumentPrototype(void) const {
        if (this->Commands[0]) {
            return this->Commands[0]->GetArgumentPrototype();
        }
        return 0;
    }

    /* documented in base class */
    // virtual void ToStream(std::ostream & outputStream) const;
};


#endif // _mtsEventWriteGeneratorProxy_h

