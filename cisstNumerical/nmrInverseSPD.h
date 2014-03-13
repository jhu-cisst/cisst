/*-*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-   */
/*ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab:*/

/*
  $Id: nmrInverseSPD.h 3921 2013-01-14 21:07:15Z sleonar7 $

  Author(s):  Simon Leonard
  Created on: 2013-08-20

  (C) Copyright 2006-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of nmrInverseSPD
*/


#ifndef _nmrInverseSPD_h
#define _nmrInverseSPD_h

#include <cisstCommon/cmnThrow.h>
#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctDynamicMatrix.h>
#include <cisstNumerical/nmrNetlib.h>


#include <cisstNumerical/nmrExport.h>

//! Computes the inverse of a symmetric positive definite matrix
/**
   This computes the inverse of a symmetric positive definite (SPD) matrix. The 
   \f$ N \times N \f$ matrix is assumed SPD and only the lower triangular is
   considered in the computation. The upper triangular is assumed to be 
   symmetric even though it might not be as it is not used to compute the 
   inverse.
   \param[in] A A \f$ N \times N \f$ column major symmetric positive definite 
                matrix.
   \param[out] A The matrix inverse
*/
void CISST_EXPORT nmrInverseSPD( vctDynamicMatrix<double>& A );

#endif
