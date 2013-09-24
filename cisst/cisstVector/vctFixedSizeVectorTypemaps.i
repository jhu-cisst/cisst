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

#ifndef _vctFixedSizeVectorTypemaps_i
#define _vctFixedSizeVectorTypemaps_i

%include "cisstVector/vctDynamicVectorTypemaps.i"

// macro used to apply using the full name of fixed size vector
%define VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_FULL_NAME_ONE(name)
%apply vctDynamicVector         {name};
%apply vctDynamicVector &       {name &};
%apply const vctDynamicVector & {const name &};
%enddef

// macro used to apply using a full prefix (e.g. vctDouble) and will add suffix for size 
%define VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_FULL_NAME_ALL(name)
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_FULL_NAME_ONE(name##1);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_FULL_NAME_ONE(name##2);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_FULL_NAME_ONE(name##3);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_FULL_NAME_ONE(name##4);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_FULL_NAME_ONE(name##5);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_FULL_NAME_ONE(name##6);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_FULL_NAME_ONE(name##7);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_FULL_NAME_ONE(name##8);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_FULL_NAME_ONE(name##9);
%enddef

// macro used to apply using the parameter types, both elementType and size
%define VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ONE(elementType, size)
%apply vctDynamicVector         {vctFixedSizeVector<elementType, size>};
%apply vctDynamicVector &       {vctFixedSizeVector<elementType, size> &};
%apply const vctDynamicVector & {const vctFixedSizeVector<elementType, size> &};
%enddef

// macro used to apply using the elementType, will apply for sizes 1 to ...
%define VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ALL(elementType)
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ONE(elementType, 1);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ONE(elementType, 2);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ONE(elementType, 3);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ONE(elementType, 4);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ONE(elementType, 5);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ONE(elementType, 6);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ONE(elementType, 7);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ONE(elementType, 8);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ONE(elementType, 9);
%enddef
 
// now call macros for basic types, template specializations and typedefs 
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ALL(bool);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_FULL_NAME_ALL(vctBool);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ALL(char);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_FULL_NAME_ALL(vctChar);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ALL(unsigned char);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_FULL_NAME_ALL(vctUChar);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ALL(short);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_FULL_NAME_ALL(vctShort);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ALL(unsigned short);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_FULL_NAME_ALL(vctUShort);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ALL(int);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_FULL_NAME_ALL(vctInt);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ALL(unsigned int);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_FULL_NAME_ALL(vctUInt);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ALL(long);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_FULL_NAME_ALL(vctLong);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ALL(unsigned long);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_FULL_NAME_ALL(vctULong);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ALL(float);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_FULL_NAME_ALL(vctFloat);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ALL(double);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_FULL_NAME_ALL(vctDouble);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_FULL_NAME_ALL(vct);

// to access sizes and strides of matrices and N-arrays
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ALL(vct::size_type);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ALL(vct::stride_type);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ALL(size_type);
VCT_TYPEMAPS_APPLY_FIXED_SIZE_VECTORS_TEMPLATE_PARAMETERS_ALL(stride_type);

#endif // _vctFixedSizeVectorTypemaps_i
