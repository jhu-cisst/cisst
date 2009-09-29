/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsFunctionQualifiedReadOrWrite.h 811 2009-09-04 21:30:24Z mjung5 $

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

#ifndef _mtsFunctionQualifiedReadOrWrite_h
#define _mtsFunctionQualifiedReadOrWrite_h


#include <cisstMultiTask/mtsFunctionBase.h>
#include <cisstMultiTask/mtsCommandQualifiedReadOrWriteBase.h>
#include <cisstMultiTask/mtsDeviceInterface.h>
#include <cisstMultiTask/mtsRequiredInterface.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

template <class _argumentType>
class CISST_EXPORT mtsFunctionQualifiedReadOrWrite: public mtsFunctionBase {
protected:
    typedef _argumentType ArgumentType;
    typedef mtsCommandQualifiedReadOrWriteBase<ArgumentType> CommandType;
    CommandType * Command;
    
 public:
    /*! Default constructor.  Does nothing, use Bind before
      using. */
    mtsFunctionQualifiedReadOrWrite(void): Command(0) {}

    /*! Constructor from an interface and a command name.  Uses
      Bind internally. */
    mtsFunctionQualifiedReadOrWrite(const mtsDeviceInterface * associatedInterface, const std::string & commandName) {
        this->Bind(associatedInterface, commandName);
    }
    
    /*! Destructor. */
    ~mtsFunctionQualifiedReadOrWrite() {}

    /*! Return whether function is valid (i.e., command pointer is non-zero) */
    bool IsValid(void) const { return (Command != 0); }

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
    mtsCommandBase::ReturnType operator()(const mtsGenericObject & qualifier,
                                          ArgumentType & argument) const;

    /*! Access to underlying command object. */
    mtsCommandQualifiedReadOrWriteBase<ArgumentType> * GetCommand(void) const { return Command; }

    /*! Access to the command argument 1 prototype. */
    const mtsGenericObject * GetArgument1Prototype(void) const;

    /*! Access to the command argument 2 prototype. */
    const mtsGenericObject * GetArgument2Prototype(void) const;

    /*! Human readable output to stream. */
    void ToStream(std::ostream & outputStream) const;
};


typedef mtsFunctionQualifiedReadOrWrite<mtsGenericObject> mtsFunctionQualifiedRead;


#endif // _mtsFunctionQualifiedReadOrWrite_h

