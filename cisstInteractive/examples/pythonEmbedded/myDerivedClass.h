/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Anton Deguet
  Created on: 2004-10-05

  (C) Copyright 2004-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _myDerivedClass_h
#define _myDerivedClass_h


#include <vector>
#include <iostream>
#include <string>


#include <cisstCommon/cmnGenericObject.h>
#include <cisstVector/vctTypes.h>


// code required for the DLL generation
#ifdef myDerivedClass_EXPORTS
#define CISST_THIS_LIBRARY_AS_DLL
#endif
#include <cisstCommon/cmnExportMacros.h>
#undef CISST_THIS_LIBRARY_AS_DLL
// end of code for the DLL generation

class CISST_EXPORT myDerivedClass: public cmnGenericObject {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
    
protected:
    vctDouble3 FixedSizeVectorMember;
    vctDoubleVec DynamicVectorMember;
    
public:

    myDerivedClass(void);

    virtual ~myDerivedClass() {}

    inline virtual void CommonMethod(void) {
        std::cout << "CommonMethod from derived (+= 5.0)" << std::endl;
        this->FixedSizeVectorMember += 5.0;
        this->DynamicVectorMember += 5.0;
    }

    inline vctDouble3 & FixedSizeVector(void) {
        return this->FixedSizeVectorMember;
    }

    inline const vctDouble3 & FixedSizeVector(void) const {
        return this->FixedSizeVectorMember;
    }

    inline vctDoubleVec & DynamicVector(void) {
        return this->DynamicVectorMember;
    }

    inline const vctDoubleVec & DynamicVector(void) const {
        return this->DynamicVectorMember;
    }

};

CMN_DECLARE_SERVICES_INSTANTIATION(myDerivedClass);

#endif // _myDerivedClass_h

