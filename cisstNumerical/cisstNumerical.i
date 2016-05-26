/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  File: cisstNumerical.i

  Author(s):  Anton Deguet
  Created on: 2005-08-14

  (C) Copyright 2005-2016 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

%module cisstNumericalPython

%include <std_pair.i>
%include <std_string.i>
%include <std_except.i>
%include <std_vector.i>

%import "cisstCommon/cisstCommon.i"
%import "cisstVector/cisstVector.i"

%init %{
        import_array()   /* Initial function for NumPy */
%}

%header %{
#include <cisstNumerical/nmrPython.h>
%}

// Generate parameter documentation for IRE
%feature("autodoc", "1");

// Import nmrConfig.h for definition of CISST_HAS_CISSTNETLIB
// Note: Even though nmrNetlib.h includes nmrConfig.h, importing just nmrNetlib.h
//       does not work.
%import "cisstNumerical/nmrConfig.h"
%import "cisstNumerical/nmrNetlib.h"

#if CISST_HAS_CISSTNETLIB
// Programmer note: Following Python wrapping doesn't really work, as it does not properly use
//                  the cisstVector/numpy typemaps (e.g., vctDynamicMatrixTypemaps.i).
//                  Anyway, it is questionable whether it is useful to wrap cisstNumerical, since one
//                  could use methods from numpy / scipy.
%include "cisstNumerical/nmrSVD.i"

%include "cisstNumerical/nmrPInverse.h"
// Instantiate for vctDynamicMatrix. Could also have combinations of vctDynamicMatrix and vctDynamicMatrixRef.
%template(nmrPInverse) nmrPInverse<vctDynamicMatrixOwner<CISSTNETLIB_DOUBLE>, vctDynamicMatrixOwner<CISSTNETLIB_DOUBLE> >;

// The following is more Python-friendly than the templated C++ code in "cisstNumerical/nmrRegistrationRigid.h".
// Due to the typemaps, dataSet1 and dataSet2 are provided as Python numpy arrays (2D arrays).
// The transformation and FRE are returned as a Python tuple.
// The body of the code is in nmrPython.h
%template() std::pair<vctFrm3, double>;
std::pair<vctFrm3, double> nmrRegistrationRigid(const vctDynamicMatrix<double> &dataSet1,
                                                const vctDynamicMatrix<double> &dataSet2) throw(std::runtime_error);

#endif
