/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:   2013-05-14

  (C) Copyright 2013-2021 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _prmJointType_h
#define _prmJointType_h

#include <cisstConfig.h>
#include <cisstCommon/cmnDataFunctionsEnumMacros.h>
#include <cisstMultiTask/mtsGenericObjectProxy.h>

// Always include last!
#include <cisstParameterTypes/prmExport.h>

typedef enum JointType {
    PRM_JOINT_UNDEFINED,
    PRM_JOINT_PRISMATIC,
    PRM_JOINT_REVOLUTE,
    PRM_JOINT_INACTIVE
} prmJointType;

typedef vctDynamicVector<prmJointType> prmJointTypeVec;
typedef mtsGenericObjectProxy<prmJointTypeVec> prmJointTypeProxy;
CMN_DECLARE_SERVICES_INSTANTIATION(prmJointTypeProxy);

inline std::string cmnDataHumanReadable(const prmJointType & data)
{
    switch (data) {
    case PRM_JOINT_UNDEFINED:
        return "undefined";
        break;
    case PRM_JOINT_PRISMATIC:
        return "prismatic";
        break;
    case PRM_JOINT_REVOLUTE:
        return "revolute";
        break;
    case PRM_JOINT_INACTIVE:
        return "inactive";
        break;
    default:
        return "unknown";
        break;
    }
    return "unknown";
}

CMN_DATA_SPECIALIZATION_FOR_ENUM(prmJointType, int);

#if CISST_HAS_JSON
CMN_DECLARE_DATA_FUNCTIONS_JSON_FOR_ENUM_EXPORT(prmJointType);
#endif

void CISST_EXPORT prmJointTypeToFactor(const vctDynamicVector<prmJointType> & types,
                                       const double prismaticFactor,
                                       const double revoluteFactor,
                                       vctDynamicVector<double> & factors);

#endif // _prmJointType_h

