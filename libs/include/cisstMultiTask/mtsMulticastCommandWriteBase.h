/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsMulticastCommandWriteBase.h,v 1.4 2008/09/05 04:31:10 anton Exp $

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


#ifndef _mtsMulticastCommandWriteBase_h
#define _mtsMulticastCommandWriteBase_h


#include <cisstMultiTask/mtsCommandReadOrWriteBase.h>
#include <vector>

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  This class contains a vector of two or more command objects.
  The primary use of this class is to send events to all observers.
 */
class CISST_EXPORT mtsMulticastCommandWriteBase: public mtsCommandWriteBase
{
public:
    typedef mtsCommandWriteBase BaseType;
    
protected:
    std::vector<BaseType *> Commands;
    
public:
    /*! Default constructor. Does nothing. */
    mtsMulticastCommandWriteBase(const std::string & name):
        BaseType(name)
    {}
    
    /*! Default destructor. Does nothing. */
    ~mtsMulticastCommandWriteBase() {}

    /*! Add a command to the composite. */
    virtual void AddCommand(BaseType * command);
    
    /*! Execute all the commands in the composite. */
    virtual mtsCommandBase::ReturnType Execute(const cmnGenericObject & argument) = 0;

    /*! Return a pointer on the argument prototype.  Uses the first
      command added to find the argument prototype.  If no command is
      available, return 0 (null pointer) */
    virtual const cmnGenericObject * GetArgumentPrototype(void) const = 0;    

    /* documented in base class */
    virtual void ToStream(std::ostream & outputStream) const;
};


#endif // _mtsMulticastCommandWriteBase_h

