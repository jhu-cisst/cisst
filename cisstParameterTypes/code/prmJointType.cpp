/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2010-10-20

  (C) Copyright 2010-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstParameterTypes/prmJointType.h>

#if CISST_HAS_JSON
CMN_IMPLEMENT_DATA_FUNCTIONS_JSON_FOR_ENUM(prmJointType, int);
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
