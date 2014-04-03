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

#ifndef _mtsFunctionRead_h
#define _mtsFunctionRead_h


#include <cisstMultiTask/mtsFunctionBase.h>
#include <cisstMultiTask/mtsCommandRead.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsGenericObjectProxy.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsFunctionRead: public mtsFunctionBase {
public:
    typedef mtsCommandRead CommandType;
protected:
    CommandType * Command;

#ifndef SWIG
    template <typename _userType, bool>
    class ConditionalWrap {
    public:
        static mtsExecutionResult Call(const mtsFunctionRead * function, _userType & argument) {
            mtsGenericObjectProxyRef<_userType> argumentWrapped(argument);
            return function->ExecuteGeneric(argumentWrapped);
        }
    };
    template <typename _userType>
    class ConditionalWrap<_userType, true> {
    public:
        static mtsExecutionResult Call(const mtsFunctionRead * function, _userType & argument) {
            return function->ExecuteGeneric(argument);
        }
    };
#endif

public:
    /*! Default constructor.  Does nothing, use Bind before
      using. */
    mtsFunctionRead(void);

    /*! Destructor. */
    virtual ~mtsFunctionRead();

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
    mtsExecutionResult operator()(mtsGenericObject & argument) const
    { return ExecuteGeneric(argument); }

    mtsExecutionResult ExecuteGeneric(mtsGenericObject & argument) const;

#ifndef SWIG
	/*! Overloaded operator that accepts different argument types. */
    template <class _userType>
    mtsExecutionResult operator()(_userType & argument) const {
        mtsExecutionResult result = Command ?
            ConditionalWrap<_userType, cmnIsDerivedFrom<_userType, mtsGenericObject>::IS_DERIVED>::Call(this, argument)
          : mtsExecutionResult::FUNCTION_NOT_BOUND;
        return result;
    }
#endif

    /*! Access to underlying command object. */
    CommandType * GetCommand(void) const;

    /*! Access to the command argument prototype. */
    const mtsGenericObject * GetArgumentPrototype(void) const;

    /*! Human readable output to stream. */
    void ToStream(std::ostream & outputStream) const;

};

#endif // _mtsFunctionRead_h
