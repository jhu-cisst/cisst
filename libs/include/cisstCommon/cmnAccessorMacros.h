/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*!  
  Author(s):	Anton Deguet
  Created on:	2008-06-26

  (C) Copyright 2008-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and 
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#ifndef _cmnAccessorMacros_h
#define _cmnAccessorMacros_h

#define CMN_DECLARE_MEMBER_AND_ACCESSORS(type, name) \
 protected:                                          \
    type name##Member;                               \
 public:                                             \
    inline void Set##name(const type & newValue) {   \
        this->name##Member = newValue;               \
    }                                                \
    inline void Get##name(type & placeHolder) {      \
        placeHolder = this->name##Member;            \
    }                                                \
    inline const type & name(void) const {           \
        return this->name##Member;                   \
    }                                                \
    inline type & name(void) {                       \
        return this->name##Member;                   \
    }                                                \
 protected:


#endif // _cmnAccessorMacros_h

