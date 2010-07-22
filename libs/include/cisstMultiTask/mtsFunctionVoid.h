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
  \brief Defines a function object to use a void command (mtsCommandVoid)
*/

#ifndef _mtsFunctionVoid_h
#define _mtsFunctionVoid_h

#include <cisstMultiTask/mtsFunctionBase.h>
#include <cisstMultiTask/mtsCommandVoidBase.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsFunctionVoid: public mtsFunctionBase {
 protected:
    typedef mtsCommandVoidBase CommandType;
    CommandType * Command;

 public:
    /*! Default constructor.  Does nothing, use Instantiate before
      using. */
    mtsFunctionVoid(void): Command(0) {}

    /*! Destructor. */
    ~mtsFunctionVoid();

    // documented in base class
    inline bool Detach(void) {
        if (this->IsValid()) {
            Command = 0;
            return true;
        }
        return false;
    }

    // documented in base class
    inline bool IsValid(void) const {
        return (this->Command != 0);
    }

    /*! Bind using a command pointer.  This allows to avoid
      querying by name from an interface.
      \param command Pointer on an existing command
      \result Boolean value, true if the command pointer is not null.
    */
    inline bool Bind(CommandType * command) {
        Command = command;
        return (command != 0);
    }

    /*! Overloaded operator to enable more intuitive syntax
      e.g., Command() instead of Command->Execute(). */
    mtsCommandBase::ReturnType operator()() const;

    /*! Access to underlying command object. */
    mtsCommandVoidBase * GetCommand(void) const { return Command; }

    /*! Human readable output to stream. */
    void ToStream(std::ostream & outputStream) const;
};


#endif // _mtsFunctionVoid_h

