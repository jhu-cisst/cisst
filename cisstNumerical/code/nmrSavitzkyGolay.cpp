/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-  */
/*ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab:*/

/*
  $Id: nmrSavitzkyGolay.cpp 4267 2013-06-11 14:01:21Z sleonar7 $

  Author(s):  Simon Leonard
  Created on: 2013-06-11

  (C) Copyright 2013-2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstNumerical/nmrSavitzkyGolay.h>
#include <cisstNumerical/nmrInverse.h>

Eigen::VectorXd
CISST_EXPORT nmrSavitzkyGolay(int M, int D, int NL, int NR)
{
    if (D < 0 || M < D) {
        return Eigen::VectorXd::Zero(NL + NR + 1);
    }

    Eigen::MatrixXd A = Eigen::MatrixXd::Zero(NL+NR+1, M+1);
    Eigen::VectorXd b = Eigen::VectorXd::Zero(M);

    // Construct Vandermonde matrix
    // A
    for (int z = -NL; z <= NR; z++) {
        double azk = 1.0;
        for(int k = 0; k<M+1; k++) {
            A(z+NL, k) = azk;
            azk *= z;
        }
    }

    // Construct normal equations for Vandermonde system
    Eigen::MatrixXd N = (A.transpose() * A).inverse() * A.transpose();
    // Dth row corresponds to coefficient of z^D
    Eigen::VectorXd coefficients = N.row(D);

    double D_factorial = 1.0;
    for (int i = 2; i <= D; i++) { D_factorial *= i; }

    return D_factorial * coefficients;
}
