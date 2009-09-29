/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: newLibrary.cpp 75 2009-02-24 16:47:20Z adeguet1 $

  Author(s): Anton Deguet
  Created on: 2006-11-20

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "newLibrary.h"

void newClass::PerformRandomSVD(void)
{
    // fill a matrix with random numbers
    vctDynamicMatrix<double> A(5, 7);
    vctRandom(A, -10.0, 10.0);
    // create a data object
    nmrSVDDynamicData svdData(A);
    // and we can finally call the nmrSVD function
    vctDynamicMatrix<double> Acopy = A;
    nmrSVD(Acopy, svdData);
    // display the result
    std::cout << "U:\n" << svdData.U()
              << "\nS:\n" << svdData.S()
              << "\nV:\n" << svdData.Vt().TransposeRef() << std::endl;
    
}
