/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2006-10-27
  
  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _cmnClassRegisterTestStatic_h
#define _cmnClassRegisterTestStatic_h

#include <cisstCommon/cmnClassRegister.h>
#include <cisstCommon/cmnGenericObject.h>


class staticAllInline: public cmnGenericObject {
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    inline staticAllInline(void) {}
    
    inline std::string Name(void) const {
        return this->Services()->GetName();
    }
};

CMN_DECLARE_SERVICES_INSTANTIATION(staticAllInline);

class staticNoInline: public cmnGenericObject {
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    staticNoInline(void);

    std::string Name(void) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(staticNoInline);

#endif // _cmnClassRegisterTestStatic_h

