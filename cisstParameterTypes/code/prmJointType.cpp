/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2010-10-20

  (C) Copyright 2010-2024 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstParameterTypes/prmJointType.h>
#include <locale>

#if CISST_HAS_JSON
CMN_IMPLEMENT_DATA_FUNCTIONS_JSON_FOR_ENUM_AS_STRING(prmJointType, prmJointTypeToString, prmJointTypeFromString);
#endif

void prmJointTypeToFactor(const vctDynamicVector<prmJointType> & types,
                          const double prismaticFactor,
                          const double revoluteFactor,
                          vctDynamicVector<double> & factors)
{
    // set unitFactor;
    for (size_t i = 0; i < types.size(); i++) {
        switch (types.at(i)) {
        case PRM_JOINT_PRISMATIC:
            factors.at(i) = prismaticFactor;
            break;
        case PRM_JOINT_REVOLUTE:
            factors.at(i) = revoluteFactor;
            break;
        default:
            factors.at(i) = 1.0;
            break;
        }
    }
}

std::string prmJointTypeToString(const prmJointType & data)
{
    switch (data) {
    case PRM_JOINT_UNDEFINED:
        return "UNDEFINED";
        break;
    case PRM_JOINT_PRISMATIC:
        return "PRISMATIC";
        break;
    case PRM_JOINT_REVOLUTE:
        return "REVOLUTE";
        break;
    case PRM_JOINT_INACTIVE:
        return "INACTIVE";
        break;
    default:
        break;
    }
    cmnThrow("prmJointTypeToString called with invalid enum");
    return "";
}

prmJointType prmJointTypeFromString(const std::string & value)
{
    if (value == "UNDEFINED") {
        return PRM_JOINT_UNDEFINED;
    };
    if (value == "PRISMATIC") {
        return PRM_JOINT_PRISMATIC;
    };
    if (value == "REVOLUTE") {
        return PRM_JOINT_REVOLUTE;
    };
    if (value == "INACTIVE") {
        return PRM_JOINT_INACTIVE;
    };
    std::string message = "prmJointTypeFromString can't find matching enum for " + value + ".  Options are: ";
    std::vector<std::string> options = prmJointTypeVectorString();
    for (std::vector<std::string>::const_iterator i = options.begin(); i != options.end(); ++i) message += *i + " ";
    cmnThrow(message);
    return PRM_JOINT_UNDEFINED;
}

const std::vector<std::string> & prmJointTypeVectorString(void)
{
    static std::vector<std::string> vectorString;
    if (vectorString.empty()) {
        vectorString.push_back("UNDEFINED");
        vectorString.push_back("PRISMATIC");
        vectorString.push_back("REVOLUTE");
        vectorString.push_back("INACTIVE");
    }
    return vectorString;
}
