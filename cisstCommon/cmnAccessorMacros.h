/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:	2008-06-26

  (C) Copyright 2008-2015 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#pragma once

#ifndef _cmnAccessorMacros_h
#define _cmnAccessorMacros_h

/*!
  \file
  \brief Accessor macros.
*/


/*! Macro used to declare a data member and implement accessor methods
  in a standard way.  This macro should not be used, use
  #CMN_DECLARE_MEMBER_AND_ACCESSORS instead. */
#define CMN_DECLARE_MEMBER_AND_ACCESSORS_INTERNAL(memberType, memberName) \
protected:                                                              \
 memberType memberName##Member;                                         \
public:                                                                 \
 inline void Set##memberName(const memberType & newValue) {             \
     this->memberName##Member = newValue;                               \
 }                                                                      \
 inline void Get##memberName(memberType & placeHolder) const {          \
     placeHolder = this->memberName##Member;                            \
 }                                                                      \
 inline const memberType & memberName(void) const {                     \
     return this->memberName##Member;                                   \
 }                                                                      \
 inline memberType & memberName(void) {                                 \
     return this->memberName##Member;                                   \
 }                                                                      \
protected:

/*! Macro to declare a data member and implement accessor methods.
  This macro creates 4 different accessors, two to read and two to
  write.  For example, the following call:
  \code
    CMN_DECLARE_MEMBER_AND_ACCESSORS(double, Timestamp)
  \endcode
  will create the 4 following methods:
  \code
    double & Timestamp(void);
    const double & Timestamp(void);
    void SetTimestamp(double newValue);
    void GetTimestamp(double & placeHolder) const;
  \endcode

  Finally, when used in conjunction with SWIG, the method Get is
  modified to return a value by const reference.
 */
#ifndef SWIG
    #define CMN_DECLARE_MEMBER_AND_ACCESSORS(memberType, memberName) \
    CMN_DECLARE_MEMBER_AND_ACCESSORS_INTERNAL(memberType, memberName)
#else
    #define CMN_DECLARE_MEMBER_AND_ACCESSORS(memberType, memberName)  \
    CMN_DECLARE_MEMBER_AND_ACCESSORS_INTERNAL(memberType, memberName) \
    %ignore Get##memberName(memberType &);                            \
    public:                                                           \
    %extend {                                                         \
        const memberType & Get##memberName(void) const {              \
            return self->memberName();                                \
        }                                                             \
    }                                                                 \
    protected:
#endif

#endif // _cmnAccessorMacros_h
