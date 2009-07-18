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
  \brief Defines the command interfaces.
*/

#ifndef _mtsFunctionReadOrWrite_h
#define _mtsFunctionReadOrWrite_h


#include <cisstMultiTask/mtsFunctionBase.h>
#include <cisstMultiTask/mtsCommandReadOrWriteBase.h>
#include <cisstMultiTask/mtsDeviceInterface.h>
#include <cisstMultiTask/mtsRequiredInterface.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

template <class _argumentType>
class CISST_EXPORT mtsFunctionReadOrWrite: public mtsFunctionBase {
    typedef _argumentType ArgumentType;
    typedef mtsCommandReadOrWriteBase<ArgumentType> CommandType;
    CommandType * Command;

public:
    /*! Default constructor.  Does nothing, use Bind before
      using. */
    mtsFunctionReadOrWrite(void): Command(0) {}

    /*! Return whether function is valid (i.e., command pointer is non-zero) */
    bool IsValid(void) const { return (Command != 0); }

    /*! Constructor from an interface and a command name.  Uses
      Bind internally. */
    mtsFunctionReadOrWrite(const mtsDeviceInterface * associatedInterface, const std::string & commandName) {
        this->Bind(associatedInterface, commandName);
    }
    
    /*! Destructor. */
    ~mtsFunctionReadOrWrite() {}

    /*! Bind the function object to a command.  The method will return
      false if the interface pointer is null, if the command can not
      be found in the given interface or if the command pointer is
      found but is null.
      \param interface Pointer to an interface whose command is to be
      queried
      \param commandName Name of command
      \result Boolean value, true if success, false otherwise
    */
    bool Bind(const mtsDeviceInterface * associatedInterface, const std::string & commandName);

    /*! Bind using a command pointer.  This allows to avoid
      querying by name from an interface.
      \param command Pointer on an existing command
      \result Boolean value, true if the command pointer is not null.
    */
    inline bool Bind(CommandType * command) {
        Command = command;
        return (command != 0);
    }

    /*! Add the function object to the required interface
      \param interface Required interface
      \param commandName Name of command to bind with (string)
      \param isRequired Whether or not the command is required (false if command is optional)
      \result Boolean value, true if success, false otherwise
    */
    bool AddToRequiredInterface(mtsRequiredInterface & intfc, const std::string & commandName,
                                bool isRequired = true)
    { return intfc.AddCommandPointer(commandName, Command, isRequired); }

    /*! Overloaded operator to enable more intuitive syntax
      e.g., Command(argument) instead of Command->Execute(argument). */
    mtsCommandBase::ReturnType operator()(ArgumentType& argument) const;

    /*! Access to underlying command object. */
    mtsCommandReadOrWriteBase<ArgumentType> * GetCommand(void) const { return Command; }

    /*! Human readable output to stream. */
    void ToStream(std::ostream & outputStream) const;
};



#ifndef SWIG

template <class __argumentType>
bool mtsDeviceInterface::AddEventWrite(mtsFunctionWrite & eventTrigger, const std::string & eventName,
                                       const __argumentType & argumentPrototype) {
    mtsCommandWriteBase * command;
    command = this->AddEventWrite(eventName, argumentPrototype);
    if (command) {
        eventTrigger.Bind(command);
        return true;
    }
    return false;
}

#endif // SWIG

#endif // _mtsFunctionReadOrWrite_h

