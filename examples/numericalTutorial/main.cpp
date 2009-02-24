/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
 
  Author(s): Anton Deguet
  Created on:	2006-01-12

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/* This code is used for the cisstNumerical Quick Start.  Please
   update the LaTeX document as well. ! */

// include what is needed from cisst
#include <cisstConfig.h>
#include <cisstCommon.h>
#include <cisstVector.h>
#include <cisstNumerical.h>

// system includes
#include <iostream>

// to simplify the code
using namespace std;

#include "ExampleSVDUserOutputWorkspace.h"
#include "ExampleSVDImplicitWorkspace.h"
#include "ExampleSVDWorkspaceSize.h"
#include "ExampleSVDDynamicData.h"
#include "ExampleSVDUpdateMatrixS.h"
#include "ExampleSVDFixedSize.h"
#include "ExampleSVDFixedSizeData.h"
#include "ExampleLUDynamicData.h"
#include "ExampleInverse.h"
#include "ExamplePInverse.h"
#include "ExampleIsOrthonormal.h"
#include "ExampleGaussJordanInverse.h"

int main(void)
{
    cout << "--- ExampleSVDUserOutputWorkspace ---" << endl;
    ExampleSVDUserOutputWorkspace();

    cout << "--- ExampleSVDImplicitWorkspace ---" << endl;
    ExampleSVDImplicitWorkspace();

    cout << "--- ExampleSVDWorkspaceSize ---" << endl;
    ExampleSVDWorkspaceSize();

    cout << "--- ExampleSVDDynamicData ---" << endl;
    ExampleSVDDynamicData();

    cout << "--- ExampleSVDUpdateMatrixS ---" << endl;
    ExampleSVDUpdateMatrixS();

    cout << "--- ExampleSVDFixedSize ---" << endl;
    ExampleSVDFixedSize();

    cout << "--- ExampleSVDFixedSizeData ---" << endl;
    ExampleSVDFixedSizeData();

    cout << "--- ExampleInverse ---" << endl;
    ExampleInverse();

    cout << "--- ExamplePInverse ---" << endl;
    ExamplePInverse();

    cout << "--- ExampleSVDEconomyUserOutputWorkspace ---" << endl;
    ExampleSVDEconomyUserOutputWorkspace();

    cout << "--- ExampleSVDEconomyImplicitWorkspace ---" << endl;
    ExampleSVDEconomyImplicitWorkspace();

    cout << "--- ExampleSVDEconomyWorkspaceSize ---" << endl;
    ExampleSVDEconomyWorkspaceSize();

    cout << "--- ExampleSVDEconomyDynamicData ---" << endl;
    ExampleSVDEconomyDynamicData();

    cout << "--- ExampleSVDEconomyUpdateMatrixS ---" << endl;
    ExampleSVDEconomyUpdateMatrixS();

    cout << "--- ExamplePInverseEconomy ---" << endl;
    ExamplePInverseEconomy();

    cout << "--- ExampleLUDynamicData ---" << endl;
    ExampleLUDynamicData();

    cout << "--- ExampleIsOrthonormal ---" << endl;
    ExampleIsOrthonormal();

    cout << "--- ExampleGaussJordanInverse ---" << endl;
    ExampleGaussJordanInverse();

    return 0;
}

