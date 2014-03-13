/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-  */
/*ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab:*/

/*
  $Id: nmrSavitzkyGolay.h 4267 2013-06-11 14:01:21Z sleonar7 $
  
  Author(s):	Simon Leonard
  Created on:	2013-06-11

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _nmrSavitzkyGolay_h
#define _nmrSavitzkyGolay_h

#include <cisstVector/vctDynamicVector.h>
#include <cisstNumerical/nmrExport.h>

//! Savitzky Golay filter design
/**
   Creates a Savitzky-Golay FIR filter. The filter is defined by the order K
   of a polynomial, which derivative D to be filtered and the support defined
   by the filter size determined by the number of samples NL left of a data 
   point and the number of samples NR right of the data point. For a causal 
   filter NR=0
*/

vctDynamicVector<double> CISST_EXPORT nmrSavitzkyGolay( int K, 
							int D, 
							int NL, 
							int NR );

#endif
