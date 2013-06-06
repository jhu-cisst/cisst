/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Daniel Li, Anton Deguet
  Created on: 2009-05-20

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _vctFixedSizeMatrixTypemaps_i
#define _vctFixedSizeMatrixTypemaps_i

%include "cisstVector/vctDynamicMatrixTypemaps.i"

// macro used to apply using the full name of fixed size vector
%define VCT_TYPEMAPS_APPLY_FIXED_SIZE_MATRICES_FULL_NAME_ONE(name)
%apply vctDynamicMatrix         {name};
%apply vctDynamicMatrix &       {name &};
%apply const vctDynamicMatrix & {const name &};
%enddef

// macro used to apply using a full prefix (e.g. vctDouble) and will add suffix for size 
%define VCT_TYPEMAPS_APPLY_FIXED_SIZE_MATRICES_FULL_NAME_ALL(name)
VCT_TYPEMAPS_APPLY_FIXED_SIZE_MATRICES_FULL_NAME_ONE(name##1x1);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_MATRICES_FULL_NAME_ONE(name##2x2);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_MATRICES_FULL_NAME_ONE(name##3x3);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_MATRICES_FULL_NAME_ONE(name##4x4);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_MATRICES_FULL_NAME_ONE(name##5x5);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_MATRICES_FULL_NAME_ONE(name##6x6);
%enddef

// macro used to apply using the parameter types, both elementType and size
%define VCT_TYPEMAPS_APPLY_FIXED_SIZE_MATRICES_TEMPLATE_PARAMETERS_ONE(elementType, rows, cols)
%apply vctDynamicMatrix         {vctFixedSizeMatrix<elementType, rows, cols>};
%apply vctDynamicMatrix &       {vctFixedSizeMatrix<elementType, rows, cols> &};
%apply const vctDynamicMatrix & {const vctFixedSizeMatrix<elementType, rows, cols> &};
%enddef

// macro used to apply using the elementType, will apply for sizes 1 to ...
%define VCT_TYPEMAPS_APPLY_FIXED_SIZE_MATRICES_TEMPLATE_PARAMETERS_ALL(elementType)
VCT_TYPEMAPS_APPLY_FIXED_SIZE_MATRICES_TEMPLATE_PARAMETERS_ONE(elementType, 1, 1);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_MATRICES_TEMPLATE_PARAMETERS_ONE(elementType, 2, 2);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_MATRICES_TEMPLATE_PARAMETERS_ONE(elementType, 3, 3);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_MATRICES_TEMPLATE_PARAMETERS_ONE(elementType, 4, 4);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_MATRICES_TEMPLATE_PARAMETERS_ONE(elementType, 5, 5);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_MATRICES_TEMPLATE_PARAMETERS_ONE(elementType, 6, 6);
%enddef
 
// now call macros for basic types, template specializations and typedefs 
VCT_TYPEMAPS_APPLY_FIXED_SIZE_MATRICES_TEMPLATE_PARAMETERS_ALL(int);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_MATRICES_FULL_NAME_ALL(vctInt);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_MATRICES_TEMPLATE_PARAMETERS_ALL(unsigned int);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_MATRICES_FULL_NAME_ALL(vctUInt);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_MATRICES_TEMPLATE_PARAMETERS_ALL(double);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_MATRICES_FULL_NAME_ALL(vctDouble);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_MATRICES_FULL_NAME_ALL(vct);

#endif // _vctFixedSizeMatrixTypemaps_i
