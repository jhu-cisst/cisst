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

#ifndef _mtsFunctionQualifiedRead_h
#define _mtsFunctionQualifiedRead_h


#include <cisstMultiTask/mtsFunctionBase.h>
#include <cisstMultiTask/mtsCommandQualifiedRead.h>
#include <cisstMultiTask/mtsGenericObjectProxy.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsFunctionQualifiedRead: public mtsFunctionBase {
public:
    typedef mtsCommandQualifiedRead CommandType;
protected:
    CommandType * Command;

#ifndef SWIG
    // Portability note:  Visual Studio.NET 2003 did not compile with following (Error C2365), needed to add "a" and "b".
    // template<typename _userType1, typename _userType2, bool, bool>
    template <typename _userType1, typename _userType2, bool a, bool b>
    class ConditionalWrap {
        // default case: both parameters need to be wrapped
    public:
        static mtsExecutionResult Call(const mtsFunctionQualifiedRead * function,
                                       const _userType1 & argument1, _userType2 & argument2) {
            const mtsGenericObjectProxyRef<_userType1> argument1Wrapped(argument1);
            mtsGenericObjectProxyRef<_userType2> argument2Wrapped(argument2);
            return  function->ExecuteGeneric(argument1Wrapped, argument2Wrapped);
        }
    };
    template <typename _userType1, typename _userType2>
    class ConditionalWrap<_userType1, _userType2, false, true> {
        // specialization: only first parameter needs to be wrapped
    public:
        static mtsExecutionResult Call(const mtsFunctionQualifiedRead * function,
                                       const _userType1 & argument1, _userType2 & argument2) {
            const mtsGenericObjectProxyRef<_userType1> argument1Wrapped(argument1);
            return  function->ExecuteGeneric(argument1Wrapped, argument2);
        }
    };
    template <typename _userType1, typename _userType2>
    class ConditionalWrap<_userType1, _userType2, true, false> {
        // specialization: only second parameter needs to be wrapped
    public:
        static mtsExecutionResult Call(const mtsFunctionQualifiedRead * function,
                                       const _userType1 & argument1, _userType2 & argument2) {
            mtsGenericObjectProxyRef<_userType2> argument2Wrapped(argument2);
            return function->ExecuteGeneric(argument1, argument2Wrapped);
        }
    };
    template <typename _userType1, typename _userType2>
    class ConditionalWrap<_userType1, _userType2, true, true> {
        // specialization: neither parameter needs to be wrapped
    public:
        static mtsExecutionResult Call(const mtsFunctionQualifiedRead * function,
                                       const _userType1 & argument1, _userType2 & argument2) {
            return function->ExecuteGeneric(argument1, argument2);
        }
    };
#endif

 public:
    /*! Default constructor.  Does nothing, use Bind before
      using. */
    mtsFunctionQualifiedRead(void);

    /*! Destructor. */
    ~mtsFunctionQualifiedRead();

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
    mtsExecutionResult operator()(const mtsGenericObject & qualifier,
                                  mtsGenericObject & argument) const
    { return ExecuteGeneric(qualifier, argument); }

    mtsExecutionResult ExecuteGeneric(const mtsGenericObject & qualifier,
                                      mtsGenericObject & argument) const;

#ifndef SWIG
	/*! Overloaded operator that accepts different argument types (for qualified read). */
    template <class _userType1, class _userType2>
    mtsExecutionResult operator()(const _userType1 & argument1, _userType2 & argument2) const
    { return Execute(argument1, argument2); }

    template <class _userType1, class _userType2>
    mtsExecutionResult Execute(const _userType1 & argument1, _userType2 & argument2) const {
        mtsExecutionResult result = Command ?
            ConditionalWrap<_userType1, _userType2,
                            cmnIsDerivedFrom<_userType1, mtsGenericObject>::IS_DERIVED,
                            cmnIsDerivedFrom<_userType2, mtsGenericObject>::IS_DERIVED>::Call(this, argument1, argument2)
          : mtsExecutionResult::FUNCTION_NOT_BOUND;
        return result;
    }
#endif

    /*! Access to underlying command object. */
    CommandType * GetCommand(void) const;

    /*! Access to the command argument 1 prototype. */
    const mtsGenericObject * GetArgument1Prototype(void) const;

    /*! Access to the command argument 2 prototype. */
    const mtsGenericObject * GetArgument2Prototype(void) const;

    /*! Human readable output to stream. */
    void ToStream(std::ostream & outputStream) const;
};

#endif // _mtsFunctionQualifiedRead_h
