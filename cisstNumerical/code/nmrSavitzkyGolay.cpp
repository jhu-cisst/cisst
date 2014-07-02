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

vctDynamicVector<double>
CISST_EXPORT nmrSavitzkyGolay( int M,
                               int D,
                               int NL,
                               int NR ){

  vctDynamicMatrix<double> A( NL+NR+1, M+1, 0.0, VCT_COL_MAJOR );
  vctDynamicVector<double> b( M, 0.0 );

  for( int t=-NL; t<=NR; t++ ){
    double tk=1.0;
    A[ t+NL ][ 0 ] = tk;
    for( int k=1; k<M+1; k++ ){
      tk *= t;
      A[ t+NL ][ k ] = tk;
    }
  }

  vctDynamicMatrix<double> ATA = A.Transpose() * A;
  nmrInverse( ATA );
  vctDynamicMatrix<double> ATAAT = ATA * A.Transpose();

  vctDynamicVector<double> c( NL+NR+1, 0.0 );

  for( int n=0; n<NL+NR+1; n++ ){
    vctDynamicVector<double> en( NL+NR+1, 0.0 );
    en[n] = 1.0;

    vctDynamicVector<double> x = ATAAT * en;
    c[n] = x[D];
  }

  double frac=1.0;
  if( 1 < D ){
    for( int i=1; i<=D; i++ ){
      frac *= i;
    }
  }

  return c*frac;

}
