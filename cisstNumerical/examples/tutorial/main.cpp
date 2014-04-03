/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Anton Deguet
  Created on:	2006-01-12

  (C) Copyright 2006-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

// include what is needed from cisst
#include <cisstNumerical/nmrConfig.h>

// system includes
#include <iostream>

#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctDynamicMatrix.h>
#include <cisstVector/vctRandom.h>

#include <cisstNumerical/nmrSVD.h>
#include <cisstNumerical/nmrSVDEconomy.h>
#include "ExampleSVDUserOutputWorkspace.h"
#include "ExampleSVDImplicitWorkspace.h"
#include "ExampleSVDWorkspaceSize.h"
#include "ExampleSVDDynamicData.h"
#include "ExampleSVDUpdateMatrixS.h"
#include "ExampleSVDFixedSize.h"
#include "ExampleSVDFixedSizeData.h"
#include <cisstNumerical/nmrLU.h>
#include "ExampleLUDynamicData.h"
#include <cisstNumerical/nmrInverse.h>
#include "ExampleInverse.h"
#include <cisstNumerical/nmrPInverse.h>
#include <cisstNumerical/nmrPInverseEconomy.h>
#include "ExamplePInverse.h"
#include <cisstNumerical/nmrIsOrthonormal.h>
#include "ExampleIsOrthonormal.h"
#include <cisstNumerical/nmrGaussJordanInverse.h>
#include "ExampleGaussJordanInverse.h"
#include <cisstNumerical/nmrSavitzkyGolay.h>
#include "ExampleSavitzkyGolay.h"
#include <cisstNumerical/nmrInverseSPD.h>
#include "ExampleInverseSPD.h"

int main(void)
{
    std::cout << "--- ExampleSVDUserOutputWorkspace ---" << std::endl;
    ExampleSVDUserOutputWorkspace();

    std::cout << "--- ExampleSVDImplicitWorkspace ---" << std::endl;
    ExampleSVDImplicitWorkspace();

    std::cout << "--- ExampleSVDWorkspaceSize ---" << std::endl;
    ExampleSVDWorkspaceSize();

    std::cout << "--- ExampleSVDDynamicData ---" << std::endl;
    ExampleSVDDynamicData();

    std::cout << "--- ExampleSVDUpdateMatrixS ---" << std::endl;
    ExampleSVDUpdateMatrixS();

    std::cout << "--- ExampleSVDFixedSize ---" << std::endl;
    ExampleSVDFixedSize();

    std::cout << "--- ExampleSVDFixedSizeData ---" << std::endl;
    ExampleSVDFixedSizeData();

    std::cout << "--- ExampleInverse ---" << std::endl;
    ExampleInverse();

    std::cout << "--- ExamplePInverse ---" << std::endl;
    ExamplePInverse();

    std::cout << "--- ExampleSVDEconomyUserOutputWorkspace ---" << std::endl;
    ExampleSVDEconomyUserOutputWorkspace();

    std::cout << "--- ExampleSVDEconomyImplicitWorkspace ---" << std::endl;
    ExampleSVDEconomyImplicitWorkspace();

    std::cout << "--- ExampleSVDEconomyWorkspaceSize ---" << std::endl;
    ExampleSVDEconomyWorkspaceSize();

    std::cout << "--- ExampleSVDEconomyDynamicData ---" << std::endl;
    ExampleSVDEconomyDynamicData();

    std::cout << "--- ExampleSVDEconomyUpdateMatrixS ---" << std::endl;
    ExampleSVDEconomyUpdateMatrixS();

    std::cout << "--- ExamplePInverseEconomy ---" << std::endl;
    ExamplePInverseEconomy();

    std::cout << "--- ExampleLUDynamicData ---" << std::endl;
    ExampleLUDynamicData();

    std::cout << "--- ExampleIsOrthonormal ---" << std::endl;
    ExampleIsOrthonormal();

    std::cout << "--- ExampleGaussJordanInverse ---" << std::endl;
    ExampleGaussJordanInverse();

    std::cout << "--- ExampleSavitzkyGolay ---" << std::endl;
    ExampleSavitzkyGolay();

    std::cout << "--- ExampleInverseSPD ---" << std::endl;
    ExampleInverseSPD();

    return 0;
}

