/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
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


#ifndef _mtsMulticastCommandQualifiedReadOrWrite_h
#define _mtsMulticastCommandQualifiedReadOrWrite_h

#if 0 // AD  it seems that this code is not used anymore

#include <cisstMultiTask/mtsCommandQualifiedReadOrWriteBase.h>
#include <vector>

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  This class contains a vector of two or more command
  objects. Currently it is used by composite interface, where the
  command needs to be chained, such that it is executed on more than
  one interface. This is the one argument version, that is the execute
  method takes one argument.
 */
template <class _argumentType>
class mtsMulticastCommandQualifiedReadOrWrite: public mtsCommandQualifiedReadOrWriteBase<_argumentType>
{
public:
    typedef _argumentType ArgumentType;
    typedef mtsCommandQualifiedReadOrWriteBase<ArgumentType> BaseType;

protected:
    /*! Vector to commands that constitute the composite command. */
    std::vector<BaseType *> Commands;

public:
    /*! Default constructor. Does nothing. */
    mtsMulticastCommandQualifiedReadOrWrite(void): BaseType("", 0, 0) {}

    /*! Default destructor. Does nothing. */
    ~mtsMulticastCommandQualifiedReadOrWrite() {}

    /*! Add a command to the composite. */
    void AddCommand(BaseType * command);

    /*! Get command (should use iterator instead) */


    /*! Execute all the commands in the composite. */
    virtual mtsCommandBase::ReturnType Execute(const cmnGenericObject & qualifier, ArgumentType argument);

    /* documented in base class */
    virtual void ToStream(std::ostream & out) const;
};


typedef mtsMulticastCommandQualifiedReadOrWrite<cmnGenericObject & > mtsMulticastCommandQualifiedRead;

#endif


#endif // _mtsMulticastCommandQualifiedReadOrWrite_h

