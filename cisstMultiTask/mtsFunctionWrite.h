/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Peter Kazanzides, Anton Deguet

  (C) Copyright 2007-2014 Johns Hopkins University (JHU), All Rights Reserved.

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

#ifndef _mtsFunctionWrite_h
#define _mtsFunctionWrite_h


#include <cisstMultiTask/mtsFunctionBase.h>
#include <cisstMultiTask/mtsCommandWriteBase.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsGenericObjectProxy.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsFunctionWrite: public mtsFunctionBase {
public:
    typedef mtsCommandWriteBase CommandType;
    typedef mtsExecutionResult(mtsFunctionWrite::*ActionType)(const mtsGenericObject &) const;
protected:
    /*! Internal pointer to command.  Command pointer should be set
      when interfaces get connected. */
    CommandType * Command;

#ifndef SWIG
    template <typename _userType, bool>
    class ConditionalWrap {
    public:
        static mtsExecutionResult Call(mtsFunctionWrite::ActionType executeMethod, const mtsFunctionWrite * function, const _userType & argument) {
            mtsGenericObjectProxyRef<_userType> argumentWrapped(argument);
            return (function->*executeMethod)(argumentWrapped);
        }
    };
    template <typename _userType>
    class ConditionalWrap<_userType, true> {
    public:
        static mtsExecutionResult Call(mtsFunctionWrite::ActionType executeMethod, const mtsFunctionWrite * function, const _userType & argument) {
            return (function->*executeMethod)(argument);
        }
    };
#endif

public:
    /*! Default constructor.  Does nothing, use Bind before
      using. */
    mtsFunctionWrite(const bool isProxy = false);

    /*! Destructor. */
    virtual ~mtsFunctionWrite();

    // documented in base class
    bool Detach(void);

    // documented in base class
    bool IsValid(void) const;

    /*! Bind using a command pointer.  This allows to avoid
      querying by name from an interface.
      \param command Pointer on an existing command
      \result Boolean value, true if the command pointer is not null.
    */
    bool Bind(CommandType * command);

    /*! Overloaded operator to enable more intuitive syntax
      e.g., Command(argument) instead of Command->Execute(argument). */
    mtsExecutionResult operator()(const mtsGenericObject & argument) const
    { return ExecuteGeneric(argument); }

    mtsExecutionResult ExecuteGeneric(const mtsGenericObject & argument) const;
    mtsExecutionResult ExecuteBlockingGeneric(const mtsGenericObject & argument) const;

#ifndef SWIG
	/*! Overloaded operator that accepts different argument types. */
    template <class _userType>
    mtsExecutionResult operator()(const _userType & argument) const
    { return Execute(argument); }

    template <class _userType>
    mtsExecutionResult Execute(const _userType & argument) const {
        return ConditionalWrap<_userType, cmnIsDerivedFrom<_userType, mtsGenericObject>::IS_DERIVED>::Call(&mtsFunctionWrite::ExecuteGeneric, this, argument);
    }

    template <class _userType>
    mtsExecutionResult ExecuteBlocking(const _userType & argument) const {
        return ConditionalWrap<_userType, cmnIsDerivedFrom<_userType, mtsGenericObject>::IS_DERIVED>::Call(&mtsFunctionWrite::ExecuteBlockingGeneric, this, argument);
    }
#endif

    /*! Access to underlying command object. */
    CommandType * GetCommand(void) const;

    /*! Access to the command argument prototype. */
    const mtsGenericObject * GetArgumentPrototype(void) const;

    /*! Human readable output to stream. */
    void ToStream(std::ostream & outputStream) const;

};

#endif // _mtsFunctionWrite_h

