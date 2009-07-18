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

#ifndef _mtsMacros_h
#define _mtsMacros_h

#include <cisstCommon/cmnAccessorMacros.h>
// the following macro is now part of cisstCommon, cmnAccessorMacros.h
#define MTS_DECLARE_MEMBER_AND_ACCESSORS CMN_DECLARE_MEMBER_AND_ACCESSORS

#define MTS_PROXY_CLASS_DECLARATION_FROM(className, newName)          \
class CISST_EXPORT newName: public mtsGenericObject, public className \
{                                                                     \
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_DEFAULT_LOD);  \
public:                                                               \
    newName(void): mtsGenericObject(), className() {}                 \
    newName(const className & other):                                 \
        mtsGenericObject(),                                           \
        className(other) {}                                           \
};                                                                    \
CMN_DECLARE_SERVICES_INSTANTIATION(newName)


#endif // _mtsMacros_h

