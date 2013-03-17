/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Ofri Sadowsky
  Created on: 2003-10-15

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/* This code is used for the cisstVector library tutorial.  Please
   update the tutorial too! */

// include what is needed from cisst
#include <cisstConfig.h>
#include <cisstCommon.h>
#include <cisstVector.h>

// system includes
#include <iostream>

#include "ExampleFrame.h"
#include "ExampleDynamic.h"
#include "ExampleReference.h"
#include "ExampleReferenceFixedSize.h"
#include "ExampleNArray.h"
#include "ExampleSTL.h"
#include "ExampleCommon.h"
#include "ExampleDynamicFunctions.h"
#include "ExampleFixedSizeFunctions.h"
#include "ExampleMatrix.h"

int main(void)
{
    Json::Value jsonValue;
    vctDynamicVector<double> vector(4);
    vctRandom(vector, -10.0, 10.0);
    cmnDataToJSON(vector, jsonValue["vector"]);
    std::cout << jsonValue << std::endl;

    vctDynamicVector<double> result;
    cmnDataFromJSON(result, jsonValue["vector"]);
    std::cout << result.size() << " -> " << result << std::endl;

    vctDynamicVector<vctDynamicVector<double> > VofV(2);
    VofV[0].SetSize(3);
    VofV[0].SetAll(-1.0);
    VofV[1].SetSize(4);
    VofV[1].SetAll(2.231);
    cmnDataToJSON(VofV, jsonValue["VofV"]);
    std::cout << jsonValue << std::endl;

    vctDynamicVector<vctDynamicVector<double> > VofVResult;
    cmnDataFromJSON(VofVResult, jsonValue["VofV"]);
    std::cout << VofVResult << std::endl;

    return 0;

    std::cout << "=== ExampleFrame ===" << std::endl;
    ExampleFrame();
    std::cout << "=== ExampleDynamic ===" << std::endl;
    ExampleDynamic();
    std::cout << "=== ExampleReference ===" << std::endl;
    ExampleReference();
    std::cout << "=== ExampleReferenceFixedSize ===" << std::endl;
    ExampleReferenceFixedSize();
    std::cout << "=== ExampleNArray ===" << std::endl;
    ExampleNArray();
    std::cout << "=== ExampleSTL ===" << std::endl;
    ExampleSTL();
    std::cout << "=== ExampleCommon ===" << std::endl;
    ExampleCommon();
    std::cout << "=== ExampleMatrix ===" << std::endl;
    MatrixExample();
    return 0;
}
