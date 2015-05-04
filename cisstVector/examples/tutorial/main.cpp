/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet, Ofri Sadowsky
  Created on: 2003-10-15

  (C) Copyright 2003-2014 Johns Hopkins University (JHU), All Rights
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
#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctFixedSizeMatrixTypes.h>
#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstVector/vctDynamicMatrixTypes.h>
#include <cisstVector/vctDynamicNArray.h>
#include <cisstVector/vctTransformationTypes.h>
#include <cisstVector/vctRandom.h>

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
#include "ExampleReferences.h"

int main(void)
{
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
    ExampleMatrix();
    std::cout << "=== ExampleReferences ===" << std::endl;
    ExampleReferences();
    return 0;
}
