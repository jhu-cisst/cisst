/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):	Anton Deguet
  Created on:   2013-05-14

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _prmJointType_h
#define _prmJointType_h


#include <cisstMultiTask/mtsGenericObjectProxy.h>


typedef enum JointType {
    PRM_PRISMATIC,
    PRM_REVOLUTE
} prmJointType;
typedef vctDynamicVector<prmJointType> prmJointTypeVec;
typedef mtsGenericObjectProxy<prmJointTypeVec> prmJointTypeProxy;
CMN_DECLARE_SERVICES_INSTANTIATION(prmJointTypeProxy);


#endif // _prmJointType_h

